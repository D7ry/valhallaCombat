#pragma once
#include "data.h"
#include "debuffHandler.h"
namespace staminaHandler
{
	inline void checkStamina(RE::Actor* actor) {
		DEBUG("checking stamina");
		if (actor->GetActorValue(RE::ActorValue::kStamina) <= 0) {
			debuffHandler::GetSingleton()->initStaminaDebuff(actor);
		}
		DEBUG("stamina checked");
	}
	inline void staminaLightMiss(RE::Actor* a) {
		DEBUG("stamina light miss");
		Utils::damageav(a, RE::ActorValue::kStamina, settings::fMeleeCostLightMiss_Point);
		checkStamina(a);
	}

	
	inline void staminaLightHit(RE::Actor* a) {
		DEBUG("stamina light hit");
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::restoreav(a, RE::ActorValue::kStamina, maxStamina * settings::fMeleeRewardLightHit_Percent);
		DEBUG("stamina restored");
		checkStamina(a);
	}

	inline void staminaHeavyMiss(RE::Actor* a) {
		DEBUG("stamina heavy miss");
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::damageav(a, RE::ActorValue::kStamina, maxStamina * settings::fMeleeCostHeavyMiss_Percent);
		checkStamina(a);
	}

	inline void staminaHeavyHit(RE::Actor* a) {
		DEBUG("stamina heavy hit");
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::damageav(a, RE::ActorValue::kStamina, maxStamina * settings::fMeleeCostHeavyHit_Percent);
		checkStamina(a);
	} 
	


};

