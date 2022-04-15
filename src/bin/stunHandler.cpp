#include "ValhallaCombat.hpp"
#include "include/stunHandler.h"
#include "include/hitProcessor.h"
#include "include/settings.h"
#include "include/reactionHandler.h"
#include "include/offsets.h"
#include "include/Utils.h"
void stunHandler::safeErase_ActorStunMap(RE::Actor* actor) {
	mtx_ActorStunMap.lock();
	actorStunMap.erase(actor);
	mtx_ActorStunMap.unlock();
}
void stunHandler::safeErase_StunnedActors(RE::Actor* actor) {
	mtx_StunnedActors.lock();
	stunnedActors.erase(actor);
	mtx_StunnedActors.unlock();
}
void stunHandler::safeErase_StunRegenQueue(RE::Actor* actor) {
	mtx_StunRegenQueue.lock();
	stunRegenQueue.erase(actor);
	mtx_StunRegenQueue.unlock();
}
void stunHandler::update() {
	//mtx.lock();
	auto deltaTime = *RE::Offset::g_deltaTime;
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
#define a_stunData actorStunMap.find(actor)->second
				float a_regenVal = a_stunData.first * deltaTime * 1 / 7;
				if (a_stunData.second + a_regenVal >= a_stunData.first) {//curren stun + regen exceeds max stun, meaning regen has complete.
					//>>>>>>>>actor has finished regen.
					a_stunData.second = a_stunData.first;
					mtx_ActorStunMap.unlock();
					it_StunRegenQueue = stunRegenQueue.erase(it_StunRegenQueue);
					
					//if the is currently stunned, actor will no longer be stunned, 
					//their stun meter no longer flash, and they are recovered from downed state.
					mtx_StunnedActors.lock();
					if (stunnedActors.contains(actor)) {
						stunnedActors.erase(actor);
						if (settings::bStunMeterToggle && settings::TrueHudAPI_HasSpecialBarControl) {
							revertStunMeter(actor);
						}
						reactionHandler::recoverDownedState(actor);
					}
					mtx_StunnedActors.unlock();
					continue; //regeneration complete.
				}
				else {
					//>>>>>>>>>perform a simple regen.
					a_stunData.second += a_regenVal;
					mtx_ActorStunMap.unlock();
				}

			}
			else {
				it_StunRegenQueue->second -= deltaTime;//keep decrementing regen timer.
			}
		}
		/*---------------------------Regen area-------------------------------------*/

		++it_StunRegenQueue;
	}
	mtx_StunRegenQueue.unlock();
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

float stunHandler::getMaxStun(RE::Actor* actor) {
	auto temp_ActorStunMap = stunHandler::GetSingleton()->actorStunMap;
	auto it = temp_ActorStunMap.find(actor);
	if (it != temp_ActorStunMap.end()) {
		return it->second.first;
	}
	else {
		stunHandler::GetSingleton()->trackStun(actor);
		return getMaxStun(actor);
	}
}

float stunHandler::getStun(RE::Actor* actor) {
	auto temp_ActorStunMap = stunHandler::GetSingleton()->actorStunMap;
	auto it = temp_ActorStunMap.find(actor);
	if (it != temp_ActorStunMap.end()) {
		return it->second.second;
	}
	else {
		stunHandler::GetSingleton()->trackStun(actor);
		return getStun(actor);
	}

}

void stunHandler::damageStun(RE::Actor* aggressor, RE::Actor* actor, float damage) {
	//DEBUG("Damaging {}'s stun by {} points.", actor->GetName(), damage);
	mtx_ActorStunMap.lock();
	if (!actorStunMap.contains(actor)) {
		mtx_ActorStunMap.unlock();
		trackStun(actor);
		damageStun(aggressor, actor, damage);//recursively call itself, once stun is tracked.
		return;
	}
	auto it = actorStunMap.find(actor);
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
			stunnedActors.insert(actor);
			//launch health bar flash thread if not done so.
			if (!async_HealthBarFlash_b) {
				std::jthread stunMeterFlashThread(async_HealthBarFlash);
				stunMeterFlashThread.detach();
				async_HealthBarFlash_b = true;
			}
			//gray out this actor's stun meter.
			if (settings::bStunMeterToggle && settings::TrueHudAPI_HasSpecialBarControl) {
				greyOutStunMeter(actor);
			}
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
	//DEBUG("damaging done");
}

bool stunHandler::isActorStunned(RE::Actor* a_actor) {
	return stunnedActors.contains(a_actor);
}

void stunHandler::calculateStunDamage(
	STUNSOURCE stunSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage) {
	//DEBUG("Calculating stun damage");
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
};
void stunHandler::untrackStun(RE::Actor* actor) {
	safeErase_ActorStunMap(actor);
	safeErase_StunnedActors(actor);
	safeErase_StunRegenQueue(actor);
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
	mtx_StunnedActors.lock();
	stunnedActors.clear();
	mtx_StunnedActors.unlock();
}
void stunHandler::cleanUpStunMap() {
	INFO("Cleaning up stun map...");
	mtx_ActorStunMap.lock();
	auto it = actorStunMap.begin();
	while (it != actorStunMap.end()) {
		auto actor = it->first;
		if (!actor || !actor->currentProcess || !actor->currentProcess->InHighProcess() || actor->IsDead()) {
			safeErase_StunnedActors(actor);
			safeErase_StunRegenQueue(actor);
			it = actorStunMap.erase(it);
			continue;
		}
		it++;
	}
	mtx_ActorStunMap.unlock();
	INFO("...done");
}

void stunHandler::stunMapCleanUpTask() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::minutes(3));
		stunHandler::GetSingleton()->cleanUpStunMap();
	}
}
void stunHandler::launchStunMapCleaner() {
	std::jthread cleanUpThread(stunMapCleanUpTask);
	cleanUpThread.detach();
}


void stunHandler::greyOutStunMeter(RE::Actor* a_actor) {
	auto ersh = ValhallaCombat::GetSingleton()->ersh;
	ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
	ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::BarColor, 0x7d7e7d);
	ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::PhantomColor, 0xb30d10);
	ersh->OverrideBarColor(a_actor->GetHandle(), RE::ActorValue::kHealth, TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
}

void stunHandler::revertStunMeter(RE::Actor* a_actor) {
	auto ersh = ValhallaCombat::GetSingleton()->ersh;
	ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::FlashColor);
	ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::BarColor);
	ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::PhantomColor);
	ersh->RevertBarColor(a_actor->GetHandle(), RE::ActorValue::kHealth, TRUEHUD_API::BarColorType::FlashColor);
}

void stunHandler::flashHealthBar(RE::Actor* a_actor) {
	ValhallaCombat::GetSingleton()->ersh->FlashActorValue(a_actor->GetHandle(), RE::ActorValue::kHealth, false);
	//ersh->FlashActorSpecialBar(SKSE::GetPluginHandle(), a_actor->GetHandle(), true);
}
#pragma endregion