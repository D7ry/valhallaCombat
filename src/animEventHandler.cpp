#include "animEventHandler.h"


EventResult animEventHandler::ProcessEvent(const RE::BSAnimationGraphEvent* a_event,
	RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) {
	RE::BSFixedString event = a_event->tag;
	if (event == "HitFrame") {
		DEBUG("player hitframe trigger!");
		DEBUG("Damaging player stamina");
		RE::PlayerCharacter* pc = RE::PlayerCharacter::GetSingleton();
		avHandler::damageStamina(pc);
	}
	return EventResult::kContinue;
}