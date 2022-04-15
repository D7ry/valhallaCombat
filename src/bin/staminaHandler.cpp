#include "include/staminaHandler.h"
#include "include/debuffHandler.h"
#include "include/Utils.h"
void staminaHandler::checkStamina(RE::Actor* actor) {
	if (actor->GetActorValue(RE::ActorValue::kStamina) <= 0) {
		//
		("{}'s stamina is less than 0, initializing debuff.", actor->GetName());
		debuffHandler::GetSingleton()->initStaminaDebuff(actor);
	}
}
void staminaHandler::staminaLightMiss(RE::Actor* actor) {
	if (!actor->IsInCombat() && !settings::bNonCombatStaminaDebuff) {
		return;
	}
	Utils::damageav(actor, RE::ActorValue::kStamina, settings::fMeleeCostLightMiss_Point);
	checkStamina(actor);
}

void staminaHandler::staminaLightHit(RE::Actor* a) {
	if (!a->IsInCombat() && !settings::bNonCombatStaminaDebuff) {
		return;
	}
	Utils::restoreav(a, RE::ActorValue::kStamina, a->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeRewardLightHit_Percent);
	checkStamina(a);
}

void staminaHandler::staminaHeavyMiss(RE::Actor* a) {
	if (!a->IsInCombat() && !settings::bNonCombatStaminaDebuff) {
		return;
	}
	Utils::damageav(a, RE::ActorValue::kStamina, a->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeCostHeavyMiss_Percent);
	checkStamina(a);
}

void staminaHandler::staminaHeavyHit(RE::Actor* a) {
	//DEBUG("stamina heavy hit");
	if (!a->IsInCombat() && !settings::bNonCombatStaminaDebuff) {
		return;
	}
	Utils::damageav(a, RE::ActorValue::kStamina, a->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeCostHeavyHit_Percent);
	checkStamina(a);
}