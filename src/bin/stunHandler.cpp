#include "ValhallaCombat.hpp"
#include "include/stunHandler.h"
#include "include/hitProcessor.h"
#include "include/settings.h"
#include "include/reactionHandler.h"
#include "include/offsets.h"
#include "include/Utils.h"
using uniqueLocker = std::unique_lock<std::shared_mutex>;
using sharedLocker = std::shared_lock<std::shared_mutex>;

void stunHandler::safeErase_ActorStunDataMap(RE::Actor* a_actor) {
	uniqueLocker lock(mtx_ActorStunDataMap);
	actorStunDataMap.erase(a_actor);
}
void stunHandler::safeErase_StunBrokenActors(RE::Actor* a_actor) {
	uniqueLocker lock(mtx_StunBrokenActors);
	stunBrokenActors.erase(a_actor);
}
void stunHandler::safeErase_StunRegenQueue(RE::Actor* a_actor) {
	uniqueLocker lock(mtx_StunRegenQueue);
	stunRegenQueue.erase(a_actor);
}

void stunHandler::safeInsert_StunRegenQueue(RE::Actor* a_actor) {
	uniqueLocker lock(mtx_StunRegenQueue);
	stunRegenQueue[a_actor] = 3;
}

void stunHandler::safeInsert_StunBrokenActors(RE::Actor* a_actor) {
	uniqueLocker lock(mtx_StunBrokenActors);
	if (!stunBrokenActors.contains(a_actor)) {
		stunBrokenActors.insert(a_actor);
	}
}

bool stunHandler::safeGet_isStunBroken(RE::Actor* a_actor) {
	sharedLocker lock(mtx_StunBrokenActors);
	return stunBrokenActors.contains(a_actor);
}

stunHandler::actorStunData_ptr stunHandler::safeGet_ActorStunData(RE::Actor* a_actor) {
	{
		sharedLocker lock(mtx_ActorStunDataMap);
		if (actorStunDataMap.contains(a_actor)) {
			return actorStunDataMap[a_actor];
		}
	}
	return trackStun(a_actor);
}

stunHandler::actorStunData_ptr stunHandler::trackStun(RE::Actor* a_actor) {
	uniqueLocker lock(mtx_ActorStunDataMap);
	if (actorStunDataMap.contains(a_actor)) {//check if the actor's stun is already tracked.
		return actorStunDataMap[a_actor];
	}

	actorStunData_ptr stunDataPtr(new actorStunData(a_actor));
	actorStunDataMap[a_actor] = stunDataPtr;
	return stunDataPtr;
};
void stunHandler::untrackStun(RE::Actor* a_actor) {
	safeErase_ActorStunDataMap(a_actor);
	safeErase_StunBrokenActors(a_actor);
	safeErase_StunRegenQueue(a_actor);
}

void stunHandler::refillStun(RE::Actor* a_actor) {
	sharedLocker lock(mtx_ActorStunDataMap);
	if (!actorStunDataMap.contains(a_actor)) {
		return;
	}
	actorStunDataMap[a_actor]->refillStun();
}


bool stunHandler::getIsStunBroken(RE::Actor* a_actor) {
	return safeGet_isStunBroken(a_actor);
}

void stunHandler::update() {
	uniqueLocker lock_stunRegenQueue(mtx_StunRegenQueue);
	//stop update if there is nothing in the queue.
	if (stunRegenQueue.empty()) {
		ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::stunHandler);
		return;
	}
	auto it_StunRegenQueue = stunRegenQueue.begin();
	while (it_StunRegenQueue != stunRegenQueue.end()) {
		auto& one_actor = it_StunRegenQueue->first;
		if (!one_actor || !one_actor->currentProcess || !one_actor->currentProcess->InHighProcess()) {//edge case: actor not loaded or no longer in high process.
			//if an actor is no longer present=>remove actor.
			safeErase_ActorStunDataMap(one_actor);
			safeErase_StunBrokenActors(one_actor);
			it_StunRegenQueue = stunRegenQueue.erase(it_StunRegenQueue); continue;
		}

		/*---------------------------Regen area-------------------------------------*/
		if (!one_actor->IsInCombat()
			|| safeGet_isStunBroken(one_actor)) {

			//>>>>>>>>actor that can regen
			if (it_StunRegenQueue->second <= 0) {//timer reached zero, time to regen.
				bool isStunFullRegenerated = false;
				{//local scope for shared actorStunDataMap access
					sharedLocker lock_actorStunDataMap(mtx_ActorStunDataMap);
					if (actorStunDataMap.contains(one_actor)) {
						isStunFullRegenerated = actorStunDataMap[one_actor]->regenStun();
					}
					else {
						isStunFullRegenerated = true;
					}
				}

				if (isStunFullRegenerated) {
					//DEBUG("stun fully regenerated for {}", one_actor->GetName());
					TrueHUDUtils::revertSpecialMeter(one_actor);
					reactionHandler::recoverDownedState(one_actor);
					safeErase_StunBrokenActors(one_actor);
					it_StunRegenQueue = stunRegenQueue.erase(it_StunRegenQueue); continue;
				}
			}
			else {
				it_StunRegenQueue->second -= *RE::Offset::g_deltaTime;//keep decrementing regen timer.
			}
		}
		/*---------------------------Regen area-------------------------------------*/

		++it_StunRegenQueue;
	}

}

