#pragma once
#include "Utils.h"
#include "dataHandler.h"
#include "debuffHandler.h"
namespace avHandler
{
	inline auto staminaAv = RE::ActorValue::kStamina;
	inline auto data = dataHandler::GetSingleton();

	inline void staminaLightMiss(RE::Actor* a) {
		INFO("stamina light miss");
		INFO("damaging {} stamina", data->meleeCostLight);
		Utils::damageav(a, staminaAv, data->meleeCostLight);
	}

	inline void staminaLightHit(RE::Actor* a) {
		INFO("stamina light hit");
		INFO("restoring {} of stamina", data->meleeHitStaminaRecoverLight);
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::restoreav(a, staminaAv, maxStamina * data->meleeHitStaminaRecoverLight);
	}

	inline void staminaHeavyMiss(RE::Actor* a) {
		INFO("stamina heavy miss");
		INFO("damaging {} of stamina", data->meleeCostHeavy);
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::damageav(a, staminaAv, maxStamina * data->meleeCostHeavy);
	}


	inline void staminaHeavyHit(RE::Actor* a) {
		INFO("stamina heavy hit");
		INFO("damaging {} of stamina", data->meleeHitStaminaCostHeavy);
		float maxStamina = a->GetPermanentActorValue(RE::ActorValue::kStamina);
		Utils::damageav(a, staminaAv, maxStamina * data->meleeHitStaminaCostHeavy);
	}

};

