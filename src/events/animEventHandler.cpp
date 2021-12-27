#include "animEventHandler.h"

namespace anno
{
	const char* attackStop_anno = "attackStop";
	const char* hitFrame_anno = "HitFrame";
	const char* preHitFrame_anno = "preHitFrame";
	const char* atkWin_Skysa_Anno = "SkySA_AttackWinStart";
}

namespace attackCounter
{
	inline bool attackHit = false;
	inline int swing_Ct = 0;
}


EventResult animEventHandler::ProcessEvent(const RE::BSAnimationGraphEvent* a_event,
	RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) {
	using namespace anno;
	if (!a_event || !a_eventSource) {
		ERROR("Event Source not found");
		return EventResult::kContinue;
	}
	if (a_event->tag != nullptr) {
		RE::BSFixedString _event = a_event->tag;
		if (_event == hitFrame_anno) {
			//DEBUG("player hitframe trigger!");
			//attackHandler::registerAtk();
		}
		else if (_event == preHitFrame_anno) {
			DEBUG("prehitFrame trigger!");
			attackHandler::proceed();
		}
		else if (_event == attackStop_anno) {
			DEBUG("player attackstop trigger!");
			attackHandler::checkout();
		}
		else if (_event == atkWin_Skysa_Anno) {
			DEBUG("attackWin trigger!");
			//attackHandler::checkout(); //doing so is way too early
		}
	}
	return EventResult::kContinue;
}

