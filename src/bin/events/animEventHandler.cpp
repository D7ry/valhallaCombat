#include "include/events.h"
#include "include/attackHandler.h"
#include "include/blockHandler.h"
#include "include/executionHandler.h"
#include "include/settings.h"
//all credits to Bingle&Ersh
constexpr uint32_t hash(const char* data, size_t const size) noexcept
{
	uint32_t hash = 5381;

	for (const char* c = data; c < data + size; ++c) {
		hash = ((hash << 5) + hash) + (unsigned char)*c;
	}

	return hash;
}

constexpr uint32_t operator"" _h(const char* str, size_t size) noexcept
{
	return hash(str, size);
}

RE::BSEventNotifyControl animEventHandler::HookedProcessEvent(RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* src) {
    FnProcessEvent fn = fnHash.at(*(uint64_t*)this);
	std::string_view eventTag = a_event.tag.data();
	if (a_event.holder == nullptr) {
		return fn ? (this->*fn)(a_event, src) : RE::BSEventNotifyControl::kContinue;
	}
	switch (hash(eventTag.data(), eventTag.size())) {
	case "preHitFrame"_h:
		//DEBUG("==========prehitFrame==========");
		if (settings::bAttackStaminaToggle) {
			attackHandler::GetSingleton()->registerAtk(a_event.holder->As<RE::Actor>());
		}
		break;
	case "attackStop"_h:
		//DEBUG("==========attackstop==========");
		if (settings::bAttackStaminaToggle) {
			attackHandler::GetSingleton()->checkout(a_event.holder->As<RE::Actor>());
		}

		break;
	//case "BlockStop"_h:
		//if (settings::bTimedBlockToggle) {
			//blockHandler::GetSingleton()->blockStop();
		///}
	//case "blockStartOut"_h:
		//DEBUG("===========blockStartOut===========");
		/*\if (settings::bTimedBlockToggle) {
			blockHandler::GetSingleton()->registerPerfectBlock(a_event.holder->As<RE::Actor>());
		}
		break;*/
	//case "tailcombatidle"_h:
		/*Unghost the executor on finish*/
		//break;
	case "TKDR_IFrameEnd"_h:
		//DEBUG("==========TK DODGE============");
		staminaHandler::checkStamina(a_event.holder->As<RE::Actor>());
		break;
	case "Dodge"_h:
		staminaHandler::checkStamina(a_event.holder->As<RE::Actor>());
		break;
	}
    return fn ? (this->*fn)(a_event, src) : RE::BSEventNotifyControl::kContinue;
}

std::unordered_map<uint64_t, animEventHandler::FnProcessEvent> animEventHandler::fnHash;