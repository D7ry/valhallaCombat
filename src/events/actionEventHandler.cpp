#include "actionEventHandler.h"
EventResult actionEventHandler::ProcessEvent(const SKSE::ActionEvent* a_event,
	RE::BSTEventSource<SKSE::ActionEvent>* a_eventSource) {
	DEBUG("action event triggers, type: {}", a_event->type.get());
	if ((int)a_event->type.get() == 0) {
		if (a_event->actor->IsPlayerRef()) {
			DEBUG("player swing action event triggers");
			auto pc = RE::PlayerCharacter::GetSingleton();
			//avHandler::damageStamina(pc);
		}
		else {
			DEBUG("not player swing!");
		}
	}
	return EventResult::kContinue;
}