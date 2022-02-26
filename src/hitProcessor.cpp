#include "hitProcessor.h"
#include "attackHandler.h"
#include "stunHandler.h"
#include "executionHandler.h"
void hitProcessor::processHit(RE::Actor* aggressor, RE::Actor* victim, RE::HitData& hitData) {
	//offset damage from hitdata, based on player difficulty setting.
	float realDamage = hitData.totalDamage;
	if (victim->IsPlayerRef() || victim->IsPlayerTeammate()) {
		Utils::offsetRealDamage(realDamage, false);
	}
	else if (aggressor->IsPlayerRef() || aggressor->IsPlayerTeammate()) {
		Utils::offsetRealDamage(realDamage, true);
	}
	int hitFlag = (int)hitData.flags;
	if (hitFlag & (int)HITFLAG::kBlocked) {
		if (blockHandler::GetSingleton()->processBlock(victim, aggressor, hitFlag, hitData, realDamage)) {
			DEBUG("attack perfect blocked");
			debuffHandler::GetSingleton()->quickStopStaminaDebuff(victim);
			return; //if the hit is perfect blocked, no hit registration
		}
		// if not perfect blocked, regenerate stamina only if set so.
		if (settings::bBlockedHitRegenStamina && !(hitFlag & (int)HITFLAG::kBash)) {
			attackHandler::GetSingleton()->registerHit(aggressor);
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
	attackHandler::GetSingleton()->registerHit(aggressor);
	//DEBUG("test execution");
	//executionHandler::GetSingleton()->playExecutionIdle(aggressor, victim, gameDataCache::testIdle);
	if (hitFlag & (int)HITFLAG::kPowerAttack) {
		stunHandler::GetSingleton()->calculateStunDamage(stunHandler::STUNSOURCE::powerAttack, hitData.weapon, aggressor, victim, realDamage);
	}
	else {
		stunHandler::GetSingleton()->calculateStunDamage(stunHandler::STUNSOURCE::lightAttack, hitData.weapon, aggressor, victim, realDamage);
	}

	//TODO: a better execution module
	//Temporary execution module
	if (settings::bStunToggle && //stun must be toggled to trigger execution.
		!victim->IsPlayerRef() && !victim->IsPlayerTeammate() && !victim->IsEssential() && !victim->IsInKillMove()) {
		DEBUG("Victim stun is {}", stunHandler::GetSingleton()->getStun(victim));
		if (stunHandler::GetSingleton()->getStun(victim) <= 0) {
			executionHandler::GetSingleton()->attemptExecute(aggressor, victim);
		}
	}
}
