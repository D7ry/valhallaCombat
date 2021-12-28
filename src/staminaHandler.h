#pragma once
#include "dataHandler.h"
#include "debuffHandler.h"
namespace staminaHandler
{	//FIXME: this is fucked up
	inline void checkStamina(RE::Actor* a) {
		if (a->GetActorValue(RE::ActorValue::kStamina) <= 0) {
			debuffHandler::GetSingleton()->initStaminaDebuff();
		}
	}
	inline void staminaLightMiss(RE::Actor* a) {
		DEBUG("stamina light miss");
		DEBUG("damaging {} stamina", dataHandler::GetSingleton()->meleeCostLightMiss);
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::damageav(a, RE::ActorValue::kStamina, dataHandler::GetSingleton()->meleeCostLightMiss);
		checkStamina(a);
	}

	
	inline void staminaLightHit(RE::Actor* a) {
		DEBUG("stamina light hit");
		DEBUG("restoring {} of stamina", dataHandler::GetSingleton()->meleeRecovLightHit);
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::restoreav(a, RE::ActorValue::kStamina, maxStamina * dataHandler::GetSingleton()->meleeRecovLightHit);
		checkStamina(a);
	}

	inline void staminaHeavyMiss(RE::Actor* a) {
		DEBUG("stamina heavy miss");
		DEBUG("damaging {} of stamina", dataHandler::GetSingleton()->meleeCostHeavyMiss);
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::damageav(a, RE::ActorValue::kStamina, maxStamina * dataHandler::GetSingleton()->meleeCostHeavyMiss);
		checkStamina(a);
	}

	inline void staminaHeavyHit(RE::Actor* a) {
		DEBUG("stamina heavy hit");
		DEBUG("damaging {} of stamina", dataHandler::GetSingleton()->meleeCostHeavyHit);
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::damageav(a, RE::ActorValue::kStamina, maxStamina * dataHandler::GetSingleton()->meleeCostHeavyHit);
		checkStamina(a);
	} 
	

};

