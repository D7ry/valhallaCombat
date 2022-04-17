#include "include/balanceHandler.h"
#include "include/reactionHandler.h"
#include "include/offsets.h"
#include "include/Utils.h"
void balanceHandler::update() {
	//DEBUG("update");
	mtx_balanceBrokenActors.lock();
	if (balanceBrokenActors.empty()) {//stop updating when there is 0 actor need to regen balance.
		mtx_balanceBrokenActors.unlock();
		//DEBUG("no balance broken actors, stop update");
		ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::HANDLER::balanceHandler);
		return;
	}

	//DEBUG("non-empty balance map");
	//regenerate balance for all balance broken actors.
	
	auto it = balanceBrokenActors.begin();
	mtx_actorBalanceMap.lock();
	while (it != balanceBrokenActors.end()) {
		if (!actorBalanceMap.contains(*it)) { //edge case: actor's balance broken but no longer tracked on actor balance map.
			//DEBUG("edge case");
			it = balanceBrokenActors.erase(it);
			continue;
		}
		//regen a single actor's balance.
#define a_balanceData actorBalanceMap.find(*it)->second
		float regenVal = a_balanceData.first * *RE::Offset::g_deltaTime * 1 / 3.5;//33% of all balance regened per sec.
		//DEBUG(regenVal);
		//DEBUG(a_balanceData.second);
		//DEBUG(a_balanceData.first);
		if (a_balanceData.second + regenVal >= a_balanceData.first) {//this regen exceeds actor's max balance.
			DEBUG("{}'s balance has recovered", (*it)->GetName());
			a_balanceData.second = a_balanceData.first;//reset balance.
			it = balanceBrokenActors.erase(it);
			continue;
		}
		else {
			//DEBUG("normal regen");
			a_balanceData.second += regenVal;
		}
		it++;
	}

	mtx_actorBalanceMap.unlock();
	mtx_balanceBrokenActors.unlock();
	
}

float balanceHandler::calculateMaxBalance(RE::Actor* a_actor) {
	return a_actor->GetPermanentActorValue(RE::ActorValue::kHealth);
}

void balanceHandler::trackBalance(RE::Actor* a_actor) {
	float maxBalance = calculateMaxBalance(a_actor);
	mtx_actorBalanceMap.lock();
	actorBalanceMap.emplace(a_actor, std::pair<float, float> {maxBalance, maxBalance});
	mtx_actorBalanceMap.unlock();
}

void balanceHandler::untrackBalance(RE::Actor* a_actor) {
	mtx_actorBalanceMap.lock();
	actorBalanceMap.erase(a_actor);
	mtx_actorBalanceMap.unlock();
}

bool balanceHandler::isBalanceBroken(RE::Actor* a_actor) {
	bool isBalanceBroken;
	mtx_balanceBrokenActors.lock();
	isBalanceBroken = balanceBrokenActors.contains(a_actor);
	mtx_balanceBrokenActors.unlock();
	return isBalanceBroken;
}

void balanceHandler::damageBalance(DMGSOURCE dmgSource, RE::Actor* aggressor, RE::Actor* victim, float damage) {
	DEBUG("damaging balance: aggressor: {}, victim: {}, damage: {}", aggressor->GetName(), victim->GetName(), damage);
	mtx_actorBalanceMap.lock();
	if (!actorBalanceMap.contains(victim)) {
		mtx_actorBalanceMap.unlock();
		trackBalance(victim);
		damageBalance(dmgSource, aggressor, victim, damage);
		return;
	}
#define a_balanceData actorBalanceMap.find(victim)->second
	//DEBUG("curr balance: {}", a_balanceData.second);
	if (a_balanceData.second - damage <= 0) { //balance broken, ouch!
		a_balanceData.second = 0;
		mtx_actorBalanceMap.unlock();
		mtx_balanceBrokenActors.lock();
		if (!balanceBrokenActors.contains(victim)) {
			//DEBUG("{}'s balance has broken", victim->GetName());
			balanceBrokenActors.insert(victim);
			reactionHandler::triggerContinuousStagger(aggressor, victim, reactionHandler::kLarge);
			ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::HANDLER::balanceHandler);
		}
		else {//balance already broken, yet broken again, ouch!
			//DEBUG("{}'s balance double broken", victim->GetName());
			reactionHandler::triggerContinuousStagger(aggressor, victim, reactionHandler::kLargest);
		}
		mtx_balanceBrokenActors.unlock();
		
	}
	else {
		//DEBUG("normal balance damage.");
		a_balanceData.second -= damage;
		mtx_actorBalanceMap.unlock();
		mtx_balanceBrokenActors.lock();
		if (balanceBrokenActors.contains(victim)) {//if balance broken, trigger stagger.
			reactionHandler::triggerContinuousStagger(aggressor, victim, reactionHandler::kMedium);
		}
		else {
			if (dmgSource != DMGSOURCE::parry) {
				//attack interruption
				auto victimAttackState = victim->GetAttackState();
				if (victimAttackState > RE::ATTACK_STATE_ENUM::kNone && victimAttackState <= RE::ATTACK_STATE_ENUM::kBowFollowThrough) {
					if (Utils::isPowerAttacking(victim)) {
						if (Utils::isPowerAttacking(aggressor)) {//only interrupt power attacking enemies with power attack
							reactionHandler::triggerStagger(aggressor, victim, reactionHandler::kSmall);
						}
					}
					else {
						reactionHandler::triggerStagger(aggressor, victim, reactionHandler::kSmall);
					}
				}
			}
			
		}
		mtx_balanceBrokenActors.unlock();
	}
}

void balanceHandler::calculateBalanceDamage(DMGSOURCE dmgSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage) {
	if (!settings::bBalanceToggle) {
		return;
	}
	mtx_balanceBrokenActors.lock();
	if (balanceBrokenActors.contains(victim)) {
		//if balance broken, no damage mult.
	}
	else {
		baseDamage *= 5;
	}
	mtx_balanceBrokenActors.unlock();
	damageBalance(dmgSource, aggressor, victim, baseDamage);

}