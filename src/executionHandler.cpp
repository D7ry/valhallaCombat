#include "executionHandler.h"
#include "data.h"
void executionHandler::attemptExecute(RE::Actor* executer, RE::Actor* victim) {
	//check if victim can be executed
	if (!settings::bExecutionToggle
		|| executer->IsDead() || victim->IsDead()
		|| !executer->Is3DLoaded() || !victim->Is3DLoaded()
		|| executer->IsInKillMove() || victim->IsInKillMove()
		|| (victim->IsPlayer() && !settings::bPlayerExecution)
		|| (victim->IsPlayerTeammate() && !settings::bPlayerTeammateExecution)
		|| (victim->IsEssential() && !settings::bEssentialExecution)
		|| !executer->GetRace() || !executer->GetRace()->bodyPartData
		|| !victim->GetRace() || !victim->GetRace()->bodyPartData) {
		DEBUG("Execution preconditions not met, terminating execution.");
		return;
	}
	/*For debuf purposes*/
	DEBUG("executer body part data: {}", executer->GetRace()->bodyPartData->GetFormID());
	DEBUG("victim body part data: {}", victim->GetRace()->bodyPartData->GetFormID());
	//check relative position and rotation
	//execute()
	execute(executer, victim, EXECUTIONTYPE::humanoid);
}

void executionHandler::execute(RE::Actor* executer, RE::Actor* victim, EXECUTIONTYPE executionType) {
	switch (executionType) {
	case EXECUTIONTYPE::humanoid:
		sendExecutionCommand(executer, victim, kmStr_Humanoid_Front);
	}
}


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

	std::string executionStr = executionStrSet[rand() % executionStrSet.size()]; //get an random element from this execution string set.
	std::string cmd = executerStr + ".playidle " + executionStr + " " + victimStr;
	DEBUG("sending execution command: " + cmd);
	Utils::sendConsoleCommand(cmd);
}