#include "ValhallaCombat.hpp"
#include "include/stunHandler.h"
#include "include/hitProcessor.h"
void stunHandler::update() {
	mtx.lock();
	auto deltaTime = *Utils::g_deltaTimeRealTime;
	auto it = stunRegenQueue.begin();
	while (it != stunRegenQueue.end()) {
		auto actor = it->first;
		if (!actor || !actor->currentProcess || !actor->currentProcess->InHighProcess()) {
			//if an actor is no longer present=>remove actor.
			actorStunMap.erase(actor);
			it = stunRegenQueue.erase(it); continue;
		}
		if (!actor->IsInCombat()) {//no regen during combat.
			if (it->second <= 0) {//timer reached zero, time to regen.
				//start regenerating stun from actorStunMap.
				if (actorStunMap.find(actor) == actorStunMap.end()) {//oops, somehow the actor is not found in the stun map.
					it = stunRegenQueue.erase(it); continue; 
				}
				else {
					if (actorStunMap.find(actor)->second.second < actorStunMap.find(actor)->second.first) {
						actorStunMap.find(actor)->second.second += 
							deltaTime * 1 / 10 * actorStunMap.find(actor)->second.first;
					}
					else {
						it = stunRegenQueue.erase(it); continue; //regeneration complete.
					}
				}
			}
			else {
				it->second -= deltaTime;//keep decrementing regen timer.
			}
		}
		++it;
	}
	mtx.unlock();
}



float stunHandler::getMaxStun(RE::Actor* actor) {
	auto actorStunMap = stunHandler::GetSingleton()->actorStunMap;
	auto it = actorStunMap.find(actor);
	if (it != actorStunMap.end()) {
		return it->second.first;
	}
	else {
		stunHandler::GetSingleton()->trackStun(actor);
		return getMaxStun(actor);
	}
}

float stunHandler::getStun(RE::Actor* actor) {
	auto actorStunMap = stunHandler::GetSingleton()->actorStunMap;
	auto it = actorStunMap.find(actor);
	if (it != actorStunMap.end()) {
		return it->second.second;
	}
	else {
		stunHandler::GetSingleton()->trackStun(actor);
		return getStun(actor);
	}

}

void stunHandler::damageStun(RE::Actor* actor, float damage) {
	//DEBUG("Damaging {}'s stun by {} points.", actor->GetName(), damage);
	auto it = actorStunMap.find(actor);
	if (it == actorStunMap.end()) { //actor's stun is not yet tracked.
		trackStun(actor);
		damageStun(actor, damage);
	}
	else {
		it->second.second -= damage;
		mtx.lock();
		stunRegenQueue.emplace(actor, 3); //3 seconds cooldown to regenerate stun.
		mtx.unlock();
	}

}

void stunHandler::knockDown(RE::Actor* aggressor, RE::Actor* victim) {

}

void stunHandler::calculateStunDamage(
	STUNSOURCE stunSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage) {
	DEBUG("Calculating stun damage");
	//TODO:see if I can optimize it a bit more
	if (!settings::bStunToggle) { //stun damage will not be applied with stun turned off.
		return;
	}
	float stunDamage;
	switch (stunSource) {
	case STUNSOURCE::parry:
		stunDamage = baseDamage * settings::fStunParryMult;
		break;
	case STUNSOURCE::bash:
		stunDamage = aggressor->GetActorValue(RE::ActorValue::kBlock) * settings::fStunBashMult;
		if (aggressor->IsPlayerRef()) {
			Utils::offsetRealDamage(stunDamage, true);
		}
		if (victim->IsPlayerRef()) {
			Utils::offsetRealDamage(stunDamage, false);
		}
		break;
	case STUNSOURCE::powerBash:
		stunDamage = aggressor->GetActorValue(RE::ActorValue::kBlock) * settings::fStunPowerBashMult;
		if (aggressor->IsPlayerRef()) {
			Utils::offsetRealDamage(stunDamage, true);
		}
		if (victim->IsPlayerRef()) {
			Utils::offsetRealDamage(stunDamage, false);
		}
		break;
	case STUNSOURCE::powerAttack:
		baseDamage *= settings::fStunPowerAttackMult;
	case STUNSOURCE::lightAttack:
		if (!weapon) {
			stunDamage = baseDamage * settings::fStunUnarmedMult;
		}
		else {
			switch (weapon->GetWeaponType()) {
			case RE::WEAPON_TYPE::kHandToHandMelee: stunDamage = baseDamage * settings::fStunUnarmedMult; break;
			case RE::WEAPON_TYPE::kOneHandDagger: stunDamage = baseDamage * settings::fStunDaggerMult; break;
			case RE::WEAPON_TYPE::kOneHandSword: stunDamage = baseDamage * settings::fStunSwordMult; break;
			case RE::WEAPON_TYPE::kOneHandAxe: stunDamage = baseDamage * settings::fStunWarAxeMult; break;
			case RE::WEAPON_TYPE::kOneHandMace: stunDamage = baseDamage * settings::fStunMaceMult; break;
			case RE::WEAPON_TYPE::kTwoHandAxe: stunDamage = baseDamage * settings::fStun2HBluntMult; break;
			case RE::WEAPON_TYPE::kTwoHandSword: stunDamage = baseDamage * settings::fStunGreatSwordMult; break;
			}
		}
		break;
	}
	damageStun(victim, stunDamage);
}

