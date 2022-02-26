#include "executionHandler.h"
#include "data.h"
using namespace Utils;
void executionHandler::attemptExecute(RE::Actor* executor, RE::Actor* victim) {
	DEBUG("attempting to execute {}, executor: {}", victim->GetName(), executor->GetName());

	//check if victim can be executed
	if (!settings::bStunToggle
		|| executor->IsDead() || victim->IsDead()
		|| !executor->Is3DLoaded() || !victim->Is3DLoaded()
		|| executor->IsInKillMove() || victim->IsInKillMove()
		|| (!settings::bPlayerExecution && (victim->IsPlayerTeammate() || victim->IsPlayer()))
		|| (!settings::bEssentialExecution && victim->IsEssential())
		|| !executor->GetRace() || executor->GetRace()->bodyPartData->GetFormID() != 29 //executor can only be human.
		|| !victim->GetRace() || !victim->GetRace()->bodyPartData
		|| victim->HasEffectWithArchetype(RE::MagicTarget::Archetype::kParalysis)) {
		DEBUG("Execution preconditions not met, terminating execution.");
		return;
	}

	if (activeExecutor.find(executor) != activeExecutor.end() //executor is executing
		|| activeExecutor.find(victim) != activeExecutor.end()) { //victim is executing
		return;
	}

	//debug body data
	DEBUG("executor body part data: {}", executor->GetRace()->bodyPartData->GetFormID());
	DEBUG("victim body part data: {}", victim->GetRace()->bodyPartData->GetFormID());
	RE::WEAPON_TYPE weaponType;
	auto weapon = Utils::getWieldingWeapon(executor);
	if (!weapon) {
		DEBUG("Executor weapon not found, using unarmed as weapon type.");
		weaponType = RE::WEAPON_TYPE::kHandToHandMelee;
	}
	else {
		weaponType = weapon->GetWeaponType();
	}
	if (weaponType == RE::WEAPON_TYPE::kBow || weaponType == RE::WEAPON_TYPE::kCrossbow) {
		return;
	}
	DEBUG("weapon type is {}", weaponType);
	DEBUG("victim body part is {}", victim->GetRace()->bodyPartData->GetFormID());
	switch (victim->GetRace()->bodyPartData->GetFormID()) {
	case uIntBodyPartData_Humanoid: executeHumanoid(executor, victim, weaponType); break;
	case uIntBodyPartData_Draugr: executeDraugr(executor, victim, weaponType); break;
	case uIntBodyPartData_Falmer: executeFalmer(executor, victim, weaponType); break;
	case uIntBodyPartData_FrostbiteSpider: executeSpider(executor, victim, weaponType); break;
	case uIntBodyPartData_Gargoyle: executeGargoyle(executor, victim, weaponType); break;
	case uIntBodyPartData_Giant: executeGiant(executor, victim, weaponType); break;
	case uIntBodyPartData_Bear: executeBear(executor, victim, weaponType); break;
	case uIntBodyPartData_SabreCat: executeSabreCat(executor, victim, weaponType); break;
	case uIntBodyPartData_Dog: executeWolf(executor, victim, weaponType); break;
	case uIntBodyPartData_Troll: executeTroll(executor, victim, weaponType); break;
	case uIntBodyPartData_Hagraven: executeHagraven(executor, victim, weaponType); break;
	case uIntBodyPartData_Spriggan: executeSpriggan(executor, victim, weaponType); break;
		//FIXME: boar uses default body part wtf
	case uIntBodyPartData_DLC2_Riekling: executeRiekling(executor, victim, weaponType); break;
	case uIntBodyPartData_DLC2_Scrib: executeAshHopper(executor, victim, weaponType); break;
	case uIntBodyPartData_DwarvenSteamCenturion: executeSteamCenturion(executor, victim, weaponType); break;
	case uIntBodyPartData_DwarvenBallistaCenturion: executeDwarvenBallista(executor, victim, weaponType); break;
	case uIntBodyPartData_ChaurusFlyer: executeChaurusFlyer(executor, victim, weaponType); break;
	case uIntBodyPartData_DLC2_BenthicLurker: executeLurker(executor, victim, weaponType); break;
	case uIntBodyPartData_Dragon: executeDragon(executor, victim, weaponType); break;
	default: return; //iff no body part match, no need to set ghost.
	}


	/*Set the executor as ghost and start tracking them.*/
	setIsGhost(executor, true);
	activeExecutor.emplace(executor);
	victim->SetGraphVariableBool("bIdlePlaying", true);
	//sendExecutionCommand(executor, victim, kmStr_Humanoid_1hm_Front);
};

void executionHandler::concludeExecution(RE::Actor* executor) {
	if (activeExecutor.find(executor) != activeExecutor.end()) {
		Utils::setIsGhost(executor, false);
		activeExecutor.erase(executor);
	}
}



