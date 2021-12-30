#include "actorLoadEventHandler.h"
EventResult actorLoadEventHandler::ProcessEvent(const RE::TESObjectLoadedEvent* a_event, RE::BSTEventSource<RE::TESObjectLoadedEvent>* a_eventSource) {
	if (!a_event || !a_eventSource) {
		ERROR("Event Source not found");
		return EventResult::kContinue;
	}
	INFO("actor load event triggers!");
	if (a_event->formID == 0x14) {
		INFO("=======================player loaded=============================");
		auto pc = RE::PlayerCharacter::GetSingleton();
		INFO("registering anim event");
		if (!animEventHandler::RegisterSink(pc)) {
			INFO("in race menu, postponing registration");
			menuEventHandler::Register();
		}
		else {
			INFO("registered anim event");
			debuffHandler::GetSingleton()->rmDebuffPerk();
		}
	}
	return EventResult::kContinue;
}
