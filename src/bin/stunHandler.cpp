#include "ValhallaCombat.hpp"
#include "include/stunHandler.h"
#include "include/hitProcessor.h"
#include "include/settings.h"
#include "include/reactionHandler.h"
#include "include/offsets.h"
#include "include/Utils.h"
using uniqueLocker = std::unique_lock<std::shared_mutex>;
using sharedLocker = std::shared_lock<std::shared_mutex>;

void stunHandler::safeErase_ActorStunDataMap(RE::ActorHandle a_handle) {
	uniqueLocker lock(mtx_ActorStunDataMap);
	actorStunDataMap.erase(a_handle);
}
void stunHandler::safeErase_ActorStunDataMap(RE::Actor* a_actor) {
	safeErase_ActorStunDataMap(a_actor->GetHandle());
}

void stunHandler::safeErase_StunBrokenActors(RE::ActorHandle a_handle) {
	uniqueLocker lock(mtx_StunBrokenActors);
	stunBrokenActors.erase(a_handle);
}
void stunHandler::safeErase_StunBrokenActors(RE::Actor* a_actor) {
	safeErase_StunBrokenActors(a_actor->GetHandle());
}

void stunHandler::safeInsert_StunBrokenActors(RE::ActorHandle a_handle) {
	uniqueLocker lock(mtx_StunBrokenActors);
	stunBrokenActors.insert(a_handle);
}
void stunHandler::safeInsert_StunBrokenActors(RE::Actor* a_actor) {
	safeInsert_StunBrokenActors(a_actor->GetHandle());
}

bool stunHandler::safeGet_isStunBroken(RE::ActorHandle a_handle) {
	sharedLocker lock(mtx_StunBrokenActors);
	auto it = stunBrokenActors.find(a_handle);
	return it != stunBrokenActors.end();
}

bool stunHandler::safeGet_isStunBroken(RE::Actor* a_actor) {
	return safeGet_isStunBroken(a_actor->GetHandle());
}



stunHandler::actorStunData_ptr stunHandler::safeInsert_ActorStunDataMap(RE::ActorHandle a_handle) {
	actorStunData_ptr stunDataPtr(new actorStunData(a_handle));
	uniqueLocker lock(mtx_ActorStunDataMap);
	actorStunDataMap.insert(std::pair<RE::ActorHandle, actorStunData_ptr>(a_handle, stunDataPtr));
	return stunDataPtr;
}

stunHandler::actorStunData_ptr stunHandler::safeInsert_ActorStunDataMap(RE::Actor* a_actor) {
	return safeInsert_ActorStunDataMap(a_actor->GetHandle());
}


stunHandler::actorStunData_ptr stunHandler::safeGet_ActorStunData(RE::ActorHandle a_handle) {
	{
		sharedLocker lock(mtx_ActorStunDataMap);
		auto it = actorStunDataMap.find(a_handle);
		if (it != actorStunDataMap.end()) {
			return it->second;  //return found stundata
		}
	}

	return safeInsert_ActorStunDataMap(a_handle); //otherwise, insert and return inserted ptr
}


stunHandler::actorStunData_ptr stunHandler::safeGet_ActorStunData(RE::Actor* a_actor) {
	return safeGet_ActorStunData(a_actor->GetHandle());
}

stunHandler::actorStunData_ptr stunHandler::trackStun(RE::ActorHandle a_actorHandle){
	return safeInsert_ActorStunDataMap(a_actorHandle);
};

stunHandler::actorStunData_ptr stunHandler::trackStun(RE::Actor* a_actor) {
	return safeInsert_ActorStunDataMap(a_actor);
}


void stunHandler::untrackStun(RE::Actor* a_actor) {
	safeErase_ActorStunDataMap(a_actor);
	safeErase_StunBrokenActors(a_actor);
}

void stunHandler::refillStun(RE::Actor* a_actor) {
	sharedLocker lock(mtx_ActorStunDataMap);
	auto it = actorStunDataMap.find(a_actor->GetHandle());
	if (it == actorStunDataMap.end()) {
		return;
	}
	it->second->refillStun();
}


bool stunHandler::getIsStunBroken(RE::Actor* a_actor) {
	return safeGet_isStunBroken(a_actor);
}

