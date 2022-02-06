#include "hitDataProcessor.h"
#include "attackHandler.h"
#include "stunHandler.h"

/*handles hit detection. Iff hit, reward player stamina. Also nerfs damage on exhaustion.*/
/*
void hitDataProcessor::processPlayerHit(RE::TESObjectWEAP* weapon, int hitFlag){
	if ((hitFlag & (int)HITFLAG::kBlocked)) {//blocked hit doesn't regen stamina
		DEBUG("HIT BLOCKED!");
		if (!settings::bBlockedHitRegenStamina) {
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
void hitDataProcessor::processStaminaDamage(RE::ActorPtr target, RE::HitData& hitData, int hitFlag, bool isPlayerTarget) {
	float staminaDamageBase = hitData.totalDamage;
	float staminaDamageMult;
	float staminaDamage;
	DEBUG("base stamina damage is {}", staminaDamageBase);
	if (hitFlag & (int)HITFLAG::kBlockWithWeapon) {
		DEBUG("hit blocked with weapon");
		if (isPlayerTarget) {
			staminaDamageMult = settings::bckWpnStaminaPenaltyMultPlayer;
		}
		else {
			staminaDamageMult = settings::bckWpnStaminaPenaltyMultNPC;
		}
	}
	else {
		DEBUG("hit blocked with shield");
		if (isPlayerTarget) {
			staminaDamageMult = settings::bckShdStaminaPenaltyMultPlayer;
		}
		else {
			staminaDamageMult = settings::bckShdStaminaPenaltyMultNPC;
		}
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
		DEBUG("failed to block {} damage", hitData.totalDamage);
	}
	else {
		hitData.totalDamage = 0;
	}
	Utils::damageav(target.get(), RE::ActorValue::kStamina,
		staminaDamage);
}
*/
/*deals stun damage to the receiver. By the end of dealing stun, if the receiver has less than 0 stun meter, execute
the receiver.
void hitDataProcessor::processStunDamage(RE::ActorPtr aggressor, RE::ActorPtr target, RE::TESObjectWEAP* weapon, RE::HitData& hitData, int hitFlag) {
	DEBUG("processing stun damage");
	stunHandler::stunActor(hitData.totalDamage, target, weapon, (hitFlag & (int)HITFLAG::kPowerAttack));
	if (stunHandler::isStunMeterEmpty(target) && weapon->IsMelee()) {
		DEBUG("target has empety stun bar, executing target!");
		stunHandler::execute(aggressor, target);

	}
}*/
