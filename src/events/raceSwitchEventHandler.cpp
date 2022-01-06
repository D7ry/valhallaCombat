#include "raceSwitchEventHandler.h"
#include "attackHandler.h"
EventResult raceSwitchEventHandler::ProcessEvent(const RE::TESSwitchRaceCompleteEvent* a_event, RE::BSTEventSource<RE::TESSwitchRaceCompleteEvent>* a_eventSource) {
	if (!a_event || !a_eventSource) {
		ERROR("Event Source not found");
		return EventResult::kContinue;
	}
	DEBUG("{} has switched race!", a_event->subject.get()->GetName());
	auto pc = RE::PlayerCharacter::GetSingleton();
	if (pc) {
		//attackHandler::updatePcStaminaRate();
	}
	return EventResult::kContinue;
}