void stunHandler::update() {
	uniqueLocker lock_actorStunDataMap(mtx_ActorStunDataMap);
	if (actorStunDataMap.empty()) {
		ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::stunHandler);
		return;
	}
	auto it = actorStunDataMap.begin();
	while (it != actorStunDataMap.end()) {
		auto& one_handle = it->first;
		if (!one_handle) {
			safeErase_StunBrokenActors(one_handle);

			it = actorStunDataMap.erase(it);
			continue;
		}
		
		auto actor = one_handle.get().get();
		if (!actor) {
			safeErase_StunBrokenActors(one_handle);
			
			it = actorStunDataMap.erase(it);
			continue;
		}
		
		if (actor->IsDead()) {
			safeErase_StunBrokenActors(one_handle);
			TrueHUDUtils::revertSpecialMeter(actor);
			it = actorStunDataMap.erase(it);
			continue;
		}
		auto& stunData = it->second;

		if (stunData->getRegenCountDown() > 0) {
			stunData->modRegenCountDown(-*RE::Offset::g_deltaTime);
			
			it++;
			continue;
		}
			
		if (stunData->canRegen()) {
			if (stunData->regenStun()) {
				stunData->setCombatRegeneration(false);
				TrueHUDUtils::revertSpecialMeter(actor);

				if (settings::bDownedStateToggle) {
					reactionHandler::recoverDownedState(actor);
				}

				safeErase_StunBrokenActors(one_handle);

				it = actorStunDataMap.erase(it);
				continue;
			}
		}
		it++;
		
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
					for (auto& handle : stunBrokenActors) {
						if (!handle) {
							return;
						}
						auto actor = handle.get().get();
						if (actor) {
							TrueHUDUtils::flashActorValue(actor, RE::ActorValue::kHealth);
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

/*Get the max stun of a actor. If the actor is not tracked, return 1.*/
float stunHandler::getMaxStun(RE::Actor* a_actor) {
	sharedLocker lock(mtx_ActorStunDataMap);
	auto it = actorStunDataMap.find(a_actor->GetHandle());
	if (it == actorStunDataMap.end()) {
		return 1;
	} else {
		return it->second->getMaxStun();
	}
}

float stunHandler::getMaxStun_static(RE::Actor* a_actor) {
	return stunHandler::GetSingleton()->getMaxStun(a_actor);
}

/*Get the current stun of a actor. If the actor is not tracked, return 1.*/
float stunHandler::getCurrentStun(RE::Actor* a_actor) {
	sharedLocker lock(mtx_ActorStunDataMap);
	auto it = actorStunDataMap.find(a_actor->GetHandle());
	if (it == actorStunDataMap.end()) {
		return 1;
	} else {
		return it->second->getCurrentStun();
	}
}

float stunHandler::getCurrentStun_static(RE::Actor* a_actor) {
	return stunHandler::GetSingleton()->getCurrentStun(a_actor);
}

void stunHandler::onStunBroken(RE::Actor* a_actor) {
	safeInsert_StunBrokenActors(a_actor);
	async_launchHealthBarFlashThread();
	ValhallaUtils::playSound(a_actor, data::soundStunBreak);
	safeGet_ActorStunData(a_actor)->setCombatRegeneration(true);
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
	ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::stunHandler);
	//DEBUG("damaging done");
}


void stunHandler::processStunDamage(
	STUNSOURCE a_stunSource, RE::TESObjectWEAP* a_weapon, RE::Actor* a_aggressor, RE::Actor* a_victim, float a_baseDamage) {
	if (!settings::bStunToggle) { //stun damage will not be applied with stun turned off.
		return;
	}
	if (a_victim->IsPlayerRef()) { //player do not receive stun damage at all.
		return;
	}
	if (a_victim->IsPlayerTeammate() && !settings::bFriendlyStunToggle) { //player's team do not receive stun damage at all.
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
	if (stunDamage != 0) { //only damage when there is damage.
		damageStun(a_aggressor, a_victim, stunDamage);
	}

	if (settings::bDownedStateToggle) {
		if (safeGet_isStunBroken(a_victim) && !a_victim->IsInKillMove()) {
			reactionHandler::triggerDownedState(a_victim);
		}
	}
}

std::unordered_set<RE::ActorHandle> stunHandler::getStunBrokenActors() {
	return stunBrokenActors;
}

void stunHandler::reset() {
	uniqueLocker lock_actorStunDataMap(mtx_ActorStunDataMap);
	uniqueLocker lock_stunnedActors(mtx_StunBrokenActors);
	actorStunDataMap.clear();
	for (auto& actorHandle : stunBrokenActors) {
		if (actorHandle) {
			auto actor = actorHandle.get().get();
			if (actor) {
				TrueHUDUtils::revertSpecialMeter(actor);
			}
		}
	}
	stunBrokenActors.clear();
}

