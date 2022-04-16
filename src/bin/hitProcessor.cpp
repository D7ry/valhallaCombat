#include "include/hitProcessor.h"
#include "include/attackHandler.h"
#include "include/stunHandler.h"
#include "include/balanceHandler.h"
#include "include/executionHandler.h"
#include "include/settings.h"
#include "include/reactionHandler.h"
#include "include/AI.h"
#include "include/Utils.h"
void hitProcessor::processHit(RE::Actor* aggressor, RE::Actor* victim, RE::HitData& hitData) {
	//offset damage from hitdata, based on player difficulty setting.
	float realDamage = hitData.totalDamage;
	Utils::offsetRealDamage(realDamage, aggressor, victim);
	int hitFlag = (int)hitData.flags;
	if (hitFlag & (int)HITFLAG::kBlocked) {
		if (blockHandler::GetSingleton()->processBlock(victim, aggressor, hitFlag, hitData, realDamage)) {
			debuffHandler::GetSingleton()->quickStopStaminaDebuff(victim);
			if (!victim->IsPlayerRef()) {
				AI::GetSingleton()->action_PerformEldenCounter(victim);
			}
			return; //if the hit is perfect blocked, no hit registration
		}
		// if not perfect blocked, regenerate stamina only if set so.
		if (settings::bBlockedHitRegenStamina && !(hitFlag & (int)HITFLAG::kBash)) {
			if (settings::bAttackStaminaToggle) {
				attackHandler::GetSingleton()->registerHit(aggressor);
				
			}
		}
		return;
	}

	//from this point on the hit is not blocked/

	//bash hit
	if (hitFlag & (int)HITFLAG::kBash) {
		if (hitFlag & (int)HITFLAG::kPowerAttack) {
			stunHandler::GetSingleton()->calculateStunDamage(stunHandler::STUNSOURCE::powerBash, nullptr, aggressor, victim, 0);
		}
		else {
			stunHandler::GetSingleton()->calculateStunDamage(stunHandler::STUNSOURCE::bash, nullptr, aggressor, victim, 0);
		}
		return;
	}

	//from this point on the hit can only be unblocked melee hit.
	if (settings::bAttackStaminaToggle) {
		attackHandler::GetSingleton()->registerHit(aggressor);
	}

	//reactionHandler::triggerContinuousStagger(aggressor, victim, reactionHandler::kMedium);
	//try execution
	bool isActorStunned;
	stunHandler::GetSingleton()->isActorStunned(victim, isActorStunned);
	if (isActorStunned && hitData.weapon->IsMelee()) {
		if (aggressor->IsPlayerRef()) {
			if (settings::bAutoExecution) {//player only auto-execute if auto execution is on
				executionHandler::GetSingleton()->attemptExecute(aggressor, victim);
			}
		}
		else { //NPC auto-execute always.
			executionHandler::GetSingleton()->attemptExecute(aggressor, victim);
		}
	}
	//DEBUG("test execution");
	//executionHandler::GetSingleton()->playExecutionIdle(aggressor, victim, data::testIdle);
	if (hitFlag & (int)HITFLAG::kPowerAttack) {
		bool b;
		//TODO:redo stun damage for elden counter
		if (aggressor->GetGraphVariableBool(data::GraphBool_IsGuardCountering, b) && b) {
			realDamage *= 1.5;
		}
		stunHandler::GetSingleton()->calculateStunDamage(stunHandler::STUNSOURCE::powerAttack, hitData.weapon, aggressor, victim, realDamage);
		balanceHandler::GetSingleton()->calculateBalanceDamage(balanceHandler::DMGSOURCE::powerAttack, hitData.weapon, aggressor, victim, realDamage);
	}
	else {
		stunHandler::GetSingleton()->calculateStunDamage(stunHandler::STUNSOURCE::lightAttack, hitData.weapon, aggressor, victim, realDamage);
		balanceHandler::GetSingleton()->calculateBalanceDamage(balanceHandler::DMGSOURCE::lightAttack, hitData.weapon, aggressor, victim, realDamage);
	}

}
