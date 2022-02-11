#pragma once
#include "data.h"
#include "debuffHandler.h"
/*Deal with stamina operations and check stamina to launch debuff.*/
class staminaHandler
{
public:
	static void checkStamina(RE::Actor* actor) {
		DEBUG("checking stamina");
		if (actor->GetActorValue(RE::ActorValue::kStamina) <= 0) {
			debuffHandler::GetSingleton()->initStaminaDebuff(actor);
		}
		DEBUG("stamina checked");
	}
	static void staminaLightMiss(RE::Actor* a) {
		DEBUG("stamina light miss");
		Utils::damageav(a, RE::ActorValue::kStamina, settings::fMeleeCostLightMiss_Point);
		checkStamina(a);
	}

	
	static void staminaLightHit(RE::Actor* a) {
		DEBUG("stamina light hit");
		Utils::restoreav(a, RE::ActorValue::kStamina, a->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeRewardLightHit_Percent);
		DEBUG("stamina restored");
		checkStamina(a);
	}

	static void staminaHeavyMiss(RE::Actor* a) {
		DEBUG("stamina heavy miss");
		Utils::damageav(a, RE::ActorValue::kStamina, a->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeCostHeavyMiss_Percent);
		checkStamina(a);
	}

	static void staminaHeavyHit(RE::Actor* a) {
		DEBUG("stamina heavy hit");
		Utils::damageav(a, RE::ActorValue::kStamina, a->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeCostHeavyHit_Percent);
		checkStamina(a);
	}

	static void staminaDodgeStep(RE::Actor* a) {
		DEBUG("stamina dodge step");
		Utils::damageav(a, RE::ActorValue::kStamina, settings::fDodgeStaminaStep);
		checkStamina(a);
	}


};

