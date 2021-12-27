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
	DEBUG("finished calling register hit");
	if (hitLivingTarget(a_event)) {
		attackHandler::registerHit();
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (a_event->flags.any(HitFlag::kPowerAttack)) {
			avHandler::staminaHeavyHit(pc);
		}
		else {
			avHandler::staminaLightHit(pc);
		}
	}
	return EventResult::kContinue;
}
bool onHitEventHandler::hitLivingTarget(const RE::TESHitEvent* a_event) {
	if (!a_event->projectile
		&&a_event->cause 
		&&a_event->cause->IsPlayerRef()) {
		DEBUG("player hit");
		if (a_event->target) {
			auto _target = a_event->target->As<RE::Actor>();
			//FIXME: fix thie trash isdeadh formula
			if (_target 
				&& _target->GetActorValue(RE::ActorValue::kHealth) 
				&& _target->GetActorValue(RE::ActorValue::kHealth) > 0) {
				DEBUG("hit living target");
				if (!a_event->flags.any(HitFlag::kBashAttack)
					&& !a_event->flags.any(HitFlag::kHitBlocked)) {
					DEBUG("Correct hit flag");
					return true;
				}
			}
		}
	}
	return false;
}

