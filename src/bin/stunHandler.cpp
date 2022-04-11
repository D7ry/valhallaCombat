#include "ValhallaCombat.hpp"
#include "include/stunHandler.h"
#include "include/hitProcessor.h"
#include "include/settings.h"
#include "include/reactionHandler.h"
#include "include/offsets.h"
#include "include/Utils.h"
void stunHandler::update() {
	//mtx.lock();
	auto deltaTime = *RE::Offset::g_deltaTime;
	mtx_StunRegenQueue.lock();
	auto it_StunRegenQueue = stunRegenQueue.begin();
	while (it_StunRegenQueue != stunRegenQueue.end()) {
		auto actor = it_StunRegenQueue->first;
		if (!actor || !actor->currentProcess) {
			//if an actor is no longer present=>remove actor.
			mtx_ActorStunMap.lock();
			actorStunMap.erase(actor);
			mtx_ActorStunMap.unlock();
			it_StunRegenQueue = stunRegenQueue.erase(it_StunRegenQueue); 
			if (stunRegenQueue.size() == 0) {
				ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::stunHandler);
			}
			continue;
		}


		if (!actor->IsInCombat()//no regular regen during combat.
			|| stunnedActors.contains(actor)) {//however, when in down state, regen.
			if (it_StunRegenQueue->second <= 0) {//timer reached zero, time to regen.
				//start regenerating stun from actorStunMap.
				mtx_ActorStunMap.lock();
				if (actorStunMap.find(actor) == actorStunMap.end()) {//oops, somehow the actor is not found in the stun map.
					mtx_ActorStunMap.unlock(); //unlock mtx here, no longer needed.
					it_StunRegenQueue = stunRegenQueue.erase(it_StunRegenQueue); 
					if (stunRegenQueue.size() == 0) {
						ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::stunHandler);
					}
					continue; 
				}
				else {
					if (actorStunMap.find(actor)->second.second < actorStunMap.find(actor)->second.first) {
						actorStunMap.find(actor)->second.second += 
							deltaTime * 1 / 7 * actorStunMap.find(actor)->second.first;
						mtx_ActorStunMap.unlock();
					}
					else {
						mtx_ActorStunMap.unlock();
						//actor's stun fully regenerated. recovering its downed state and consider the actor no longer stunned.
						it_StunRegenQueue = stunRegenQueue.erase(it_StunRegenQueue);
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
				it_StunRegenQueue->second -= deltaTime;//keep decrementing regen timer.
			}
		}
		++it_StunRegenQueue;
	}
	mtx_StunRegenQueue.unlock();



	if (!stunnedActors.empty()) { //crosshair meter flash
		if (timer_StunMeterFlash <= 0) {
			RE::Actor* crossHairTarget = nullptr;
			if (RE::CrosshairPickData::GetSingleton()
				&& RE::CrosshairPickData::GetSingleton()->targetActor
				&& RE::CrosshairPickData::GetSingleton()->targetActor.get()) {
				crossHairTarget = RE::CrosshairPickData::GetSingleton()->targetActor.get()->As<RE::Actor>();
				//DEBUG("obtained crosshaird target: {}", crossHairTarget->GetName());
				if (stunnedActors.contains(crossHairTarget)) {
					flashHealthBar(crossHairTarget);
				}
			}
			timer_StunMeterFlash = 1;
		}
		else {
			timer_StunMeterFlash -= deltaTime;
		}

	}
	//mtx.unlock();
	//flash special meter for stunned actors, if they're being pointed at.

}



float stunHandler::getMaxStun(RE::Actor* actor) {
	auto actorStunMap = stunHandler::GetSingleton()->actorStunMap;
	mtx_ActorStunMap.lock();
	auto it = actorStunMap.find(actor);
	if (it != actorStunMap.end()) {
		mtx_ActorStunMap.unlock();
		return it->second.first;
	}
	else {
		mtx_ActorStunMap.unlock();
		stunHandler::GetSingleton()->trackStun(actor);
		return getMaxStun(actor);
	}
}

float stunHandler::getStun(RE::Actor* actor) {
	auto actorStunMap = stunHandler::GetSingleton()->actorStunMap;
	mtx_ActorStunMap.lock();
	auto it = actorStunMap.find(actor);
	if (it != actorStunMap.end()) {
		mtx_ActorStunMap.unlock();
		return it->second.second;
	}
	else {
		mtx_ActorStunMap.unlock();
		stunHandler::GetSingleton()->trackStun(actor);
		return getStun(actor);
	}

}