void stunHandler::async_launchHealthBarFlashThread() {
	if (async_HealthBarFlashThreadActive) {
		return;
	}

	async_HealthBarFlashThreadActive = true;
	auto flashHealthBarFunc = [&]() {
		while (true) {
			{//local scope where to safely iterate through stunned actors.
				sharedLocker lock(mtx_StunBrokenActors);
				if (stunBrokenActors.empty()) {
					async_HealthBarFlashThreadActive = false;//deactivate flash thread.
					return;
				}
				else {
					for (auto& one_actor : stunBrokenActors) {
						if (one_actor && !one_actor->IsDead()) {
							TrueHUDUtils::flashActorValue(one_actor, RE::ActorValue::kHealth);
						}
					}
				}
			}//exit scope and sleep the thread for 500 milisecs to check again.
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	};

	std::jthread t(flashHealthBarFunc);
	t.detach();
	DEBUG("...done");
}


float stunHandler::getMaxStun(RE::Actor* a_actor) {
	{
		sharedLocker lock(mtx_ActorStunDataMap);
		if (actorStunDataMap.contains(a_actor)) {
			return actorStunDataMap[a_actor]->getMaxStun();
		}
	}
	return trackStun(a_actor)->getMaxStun();
}

float stunHandler::getMaxStun_static(RE::Actor* a_actor) {
	return stunHandler::GetSingleton()->getMaxStun(a_actor);
}

float stunHandler::getCurrentStun(RE::Actor* a_actor) {
	{
		sharedLocker lock(mtx_ActorStunDataMap);
		if (actorStunDataMap.contains(a_actor)) {
			return actorStunDataMap[a_actor]->getCurrentStun();
		}
	}
	return trackStun(a_actor)->getCurrentStun();
}

float stunHandler::getCurrentStun_static(RE::Actor* a_actor) {
	return stunHandler::GetSingleton()->getCurrentStun(a_actor);
}

void stunHandler::onStunBroken(RE::Actor* a_actor) {
	safeInsert_StunBrokenActors(a_actor);
	async_launchHealthBarFlashThread();
	ValhallaUtils::playSound(a_actor, data::soundStunBreak);
	//gray out this actor's stun meter.
	if (settings::bStunMeterToggle) {
		TrueHUDUtils::greyOutSpecialMeter(a_actor);
	}
}

void stunHandler::damageStun(RE::Actor* a_aggressor, RE::Actor* a_victim, float a_damage) {
	//DEBUG("Damaging {}'s stun by {} points.", a_victim->GetName(), a_damage);
	if (safeGet_ActorStunData(a_victim)->damageStun(a_damage)) {
		if (!safeGet_isStunBroken(a_victim)) {//the current damage depletes actor stun, but actor's stunned state is not yet tracked.
			onStunBroken(a_victim);
		}
	}
	safeInsert_StunRegenQueue(a_victim);
	ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::stunHandler);
	//DEBUG("damaging done");
}


