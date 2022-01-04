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
	if (a_event->projectile || !a_event->cause || !a_event->target) {
		DEBUG("invalid hit event!");
		return EventResult::kContinue;
	}

	if (a_event->cause->IsPlayerRef()) {
		playerHit(a_event);
	}

	return EventResult::kContinue;
}

void onHitEventHandler::playerHit(const RE::TESHitEvent* a_event) {
	DEBUG("player melee hit!");
	auto target = a_event->target.get()->As<RE::Actor>();
	if (!isAlive(target)) {
		return;
	}
	DEBUG("target is alive!");
	if (a_event->flags.any(RE::TESHitEvent::Flag::kBashAttack)) {
		DEBUG("bash attack, nope!");
		return;
	}

	if (a_event->flags.any(RE::TESHitEvent::Flag::kHitBlocked)) {
		if (!dataHandler::GetSingleton()->blockedHitRegenStamina) {
			DEBUG("hit blocked, no stamina recovery!");
			return;
		}
		else {
			DEBUG("stamina recovery for blocked hit!");
		}
	}

	DEBUG("correct hit flag, registering hit!");
	attackHandler::registerHit();
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



