//onHitEventHandler.cpp
#include "onHitEventHandler.h"
using EventResult = RE::BSEventNotifyControl;
using HitFlag = RE::TESHitEvent::Flag;
EventResult onHitEventHandler::ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) {
	if (!a_event || !a_eventSource) {
		ERROR("Event Source not found");
		return EventResult::kContinue;
	}
	DEBUG("onhit event triggers!");
	DEBUG("Hit flag is{}, source is {:x}", a_event->flags.get(), a_event->source);
	if (shouldHitRestoreStamina(a_event)) { avHandler::restoreStamina(RE::PlayerCharacter::GetSingleton()); }
	return EventResult::kContinue;
}
boolean onHitEventHandler::shouldHitRestoreStamina(const RE::TESHitEvent* a_event) {
	if (a_event->cause->As<RE::Actor>() == RE::PlayerCharacter::GetSingleton()) {
		DEBUG("player hit");
		if (a_event->target && a_event->target->As<RE::Actor>() && !a_event->target->As<RE::Actor>()->IsDead()) {
			DEBUG("hit living target");
			if (!a_event->flags.any(HitFlag::kBashAttack) 
				&& !a_event->flags.any(HitFlag::kPowerAttack) 
				&& !a_event->flags.any(HitFlag::kHitBlocked)){
				DEBUG("Correct hit flag");
				return true;
			}
		}
	}
	return false;
}

