#pragma once
#include "dataHandler.h"
#include "debuffHandler.h"
namespace staminaHandler
{
	inline void checkStamina(RE::Actor* a) {
		DEBUG("checking stamina");
		if (a->GetActorValue(RE::ActorValue::kStamina) <= 0) {
			debuffHandler::GetSingleton()->initStaminaDebuff();
		}
		DEBUG("stamina checked");
	}
	inline void staminaLightMiss(RE::Actor* a) {
		DEBUG("stamina light miss");
		DEBUG("damaging {} point stamina", settings::meleeCostLightMiss);
		Utils::damageav(a, RE::ActorValue::kStamina, settings::meleeCostLightMiss);
		checkStamina(a);
	}

	
	inline void staminaLightHit(RE::Actor* a) {
		DEBUG("stamina light hit");
		DEBUG("restoring {} percent of stamina", settings::meleeRecovLightHit);
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::restoreav(a, RE::ActorValue::kStamina, maxStamina * settings::meleeRecovLightHit / 100);
		DEBUG("stamina restored");
		checkStamina(a);
	}

	inline void staminaHeavyMiss(RE::Actor* a) {
		DEBUG("stamina heavy miss");
		DEBUG("damaging {} percent of stamina", settings::meleeCostHeavyMiss);
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::damageav(a, RE::ActorValue::kStamina, maxStamina * settings::meleeCostHeavyMiss / 100);
		checkStamina(a);
	}

	inline void staminaHeavyHit(RE::Actor* a) {
		DEBUG("stamina heavy hit");
		DEBUG("damaging {} percent of stamina", settings::meleeCostHeavyHit);
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::damageav(a, RE::ActorValue::kStamina, maxStamina * settings::meleeCostHeavyHit / 100);
		checkStamina(a);
	} 
	


};

