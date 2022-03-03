#pragma once
#include "data.h"
#include "debuffHandler.h"
/*Convenience clas to deal with stamina operations.*/
class staminaHandler
{
public:
	static void checkStamina(RE::Actor* actor) {
		if (actor->GetActorValue(RE::ActorValue::kStamina) <= 0) {
			//DEBUG("{}'s stamina is less than 0, initializing debuff.", actor->GetName());
			debuffHandler::GetSingleton()->initStaminaDebuff(actor);
		}
	}
	static void staminaLightMiss(RE::Actor* a) {
		//DEBUG("stamina light miss");
		if (!a->IsInCombat() && !settings::bNonCombatStaminaDebuff) {
			return;
		}
		Utils::damageav(a, RE::ActorValue::kStamina, settings::fMeleeCostLightMiss_Point);
		checkStamina(a);
	}


	static void staminaLightHit(RE::Actor* a) {
		//DEBUG("stamina light hit");
		if (!a->IsInCombat() && !settings::bNonCombatStaminaDebuff) {
			return;
		}
		Utils::restoreav(a, RE::ActorValue::kStamina, a->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeRewardLightHit_Percent);
		//DEBUG("stamina restored");
		checkStamina(a);
	}

	static void staminaHeavyMiss(RE::Actor* a) {
		//DEBUG("stamina heavy miss");
		if (!a->IsInCombat() && !settings::bNonCombatStaminaDebuff) {
			return;
		}
		Utils::damageav(a, RE::ActorValue::kStamina, a->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeCostHeavyMiss_Percent);
		checkStamina(a);
	}

	static void staminaHeavyHit(RE::Actor* a) {
		//DEBUG("stamina heavy hit");
		if (!a->IsInCombat() && !settings::bNonCombatStaminaDebuff) {
			return;
		}
		Utils::damageav(a, RE::ActorValue::kStamina, a->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeCostHeavyHit_Percent);
		checkStamina(a);
	}



};
