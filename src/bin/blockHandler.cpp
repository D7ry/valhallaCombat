#include "include/blockHandler.h"
#include "include/data.h"
#include "include/settings.h"
#include "include/Utils.h"
#include "include/stunHandler.h"
#include "include/balanceHandler.h"
#include "include/hitProcessor.h"
#include "include/reactionHandler.h"
#include "include/lib/BlockSpark.h"
#include "include/offsets.h"
#include "ValhallaCombat.hpp"
/*Called every frame.
Decrement the timer for actors either perfect blocking or cooling down.*/
void blockHandler::update() {
	auto deltaTime = *RE::Offset::g_deltaTime;
	//mtx.lock();
	mtx_actors_PerfectBlocking.lock();
	auto it1 = actors_PerfectBlocking.begin();
	while (it1 != actors_PerfectBlocking.end()) {
		auto actor = it1->first;
		if (it1->second <= 0) {
			//DEBUG("{}'s perfect block has ended, starting cool down", actor->GetName());
			it1 = actors_PerfectBlocking.erase(it1);
			//Cool down applies if the previous block fails; if successful no cooldown.
			mtx_actors_PrevTimeBlockingSuccessful.lock();
			if (!actors_PrevTimeBlockingSuccessful.contains(actor)) {
				mtx_actors_PrevTimeBlockingSuccessful.unlock();
				//start cooling down
				mtx_actors_BlockingCoolDown.lock();
				actors_BlockingCoolDown[actor] = settings::fPerfectBlockCoolDownTime;
				mtx_actors_BlockingCoolDown.unlock();
				//actorsInBlockingCoolDown.emplace(actor, settings::fPerfectBlockCoolDownTime);
			}
			else {
				actors_PrevTimeBlockingSuccessful.erase(actor);
				mtx_actors_PrevTimeBlockingSuccessful.unlock();

				if (actors_PerfectBlocking.empty()) {
					mtx_actors_BlockingCoolDown.lock();
					if (actors_BlockingCoolDown.empty()) {
						mtx_actors_BlockingCoolDown.unlock();
						ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::blockHandler);
					}
					else {
						mtx_actors_BlockingCoolDown.unlock();
					}
				}
			}
			continue;
		}
		it1->second -= deltaTime;
		++it1;
	}
	mtx_actors_PerfectBlocking.unlock();


	mtx_actors_BlockingCoolDown.lock();
	auto it2 = actors_BlockingCoolDown.begin();
	while (it2 != actors_BlockingCoolDown.end()) {
		auto actor = it2->first;
		if (it2->second <= 0) {
			//exit cooling down phase
			//DEBUG("{}'s cool down has ended", actor->GetName());
			it2 = actors_BlockingCoolDown.erase(it2);
			if (actors_BlockingCoolDown.empty()) {
				mtx_actors_PerfectBlocking.lock();
				if (actors_PerfectBlocking.empty()) {
					mtx_actors_PerfectBlocking.unlock();
					ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::blockHandler);
				}
				else {
					mtx_actors_PerfectBlocking.unlock();
				}
				
			}
			continue;
		}
		it2->second -= deltaTime;
		++it2;
	}
	mtx_actors_BlockingCoolDown.unlock();
	//mtx.unlock();
}

/*Register a perfect block when an actor tries to block. Put the blocker into the active perfect blocker set and start timing.
@param actor actor whose block is registered as a perfect block.*/
void blockHandler::registerPerfectBlock(RE::Actor* actor) {
	//DEBUG("Registering perfect block for {}", actor->GetName());
	//mtx.lock();
	mtx_actors_PrevTimeBlockingSuccessful.lock();
	if (actors_PrevTimeBlockingSuccessful.contains(actor)) { //has previously done a successful perfect block
		actors_PrevTimeBlockingSuccessful.erase(actor); //remove from the successful map.
		mtx_actors_PrevTimeBlockingSuccessful.unlock();

		mtx_actors_PerfectBlocking.lock();
		actors_PerfectBlocking[actor] = settings::fPerfectBlockTime; //reset perfect blocking timer
		mtx_actors_PerfectBlocking.unlock();
		ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::blockHandler);
		return;//register successful, no need to run the following.
	}
	else {
		mtx_actors_PrevTimeBlockingSuccessful.unlock();
	}
	
	mtx_actors_BlockingCoolDown.lock();
	if (!actors_BlockingCoolDown.contains(actor)) { //not cooling down
		mtx_actors_BlockingCoolDown.unlock();

		mtx_actors_PerfectBlocking.lock();
		if (!actors_PerfectBlocking.contains(actor)) {//not perfect blocking now
			mtx_actors_PerfectBlocking.unlock();
			actors_PerfectBlocking[actor] = settings::fPerfectBlockTime;
			ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::blockHandler);
		}
		else {
			mtx_actors_PerfectBlocking.unlock();
		}
	}
	else {
		mtx_actors_BlockingCoolDown.unlock();
	}
	//mtx.unlock();
}
/*Make an actor break their guard through a animation event.*/


