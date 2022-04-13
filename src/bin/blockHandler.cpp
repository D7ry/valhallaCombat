#include "include/blockHandler.h"
#include "include/data.h"
#include "include/settings.h"
#include "include/Utils.h"
#include "include/stunHandler.h"
#include "include/hitProcessor.h"
#include "include/reactionHandler.h"
#include "include/lib/BlockSpark.h"
#include "include/offsets.h"
#include "ValhallaCombat.hpp"
/*Called every frame.
Decrement the timer for actors either perfect blocking or cooling down.*/
void blockHandler::update() {
	auto deltaTime = *RE::Offset::g_deltaTime;
	mtx.lock();
	auto it1 = actorsPerfectBlocking.begin();
	while (it1 != actorsPerfectBlocking.end()) {
		auto actor = it1->first;
		if (it1->second <= 0) {
			//DEBUG("{}'s perfect block has ended, starting cool down", actor->GetName());
			it1 = actorsPerfectBlocking.erase(it1);
			//Cool down applies if the previous block fails; if successful no cooldown.
			if (actorsPerfectblockSuccessful.find(actor) == actorsPerfectblockSuccessful.end()) {
				//start cooling down
				actorsInBlockingCoolDown[actor] = settings::fPerfectBlockCoolDownTime;
				//actorsInBlockingCoolDown.emplace(actor, settings::fPerfectBlockCoolDownTime);
			}
			else {
				actorsPerfectblockSuccessful.erase(actor);
				if (actorsPerfectBlocking.size() == 0
					&& actorsInBlockingCoolDown.size() == 0) {
					ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::blockHandler);
				}
			}
			continue;
		}
		it1->second -= deltaTime;
		++it1;
	}
	auto it2 = actorsInBlockingCoolDown.begin();
	while (it2 != actorsInBlockingCoolDown.end()) {
		auto actor = it2->first;
		if (it2->second <= 0) {
			//exit cooling down phase
			//DEBUG("{}'s cool down has ended", actor->GetName());
			it2 = actorsInBlockingCoolDown.erase(it2);
			if (actorsPerfectBlocking.size() == 0
				&& actorsInBlockingCoolDown.size() == 0) {
				ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::blockHandler);
			}
			continue;
		}
		it2->second -= deltaTime;
		++it2;
	}
	mtx.unlock();
}

/*Register a perfect block when an actor tries to block. Put the blocker into the active perfect blocker set and start timing.
@param actor actor whose block is registered as a perfect block.*/
void blockHandler::registerPerfectBlock(RE::Actor* actor) {
	//DEBUG("Registering perfect block for {}", actor->GetName());
	mtx.lock();
	if (actorsPerfectblockSuccessful.find(actor) != actorsPerfectblockSuccessful.end()) { //has previously done a successful perfect block
		actorsPerfectblockSuccessful.erase(actor); //remove from the successful map.
		actorsPerfectBlocking[actor] = settings::fPerfectBlockTime; //reset perfect blocking timer
		ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::blockHandler);
	}
	else if (actorsInBlockingCoolDown.find(actor) == actorsInBlockingCoolDown.end() //OR not cooling down
		&& actorsPerfectBlocking.find(actor) == actorsPerfectBlocking.end()) { //and not currently perfect blocking
		actorsPerfectBlocking[actor] = settings::fPerfectBlockTime;
		ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::blockHandler);
		//actorsPerfectBlocking.emplace(actor, settings::fPerfectBlockTime);
	}
	mtx.unlock();
}
/*Make an actor break their guard through a animation event.*/


