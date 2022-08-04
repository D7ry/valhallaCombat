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

void animEventHandler::ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
{
	if (!a_event->holder) {
		return;
	}
	std::string_view eventTag = a_event->tag.data();

	switch (hash(eventTag.data(), eventTag.size())) {
	case "preHitFrame"_h:
		if (settings::bAttackStaminaToggle) {
			attackHandler::GetSingleton()->cacheAttack(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>());
		} else if (settings::bStaminaDebuffToggle) {
			staminaHandler::checkStamina(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>());
		}
		break;
	case "attackStop"_h:
		if (settings::bAttackStaminaToggle) {
			attackHandler::GetSingleton()->checkout(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>());
		} else if (settings::bStaminaDebuffToggle) {
			staminaHandler::checkStamina(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>());
		}
		break;
	//case "blockStop"_h:
	//	if (settings::bTimedBlockToggle && a_event->holder->IsPlayerRef()) {
	//		blockHandler::GetSingleton()->onBlockStop();
	//	}
	//	break;
	case "TKDR_IFrameEnd"_h:
		staminaHandler::checkStamina(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>());
		break;
	case "Dodge"_h:
		staminaHandler::checkStamina(const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>());
		break;
	}
}

EventResult animEventHandler::ProcessEvent_NPC(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
{
	ProcessEvent(a_sink, a_event, a_eventSource);
	return _ProcessEvent_NPC(a_sink, a_event, a_eventSource);
}

EventResult animEventHandler::ProcessEvent_PC(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
{
	ProcessEvent(a_sink, a_event, a_eventSource);
	return _ProcessEvent_PC(a_sink, a_event, a_eventSource);
}