#pragma region Process Block
bool blockHandler::processBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData, float realDamage) {
	//DEBUG("Process blocking. Blocker: {} Aggressor: {}", blocker->GetName(), aggressor->GetName());
	DEBUG("processing block. Real damage: {}", realDamage);
	if (settings::bPerfectBlockToggle) {
		mtx_actors_PerfectBlocking.lock();
		if (actors_PerfectBlocking.contains(blocker)) {
			mtx_actors_PerfectBlocking.unlock();
			processTimedBlock(blocker, aggressor, iHitflag, hitData, realDamage);
			return true;
		}
		else {
			mtx_actors_PerfectBlocking.unlock();
		}
	}
	if (settings::bBlockStaminaToggle) {
		processStaminaBlock(blocker, aggressor, iHitflag, hitData, realDamage);
	}
	return false;
}

/*Return the final stamina cost multiplier based on the blocker and aggressor.*/
inline float calculateBlockStaminaCostMult(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag) {
	if (iHitflag & (int)HITFLAG::kBlockWithWeapon) {
		//DEBUG("hit blocked with weapon");
		if (blocker->IsPlayerRef()) {
			return settings::fBckWpnStaminaMult_PC_Block_NPC;
		}
		else {
			if (aggressor->IsPlayerRef()) {
				return settings::fBckWpnStaminaMult_NPC_Block_PC;
			}
			else {
				return settings::fBckWpnStaminaMult_NPC_Block_NPC;
			}
		}
	}
	else {
		//DEBUG("hit blocked with shield");
		if (blocker->IsPlayerRef()) {
			return settings::fBckShdStaminaMult_PC_Block_NPC;
		}
		else {
			if (aggressor->IsPlayerRef()) {
				return settings::fBckShdStaminaMult_NPC_Block_PC;
			}
			else {
				return settings::fBckShdStaminaMult_NPC_Block_NPC;
			}
		}
	}
}

void blockHandler::processStaminaBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData, float realDamage) {
	using HITFLAG = RE::HitData::Flag;
	DEBUG("real damage: {}", realDamage);
	float staminaDamageMult = calculateBlockStaminaCostMult(blocker, aggressor, iHitflag);
	DEBUG("stamina damagem mult: {}", staminaDamageMult);
	float staminaDamage = staminaDamageMult * realDamage;
	DEBUG("stamina damage: {}", staminaDamage);

	float targetStamina = blocker->GetActorValue(RE::ActorValue::kStamina);

	//check whether there's enough stamina to block incoming attack
	if (targetStamina < staminaDamage) {
		if (settings::bGuardBreak) {
			if (iHitflag & (int)HITFLAG::kPowerAttack) {
				reactionHandler::triggerStagger(aggressor, blocker, reactionHandler::kLarge);
			}
			else {
				reactionHandler::triggerStagger(aggressor, blocker, reactionHandler::kMedium);
			}
		}
		hitData.totalDamage =
			(realDamage - (targetStamina / staminaDamageMult)) //real damage actor will be receiving.
			* (hitData.totalDamage) / realDamage; //offset real damage back into raw damage to be converted into real damage again later.
		Utils::damageav(blocker, RE::ActorValue::kStamina,
			targetStamina);
		debuffHandler::GetSingleton()->initStaminaDebuff(blocker); //initialize debuff for the failed blocking attempt
	}
	else {
		hitData.totalDamage = 0;
		Utils::damageav(blocker, RE::ActorValue::kStamina,
			staminaDamage);
	}
}


