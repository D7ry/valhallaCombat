#include "cellLoadEventHandler.h"



EventResult cellLoadEventHandler::ProcessEvent(const RE::TESCellFullyLoadedEvent* a_event, RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource) {

	if (!a_event || !a_eventSource) {
		ERROR("Event Source not found");
		return EventResult::kContinue;
	}
	DEBUG("cell load event triggers!");
	DEBUG("+++++++++++++++++++++cell loaded+++++++++++++++++++++");
	auto pc = RE::PlayerCharacter::GetSingleton();
	const auto task = SKSE::GetTaskInterface();
	if (task != nullptr) {
		task->AddTask(unRegister);
	}
	return EventResult::kStop;
}


