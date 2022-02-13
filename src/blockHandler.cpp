#include "blockHandler.h"
#include "data.h"
#include "Utils.h"
#include "include/BlockSpark.h"
/*Called every frame. 
Decrement the timer for actors either perfect blocking or cooling down.*/
void blockHandler::update() {
	auto it1 = actorsPerfectBlocking.begin();
	while (it1 != actorsPerfectBlocking.end()) {
		auto actor = it1->first;
		if (!actor) {
			it1 = actorsPerfectBlocking.erase(it1);
			continue;
		}
		if (it1->second <= 0) {
			DEBUG("{}'s perfect block has ended, starting cool down", actor->GetName());
			it1 = actorsPerfectBlocking.erase(it1);
			//start cooling down phase
			actorsInBlockingCoolDown.emplace(actor, settings::fPerfectBlockCoolDownTime);
			continue;
		}
		it1->second -= *Utils::g_deltaTimeRealTime;
		++it1;
	}

	auto it2 = actorsInBlockingCoolDown.begin();
	while (it2 != actorsInBlockingCoolDown.end()) {
		auto actor = it2->first;
		if (!actor) {
			it2 = actorsInBlockingCoolDown.erase(it2);
			continue;
		}
		if (it2->second <= 0) {
			//exit cooling down phase
			DEBUG("{}'s cool down has ended", actor->GetName());
			it2 = actorsInBlockingCoolDown.erase(it2);
			continue;
		}
		it2->second -= *Utils::g_deltaTimeRealTime;
		++it2;
	}
}

/*Register a perfect block when an actor tries to block. Put the blocker into the active perfect blocker set and start timing.
@param actor actor whose block is registered as a perfect block.*/
void blockHandler::registerPerfectBlock(RE::Actor* actor) {
	DEBUG("Registering perfect block for {}", actor->GetName());
	/*bool a = actorsInBlockingCoolDown.find(actor) == actorsInBlockingCoolDown.end();
	DEBUG(a);
	bool b = actorsPerfectBlocking.find(actor) == actorsPerfectBlocking.end();
	DEBUG(b);*/
	if (actorsInBlockingCoolDown.find(actor) == actorsInBlockingCoolDown.end() //not cooling down
		&& actorsPerfectBlocking.find(actor) == actorsPerfectBlocking.end()) { //and not perfect blocking right now
		actorsPerfectBlocking.emplace(actor, settings::fPerfectBlockTime);
		DEBUG("perfect block registered");
	}
	else if (actorsSuccessful.find(actor) != actorsSuccessful.end()){ //has previously done a successful perfect block
		DEBUG("previous perfect block successful, registering a new perfect block!");
		actorsSuccessful.erase(actor);
		actorsInBlockingCoolDown.erase(actor); //remove from cooldown map
		actorsPerfectBlocking.erase(actor); //remove from perfect blocking map
		actorsPerfectBlocking.emplace(actor, settings::fPerfectBlockTime);
		DEBUG("perfect block registered");
	}
	else {
		DEBUG("{} is either perfect blocking or cooling down", actor->GetName());
	}
}


bool blockHandler::processBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData) {
	DEBUG("Process blocking. Blocker: {} Aggressor: {}", blocker->GetName(), aggressor->GetName());
	if (settings::bPerfectBlocking && actorsPerfectBlocking.find(blocker) != actorsPerfectBlocking.end()) {
		DEBUG("Process perfect block.");
		processPerfectBlock(blocker, aggressor, iHitflag, hitData);
		return true;
	}
	else if (settings::bStaminaBlocking) {
		DEBUG("Process stamina block.");
		processStaminaBlock(blocker, aggressor, iHitflag, hitData);
		return false;
	}
	return false;
}
/*Process a stamina block. 
Actor with enough stamina can negate all incoming health damage with stamina. Actor without enough stamina will stagger and receive partial damage.*/
void blockHandler::processStaminaBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData) {
	using HITFLAG = RE::HitData::Flag;
	float staminaDamageBase = hitData.totalDamage;
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
			if (settings::bPoiseCompatibility) {
				blocker->NotifyAnimationGraph("poise_largest_start");
			}
			else {
				blocker->NotifyAnimationGraph("staggerStart");
			}
		}
		hitData.totalDamage = hitData.totalDamage - (targetStamina / staminaDamageMult);
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
The blocker will not receive any block cooldown once the block timer ends, and may initialize another perfect block as they wish.*/
void blockHandler::processPerfectBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData) {
	hitData.totalDamage = 0;
	DEBUG("Perfect Block!");
	if (blocker->IsPlayerRef() || aggressor->IsPlayerRef()) {
		if (settings::bPerfectBlockingScreenShake) {
			Utils::shakeCamera(1, RE::PlayerCharacter::GetSingleton()->GetPosition(), 0.3f);
		}
		if (settings::bPerfectBlockingVFX) {
			MaxsuBlockSpark::blockSpark::GetSingleton()->playPerfectBlockSpark(aggressor, blocker);
		}
	}
	actorsSuccessful.emplace(blocker); //register the blocker as a successful blocker.
}