void blockHandler::processTimedBlock(RE::Actor* blocker, RE::Actor* attacker, int iHitflag, RE::HitData& hitData, float realDamage) {
	float reflectedDamage = 0;
	auto a_weapon = blocker->getWieldingWeapon();
	if (a_weapon) {
		reflectedDamage = a_weapon->GetAttackDamage();//get attack damage of blocker's weapon
	}
	Utils::offsetRealDamage(reflectedDamage, blocker, attacker);
	stunHandler::GetSingleton()->calculateStunDamage(stunHandler::STUNSOURCE::parry, nullptr, blocker, attacker, reflectedDamage);
	balanceHandler::GetSingleton()->calculateBalanceDamage(balanceHandler::DMGSOURCE::parry, nullptr, blocker, attacker, reflectedDamage);
	hitData.totalDamage = 0;
	mtx_actors_PrevTimeBlockingSuccessful.lock();
	actors_PrevTimeBlockingSuccessful.insert(blocker); //register the blocker as a successful blocker.
	mtx_actors_PrevTimeBlockingSuccessful.unlock();
	if (balanceHandler::GetSingleton()->isBalanceBroken(attacker)
		|| stunHandler::GetSingleton()->isActorStunned(attacker)) {
		playeBlockEffects(blocker, attacker, iHitflag, blockType::guardBreaking);
	}
	else {
		playeBlockEffects(blocker, attacker, iHitflag, blockType::timed);
	}
	
	Utils::damageav(blocker, RE::ActorValue::kStamina, 
		realDamage * calculateBlockStaminaCostMult(blocker, attacker, iHitflag) * settings::fPerfectBlockStaminaCostMult);
	//damage blocker's stamina

}
#pragma endregion

void blockHandler::playBlockSFX(RE::Actor* blocker, int iHitflag, blockType blockType) {
	if (iHitflag & (int)RE::HitData::Flag::kBlockWithWeapon) {
		switch (blockType) {
		case blockType::guardBreaking: ValhallaUtils::playSound(blocker, data::soundParryWeapon_gbD->GetFormID()); break;
		case blockType::timed: ValhallaUtils::playSound(blocker, data::soundParryWeaponD->GetFormID()); break;
		}
	}
	else {
		switch (blockType) {
		case blockType::guardBreaking: ValhallaUtils::playSound(blocker, data::soundParryShield_gbD->GetFormID()); break;
		case blockType::timed: ValhallaUtils::playSound(blocker, data::soundParryShieldD->GetFormID()); break;
		}
	}
}
void blockHandler::playBlockVFX(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, blockType blockType) {
	MaxsuBlockSpark::blockSpark::GetSingleton()->playPerfectBlockSpark(aggressor, blocker);
}
void blockHandler::playBlockScreenShake(RE::Actor* blocker, int iHitflag, blockType blockType) {
	switch (blockType) {
	case blockType::guardBreaking:RE::Offset::shakeCamera(1.7, RE::PlayerCharacter::GetSingleton()->GetPosition(), 0.8f); break;
	case blockType::timed: RE::Offset::shakeCamera(1.5, RE::PlayerCharacter::GetSingleton()->GetPosition(), 0.3f); break;
	}
}

void blockHandler::playerBlockSlowTime(blockType blockType) {
	if (blockType != blockType::guardBreaking && settings::bPerfectBlockSlowTime_GuardBreakOnly) {
		return;
	}
	//DEBUG("start slow!");
	Utils::SGTM(0.1); 
	auto resetSlowTime = [](int stopTime_MS) {
		std::this_thread::sleep_for(std::chrono::milliseconds(stopTime_MS));
		Utils::SGTM(1);
	};
	float a_time;
	switch (blockType) {
	case blockType::guardBreaking: a_time = 500; break;
	case blockType::timed: a_time = 200; break;
	case blockType::perfect: a_time = 300; break;
	}
	std::jthread t(resetSlowTime, a_time);
	t.detach();
}

void blockHandler::playeBlockEffects(RE::Actor* blocker, RE::Actor* attacker, int iHitFlag, blockType blockType) {
	DEBUG("playing effects");
	if (settings::bPerfectBlockVFX) {
		playBlockVFX(blocker, attacker, iHitFlag, blockType);
	}
	if (settings::bPerfectBlockScreenShake
		&&(blocker->IsPlayerRef() || attacker->IsPlayerRef())) {
		playBlockScreenShake(blocker, iHitFlag, blockType);
	}
	if (settings::bPerfectBlockSFX) {
		playBlockSFX(blocker, iHitFlag, blockType);
	}
	if (settings::bPerfectBlockSlowTime
		&&(attacker->IsPlayerRef() || blocker->IsPlayerRef())) {
		playerBlockSlowTime(blockType);
	}
}