void executionHandler::executeHumanoid(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	auto gameData = gameDataCache::GetSingleton();
	if (executor->isDualWielding()) {
		playExecutionIdle(executor, victim, gameData->KM_Humanoid_DW);
	}
	else if (isBackFacing(victim, executor)) {
		switch (weaponType) {
		case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, gameData->KM_Humanoid_2hw_Back); break;
		case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, gameData->KM_Humanoid_2hm_Back); break;
		case RE::WEAPON_TYPE::kHandToHandMelee: playExecutionIdle(executor, victim, gameData->KM_Humanoid_H2H_Back); break;
		default: playExecutionIdle(executor, victim, gameData->KM_Humanoid_1hm_Back); break;
		}
	}
	else {
		switch (weaponType) {
		case RE::WEAPON_TYPE::kOneHandSword: playExecutionIdle(executor, victim, gameData->KM_Humanoid_Sword); break;
		case RE::WEAPON_TYPE::kOneHandDagger: playExecutionIdle(executor, victim, gameData->KM_Humanoid_Dagger); break;
		case RE::WEAPON_TYPE::kOneHandAxe:  playExecutionIdle(executor, victim, gameData->KM_Humanoid_Axe); break;
		case RE::WEAPON_TYPE::kOneHandMace:  playExecutionIdle(executor, victim, gameData->KM_Humanoid_Mace); break;
		case RE::WEAPON_TYPE::kTwoHandAxe:  playExecutionIdle(executor, victim, gameData->KM_Humanoid_2hw); break;
		case RE::WEAPON_TYPE::kTwoHandSword:  playExecutionIdle(executor, victim, gameData->KM_Humanoid_GreatSword); break;
		case RE::WEAPON_TYPE::kHandToHandMelee:  playExecutionIdle(executor, victim, gameData->KM_Humanoid_H2H); break;
		}
	}
};
void executionHandler::executeDraugr(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Undead_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Undead_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: sendExecutionCommand(executer, victim, kmStr_Undead_1hm); break;
	}
};
void executionHandler::executeFalmer(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Falmer_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Falmer_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: sendExecutionCommand(executer, victim, kmStr_Falmer_1hm);
	}
};
void executionHandler::executeSpider(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Spider_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Spider_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: sendExecutionCommand(executer, victim, kmStr_Spider_1hm);
	}
}
void executionHandler::executeGargoyle(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	if (isBackFacing(victim, executer)) {
		sendExecutionCommand(executer, victim, kmStr_Gargoyle_1hm_Back);
		return;
	}
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Gargoyle_2hm); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Gargoyle_2hw); break;
	default: sendExecutionCommand(executer, victim, kmStr_Gargoyle_1hm); break;
	}
}
void executionHandler::executeGiant(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Giant_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Giant_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_Giant_1hm); break;
	}
}
void executionHandler::executeBear(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Bear_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Bear_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_Bear_1hm); break;
	}
}
void executionHandler::executeSabreCat(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_SabreCat_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_SabreCat_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_SabreCat_1hm); break;
	}
}
void executionHandler::executeWolf(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Wolf_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Wolf_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_Wolf_1hm); break;
	}
}
void executionHandler::executeTroll(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Troll_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Troll_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_Troll_1hm); break;
	}
}
void executionHandler::executeHagraven(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Hagraven_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Hagraven_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_Hagraven_1hm); break;
	}
}
void executionHandler::executeSpriggan(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Spriggan_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Spriggan_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_Spriggan_1hm); break;
	}
}
void executionHandler::executeBoar(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Boar_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Boar_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_Boar_1hm); break;
	}
}
void executionHandler::executeRiekling(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Riekling_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Riekling_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_Riekling_1hm); break;
	}
}
void executionHandler::executeAshHopper(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_AshHopper_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_AshHopper_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_AshHopper_1hm); break;
	}
}
void executionHandler::executeSteamCenturion(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_SteamCenturion_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_SteamCenturion_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_SteamCenturion_1hm); break;
	}
}
void executionHandler::executeDwarvenBallista(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_DwarvenBallista_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_DwarvenBallista_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_DwarvenBallista_1hm); break;
	}
}
void executionHandler::executeChaurusFlyer(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_ChaurusFlyer_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_ChaurusFlyer_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_ChaurusFlyer_1hm); break;
	}
}
void executionHandler::executeLurker(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Lurker_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Lurker_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_Lurker_1hm); break;
	}
}
void executionHandler::executeDragon(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Dragon_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Dragon_2hm); break;
	default: sendExecutionCommand(executer, victim, kmStr_Dragon_1hm); break;
	}
}


void executionHandler::playExecutionIdle(RE::Actor* executor, RE::Actor* victim, RE::TESIdleForm* executionIdle) {
	playPairedIdle(executor->currentProcess, executor, RE::DEFAULT_OBJECT::kActionIdle, executionIdle, true, false, victim);
}

void executionHandler::playExecutionIdle(RE::Actor* executor, RE::Actor* victim, std::vector<RE::TESIdleForm*> executionIdleV) {
	DEBUG("playing execution idle!");
	DEBUG(executionIdleV.size());
	if (executionIdleV.size() == 0) {
		DEBUG("error: no idle present in vector");
		return;
	}
	auto idle = getRandomIdle(executionIdleV);
	if (!idle) {
		DEBUG("Error! no idle received");
	}
	else {
		DEBUG("received idle with name {}", idle->GetName());
		playExecutionIdle(executor, victim, idle);
	}
	
}