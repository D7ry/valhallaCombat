#include "include/staminaHandler.h"
#include "include/debuffHandler.h"
#include "include/Utils.h"
void staminaHandler::checkStamina(RE::Actor* a_actor) {
	if (a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) <= 0) {
		debuffHandler::GetSingleton()->initStaminaDebuff(a_actor);
	}
}
void staminaHandler::staminaLightMiss(RE::Actor* a_actor) {
	if (!a_actor->IsInCombat() && !settings::bNonCombatStaminaCost) {
		return;
	}
	inlineUtils::damageav(a_actor, RE::ActorValue::kStamina, settings::fMeleeCostLightMiss_Point);
	checkStamina(a_actor);
}

void staminaHandler::staminaLightHit(RE::Actor* a_actor) {
	if (!a_actor->IsInCombat() && !settings::bNonCombatStaminaCost) {
		return;
	}
	inlineUtils::restoreav(a_actor, RE::ActorValue::kStamina, a_actor->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeRewardLightHit_Percent);
	checkStamina(a_actor);
}

void staminaHandler::staminaHeavyMiss(RE::Actor* a_actor) {
	if (!a_actor->IsInCombat() && !settings::bNonCombatStaminaCost) {
		return;
	}
	inlineUtils::damageav(a_actor, RE::ActorValue::kStamina, a_actor->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeCostHeavyMiss_Percent);
	checkStamina(a_actor);
}

void staminaHandler::staminaHeavyHit(RE::Actor* a_actor) {
	//DEBUG("stamina heavy hit");
	if (!a_actor->IsInCombat() && !settings::bNonCombatStaminaCost) {
		return;
	}
	inlineUtils::damageav(a_actor, RE::ActorValue::kStamina, a_actor->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeCostHeavyHit_Percent);
	checkStamina(a_actor);
}
