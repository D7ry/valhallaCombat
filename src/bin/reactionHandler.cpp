#include "include/reactionHandler.h"
#include "include/settings.h"
#include "include/Utils.h"
#include "include/data.h"
static inline const RE::BSFixedString poise_largest = "poise_largest_start";
static inline const RE::BSFixedString poise_largest_fwd = "poise_largest_start_fwd";
static inline const RE::BSFixedString poise_large = "poise_large_start";
static inline const RE::BSFixedString poise_large_fwd = "poise_large_start_fwd";
static inline const RE::BSFixedString poise_med = "poise_med_start";
static inline const RE::BSFixedString poise_med_fwd = "poise_med_start_fwd";
static inline const RE::BSFixedString poise_small = "poise_small_start";
static inline const RE::BSFixedString poise_small_fwd = "poise_small_start_fwd";

static inline const RE::BSFixedString staggerDirection = "staggerDirection";
static inline const RE::BSFixedString StaggerMagnitude = "StaggerMagnitude";
static inline const RE::BSFixedString staggerStart = "staggerStart";
static inline const RE::BSFixedString staggerStop = "staggerStop";
static inline const RE::BSFixedString bleedOutStart = "BleedoutStart";
static inline const RE::BSFixedString bleedOutStop = "BleedOutStop";
static inline const RE::BSFixedString bleedOutGraphBool = "IsBleedingOut";
void reactionHandler::triggerContinuousStagger(RE::Actor* aggressor, RE::Actor* reactor, reactionType reactionType) {
	reactor->NotifyAnimationGraph(staggerStop);
	std::jthread asynStaggerThread(async_triggerContinuousStagger, aggressor, reactor, reactionType);
	asynStaggerThread.detach();
}
void reactionHandler::async_triggerContinuousStagger(RE::Actor* aggressor, RE::Actor* reactor, reactionType reactionType) {
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	const auto task = SKSE::GetTaskInterface();
	if (task != nullptr) {
		task->AddTask([aggressor, reactor, reactionType]() {
			triggerStagger(aggressor, reactor, reactionType);
			});
	}
};
	
void reactionHandler::triggerStagger(RE::Actor* aggressor, RE::Actor* reactor, float magnitude) {
	auto headingAngle = reactor->GetHeadingAngle(aggressor->GetPosition(), false);
	auto direction = (headingAngle >= 0.0f) ? headingAngle / 360.0f : (360.0f + headingAngle) / 360.0f;
	reactor->SetGraphVariableFloat(staggerDirection, direction);
	reactor->SetGraphVariableFloat(StaggerMagnitude, magnitude);
	reactor->NotifyAnimationGraph(staggerStart);
}

void reactionHandler::triggerKnockBack(RE::Actor* aggressor, RE::Actor* reactor) {
	ValhallaUtils::PushActorAway(aggressor, reactor, 7);
}

void reactionHandler::triggerPoiseReaction(RE::Actor* aggressor, RE::Actor* reactor, reactionType reactionType) {
	RE::BSFixedString reaction;
	if (ValhallaUtils::isBackFacing(reactor, aggressor)) {
		switch (reactionType) {
		case reactionType::kSmall: reaction = poise_small_fwd; break;
		case reactionType::kMedium: reaction = poise_med_fwd; break;
		case reactionType::kLarge: reaction = poise_large_fwd; break;
		case reactionType::kLargest: reaction = poise_largest_fwd; break;
		}
	}
	else {
		switch (reactionType) {
		case reactionType::kSmall: reaction = poise_small; break;
		case reactionType::kMedium: reaction = poise_med; break;
		case reactionType::kLarge: reaction = poise_large; break;
		case reactionType::kLargest: reaction = poise_largest; break;
		}
	}
	reactor->NotifyAnimationGraph(reaction);
}



void reactionHandler::triggerStagger(RE::Actor* causer, RE::Actor* reactor, reactionType reactionType) {
	if (reactor->IsSwimming()) {
		return;
	}
	if (settings::bStunToggle) {
		if (data::GetSingleton()->raceMapping[reactor->GetRace()] == data::raceCatagory::Humanoid) {
			if (stunHandler::GetSingleton()->isActorStunned(reactor)) {
				return;//do not stagger stunned actors.
			}
		}
	}
	if (settings::bPoiseCompatibility
		&& data::GetSingleton()->isRaceType(reactor, data::raceCatagory::Humanoid)) {
		triggerPoiseReaction(causer, reactor, reactionType);
	}
	else {
		switch (reactionType) {
		case kSmall: triggerStagger(causer, reactor, 0); break;
		case kMedium: triggerStagger(causer, reactor, 0.3); break;
		case kLarge: triggerStagger(causer, reactor, 0.7); break;
		case kLargest: triggerStagger(causer, reactor, 10); break;
		case kKnockBack: triggerKnockBack(causer, reactor); break;
		}
	}

}

void reactionHandler::triggerDownedState(RE::Actor* a_actor) {
	auto raceMapping = data::GetSingleton()->raceMapping;
	auto it = raceMapping.find(a_actor->GetRace());
	if (it != raceMapping.end()) {
		switch (it->second) {
		case data::Humanoid:
		case data::Undead:			
			if (a_actor->IsInKillMove()) {
				return;
			}
			a_actor->SetGraphVariableBool(bleedOutGraphBool, true);
			a_actor->NotifyAnimationGraph(bleedOutStart);
		}
	}
}

void reactionHandler::recoverDownedState(RE::Actor* a_actor) {
	auto raceMapping = data::GetSingleton()->raceMapping;
	auto it = raceMapping.find(a_actor->GetRace());
	if (it != raceMapping.end()) {
		switch (it->second) {
		case data::Humanoid:
		case data::Undead:
			if (a_actor->IsInKillMove()) {
				return;
			}
			a_actor->SetGraphVariableBool(bleedOutGraphBool, false);
			a_actor->NotifyAnimationGraph(bleedOutStop);
		}
	}
}