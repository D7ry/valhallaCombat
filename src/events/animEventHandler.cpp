#include "animEventHandler.h"
#include "attackHandler.h"
//all credits to Bingle
namespace anno
{
	const char* attackStop_anno = "attackStop";
	const char* hitFrame_anno = "HitFrame";
	const char* preHitFrame_anno = "preHitFrame";
	const char* atkWin_Skysa_Anno = "SkySA_AttackWinStart";
	const char* block_start_anno = "blockStartOut";
	const char* block_stop_anno = "blockStop";
	const char* weaponSwing_anno = "weaponSwing";
}

RE::BSEventNotifyControl animEventHandler::HookedProcessEvent(RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* src) {
    
	using namespace anno;
    FnProcessEvent fn = fnHash.at(*(uint64_t*)this);
	if (a_event.tag != NULL) {
		RE::BSFixedString _event = a_event.tag;
		//DEBUG(_event);
		if (_event == preHitFrame_anno) {
			DEBUG("==========prehitFrame==========");
			if (debuffHandler::isPlayerExhausted) {
				DEBUG("player is exhausted, init attack speed debuff");
				//a_event.holder->SetGraphVariableFloat("SkySA_weaponSpeedMult", 0.5);
			}
			if (debuffHandler::isPlayerExhausted && attackHandler::attackPreSwing) {
				DEBUG("player is exhausted, init attack speed debuff");
				a_event.holder->SetGraphVariableFloat("SkySA_weaponSpeedMult", 0.5);
			}
			attackHandler::registerAtk();
		}
		else if (_event == weaponSwing_anno) {
			DEBUG("=======weaponSwing========");
			a_event.holder->SetGraphVariableFloat("SkySA_weaponSpeedMult", 1);
			attackHandler::attackPreSwing = false;
		}
		else if (_event == attackStop_anno) {
			DEBUG("==========attackstop==========");
			attackHandler::checkout();
		}
		/*else if (_event == block_start_anno) {
			DEBUG("==========blockStart==========");
			blockHandler::GetSingleton()->raiseShield();
		}
		else if (_event == block_stop_anno) {
			DEBUG("==========blockStop==========");
			blockHandler::GetSingleton()->unRaiseShield();
		}*/
	}
    return fn ? (this->*fn)(a_event, src) : RE::BSEventNotifyControl::kContinue;
}

std::unordered_map<uint64_t, animEventHandler::FnProcessEvent> animEventHandler::fnHash;