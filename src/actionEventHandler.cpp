#include "C:/Users/Ty/Documents/SKSEPlugins/Build/Plugins/valhallaCombat/CMakeFiles/valhallaCombat.dir/Debug/cmake_pch.hxx"
#include "actionEventHandler.h"
EventResult actionEventHandler::ProcessEvent(const SKSE::ActionEvent* a_event,
	RE::BSTEventSource<SKSE::ActionEvent>* a_eventSource) {
	DEBUG("action event triggers, type: {}", a_event->type.get());
	if ((int) a_event->type.get() == 0) {
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (a_event->actor->IsPlayerRef()) {
			DEBUG("player swing action event triggers");
			avHandler::damageStamina(pc);
		}
		else {
			DEBUG("not player swing!");
		}
	}
	return EventResult::kContinue;
}