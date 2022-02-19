#include "executionHandler.h"
#include "data.h"
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
	DEBUG("calculating victim skeleton");
	switch (victim->GetRace()->bodyPartData->GetFormID()) {
	case uIntBodyPartData_Humanoid: //for humanoids, determine relative angle to trigger backstab.
		DEBUG("humanoid skeleton");
		DEBUG("angle is {}", victim->GetHeadingAngle(executer->GetPosition(), false));
		auto angle = victim->GetHeadingAngle(executer->GetPosition(), false);
		if (90 < angle || angle < -90) {
			sendExecutionCommand(executer, victim, kmStr_Humanoid_1hm_Back);
		}
		else {
			sendExecutionCommand(executer, victim, kmStr_Humanoid_1hm_Front);
		}
		
	}
	//sendExecutionCommand(executer, victim, kmStr_Humanoid_1hm_Front);
}

void executionHandler::execute(RE::Actor* executer, RE::Actor* victim, EXECUTIONTYPE executionType) {
	switch (executionType) {
	case EXECUTIONTYPE::humanoid:
		sendExecutionCommand(executer, victim, kmStr_Humanoid_1hm_Front);
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