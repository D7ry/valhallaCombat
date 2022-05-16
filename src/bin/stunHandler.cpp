#include "ValhallaCombat.hpp"
#include "include/stunHandler.h"
#include "include/hitProcessor.h"
#include "include/settings.h"
#include "include/reactionHandler.h"
#include "include/offsets.h"
#include "include/Utils.h"
void stunHandler::safeErase_ActorStunMap(RE::Actor* a_actor) {
	mtx_ActorStunMap.lock();
	actorStunMap.erase(a_actor);
	mtx_ActorStunMap.unlock();
}
void stunHandler::safeErase_StunnedActors(RE::Actor* a_actor) {
	mtx_StunnedActors.lock();
	stunnedActors.erase(a_actor);
	mtx_StunnedActors.unlock();
}
void stunHandler::safeErase_StunRegenQueue(RE::Actor* a_actor) {
	mtx_StunRegenQueue.lock();
	stunRegenQueue.erase(a_actor);
	mtx_StunRegenQueue.unlock();
}
void stunHandler::update() {

	if (garbageCollectionQueued) {
		collectGarbage();
		garbageCollectionQueued = false;
	}

	mtx_StunRegenQueue.lock();
	//stop update if there is nothing in the queue.
	if (stunRegenQueue.empty()) {
		mtx_StunRegenQueue.unlock();
		ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::stunHandler);
		async_HealthBarFlash_b = false;
		return;
	}

	auto it_StunRegenQueue = stunRegenQueue.begin();
	while (it_StunRegenQueue != stunRegenQueue.end()) {
		auto actor = it_StunRegenQueue->first;
		if (!actor || !actor->currentProcess || !actor->currentProcess->InHighProcess()) {//edge case: actor not loaded or no longer in high process.
			//if an actor is no longer present=>remove actor.
			safeErase_ActorStunMap(actor);
			safeErase_StunnedActors(actor);
			it_StunRegenQueue = stunRegenQueue.erase(it_StunRegenQueue); 
			continue;
		}

		/*---------------------------Regen area-------------------------------------*/
		if (!actor->IsInCombat()
			|| stunnedActors.contains(actor)) {

			//>>>>>>>>actor that can regen
			if (it_StunRegenQueue->second <= 0) {//timer reached zero, time to regen.
				//>>>>>>>>>>actor whose regen timer has reached zero.
				mtx_ActorStunMap.lock();
				if (!actorStunMap.contains(actor)) {//edge case: actorStunMap no longer contains this actor.
					mtx_ActorStunMap.unlock(); //unlock mtx here, no longer needed.
					safeErase_StunnedActors(actor);
					it_StunRegenQueue = stunRegenQueue.erase(it_StunRegenQueue); 
					continue; 
				}
				//>>>>>>>>>>>actually start regenerating stun.
				auto* stunData = &actorStunMap.find(actor)->second;
				float regenVal = stunData->first * *RE::Offset::g_deltaTime * 1 / 7;
				if (stunData->second + regenVal >= stunData->first) {//curren stun + regen exceeds max stun, meaning regen has complete.
					//>>>>>>>>actor has finished regen.
					stunData->second = stunData->first;
					mtx_ActorStunMap.unlock();
					it_StunRegenQueue = stunRegenQueue.erase(it_StunRegenQueue);
					
					//if the is currently stunned, actor will no longer be stunned, 
					//their stun meter no longer flash, and they are recovered from downed state.
					mtx_StunnedActors.lock();
					if (stunnedActors.contains(actor)) {
						stunnedActors.erase(actor);
						if (settings::bStunMeterToggle && settings::TrueHudAPI_HasSpecialBarControl) {
							TrueHUDUtils::revertSpecialMeter(actor);
						}
						reactionHandler::recoverDownedState(actor);
					}
					mtx_StunnedActors.unlock();
					continue; //regeneration complete.
				}
				else {
					//>>>>>>>>>perform a simple regen.
					stunData->second += regenVal;
					mtx_ActorStunMap.unlock();
				}

			}
			else {
				it_StunRegenQueue->second -= *RE::Offset::g_deltaTime;//keep decrementing regen timer.
			}
		}
		/*---------------------------Regen area-------------------------------------*/

		++it_StunRegenQueue;
	}
	mtx_StunRegenQueue.unlock();


}

void stunHandler::queueGarbageCollection() {
	garbageCollectionQueued = true;
}

void stunHandler::async_HealthBarFlash() {
	while (true) {
		if (async_HealthBarFlash_b) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			auto ersh = ValhallaCombat::GetSingleton()->ersh;
			auto temp_set = stunHandler::GetSingleton()->stunnedActors;
			for (auto a_actor : temp_set) {
				if (!a_actor->IsDead()) {
					ersh->FlashActorValue(a_actor->GetHandle(), RE::ActorValue::kHealth, false);
				}
				else {
					//erase this actor if dead, so the actor does not need to be iterated over again.
					stunHandler::GetSingleton()->safeErase_StunnedActors(a_actor);
				}
			}
		}
		else {
			return;
		}
	}
}

float stunHandler::getMaxStun(RE::Actor* a_actor) {
	if (a_actor->IsDead()) {
		return 0;
	}
	auto temp_ActorStunMap = stunHandler::GetSingleton()->actorStunMap;
	auto it = temp_ActorStunMap.find(a_actor);
	if (it != temp_ActorStunMap.end()) {
		return it->second.first;
	}
	else {
		stunHandler::GetSingleton()->trackStun(a_actor);
		return getMaxStun(a_actor);
	}
}

