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
	//DEBUG(eventTag);
	switch (hash(eventTag.data(), eventTag.size())) {
	case "preHitFrame"_h:
		if (settings::bAttackStaminaToggle) {
			attackHandler::GetSingleton()->registerAtk(const_cast<RE::TESObjectREFR*>(a_event.holder)->As<RE::Actor>());
		} else if (settings::bStaminaDebuffToggle) {
			staminaHandler::checkStamina(const_cast<RE::TESObjectREFR*>(a_event.holder)->As<RE::Actor>());
		}
		
		break;
	case "attackStop"_h:
		if (settings::bAttackStaminaToggle) {
			attackHandler::GetSingleton()->checkout(const_cast<RE::TESObjectREFR*>(a_event.holder)->As<RE::Actor>());
		} else if (settings::bStaminaDebuffToggle) {
			staminaHandler::checkStamina(const_cast<RE::TESObjectREFR*>(a_event.holder)->As<RE::Actor>());
		}
		break;
	case "blockStop"_h:
		if (settings::bTimedBlockToggle && a_event.holder->IsPlayerRef()) {
			blockHandler::GetSingleton()->onBlockStop();
		}
		break;
	case "TKDR_IFrameEnd"_h:
		staminaHandler::checkStamina(const_cast<RE::TESObjectREFR*>(a_event.holder)->As<RE::Actor>());
		break;
	case "Dodge"_h:
		staminaHandler::checkStamina(const_cast<RE::TESObjectREFR*>(a_event.holder)->As<RE::Actor>());
		break;
	}
    return fn ? (this->*fn)(a_event, src) : RE::BSEventNotifyControl::kContinue;
}

std::unordered_map<uint64_t, animEventHandler::FnProcessEvent> animEventHandler::fnHash;
