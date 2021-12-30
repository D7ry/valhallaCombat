#include "animEventHandler.h"

namespace attackCounter
{
	inline bool attackHit = false;
	inline int swing_Ct = 0;
}


EventResult animEventHandler::ProcessEvent(const RE::BSAnimationGraphEvent* a_event,
	RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) {
	if (!a_event || !a_eventSource) {
		ERROR("Event Source not found");
		return EventResult::kContinue;
	}
	if (a_event->tag != nullptr) {
		RE::BSFixedString _event = a_event->tag;
	}
	return EventResult::kContinue;
}