float stunHandler::getStun(RE::Actor* a_actor) {
	if (a_actor->IsDead()) {
		return 0;
	}
	auto temp_ActorStunMap = stunHandler::GetSingleton()->actorStunMap;
	auto it = temp_ActorStunMap.find(a_actor);
	if (it != temp_ActorStunMap.end()) {
		return it->second.second;
	}
	else {
		stunHandler::GetSingleton()->trackStun(a_actor);
		return getStun(a_actor);
	}

}

void stunHandler::damageStun(RE::Actor* a_aggressor, RE::Actor* a_victim, float a_damage) {
	//DEBUG("Damaging {}'s stun by {} points.", actor->GetName(), damage);
	mtx_ActorStunMap.lock();
	if (!actorStunMap.contains(a_victim)) {
		mtx_ActorStunMap.unlock();
		trackStun(a_victim);
		damageStun(a_aggressor, a_victim, a_damage);//recursively call itself, once stun is tracked.
		return;
	}
	std::pair<float, float>* actorStunData = &actorStunMap.find(a_victim)->second;
	//prevent stun from getting below 0
	if (actorStunData->second - a_damage <= 0) {
		actorStunData->second = 0;
	}
	else {
		actorStunData->second -= a_damage;
	}

	//actor has 0 stun
	if (actorStunData->second <= 0) {
		mtx_ActorStunMap.unlock();
		if (!stunnedActors.contains(a_victim)) {
			ValhallaUtils::playSound(a_victim, data::soundStunBreak);
			stunnedActors.insert(a_victim);
			//launch health bar flash thread if not done so.
			if (!async_HealthBarFlash_b) {
				std::jthread stunMeterFlashThread(async_HealthBarFlash);
				stunMeterFlashThread.detach();
				async_HealthBarFlash_b = true;
			}
			//gray out this actor's stun meter.
			if (settings::bStunMeterToggle && settings::TrueHudAPI_HasSpecialBarControl) {
				TrueHUDUtils::greyOutSpecialMeter(a_victim);
			}
		}
	}
	else {
		mtx_ActorStunMap.unlock();
	}

	if (stunnedActors.contains(a_victim)
		&& !a_victim->IsInKillMove()) {
		reactionHandler::triggerDownedState(a_victim);
	}
	mtx_StunRegenQueue.lock();
	stunRegenQueue[a_victim] = 3; //3 seconds cooldown to regenerate stun.
	mtx_StunRegenQueue.unlock();
	ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::stunHandler);
	//DEBUG("damaging done");
}

void stunHandler::isActorStunned(RE::Actor* a_actor, bool& isStunned) {
	mtx_StunnedActors.lock();
	isStunned = stunnedActors.contains(a_actor);
	mtx_StunnedActors.unlock();
}

bool stunHandler::isActorStunned(RE::Actor* a_actor) {
	bool isActorStunned;
	mtx_StunnedActors.lock();
	isActorStunned = stunnedActors.contains(a_actor);
	mtx_StunnedActors.unlock();
	return isActorStunned;
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
}





/*Bunch of abstracted utilities.*/
#pragma region stunUtils
void stunHandler::trackStun(RE::Actor* a_actor) {
	float maxStun = calcMaxStun(a_actor);
	mtx_ActorStunMap.lock();
	actorStunMap.emplace(a_actor, std::pair<float, float>(maxStun, maxStun));
	mtx_ActorStunMap.unlock();
};
void stunHandler::untrackStun(RE::Actor* a_actor) {
	safeErase_ActorStunMap(a_actor);
	safeErase_StunnedActors(a_actor);
	safeErase_StunRegenQueue(a_actor);
}
float stunHandler::calcMaxStun(RE::Actor* a_actor) {
	return (a_actor->GetPermanentActorValue(RE::ActorValue::kHealth) + a_actor->GetPermanentActorValue(RE::ActorValue::kStamina)) / 2;
}
void stunHandler::refillStun(RE::Actor* a_actor) {
	mtx_ActorStunMap.lock();
	auto it = actorStunMap.find(a_actor);
	if (it != actorStunMap.end()) {
		it->second.second = it->second.first;
	}
	mtx_ActorStunMap.unlock();
}

void stunHandler::reset() {
	mtx_StunRegenQueue.lock();
	stunRegenQueue.clear();
	mtx_StunRegenQueue.unlock();
	mtx_ActorStunMap.lock();
	actorStunMap.clear();
	mtx_ActorStunMap.unlock();
	mtx_StunnedActors.lock();
	for (auto actor : stunnedActors) {
		TrueHUDUtils::revertSpecialMeter(actor);
	}
	stunnedActors.clear();
	mtx_StunnedActors.unlock();
}
void stunHandler::collectGarbage() {
	INFO("Cleaning up stun map...");
	int ct = 0;
	mtx_ActorStunMap.lock();
	auto it = actorStunMap.begin();
	while (it != actorStunMap.end()) {
		auto actor = it->first;
		if (!actor || !actor->currentProcess || !actor->currentProcess->InHighProcess()) {
			safeErase_StunnedActors(actor);
			safeErase_StunRegenQueue(actor);
			it = actorStunMap.erase(it);
			ct++;
			continue;
		}
		it++;
	}
	mtx_ActorStunMap.unlock();
	INFO("...done; cleaned up {} inactive actors.", ct);
}


void stunHandler::flashHealthBar(RE::Actor* a_actor) {
	ValhallaCombat::GetSingleton()->ersh->FlashActorValue(a_actor->GetHandle(), RE::ActorValue::kHealth, false);
	//ersh->FlashActorSpecialBar(SKSE::GetPluginHandle(), a_actor->GetHandle(), true);
}
#pragma endregion