void stunHandler::damageStun(RE::Actor* aggressor, RE::Actor* actor, float damage) {
	//DEBUG("Damaging {}'s stun by {} points.", actor->GetName(), damage);
	mtx_ActorStunMap.lock();
	auto it = actorStunMap.find(actor);
	if (it == actorStunMap.end()) { //actor's stun is not yet tracked.
		mtx_ActorStunMap.unlock();
		trackStun(actor);
		damageStun(aggressor, actor, damage);//recursively call itself, once stun is tracked.
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
			mtx_ActorStunMap.unlock();
			if (!stunnedActors.contains(actor)) {
				ValhallaUtils::playSound(actor, data::GetSingleton()->soundStunBreakD->GetFormID());
				actor->AllowBleedoutDialogue(true);
				actor->AllowPCDialogue(true);
				stunnedActors.insert(actor);
				greyOutStunMeter(actor);
			}
		}
		else {
			mtx_ActorStunMap.unlock();
		}

		if (stunnedActors.contains(actor)
			&& !actor->IsInKillMove()) {
			reactionHandler::triggerDownedState(actor);
		}
		mtx_StunRegenQueue.lock();
		stunRegenQueue[actor] = 3; //3 seconds cooldown to regenerate stun.
		mtx_StunRegenQueue.unlock();
		ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::stunHandler);
	}

}

bool stunHandler::isActorStunned(RE::Actor* a_actor) {
	return stunnedActors.contains(a_actor);
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





/*Bunch of abstracted utilities.*/
#pragma region stunUtils
void stunHandler::trackStun(RE::Actor* actor) {
	float maxStun = calcMaxStun(actor);
	mtx_ActorStunMap.lock();
	actorStunMap.emplace(actor, std::pair<float, float>(maxStun, maxStun));
	mtx_ActorStunMap.unlock();
	//DEBUG("Start tracking {}'s stun. Max Stun: {}.", actor->GetName(), maxStun);
};
void stunHandler::untrackStun(RE::Actor* actor) {
	mtx_ActorStunMap.lock();
	actorStunMap.erase(actor);
	mtx_ActorStunMap.unlock();
}
float stunHandler::calcMaxStun(RE::Actor* actor) {
	return (actor->GetPermanentActorValue(RE::ActorValue::kHealth) + actor->GetPermanentActorValue(RE::ActorValue::kStamina)) / 2;
}
void stunHandler::refillStun(RE::Actor* actor) {
	mtx_ActorStunMap.lock();
	auto it = actorStunMap.find(actor);
	if (it != actorStunMap.end()) {
		it->second.second = it->second.first;
	}
	mtx_ActorStunMap.unlock();
}

void stunHandler::refreshStun() {
	mtx_StunRegenQueue.lock();
	stunRegenQueue.clear();
	mtx_StunRegenQueue.unlock();
	mtx_ActorStunMap.lock();
	actorStunMap.clear();
	mtx_ActorStunMap.unlock();
}
void stunHandler::cleanUpStunMap() {
	INFO("Cleaning up stun map...");
	mtx_ActorStunMap.lock();
	auto it = actorStunMap.begin();
	while (it != actorStunMap.end()) {
		auto actor = it->first;
		if (!actor || !actor->currentProcess || !actor->currentProcess->InHighProcess()) {
			it = actorStunMap.erase(it); continue;
		}
		it++;
	}
	mtx_ActorStunMap.unlock();
	INFO("...done");
}

void stunHandler::stunMapCleanUpTask() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::minutes(5));
		stunHandler::GetSingleton()->cleanUpStunMap();
	}
}
void stunHandler::launchStunMapCleaner() {
	std::jthread cleanUpThread(stunMapCleanUpTask);
	cleanUpThread.detach();
}


void stunHandler::greyOutStunMeter(RE::Actor* a_actor) {
	auto ersh = ValhallaCombat::GetSingleton()->g_trueHUD;
	ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
	ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::BarColor, 0x7d7e7d);
	ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::PhantomColor, 0xb30d10);
	ersh->OverrideBarColor(a_actor->GetHandle(), RE::ActorValue::kHealth, TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
}

void stunHandler::revertStunMeter(RE::Actor* a_actor) {
	auto ersh = ValhallaCombat::GetSingleton()->g_trueHUD;
	ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::FlashColor);
	ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::BarColor);
	ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::PhantomColor);
	ersh->RevertBarColor(a_actor->GetHandle(), RE::ActorValue::kHealth, TRUEHUD_API::BarColorType::FlashColor);
}

void stunHandler::flashHealthBar(RE::Actor* a_actor) {
	auto ersh = ValhallaCombat::GetSingleton()->g_trueHUD;
	ersh->FlashActorValue(a_actor->GetHandle(), RE::ActorValue::kHealth, true);
	//ersh->FlashActorSpecialBar(SKSE::GetPluginHandle(), a_actor->GetHandle(), true);
}
#pragma endregion