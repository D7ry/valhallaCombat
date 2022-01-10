#include "hitDataProcessor.h"
#include "attackHandler.h"
void hitDataProcessor::processPlayerHit(RE::TESObjectWEAP* weapon, int hitFlag){
	if ((hitFlag & (int)HITFLAG::kBlocked)) {//blocked hit doesn't regen stamina
		DEBUG("HIT BLOCKED!");
		if (!settings::blockedHitRegenStamina) {
			DEBUG("blocked hit don't regen stamina!");
			return;
		}
	}
	if (hitFlag & (int)HITFLAG::kBash) {//bash hit doesn't regen stamina
		return;
	}
	if (weapon->IsMelee()) {
		attackHandler::registerHit();
	}

}
void hitDataProcessor::processStaminaDamage(RE::ActorPtr target, RE::HitData& hitData, int hitFlag) {
	float staminaDamageBase = hitData.totalDamage;
	float staminaDamageMult;
	float staminaDamage;
	DEBUG("base stamina damage is {}", staminaDamageBase);
	if (hitFlag & (int)HITFLAG::kBlockWithWeapon) {
		DEBUG("hit blocked with weapon");
		staminaDamageMult = settings::bckWpnStaminaPenaltyMult;
	}
	else {
		DEBUG("hit blocked with shield");
		staminaDamageMult = settings::bckShdStaminaPenaltyMult;
	}
	staminaDamage = staminaDamageBase * staminaDamageMult;
	DEBUG("stamina damage is {}", staminaDamage);
	float currStamina = target->GetActorValue(RE::ActorValue::kStamina);
	if (currStamina < staminaDamage) {
		DEBUG("not enough stamina to block, blocking part of damage!");
		if (settings::guardBreak) {
			DEBUG("guard break!");
			target->NotifyAnimationGraph("staggerStart");
		}
		hitData.totalDamage = hitData.totalDamage - (currStamina / staminaDamageMult);
	}
	else {
		hitData.totalDamage = 0;
	}
	Utils::damageav(target.get(), RE::ActorValue::kStamina,
		staminaDamage);
}

void hitDataProcessor::processStunDamage(RE::ActorPtr target, RE::TESObjectWEAP* weapon, RE::HitData& hitData, int hitFlag) {
	DEBUG("processing stun damage");
	if (!hitData.weapon->IsMelee()) {
		DEBUG("ranged attack!");
		
	}
	else if (hitFlag & (int)HITFLAG::kPowerAttack) {
		DEBUG("power attack!");
	} 
	else if (hitFlag & (int)HITFLAG::kBash) {
		DEBUG("bash attack!");
	}
	else {
		DEBUG("normal attack!");

	}
}
