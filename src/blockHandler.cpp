#include "blockHandler.h"
#include "data.h"
#include "Utils.h"
#include "stunHandler.h"
#include "include/BlockSpark.h"
#include "RE/B/BGSSoundDescriptorForm.h"
#include "RE/B/BSAudioManager.h"
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
			//Cool down applies if the previous block fails; if successful no cooldown.
			if (actorsPerfectblockSuccessful.find(actor) == actorsPerfectblockSuccessful.end()) {
				//start cooling down
				actorsInBlockingCoolDown.emplace(actor, settings::fPerfectBlockCoolDownTime);
			}
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
		&& actorsPerfectBlocking.find(actor) == actorsPerfectBlocking.end()) { //and not currently perfect blocking
		actorsPerfectBlocking.emplace(actor, settings::fPerfectBlockTime);
		DEBUG("perfect block registered");
	}
	else if (actorsPerfectblockSuccessful.find(actor) != actorsPerfectblockSuccessful.end()){ //OR has previously done a successful perfect block
		DEBUG("previous perfect block successful, registering a new perfect block!");
		actorsPerfectblockSuccessful.erase(actor); //remove from the successful map.
		actorsInBlockingCoolDown.erase(actor); //remove from cooldown map
		actorsPerfectBlocking.erase(actor); //remove from perfect blocking map
		actorsPerfectBlocking.emplace(actor, settings::fPerfectBlockTime);
		DEBUG("perfect block registered");
	}
	else {
		DEBUG("{} is either perfect blocking or cooling down", actor->GetName());
	}
}
/*Make an actor break their guard through a animation event.*/
<<<<<<< HEAD
<<<<<<< HEAD
void blockHandler::guardBreak(RE::Actor* actor, RE::Actor* actorToPush) {
	RE::NiPoint3 vec = actor->GetPosition();
	Utils::pushActorAway(actor->currentProcess, actorToPush, vec, 7);
=======
void blockHandler::guardBreak(RE::Actor* actor) {
	auto formID = actor->GetFormID();
	DEBUG("form ID is {}", formID);
	std::stringstream sstream;
	sstream << std::hex << formID;
	std::string result = sstream.str();
	DEBUG(result);
	std::string cmd1 = "player.pushactoraway";
	DEBUG(cmd1);
	std::string cmd2 = ' ' + result + ' ' + "10";
	DEBUG(cmd2);
	std::string cmd = cmd1 + cmd2;
	DEBUG(cmd);
	Utils::sendConsoleCommand(cmd);
>>>>>>> parent of 53af9c6 (new guardbreak function)
=======
void blockHandler::guardBreak(RE::Actor* actor, RE::Actor* aggressor) {
	RE::NiPoint3 vec = actor->GetPosition();
	Utils::pushActorAway(actor->currentProcess, aggressor, vec, 7);
>>>>>>> parent of dfef385 (buggy shit)
}

bool blockHandler::processBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData, float realDamage) {
	DEBUG("Process blocking. Blocker: {} Aggressor: {}", blocker->GetName(), aggressor->GetName());
	if (settings::bPerfectBlocking && actorsPerfectBlocking.find(blocker) != actorsPerfectBlocking.end()) {
		processPerfectBlock(blocker, aggressor, iHitflag, hitData, realDamage);
		return true;
	}
	else if (settings::bStaminaBlocking) {
		processStaminaBlock(blocker, aggressor, iHitflag, hitData, realDamage);
		return false;
	}
	return false;
}
/*Process a stamina block. 
Actor with enough stamina can negate all incoming health damage with stamina. Actor without enough stamina will stagger and receive partial damage.*/
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
<<<<<<< HEAD
<<<<<<< HEAD
			guardBreak(aggressor, blocker);
=======
			guardBreak(blocker);
>>>>>>> parent of 53af9c6 (new guardbreak function)
=======
			guardBreak(blocker, aggressor);
>>>>>>> parent of dfef385 (buggy shit)
		}
		hitData.totalDamage = 
			hitData.totalDamage - ((targetStamina / staminaDamageMult) * (hitData.totalDamage/realDamage)); //offset real damage back into hit data.
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
void blockHandler::processPerfectBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData, float realDamage) {
	DEBUG("Perfect Block!");
	//float stunDmg = hitData.totalDamage;
	stunHandler::GetSingleton()->calculateStunDamage(stunHandler::STUNSOURCE::parry, nullptr, blocker, aggressor, hitData.totalDamage);
	hitData.totalDamage = 0;
	//guardBreak(aggressor);
	if (settings::bPerfectBlockingVFX) {
		DEBUG("playing perfect block vfx!");
		MaxsuBlockSpark::blockSpark::GetSingleton()->playPerfectBlockSpark(aggressor, blocker);
	}
	if ((blocker->IsPlayerRef() || aggressor->IsPlayerRef())
		&& settings::bPerfectBlockingScreenShake) {
			Utils::shakeCamera(1, RE::PlayerCharacter::GetSingleton()->GetPosition(), 0.3f);
	}
	if (settings::bPerfectBlockingSFX && (blocker->IsPlayerRef() || aggressor->IsPlayerRef())) {
		DEBUG("playing perfect block sfx!");
		if (iHitflag & (int)RE::HitData::Flag::kBlockWithWeapon) {
			if (RE::BSAudioManager::GetSingleton()->Play(data::GetSingleton()->soundParryWeaponD)) {
				DEBUG("play success!");
			}
		}
		else {
			if (RE::BSAudioManager::GetSingleton()->Play(data::GetSingleton()->soundParryShieldD)) {
				DEBUG("play success!");
			}
		}

	}
	actorsPerfectblockSuccessful.emplace(blocker); //register the blocker as a successful blocker.
	/*if (aggressor->GetActorValue(RE::ActorValue::kStamina) <= 0) {
		DEBUG("guard break!");
<<<<<<< HEAD
<<<<<<< HEAD
		guardBreak(blocker, aggressor);
=======
		guardBreak(aggressor, blocker);
>>>>>>> parent of dfef385 (buggy shit)
	}
=======
		guardBreak(aggressor);
	}*/
>>>>>>> parent of 53af9c6 (new guardbreak function)
}
