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
	else {
		executerStr = '\"' + executerStr + '\"';
	}
	std::stringstream sstream2;
	sstream2 << std::hex << victimForm;
	std::string victimStr = sstream2.str();

	std::string cmd = executerStr  + ".playidle " + executionStr + " " + victimStr;
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

	//debug body data
	DEBUG("executer body part data: {}", executer->GetRace()->bodyPartData->GetFormID());
	DEBUG("victim body part data: {}", victim->GetRace()->bodyPartData->GetFormID());
	RE::WEAPON_TYPE weaponType;
	auto weapon = Utils::getWieldingWeapon(executer);
	if (!weapon) {
		DEBUG("Executor weapon not found, using unarmed as weapon type.");
		weaponType = RE::WEAPON_TYPE::kHandToHandMelee;
	}
	else {
		weaponType = weapon->GetWeaponType();
	}
	DEBUG("weapon type is {}", weaponType);
	DEBUG("victim body part is {}", victim->GetRace()->bodyPartData->GetFormID());
	switch (victim->GetRace()->bodyPartData->GetFormID()) {
	case uIntBodyPartData_Humanoid: executeHumanoid(executer, victim, weaponType); break;
	case uIntBodyPartData_Draugr: executeDraugr(executer, victim, weaponType); break;
	case uIntBodyPartData_Falmer: executeFalmer(executer, victim, weaponType); break;
	case uIntBodyPartData_FrostbiteSpider: executeSpider(executer, victim, weaponType); break;
	case uIntBodyPartData_Gargoyle: executeGargoyle(executer, victim, weaponType); break;
	case uIntBodyPartData_Giant: executeGiant(executer, victim, weaponType); break;
	case uIntBodyPartData_Bear: executeBear(executer, victim, weaponType); break;
	case uIntBodyPartData_SabreCat: executeSabreCat(executer, victim, weaponType); break;
	case uIntBodyPartData_Dog: executeWolf(executer, victim, weaponType); break;
	case uIntBodyPartData_Troll: executeTroll(executer, victim, weaponType); break;
	case uIntBodyPartData_Hagraven: executeHagraven(executer, victim, weaponType); break;
	case uIntBodyPartData_Spriggan: executeSpriggan(executer, victim, weaponType); break;
		//FIXME: boar uses default body part wtf
	case uIntBodyPartData_DLC2_Riekling: executeRiekling(executer, victim, weaponType); break;
	case uIntBodyPartData_DLC2_Scrib: executeAshHopper(executer, victim, weaponType); break;
	case uIntBodyPartData_DwarvenSteamCenturion: executeSteamCenturion(executer, victim, weaponType); break;
	case uIntBodyPartData_DwarvenBallistaCenturion: executeDwarvenBallista(executer, victim, weaponType); break;
	case uIntBodyPartData_ChaurusFlyer: executeChaurusFlyer(executer, victim, weaponType); break;
	case uIntBodyPartData_DLC2_BenthicLurker: executeLurker(executer, victim, weaponType); break;
	case uIntBodyPartData_Dragon: executeDragon(executer, victim, weaponType); break;
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