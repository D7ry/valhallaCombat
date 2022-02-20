#include "executionHandler.h"
#include "data.h"

void executionHandler::sendExecutionCommand(RE::Actor* executer, RE::Actor* victim, std::vector<std::string> executionStrSet) {
	auto executerForm = executer->GetFormID();
	auto victimForm = victim->GetFormID();

	//convert executer and victim's formID to hex.
	std::stringstream sstream1;
	sstream1 << std::hex << executerForm;
	std::string executerStr = sstream1.str();
	if (executerStr == "14") {
		executerStr = "player";
	}
	std::stringstream sstream2;
	sstream2 << std::hex << victimForm;
	std::string victimStr = sstream2.str();

	std::string executionStr = getRandomStr(executionStrSet); //get an random element from this execution string set.
	std::string cmd = executerStr + ".playidle " + executionStr + " " + victimStr;
	DEBUG("sending execution command: " + cmd);
	Utils::sendConsoleCommand(cmd);
}

void executionHandler::sendExecutionCommand(RE::Actor* executer, RE::Actor* victim, std::string executionStr) {
	auto executerForm = executer->GetFormID();
	auto victimForm = victim->GetFormID();

	//convert executer and victim's formID to hex.
	std::stringstream sstream1;
	sstream1 << std::hex << executerForm;
	std::string executerStr = sstream1.str();
	if (executerStr == "14") {
		executerStr = "player";
	}
	std::stringstream sstream2;
	sstream2 << std::hex << victimForm;
	std::string victimStr = sstream2.str();

	std::string cmd = executerStr + ".playidle " + executionStr + " " + victimStr;
	DEBUG("sending execution command: " + cmd);
	Utils::sendConsoleCommand(cmd);
}

void executionHandler::attemptExecute(RE::Actor* executer, RE::Actor* victim) {
	DEBUG("attempting to execute {}, executor: {}", victim->GetName(), executer->GetName());

	//check if victim can be executed
	if (!settings::bExecutionToggle
		|| executer->IsDead() || victim->IsDead()
		|| !executer->Is3DLoaded() || !victim->Is3DLoaded()
		|| executer->IsInKillMove() || victim->IsInKillMove()
		|| (victim->IsPlayer() && !settings::bPlayerExecution)
		|| (victim->IsPlayerTeammate() && !settings::bPlayerTeammateExecution)
		|| (victim->IsEssential() && !settings::bEssentialExecution)
		|| !executer->GetRace() || executer->GetRace()->bodyPartData->GetFormID() != 29 //executer can only be human.
		|| !victim->GetRace() || !victim->GetRace()->bodyPartData) {
		DEBUG("Execution preconditions not met, terminating execution.");
		return;
	}

	//DEBUGging
	DEBUG("executer body part data: {}", executer->GetRace()->bodyPartData->GetFormID());
	DEBUG("victim body part data: {}", victim->GetRace()->bodyPartData->GetFormID());
	//check relative position and rotation
	auto weapon = Utils::getWieldingWeapon(executer);
	if (!weapon) {
		DEBUG("Error: executer weapon not found");
		return;
	}
	RE::WEAPON_TYPE weaponType = weapon->GetWeaponType();
	DEBUG("weapon type is {}", weaponType);
	switch (victim->GetRace()->bodyPartData->GetFormID()) {
	case uIntBodyPartData_Draugr:
		DEBUG("execute draugr");
		executeDraugr(executer, victim);
		break;
	case uIntBodyPartData_Humanoid: //for humanoids, determine relative angle to trigger backstab.
		DEBUG("execute humanoid");
		executeHumanoid(executer, victim);
		break;
	case uIntBodyPartData_Giant:
		break;
	}

	//sendExecutionCommand(executer, victim, kmStr_Humanoid_1hm_Front);
};





void executionHandler::executeHumanoid(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	if (isBackFacing(victim, executer)) {
		switch (weaponType) {
		case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Humanoid_2hw_Back); break;
		case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Humanoid_2hm_Back); break;
		case RE::WEAPON_TYPE::kHandToHandMelee: sendExecutionCommand(executer, victim, kmStr_Humanoid_h2h_Back); break;
		default: sendExecutionCommand(executer, victim, kmStr_Humanoid_1hm_Back); break;
		}
	}
	else {
		switch (weaponType) {
		case RE::WEAPON_TYPE::kOneHandSword: sendExecutionCommand(executer, victim, kmStr_Humanoid_1hm_Sword); break;
		case RE::WEAPON_TYPE::kOneHandDagger: sendExecutionCommand(executer, victim, kmStr_Humanoid_1hm_Axe_Mace); break;
		case RE::WEAPON_TYPE::kOneHandAxe: sendExecutionCommand(executer, victim, kmStr_Humanoid_1hm_Axe_Mace); break;
		case RE::WEAPON_TYPE::kOneHandMace: sendExecutionCommand(executer, victim, kmStr_Humanoid_1hm_Axe_Mace); break;
		case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Humanoid_2hw); break;
		case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Humanoid_2hm); break;
		case RE::WEAPON_TYPE::kHandToHandMelee: sendExecutionCommand(executer, victim, kmStr_Humanoid_h2h); break;
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
	case RE::WEAPON_TYPE::kTwoHandAxe: sendExecutionCommand(executer, victim, kmStr_Falmer_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: sendExecutionCommand(executer, victim, kmStr_Falmer_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: sendExecutionCommand(executer, victim, kmStr_Falmer_1hm);
	}
}
void executionHandler::executeGargoyle(RE::Actor* executer, RE::Actor* victim) {

}
void executionHandler::executeGiant(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeBear(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeSabreCat(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeWolf(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeTroll(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeHagraven(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeSpriggan(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeBoar(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeRiekling(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeAshHopper(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeSteamCenturion(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeDwarvenBallista(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeChaurusFlyer(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeLurker(RE::Actor* executer, RE::Actor* victim);
void executionHandler::executeDragon(RE::Actor* executer, RE::Actor* victim);