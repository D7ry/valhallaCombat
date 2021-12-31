#include "AnimEventWatcher.h"
#include "attackHandler.h"
//all credits to Bingle
namespace anno
{
	const char* attackStop_anno = "attackStop";
	const char* hitFrame_anno = "HitFrame";
	const char* preHitFrame_anno = "preHitFrame";
	const char* atkWin_Skysa_Anno = "SkySA_AttackWinStart";
}

RE::BSEventNotifyControl AnimationGraphEventWatcher::HookedProcessEvent(RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* src) {
    
	using namespace anno;
    FnProcessEvent fn = fnHash.at(*(uint64_t*)this);
	if (a_event.tag != NULL) {
		RE::BSFixedString _event = a_event.tag;
		if (_event == preHitFrame_anno) {
			DEBUG("==========prehitFrame==========");
			attackHandler::proceed();
		}
		else if (_event == attackStop_anno) {
			DEBUG("==========attackstop==========");
			attackHandler::checkout();
		}
	}
    return fn ? (this->*fn)(a_event, src) : RE::BSEventNotifyControl::kContinue;
}

std::unordered_map<uint64_t, AnimationGraphEventWatcher::FnProcessEvent> AnimationGraphEventWatcher::fnHash;