#include "hitProcessor.h"
#include "attackHandler.h"
#include "stunHandler.h"
#include "executionHandler.h"
void hitProcessor::processHit(RE::Actor* aggressor, RE::Actor* victim, RE::HitData& hitData) {
	//offset damage from hitdata, based on player difficulty setting.
	float realDamage;
	if (victim->IsPlayerRef()) {
		realDamage = calculateRealDamage(hitData.totalDamage, false);
	}
	else if (aggressor->IsPlayerRef()) {
		realDamage = calculateRealDamage(hitData.totalDamage, true);
	}
	else {
		realDamage = hitData.totalDamage;
	}
	int hitFlag = (int)hitData.flags;
	if (hitFlag & (int)HITFLAG::kBlocked) {
		if (blockHandler::GetSingleton()->processBlock(victim, aggressor, hitFlag, hitData, realDamage)) {
			DEBUG("attack perfect blocked");
			return; //if the hit is perfect blocked, no hit registration
		}
		if (settings::bBlockedHitRegenStamina && !(hitFlag & (int)HITFLAG::kBash)) { //regenerate stamina only if set so.
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

	if (hitFlag & (int)HITFLAG::kPowerAttack) {
		stunHandler::GetSingleton()->calculateStunDamage(stunHandler::STUNSOURCE::powerAttack, hitData.weapon, aggressor, victim, realDamage);
	}
	else {
		stunHandler::GetSingleton()->calculateStunDamage(stunHandler::STUNSOURCE::lightAttack, hitData.weapon, aggressor, victim, realDamage);
	}

	//TODO: a better execution module
	//Temporary execution module
	if (!victim->IsPlayerRef() && !victim->IsPlayerTeammate() && !victim->IsEssential() && !victim->IsInKillMove()) {
		DEBUG("Victim stun is {}", stunHandler::GetSingleton()->getStun(victim));
		if (stunHandler::GetSingleton()->getStun(victim) <= 0) {
			executionHandler::GetSingleton()->attemptExecute(aggressor, victim);
		}
	}
}

float hitProcessor::calculateRealDamage(float damage, bool isPlayerAggressor) {
	DEBUG("damage before offset: {}", damage);
	if (isPlayerAggressor) {
		switch (RE::PlayerCharacter::GetSingleton()->getDifficultySetting()) {
		case RE::DIFFICULTY::kNovice: damage *= data::GetSingleton()->fDiffMultHPByPCVE; break;
		case RE::DIFFICULTY::kApprentice: damage *= data::GetSingleton()->fDiffMultHPByPCE; break;
		case RE::DIFFICULTY::kAdept: damage *= data::GetSingleton()->fDiffMultHPByPCN; break;
		case RE::DIFFICULTY::kExpert: damage *= data::GetSingleton()->fDiffMultHPByPCH; break;
		case RE::DIFFICULTY::kMaster: damage *= data::GetSingleton()->fDiffMultHPByPCVH; break;
		case RE::DIFFICULTY::kLegendary: damage *= data::GetSingleton()->fDiffMultHPByPCL; break;
		}
	}
	else {
		switch (RE::PlayerCharacter::GetSingleton()->getDifficultySetting()) {
		case RE::DIFFICULTY::kNovice: damage *= data::GetSingleton()->fDiffMultHPToPCVE; break;
		case RE::DIFFICULTY::kApprentice: damage *= data::GetSingleton()->fDiffMultHPToPCE; break;
		case RE::DIFFICULTY::kAdept: damage *= data::GetSingleton()->fDiffMultHPToPCN; break;
		case RE::DIFFICULTY::kExpert: damage *= data::GetSingleton()->fDiffMultHPToPCH; break;
		case RE::DIFFICULTY::kMaster: damage *= data::GetSingleton()->fDiffMultHPToPCVH; break;
		case RE::DIFFICULTY::kLegendary: damage *= data::GetSingleton()->fDiffMultHPToPCL; break;
		}
	}
	DEBUG("real damage is {}", damage);
	return damage;
}