void stunHandler::houseKeeping() {
	mtx.lock();
	stunRegenQueue.clear();
	auto it = actorStunMap.begin();
	while (it != actorStunMap.end()) {
		auto actor = it->first;
		if (!actor || !actor->currentProcess || !actor->currentProcess->InHighProcess()) {
			it = actorStunMap.erase(it); continue;
		}
		auto newMax = calcMaxStun(actor); 		
		it->second.first = newMax; //refresh max stun.
		if (newMax > it->second.second) {
			stunRegenQueue.emplace(actor, 0); //if the new max is bigger, start regenerating
		}
		else {
			it->second.second = newMax; //if the new max is smaller/equal, lower the og value
		}
		it++;
	}
	mtx.unlock();
}



/*Bunch of abstracted utilities.*/
#pragma region stunUtils
void stunHandler::trackStun(RE::Actor* actor) {
	float maxStun = calcMaxStun(actor);
	mtx.lock();
	actorStunMap.emplace(actor, std::pair<float, float>(maxStun, maxStun));
	mtx.unlock();
	DEBUG("Start tracking {}'s stun. Max Stun: {}.", actor->GetName(), maxStun);
};
void stunHandler::untrackStun(RE::Actor* actor) {
	mtx.lock();
	actorStunMap.erase(actor);
	mtx.unlock();
}
float stunHandler::calcMaxStun(RE::Actor* actor) {
	return (actor->GetPermanentActorValue(RE::ActorValue::kHealth) + actor->GetPermanentActorValue(RE::ActorValue::kStamina)) / 2;
}
void stunHandler::refillStun(RE::Actor* actor) {
	mtx.lock();
	auto it = actorStunMap.find(actor);
	if (it != actorStunMap.end()) {
		it->second.second = it->second.first;
	}
	mtx.unlock();
}

void stunHandler::refreshStun() {
	mtx.lock();
	stunRegenQueue.clear();
	actorStunMap.clear();
	mtx.unlock();
}

void stunHandler::initTrueHUDStunMeter() {
	auto result = ValhallaCombat::GetSingleton()->g_trueHUD->RequestSpecialResourceBarsControl(SKSE::GetPluginHandle());
	if (result != TRUEHUD_API::APIResult::AlreadyTaken || result != TRUEHUD_API::APIResult::AlreadyGiven) {
		INFO("TrueHUD special bar request success.");
		if (
			ValhallaCombat::GetSingleton()->g_trueHUD
			->RegisterSpecialResourceFunctions(SKSE::GetPluginHandle(), getStun, getMaxStun, true) == TRUEHUD_API::APIResult::OK) {
			INFO("TrueHUD special bar init success.");
		}
	}
}

void stunHandler::releaseTrueHUDStunMeter() {
	if (ValhallaCombat::GetSingleton()->g_trueHUD
		->ReleaseSpecialResourceBarControl(SKSE::GetPluginHandle()) == TRUEHUD_API::APIResult::OK) {
		INFO("TrueHUD special bar release success.");
	}
}
#pragma endregion