#include "animEventHandler.h"


EventResult animEventHandler::ProcessEvent(const RE::BSAnimationGraphEvent* a_event,
	RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) {
	if (!a_event || !a_eventSource) {
		ERROR("Event Source not found");
		return EventResult::kContinue;
	}
	if (a_event->tag != nullptr && a_event->tag == "HitFrame") {
		DEBUG("player hitframe trigger!");
		RE::PlayerCharacter* pc = RE::PlayerCharacter::GetSingleton();
		avHandler::damageStamina(pc);
	}
	return EventResult::kContinue;
}