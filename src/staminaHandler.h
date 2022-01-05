#pragma once
#include "dataHandler.h"
#include "debuffHandler.h"
namespace staminaHandler
{
	inline void checkStamina(RE::Actor* a) {
		if (a->GetActorValue(RE::ActorValue::kStamina) <= 0) {
			debuffHandler::GetSingleton()->initStaminaDebuff();
		}
	}
	inline void staminaLightMiss(RE::Actor* a) {
		DEBUG("stamina light miss");
		DEBUG("damaging {} point stamina", dataHandler::GetSingleton()->meleeCostLightMiss);
		Utils::damageav(a, RE::ActorValue::kStamina, dataHandler::GetSingleton()->meleeCostLightMiss);
		checkStamina(a);
	}

	
	inline void staminaLightHit(RE::Actor* a) {
		DEBUG("stamina light hit");
		DEBUG("restoring {} percent of stamina", dataHandler::GetSingleton()->meleeRecovLightHit);
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::restoreav(a, RE::ActorValue::kStamina, maxStamina * dataHandler::GetSingleton()->meleeRecovLightHit / 100);
		checkStamina(a);
	}

	inline void staminaHeavyMiss(RE::Actor* a) {
		DEBUG("stamina heavy miss");
		DEBUG("damaging {} percent of stamina", dataHandler::GetSingleton()->meleeCostHeavyMiss);
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::damageav(a, RE::ActorValue::kStamina, maxStamina * dataHandler::GetSingleton()->meleeCostHeavyMiss / 100);
		checkStamina(a);
	}

	inline void staminaHeavyHit(RE::Actor* a) {
		DEBUG("stamina heavy hit");
		DEBUG("damaging {} percent of stamina", dataHandler::GetSingleton()->meleeCostHeavyHit);
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::damageav(a, RE::ActorValue::kStamina, maxStamina * dataHandler::GetSingleton()->meleeCostHeavyHit / 100);
		checkStamina(a);
	} 
	

};

