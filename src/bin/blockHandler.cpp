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
			mtx_actors_PrevPerfectBlockingSuccessful.lock();
			if (!actors_PrevPerfectBlockingSuccessful.contains(actor)) {
				mtx_actors_PrevPerfectBlockingSuccessful.unlock();
				//start cooling down
				mtx_actors_BlockingCoolDown.lock();
				actors_BlockingCoolDown[actor] = settings::fPerfectBlockCoolDownTime;
				mtx_actors_BlockingCoolDown.unlock();
				//actorsInBlockingCoolDown.emplace(actor, settings::fPerfectBlockCoolDownTime);
			}
			else {
				actors_PrevPerfectBlockingSuccessful.erase(actor);
				mtx_actors_PrevPerfectBlockingSuccessful.unlock();

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
	mtx_actors_PrevPerfectBlockingSuccessful.lock();
	if (actors_PrevPerfectBlockingSuccessful.contains(actor)) { //has previously done a successful perfect block
		actors_PrevPerfectBlockingSuccessful.erase(actor); //remove from the successful map.
		mtx_actors_PrevPerfectBlockingSuccessful.unlock();

		mtx_actors_PerfectBlocking.lock();
		actors_PerfectBlocking[actor] = settings::fPerfectBlockTime; //reset perfect blocking timer
		mtx_actors_PerfectBlocking.unlock();
		ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::blockHandler);
		return;//register successful, no need to run the following.
	}
	else {
		mtx_actors_PrevPerfectBlockingSuccessful.unlock();
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
	if (settings::bPerfectBlockToggle) {
		mtx_actors_PerfectBlocking.lock();
		if (actors_PerfectBlocking.contains(blocker)) {
			mtx_actors_PerfectBlocking.unlock();
			processPerfectBlock(blocker, aggressor, iHitflag, hitData, realDamage);
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

static inline float calculateBlockStaminaCostMult(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag) {
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

	float staminaDamageMult = calculateBlockStaminaCostMult(blocker, aggressor, iHitflag);
	float staminaDamage = staminaDamageMult * realDamage;

	float targetStamina = blocker->GetActorValue(RE::ActorValue::kStamina);

	//check whether there's enough stamina to block incoming attack
	if (targetStamina < staminaDamage) {
		//DEBUG("not enough stamina to block, blocking part of damage!");
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
		//DEBUG("failed to block {} damage", hitData.totalDamage);
		debuffHandler::GetSingleton()->initStaminaDebuff(blocker); //initialize debuff for the failed blocking attempt
	}
	else {
		hitData.totalDamage = 0;
		Utils::damageav(blocker, RE::ActorValue::kStamina,
			staminaDamage);
	}
}


void blockHandler::processPerfectBlock(RE::Actor* blocker, RE::Actor* attacker, int iHitflag, RE::HitData& hitData, float realDamage) {
	float reflectedDamage = 0;
	auto a_weapon = blocker->getWieldingWeapon();
	if (a_weapon) {
		reflectedDamage = a_weapon->GetAttackDamage();//get attack damage of blocker's weapon
	}
	else {
		reflectedDamage = hitData.totalDamage;
	}
	Utils::offsetRealDamage(reflectedDamage, blocker, attacker);
	stunHandler::GetSingleton()->calculateStunDamage(stunHandler::STUNSOURCE::parry, nullptr, blocker, attacker, reflectedDamage);
	balanceHandler::GetSingleton()->calculateBalanceDamage(balanceHandler::DMGSOURCE::parry, nullptr, blocker, attacker, reflectedDamage);
	hitData.totalDamage = 0;
	mtx_actors_PrevPerfectBlockingSuccessful.lock();
	actors_PrevPerfectBlockingSuccessful.insert(blocker); //register the blocker as a successful blocker.
	mtx_actors_PrevPerfectBlockingSuccessful.unlock();
	if (balanceHandler::GetSingleton()->isBalanceBroken(attacker)
		|| stunHandler::GetSingleton()->isActorStunned(attacker)) {
		playerPerfectBlockEffects(blocker, attacker, iHitflag, true);
	}
	else {
		playerPerfectBlockEffects(blocker, attacker, iHitflag, false);
	}
	
	Utils::damageav(blocker, RE::ActorValue::kStamina, 
		realDamage * calculateBlockStaminaCostMult(blocker, attacker, iHitflag) * settings::fPerfectBlockStaminaCostMult);
	//damage blocker's stamina

}
#pragma endregion

void blockHandler::playPerfectBlockSFX(RE::Actor* blocker, int iHitflag, bool blockBrokeGuard) {
	if (iHitflag & (int)RE::HitData::Flag::kBlockWithWeapon) {
		if (blockBrokeGuard) {
			ValhallaUtils::playSound(blocker, data::soundParryWeapon_gbD->GetFormID());
		}
		else {
			ValhallaUtils::playSound(blocker, data::soundParryWeaponD->GetFormID());
			//RE::BSAudioManager::GetSingleton()->Play(data::soundParryWeaponD);
		}
	}
	else {
		if (blockBrokeGuard) {
			ValhallaUtils::playSound(blocker, data::soundParryShield_gbD->GetFormID());
			//RE::BSAudioManager::GetSingleton()->Play(data::soundParryShield_gbD);
		}
		else {
			ValhallaUtils::playSound(blocker, data::soundParryShieldD->GetFormID());
			//RE::BSAudioManager::GetSingleton()->Play(data::soundParryWeaponD);
		}
	}
}
void blockHandler::playPerfectBlockVFX(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, bool blockBrokeGuard) {
	MaxsuBlockSpark::blockSpark::GetSingleton()->playPerfectBlockSpark(aggressor, blocker);
}
void blockHandler::playPerfectBlockScreenShake(RE::Actor* blocker, int iHitflag, bool blockBrokeGuard) {
	if (blockBrokeGuard) {
		RE::Offset::shakeCamera(2.5, RE::PlayerCharacter::GetSingleton()->GetPosition(), 0.4f);
	}
	else {
		RE::Offset::shakeCamera(1, RE::PlayerCharacter::GetSingleton()->GetPosition(), 0.3f);
	}
}

void blockHandler::playerPerfectBlockSlowTime(bool blockBrokeGuard) {
	if (!blockBrokeGuard && settings::bPerfectBlockSlowTime_GuardBreakOnly) {
		return;//doesn't play slow time if the slow time is for guard break only.
	}
	Utils::SGTM(0.1); 
	auto resetSlowTime = [](int stopTime_MS) {
		std::this_thread::sleep_for(std::chrono::milliseconds(stopTime_MS));
		Utils::SGTM(1);
	};
	float a_time;
	if (blockBrokeGuard) {
		a_time = 0.5;
	}
	else {
		a_time = 0.3;
	}
	std::jthread t(resetSlowTime, a_time);
	t.detach();
}

void blockHandler::playerPerfectBlockEffects(RE::Actor* blocker, RE::Actor* attacker, int iHitFlag, bool blockBrokeGuard) {
	if (settings::bPerfectBlockVFX) {
		playPerfectBlockVFX(blocker, attacker, iHitFlag, blockBrokeGuard);
	}
	if ((blocker->IsPlayerRef() || attacker->IsPlayerRef())
		&& settings::bPerfectBlockScreenShake) {
		playPerfectBlockScreenShake(blocker, iHitFlag, blockBrokeGuard);
	}
	if (settings::bPerfectBlockSFX) {
		playPerfectBlockSFX(blocker, iHitFlag, blockBrokeGuard);
	}
	if ((attacker->IsPlayerRef() || blocker->IsPlayerRef())
		&& settings::bPerfectBlockSlowTime) {
		playerPerfectBlockSlowTime(blockBrokeGuard);
	}
}