#include "ValhallaCombat.hpp"
#include "include/stunHandler.h"
#include "include/hitProcessor.h"
#include "include/settings.h"
#include "include/reactionHandler.h"
#include "include/offsets.h"
#include "include/Utils.h"
void stunHandler::update() {
	mtx.lock();
	auto deltaTime = *RE::Offset::g_deltaTime;
	auto it = stunRegenQueue.begin();
	while (it != stunRegenQueue.end()) {
		auto actor = it->first;
		if (!actor || !actor->currentProcess) {
			//if an actor is no longer present=>remove actor.
			actorStunMap.erase(actor);
			it = stunRegenQueue.erase(it); 
			if (stunRegenQueue.size() == 0) {
				ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::stunHandler);
			}
			continue;
		}
		if (!actor->IsInCombat() ||//no regular regen during combat.
			stunnedActors.contains(actor)) {//however, when in down state, regen.
			if (it->second <= 0) {//timer reached zero, time to regen.
				//start regenerating stun from actorStunMap.
				if (actorStunMap.find(actor) == actorStunMap.end()) {//oops, somehow the actor is not found in the stun map.
					it = stunRegenQueue.erase(it); 
					if (stunRegenQueue.size() == 0) {
						ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::stunHandler);
					}
					continue; 
				}
				else {
					if (actorStunMap.find(actor)->second.second < actorStunMap.find(actor)->second.first) {
						actorStunMap.find(actor)->second.second += 
							deltaTime * 1 / 5 * actorStunMap.find(actor)->second.first;
					}
					else {
						//actor's stun fully regenerated. recovering its downed state and consider the actor no longer stunned.
						it = stunRegenQueue.erase(it);
						if (stunnedActors.contains(actor)) {
							stunnedActors.erase(actor);
							revertStunMeter(actor);
							reactionHandler::recoverDownedState(actor);
						}

						if (stunRegenQueue.size() == 0) {
							ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::stunHandler);
						}
						continue; //regeneration complete.
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

void stunHandler::damageStun(RE::Actor* aggressor, RE::Actor* actor, float damage) {
	//DEBUG("Damaging {}'s stun by {} points.", actor->GetName(), damage);
	auto it = actorStunMap.find(actor);
	if (it == actorStunMap.end()) { //actor's stun is not yet tracked.
		trackStun(actor);
		damageStun(aggressor, actor, damage);
	}
	else {
		//prevent stun from getting below 0
		if (it->second.second - damage <= 0) {
			it->second.second = 0;
		}
		else {
			it->second.second -= damage;
		}
		//actor has 0 stun
		if (it->second.second <= 0) {
			if (!stunnedActors.contains(actor)) {
				ValhallaUtils::playSound(actor, data::GetSingleton()->soundStunBreakD->GetFormID());
				stunnedActors.insert(actor);
				greyOutStunMeter(actor);
			}
			reactionHandler::triggerDownedState(actor);
		}
		mtx.lock();
		stunRegenQueue[actor] = 3; //3 seconds cooldown to regenerate stun.
		mtx.unlock();
		ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::stunHandler);
	}

}

void stunHandler::knockDown(RE::Actor* aggressor, RE::Actor* victim) {

}

void stunHandler::calculateStunDamage(
	STUNSOURCE stunSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage) {
	DEBUG("Calculating stun damage");
	if (victim->IsPlayerRef()) { //player do not receive stun damage at all.
		return;
	}
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
	case STUNSOURCE::lightAttack:
		stunDamage = baseDamage * settings::fStunNormalAttackMult;
		if (!weapon) {
			stunDamage *= settings::fStunUnarmedMult;
		}
		else {
			switch (weapon->GetWeaponType()) {
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
		stunDamage = baseDamage * settings::fStunPowerAttackMult;
		if (!weapon) {
			stunDamage *= settings::fStunUnarmedMult;
		}
		else {
			switch (weapon->GetWeaponType()) {
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

	damageStun(aggressor, victim, stunDamage);
}

void stunHandler::houseKeeping() { //no longer used
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


void stunHandler::greyOutStunMeter(RE::Actor* a_actor) {
	auto ersh = ValhallaCombat::GetSingleton()->g_trueHUD;
	ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
	ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::BarColor, 0x7d7e7d);
	ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::PhantomColor, 0xb30d10);
}

void stunHandler::revertStunMeter(RE::Actor* a_actor) {
	auto ersh = ValhallaCombat::GetSingleton()->g_trueHUD;
	ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::FlashColor);
	ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::BarColor);
	ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::PhantomColor);
}
#pragma endregion