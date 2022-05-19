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

static inline const RE::BSFixedString recoilLargeStart = "recoilLargeStart";

void reactionHandler::triggerContinuousStagger(RE::Actor* a_causer, RE::Actor* a_reactor, reactionType a_reactionType) {
	a_reactor->NotifyAnimationGraph(staggerStop);
	std::jthread asynStaggerThread(async_triggerContinuousStagger, a_causer, a_reactor, a_reactionType);
	asynStaggerThread.detach();
}
void reactionHandler::async_triggerContinuousStagger(RE::Actor* a_causer, RE::Actor* a_reactor, reactionType a_reactionType) {
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	const auto task = SKSE::GetTaskInterface();
	if (task != nullptr) {
		task->AddTask([a_causer, a_reactor, a_reactionType]() {
			triggerStagger(a_causer, a_reactor, a_reactionType);
			});
	}
};
	
void reactionHandler::triggerStagger(RE::Actor* a_aggressor, RE::Actor* a_reactor, float a_reactionMagnitude) {
	auto headingAngle = a_reactor->GetHeadingAngle(a_aggressor->GetPosition(), false);
	auto direction = (headingAngle >= 0.0f) ? headingAngle / 360.0f : (360.0f + headingAngle) / 360.0f;
	a_reactor->SetGraphVariableFloat(staggerDirection, direction);
	a_reactor->SetGraphVariableFloat(StaggerMagnitude, a_reactionMagnitude);
	a_reactor->NotifyAnimationGraph(staggerStart);
}

void reactionHandler::triggerRecoil(RE::Actor* a_reactor, reactionType a_reactionType) {
	a_reactor->NotifyAnimationGraph(recoilLargeStart);
}

void reactionHandler::triggerKnockBack(RE::Actor* a_aggressor, RE::Actor* a_reactor) {
	ValhallaUtils::PushActorAway(a_aggressor, a_reactor, 9);
	Utils::slowTime(0.2, 0.1);
}

void reactionHandler::triggerPoiseReaction(RE::Actor* a_aggressor, RE::Actor* a_reactor, reactionType a_reactionType) {
	RE::BSFixedString reaction;
	if (ValhallaUtils::isBackFacing(a_reactor, a_aggressor)) {
		switch (a_reactionType) {
		case reactionType::kSmall: reaction = poise_small_fwd; break;
		case reactionType::kMedium: reaction = poise_med_fwd; break;
		case reactionType::kLarge: reaction = poise_large_fwd; break;
		case reactionType::kLargest: reaction = poise_largest_fwd; break;
		}
	}
	else {
		switch (a_reactionType) {
		case reactionType::kSmall: reaction = poise_small; break;
		case reactionType::kMedium: reaction = poise_med; break;
		case reactionType::kLarge: reaction = poise_large; break;
		case reactionType::kLargest: reaction = poise_largest; break;
		}
	}
	a_reactor->NotifyAnimationGraph(reaction);
}



void reactionHandler::triggerStagger(RE::Actor* a_causer, RE::Actor* a_reactor, reactionType a_reactionType) {
	if (a_reactor->IsSwimming() || a_reactor->IsInKillMove() || !a_reactor->Is3DLoaded()) {
		return;
	}
	if (settings::bStunToggle) {
		if (data::raceMapping[a_reactor->GetRace()] == data::raceCatagory::Humanoid) {
			if (stunHandler::GetSingleton()->isActorStunned(a_reactor)) {
				return;//do not stagger stunned actors.
			}
		}
	}
	if (settings::bPoiseCompatibility
		&& data::isRaceType(a_reactor, data::raceCatagory::Humanoid)) {
		triggerPoiseReaction(a_causer, a_reactor, a_reactionType);
	}
	else {
		switch (a_reactionType) {
		case kSmall: triggerStagger(a_causer, a_reactor, 0); break;
		case kMedium: triggerStagger(a_causer, a_reactor, 0.3); break;
		case kLarge: triggerStagger(a_causer, a_reactor, 0.7); break;
		case kLargest: triggerStagger(a_causer, a_reactor, 10); break;
		case kKnockBack: triggerKnockBack(a_causer, a_reactor); break;
		}
	}

}

void reactionHandler::triggerDownedState(RE::Actor* a_actor) {
	auto raceMapping = data::raceMapping;
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
	if (a_actor->IsSwimming() || a_actor->IsInKillMove() || !a_actor->Is3DLoaded()) {
		return;
	}
	auto raceMapping = data::raceMapping;
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