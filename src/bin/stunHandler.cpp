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
void stunHandler::safeErase_StunnedActors(RE::Actor* a_actor) {
	uniqueLocker lock(mtx_StunnedActors);
	stunnedActors.erase(a_actor);
}
void stunHandler::safeErase_StunRegenQueue(RE::Actor* a_actor) {
	uniqueLocker lock(mtx_StunRegenQueue);
	stunRegenQueue.erase(a_actor);
}

void stunHandler::safeInsert_StunRegenQueue(RE::Actor* a_actor) {
	uniqueLocker lock(mtx_StunRegenQueue);
	stunRegenQueue[a_actor] = 3;
}

void stunHandler::safeInsert_StunnedActors(RE::Actor* a_actor) {
	uniqueLocker lock(mtx_StunnedActors);
	if (stunnedActors.contains(a_actor)) {
		stunnedActors.insert(a_actor);
	}
}

bool stunHandler::safeGet_isStunned(RE::Actor* a_actor) {
	sharedLocker(mtx_StunnedActors);
	return stunnedActors.contains(a_actor);
}

std::shared_ptr<stunHandler::actorStunData> stunHandler::safeGet_ActorStunData(RE::Actor* a_actor) {
	sharedLocker lock(mtx_ActorStunDataMap);
	if (actorStunDataMap.contains(a_actor)) {
		return actorStunDataMap[a_actor];
	}
	else {
		lock.unlock();
		return trackStun(a_actor);
	}
}

std::shared_ptr<stunHandler::actorStunData> stunHandler::trackStun(RE::Actor* a_actor) {
	uniqueLocker lock(mtx_ActorStunDataMap);
	if (actorStunDataMap.contains(a_actor)) {//check if the actor's stun is already tracked.
		return actorStunDataMap[a_actor];
	}

	std::shared_ptr<actorStunData> one_StunData(new actorStunData(a_actor));
	actorStunDataMap.emplace(a_actor, one_StunData);
	return one_StunData;
};
void stunHandler::untrackStun(RE::Actor* a_actor) {
	safeErase_ActorStunDataMap(a_actor);
	safeErase_StunnedActors(a_actor);
	safeErase_StunRegenQueue(a_actor);
}

void stunHandler::refillStun(RE::Actor* a_actor) {
	sharedLocker lock(mtx_ActorStunDataMap);
	if (!actorStunDataMap.contains(a_actor)) {
		return;
	}
	actorStunDataMap[a_actor]->refillStun();
}


bool stunHandler::getIsStunned(RE::Actor* a_actor) {
	return safeGet_isStunned(a_actor);
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
			safeErase_StunnedActors(one_actor);
			it_StunRegenQueue = stunRegenQueue.erase(it_StunRegenQueue); 
			continue;
		}

		/*---------------------------Regen area-------------------------------------*/
		if (!one_actor->IsInCombat()
			|| safeGet_isStunned(one_actor)) {

			//>>>>>>>>actor that can regen
			if (it_StunRegenQueue->second <= 0) {//timer reached zero, time to regen.
				bool isStunFullRegenerated = false;
				{//local scope for shared actorStunDataMap access
					sharedLocker lock_actorStunDataMap(mtx_ActorStunDataMap);
					if (!actorStunDataMap.contains(one_actor)) {//edge case: actorStunMap no longer contains this actor.
						isStunFullRegenerated = true;
					}
					else {
						isStunFullRegenerated = actorStunDataMap[one_actor]->regenStun();
					}
				}

				if (isStunFullRegenerated) {
					TrueHUDUtils::revertSpecialMeter(one_actor);
					reactionHandler::recoverDownedState(one_actor);
					safeErase_StunnedActors(one_actor);
					it_StunRegenQueue = stunRegenQueue.erase(it_StunRegenQueue);
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
				sharedLocker lock(mtx_StunnedActors);
				if (stunnedActors.empty()) {
					async_HealthBarFlashThreadActive = false;//deactivate flash thread.
					return;
				}
				else {
					for (auto& one_actor : stunnedActors) {
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
}


float stunHandler::getMaxStun(RE::Actor* a_actor) {
	{
		sharedLocker lock(mtx_ActorStunDataMap);
		if (actorStunDataMap.contains(a_actor)) {
			return actorStunDataMap[a_actor]->getCurrentStun();
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

void stunHandler::damageStun(RE::Actor* a_aggressor, RE::Actor* a_victim, float a_damage) {
	//DEBUG("Damaging {}'s stun by {} points.", actor->GetName(), damage);	
	//actor has 0 stun
	if (safeGet_ActorStunData(a_victim)->damageStun(a_damage)) {
		if (!safeGet_isStunned(a_victim)) {//the current damage depletes actor stun, but actor's stunned state is not yet tracked.
			ValhallaUtils::playSound(a_victim, data::soundStunBreak);
			safeInsert_StunnedActors(a_victim);
			async_launchHealthBarFlashThread();
			//gray out this actor's stun meter.
			if (settings::bStunMeterToggle && settings::TrueHudAPI_HasSpecialBarControl) {
				TrueHUDUtils::greyOutSpecialMeter(a_victim);
			}
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
	float stunDamage;
	switch (a_stunSource) {
	case STUNSOURCE::parry:
		stunDamage = a_baseDamage * settings::fStunParryMult;
		break;
	case STUNSOURCE::bash:
		stunDamage = a_aggressor->GetActorValue(RE::ActorValue::kBlock) * settings::fStunBashMult;
		Utils::offsetRealDamage(stunDamage, a_aggressor, a_victim);
		break;
	case STUNSOURCE::powerBash:
		stunDamage = a_aggressor->GetActorValue(RE::ActorValue::kBlock) * settings::fStunPowerBashMult;
		Utils::offsetRealDamage(stunDamage, a_aggressor, a_victim);
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

	if (safeGet_isStunned(a_victim)
		&& !a_victim->IsInKillMove()) {
		reactionHandler::triggerDownedState(a_victim);
	}
}

void stunHandler::reset() {
	uniqueLocker lock_stunRegenQueue(mtx_StunRegenQueue);
	uniqueLocker lock_actorStunDataMap(mtx_ActorStunDataMap);
	uniqueLocker lock_stunnedActors(mtx_StunnedActors);
	stunRegenQueue.clear();
	actorStunMap.clear();
	for (auto& actor : stunnedActors) {
		TrueHUDUtils::revertSpecialMeter(actor);
	}
	stunnedActors.clear();
}

void stunHandler::collectGarbage() {
	INFO("Cleaning up stun map...");
	int ct = 0;
	{
		uniqueLocker lock_actorStunDataMap(mtx_ActorStunDataMap);
		auto it = actorStunDataMap.begin();
		while (it != actorStunDataMap.end()) {
			auto actor = it->first;
			if (!actor || !actor->currentProcess || !actor->currentProcess->InHighProcess()) {
				safeErase_StunnedActors(actor);
				safeErase_StunRegenQueue(actor);
				it = actorStunDataMap.erase(it);
				ct++;
				continue;
			}
			it++;
		}
	}
	INFO("...done; cleaned up {} inactive actors.", ct);
}
