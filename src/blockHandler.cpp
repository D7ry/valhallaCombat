#include "blockHandler.h"
#include "data.h"
void blockHandler::processStaminaBlock(RE::Actor* victim, RE::Actor* aggressor, int iHitflag, RE::HitData hitData) {
	using HITFLAG = RE::HitData::Flag;
	float staminaDamageBase = hitData.totalDamage;
	float staminaDamageMult;
	DEBUG("base stamina damage is {}", staminaDamageBase);
	if (iHitflag & (int)HITFLAG::kBlockWithWeapon) {
		DEBUG("hit blocked with weapon");
		if (victim->IsPlayerRef()) {
			staminaDamageMult = settings::fBckWpnStaminaMult_PC_Block_NPC;
		}
		else {
			if (aggressor->IsPlayerRef()) {
				staminaDamageMult = settings::fBckWpnStaminaMult_NPC_Block_PC;
			}
			else {
				staminaDamageMult = settings::fBckWpnStaminaMult_NPC_Block_NPC;
			}
		}
	}
	else {
		DEBUG("hit blocked with shield");
		if (victim->IsPlayerRef()) {
			staminaDamageMult = settings::fBckShdStaminaMult_PC_Block_NPC;
		}
		else {
			if (aggressor->IsPlayerRef()) {
				staminaDamageMult = settings::fBckShdStaminaMult_NPC_Block_PC;
			}
			else {
				staminaDamageMult = settings::fBckShdStaminaMult_NPC_Block_NPC;
			}
		}
	}
	float staminaDamage = staminaDamageBase * staminaDamageMult;
	float targetStamina = victim->GetActorValue(RE::ActorValue::kStamina);

	//check whether there's enough stamina to block incoming attack
	if (targetStamina < staminaDamage) {
		DEBUG("not enough stamina to block, blocking part of damage!");
		if (settings::bGuardBreak) {
			DEBUG("guard break!");
			victim->NotifyAnimationGraph("staggerStart");
		}
		hitData.totalDamage = hitData.totalDamage - (targetStamina / staminaDamageMult);
		Utils::damageav(victim, RE::ActorValue::kStamina,
			targetStamina);
		DEBUG("failed to block {} damage", hitData.totalDamage);
		debuffHandler::GetSingleton()->initStaminaDebuff(victim); //initialize debuff for the failed blocking attempt
	}
	else {
		hitData.totalDamage = 0;
		Utils::damageav(victim, RE::ActorValue::kStamina,
			staminaDamage);
	}
}