#pragma region Process Block
bool blockHandler::processBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData, float realDamage) {
	DEBUG("Process blocking. Blocker: {} Aggressor: {}", blocker->GetName(), aggressor->GetName());
	if (settings::bPerfectBlockToggle) {
		if (actorsPerfectBlocking.find(blocker) != actorsPerfectBlocking.end()) {
			processPerfectBlock(blocker, aggressor, iHitflag, hitData);
			return true;
		}
	}
	if (settings::bBlockStaminaToggle) {
		processStaminaBlock(blocker, aggressor, iHitflag, hitData, realDamage);
	}
	return false;
}
/*Process a stamina block.
Actor with enough stamina can negate all incoming health damage with stamina. Actor without enough stamina will triggerStagger and receive partial damage.*/
void blockHandler::processStaminaBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData, float realDamage) {
	DEBUG("processing stamina block");
	using HITFLAG = RE::HitData::Flag;
	//float staminaDamageBase = hitData.totalDamage;
	float staminaDamageBase = realDamage;
	float staminaDamageMult;
	DEBUG("base stamina damage is {}", staminaDamageBase);
	if (iHitflag & (int)HITFLAG::kBlockWithWeapon) {
		DEBUG("hit blocked with weapon");
		if (blocker->IsPlayerRef()) {
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
		if (blocker->IsPlayerRef()) {
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

	float targetStamina = blocker->GetActorValue(RE::ActorValue::kStamina);

	//check whether there's enough stamina to block incoming attack
	if (targetStamina < staminaDamage) {
		DEBUG("not enough stamina to block, blocking part of damage!");
		if (settings::bGuardBreak) {
			if (iHitflag & (int)HITFLAG::kPowerAttack) {
				reactionHandler::triggerReaction(aggressor, blocker, reactionHandler::kLarge);
			}
			else {
				reactionHandler::triggerReaction(aggressor, blocker, reactionHandler::kMedium);
			}
		}
		hitData.totalDamage =
			(realDamage - (targetStamina / staminaDamageMult)) //real damage actor will be receiving.
			* (hitData.totalDamage) / realDamage; //offset real damage back into raw damage to be converted into real damage again later.
		Utils::damageav(blocker, RE::ActorValue::kStamina,
			targetStamina);
		DEBUG("failed to block {} damage", hitData.totalDamage);
		debuffHandler::GetSingleton()->initStaminaDebuff(blocker); //initialize debuff for the failed blocking attempt
	}
	else {
		hitData.totalDamage = 0;
		Utils::damageav(blocker, RE::ActorValue::kStamina,
			staminaDamage);
	}
}

/*Process a perfect block.
Play block spark effects & screen shake effects if enabled.
Decrement aggressor's stamina.
The blocker will not receive any block cooldown once the block timer ends, and may initialize another perfect block as they wish.
Real damage from previous function is not passed in; instead it's being readjusted due to the swap of roles of attacker&defender.*/
void blockHandler::processPerfectBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData) {
	DEBUG("Perfect Block!");
	float reflectedDamage = hitData.totalDamage;
	//when reflecting damage, blocker is the real "attacker". So the damage is readjusted here.
	if (blocker->IsPlayerRef() || blocker->IsPlayerTeammate()) {
		Utils::offsetRealDamage(reflectedDamage, true);
	}
	else if (aggressor->IsPlayerRef() || aggressor->IsPlayerTeammate()) {
		Utils::offsetRealDamage(reflectedDamage, false);
	}
	stunHandler::GetSingleton()->calculateStunDamage(stunHandler::STUNSOURCE::parry, nullptr, blocker, aggressor, reflectedDamage);
	hitData.totalDamage = 0;
	bool blockBrokeGuard = false;
	if (aggressor->GetActorValue(RE::ActorValue::kStamina) <= 0) {
		reactionHandler::triggerReaction(blocker, aggressor, reactionHandler::kLarge);
		blockBrokeGuard = true;
	}
	if (settings::bPerfectBlockVFX) {
		DEBUG("vfx");
		playPerfectBlockVFX(blocker, aggressor, iHitflag, blockBrokeGuard);
	}
	if ((blocker->IsPlayerRef() || aggressor->IsPlayerRef())
		&& settings::bPerfectBlockScreenShake) {
		DEBUG("screen shake");
		playPerfectBlockScreenShake(blocker, iHitflag, blockBrokeGuard);
	}
	if (settings::bPerfectBlockSFX) {
		DEBUG("SFX");
		playPerfectBlockSFX(blocker, iHitflag, blockBrokeGuard);
	}
	mtx.lock();
	actorsPerfectblockSuccessful.emplace(blocker); //register the blocker as a successful blocker.
	mtx.unlock();
	DEBUG("perfect block process complete");
}
#pragma endregion

void blockHandler::playPerfectBlockSFX(RE::Actor* blocker, int iHitflag, bool blockBrokeGuard) {
	if (iHitflag & (int)RE::HitData::Flag::kBlockWithWeapon) {
		if (blockBrokeGuard) {
			ValhallaUtils::playSound(blocker, data::GetSingleton()->soundParryWeapon_gbD->GetFormID());
		}
		else {
			ValhallaUtils::playSound(blocker, data::GetSingleton()->soundParryWeaponD->GetFormID());
			//RE::BSAudioManager::GetSingleton()->Play(data::soundParryWeaponD);
		}
	}
	else {
		if (blockBrokeGuard) {
			ValhallaUtils::playSound(blocker, data::GetSingleton()->soundParryShield_gbD->GetFormID());
			//RE::BSAudioManager::GetSingleton()->Play(data::soundParryShield_gbD);
		}
		else {
			ValhallaUtils::playSound(blocker, data::GetSingleton()->soundParryShieldD->GetFormID());
			//RE::BSAudioManager::GetSingleton()->Play(data::soundParryWeaponD);
		}
	}
}
void blockHandler::playPerfectBlockVFX(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, bool blockBrokeGuard) {
	MaxsuBlockSpark::blockSpark::GetSingleton()->playPerfectBlockSpark(aggressor, blocker);
}
void blockHandler::playPerfectBlockScreenShake(RE::Actor* blocker, int iHitflag, bool blockBrokeGuard) {
	if (blockBrokeGuard) {
		RE::Offset::shakeCamera(1.8, RE::PlayerCharacter::GetSingleton()->GetPosition(), 0.5f);
	}
	else {
		RE::Offset::shakeCamera(1, RE::PlayerCharacter::GetSingleton()->GetPosition(), 0.3f);
	}
}