//parry damage needs to be offset here, since it's not calculated.
void stunHandler::processStunDamage(
	STUNSOURCE a_stunSource, RE::TESObjectWEAP* a_weapon, RE::Actor* a_aggressor, RE::Actor* a_victim, float a_baseDamage) {
	if (!settings::bStunToggle) { //stun damage will not be applied with stun turned off.
		return;
	}
	if (a_victim->IsPlayerRef()) { //player do not receive stun damage at all.
		return;
	}
	float stunDamage = 0;
	switch (a_stunSource) {
	case STUNSOURCE::timedBlock:
		stunDamage = a_baseDamage * settings::fStunTimedBlockMult;
		break;
	case STUNSOURCE::bash:
		stunDamage = a_aggressor->GetActorValue(RE::ActorValue::kBlock) * settings::fStunBashMult;
		inlineUtils::offsetRealDamage(stunDamage, a_aggressor, a_victim);
		break;
	case STUNSOURCE::powerBash:
		stunDamage = a_aggressor->GetActorValue(RE::ActorValue::kBlock) * settings::fStunPowerBashMult;
		inlineUtils::offsetRealDamage(stunDamage, a_aggressor, a_victim);
		break;
	case STUNSOURCE::parry:
		stunDamage = a_aggressor->GetActorValue(RE::ActorValue::kBlock) * settings::fStunParryMult;
		inlineUtils::offsetRealDamage(stunDamage, a_aggressor, a_victim);
		break;
	case STUNSOURCE::lightAttack:
		stunDamage = a_baseDamage * settings::fStunNormalAttackMult;
		if (!a_weapon) {
			stunDamage *= settings::fStunUnarmedMult;
		}
		else {
			switch (a_weapon->GetWeaponType()) {
			case RE::WEAPON_TYPE::kHandToHandMelee: stunDamage *= settings::fStunUnarmedMult; break;
			case RE::WEAPON_TYPE::kOneHandDagger: stunDamage *= settings::fStunDaggerMult; break;
			case RE::WEAPON_TYPE::kOneHandSword: stunDamage *= settings::fStunSwordMult; break;
			case RE::WEAPON_TYPE::kOneHandAxe: stunDamage *= settings::fStunWarAxeMult; break;
			case RE::WEAPON_TYPE::kOneHandMace: stunDamage *= settings::fStunMaceMult; break;
			case RE::WEAPON_TYPE::kTwoHandAxe: stunDamage *= settings::fStun2HBluntMult; break;
			case RE::WEAPON_TYPE::kTwoHandSword: stunDamage *= settings::fStunGreatSwordMult; break;
			}
		}
		break;
	case STUNSOURCE::powerAttack:
		stunDamage = a_baseDamage * settings::fStunPowerAttackMult;
		if (!a_weapon) {
			stunDamage *= settings::fStunUnarmedMult;
		}
		else {
			switch (a_weapon->GetWeaponType()) {
			case RE::WEAPON_TYPE::kHandToHandMelee: stunDamage *= settings::fStunUnarmedMult; break;
			case RE::WEAPON_TYPE::kOneHandDagger: stunDamage *= settings::fStunDaggerMult; break;
			case RE::WEAPON_TYPE::kOneHandSword: stunDamage *= settings::fStunSwordMult; break;
			case RE::WEAPON_TYPE::kOneHandAxe: stunDamage *= settings::fStunWarAxeMult; break;
			case RE::WEAPON_TYPE::kOneHandMace: stunDamage *= settings::fStunMaceMult; break;
			case RE::WEAPON_TYPE::kTwoHandAxe: stunDamage *= settings::fStun2HBluntMult; break;
			case RE::WEAPON_TYPE::kTwoHandSword: stunDamage *= settings::fStunGreatSwordMult; break;
			}
		}
		break;
	}

	damageStun(a_aggressor, a_victim, stunDamage);

	if (safeGet_isStunBroken(a_victim)
		&& !a_victim->IsInKillMove()) {
		reactionHandler::triggerDownedState(a_victim);
	}
}

robin_hood::unordered_set<RE::Actor*> stunHandler::getStunBrokenActors() {
	return stunBrokenActors;
}

void stunHandler::reset() {
	uniqueLocker lock_stunRegenQueue(mtx_StunRegenQueue);
	uniqueLocker lock_actorStunDataMap(mtx_ActorStunDataMap);
	uniqueLocker lock_stunnedActors(mtx_StunBrokenActors);
	stunRegenQueue.clear();
	actorStunDataMap.clear();
	for (auto& actor : stunBrokenActors) {
		TrueHUDUtils::revertSpecialMeter(actor);
	}
	stunBrokenActors.clear();
}

void stunHandler::collectGarbage() {
	logger::info("StunHandler: collecting garbage...");
	int ct = 0;
	{
		uniqueLocker lock_actorStunDataMap(mtx_ActorStunDataMap);
		auto it = actorStunDataMap.begin();
		while (it != actorStunDataMap.end()) {
			auto actor = it->first;
			if (!actor || !actor->currentProcess || !actor->currentProcess->InHighProcess()) {
				safeErase_StunBrokenActors(actor);
				safeErase_StunRegenQueue(actor);
				it = actorStunDataMap.erase(it);
				ct++;
				continue;
			}
			it++;
		}
	}
	logger::info("...done; cleaned up {} inactive actors.", ct);
}
