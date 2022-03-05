#include "include/events.h"
#include "include/stunHandler.h"

EventResult cellLoadEventHandler::ProcessEvent(const RE::TESCellFullyLoadedEvent* a_event, RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource) {
	DEBUG("cell load event");
	stunHandler::GetSingleton()->houseKeeping();
	return EventResult::kContinue;
}