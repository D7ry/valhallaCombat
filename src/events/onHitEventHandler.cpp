#include "onHitEventHandler.h"
#include "staminaHandler.h"
using EventResult = RE::BSEventNotifyControl;
using HitFlag = RE::TESHitEvent::Flag;
EventResult onHitEventHandler::ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) {
	if (!a_event || !a_eventSource) {
		ERROR("Event Source not found");
		return EventResult::kContinue;
	}

	DEBUG("onhit event triggers!");
	if (a_event->projectile || !a_event->cause || !a_event->cause->IsPlayerRef()) {
		return EventResult::kContinue;
	}

	DEBUG("player melee hit!");
	auto target = a_event->target.get()->As<RE::Actor>();

	if (!isAlive(target)) {
		return EventResult::kContinue;
	}
	DEBUG("target is alive!");
	auto pc = RE::PlayerCharacter::GetSingleton();
	if (pc) {
		if (a_event->flags.any(HitFlag::kPowerAttack)) {
			staminaHandler::staminaHeavyHit(pc);
		}
		else {
			staminaHandler::staminaLightHit(pc);
		}
	}
	attackHandler::registerHit();
	return EventResult::kContinue;
}
bool onHitEventHandler::isAlive(RE::Actor* a_target) {				//stolen from Maxsu's OHAF
	if (!a_target) {
		DEBUG("Target Actor Not Found!");
		return false;
	}
	if (a_target->formType != RE::FormType::ActorCharacter) {
		DEBUG("Target Actor Not Actor!");
		return false;
	}
	if (a_target->IsDead()) {
		DEBUG("Target Actor is Dead!");
		return false;
	}
	if (a_target->IsInKillMove()) {
		DEBUG("Target Actor Is in Kill Move!");
		return false;
	}
	return true;
}



