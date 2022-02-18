#include "ValhallaCombat.hpp"
#include "stunHandler.h"
#include <functional>
#include <iostream>

void stunHandler::initStunMeter() {
	if (ValhallaCombat::GetSingleton()->g_trueHUD->RequestSpecialResourceBarsControl(SKSE::GetPluginHandle()) != TRUEHUD_API::APIResult::AlreadyTaken) {
		INFO("TrueHUD special bar request success.");
		if (
			ValhallaCombat::GetSingleton()->g_trueHUD->RegisterSpecialResourceFunctions(SKSE::GetPluginHandle(), getStun, getMaxStun, false) == TRUEHUD_API::APIResult::OK) {
			INFO("TrueHUD special bar init success.");
		}
	}
}

float stunHandler::getMaxStun(RE::Actor* actor) {
	float stun = (actor->GetPermanentActorValue(RE::ActorValue::kStamina) + actor->GetPermanentActorValue(RE::ActorValue::kHealth)) / 2;
	//DEBUG("Calculated {}'s max stun: {}.", actor->GetName(), stun);
	return stun;
}

float stunHandler::getStun(RE::Actor* actor) {
	DEBUG("Getting {}'s stun.", actor->GetName());
	auto actorStunMap = stunHandler::GetSingleton()->actorStunMap;
	auto it = actorStunMap.find(actor);
	if (it != actorStunMap.end()) {
		return it->second.second;
	}
	else {
		DEBUG("Unable to find actor on the stun map, tracking actor and returning the actor's permanent stun.");
		trackStun(actor);
		return getMaxStun(actor);
	}
	
}

void stunHandler::damageStun(RE::Actor* actor, float damage) {
	DEBUG("Damaging {}'s stun by {} points.", actor->GetName(), damage);
	auto it = actorStunMap.find(actor);
	if (it == actorStunMap.end()) {
		DEBUG("{} not found on the stun map, tracking actor.", actor->GetName());
		trackStun(actor);
		damageStun(actor, damage);
		return;
	}
	it->second.second -= damage;
	DEBUG("{}'s stun damaged to {}", actor->GetName(), it->second.second);
}

void stunHandler::calculateStunDamage(STUNSOURCE stunSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage) {
	switch (stunSource) {
	case STUNSOURCE::parry:
		damageStun(victim, baseDamage * settings::fStunParryMult); break;
	case STUNSOURCE::bash:
		break;
	}
}


/*Bunch of abstracted utilities.*/
#pragma region stunUtils
void stunHandler::trackStun(RE::Actor* actor) {
	float maxStun = getMaxStun(actor);
	stunHandler::GetSingleton()->actorStunMap.emplace(actor, std::pair<float, float>(maxStun, maxStun));
	DEBUG("Start tracking {}'s stun. Max Stun: {}.", actor->GetName(), maxStun);
};
void stunHandler::untrackStun(RE::Actor* actor) {
	stunHandler::GetSingleton()->actorStunMap.erase(actor);
}
void stunHandler::resetStun(RE::Actor* actor) {
	DEBUG("Resetting {}'s stun.", actor->GetName());
	auto it = stunHandler::GetSingleton()->actorStunMap.find(actor);
	if (it != stunHandler::GetSingleton()->actorStunMap.end()) {
		it->second.second = it->second.first;
	}
	else {
		DEBUG("Erorr: {} not found in actor stun map, and thus cannot be reset", actor->GetName());
	}
}
#pragma endregion