#include "actorLoadEventHandler.h"
#include "attackHandler.h"
EventResult actorLoadEventHandler::ProcessEvent(const RE::TESObjectLoadedEvent* a_event, RE::BSTEventSource<RE::TESObjectLoadedEvent>* a_eventSource) {
	if (!a_event || !a_eventSource) {
		ERROR("Event Source not found");
		return EventResult::kContinue;
	}

	if (a_event->formID == 0x14) {
		DEBUG("=======================player loaded=============================");
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (pc) {
			debuffHandler::GetSingleton()->rmDebuffPerk();
			attackHandler::updatePcStaminaRate();
		}
		else {
			DEBUG("failed to load player character");
		}
		/*if (!animEventHandler::RegisterSink(pc)) {
			INFO("in race menu, postponing registration");
			menuEventHandler::Register();
		}
		else {
			INFO("registered anim event");
			debuffHandler::GetSingleton()->rmDebuffPerk();
		}*/
	}
	return EventResult::kContinue;
}
