#include "include/reactionHandler.h"
#include "include/settings.h"
#include "include/Utils.h"
static inline const RE::BSFixedString poise_largest = "poise_largest_start";
static inline const RE::BSFixedString poise_largest_fwd = "poise_largest_start_fwd";
static inline const RE::BSFixedString poise_large = "poise_large_start";
static inline const RE::BSFixedString poise_large_fwd = "poise_large_start_fwd";
static inline const RE::BSFixedString poise_med = "poise_med_start";
static inline const RE::BSFixedString poise_med_fwd = "poise_med_start_fwd";
static inline const RE::BSFixedString poise_small = "poise_small_start";
static inline const RE::BSFixedString poise_small_fwd = "poise_small_start_fwd";

static inline const RE::BSFixedString staggerDirection = "staggerDirection";
static inline const RE::BSFixedString staggerMagnitude = "staggerType";
static inline const RE::BSFixedString staggerStart = "staggerStart";

void reactionHandler::triggerStagger(RE::Actor* aggressor, RE::Actor* reactor, float magnitude) {
	if (reactor->IsSwimming()) {
		return;
	}
	auto headingAngle = reactor->GetHeadingAngle(aggressor->GetPosition(), false);
	auto direction = (headingAngle >= 0.0f) ? headingAngle / 360.0f : (360.0f + headingAngle) / 360.0f;
	reactor->SetGraphVariableFloat(staggerDirection, direction);
	reactor->SetGraphVariableFloat(staggerMagnitude, magnitude);
	reactor->NotifyAnimationGraph(staggerStart);
}

void reactionHandler::triggerPoiseReaction(RE::Actor* aggressor, RE::Actor* reactor, poiseReactionType reactionType) {
	if (reactor->IsSwimming()) {
		return;
	}
	RE::BSFixedString reaction;
	if (ValhallaUtils::isBackFacing(reactor, aggressor)) {
		switch (reactionType) {
		case poiseReactionType::kSmall: reaction = poise_small_fwd; break;
		case poiseReactionType::kMedium: reaction = poise_med_fwd; break;
		case poiseReactionType::kLarge: reaction = poise_large_fwd; break;
		case poiseReactionType::kLargest: reaction = poise_largest_fwd; break;
		}
	}
	else {
		switch (reactionType) {
		case poiseReactionType::kSmall: reaction = poise_small; break;
		case poiseReactionType::kMedium: reaction = poise_med; break;
		case poiseReactionType::kLarge: reaction = poise_large; break;
		case poiseReactionType::kLargest: reaction = poise_largest; break;
		}
	}
	reactor->NotifyAnimationGraph(reaction);
}

void reactionHandler::triggerKnockBack(RE::Actor* aggressor, RE::Actor* reactor) {
	if (!settings::bPoiseCompatibility) {
		ValhallaUtils::PushActorAway(aggressor, reactor, 6);
	}
	else {
		triggerPoiseReaction(aggressor, reactor, poiseReactionType::kLargest);
	}
}

void reactionHandler::triggerReactionLarge(RE::Actor* causer, RE::Actor* reactor) {
	if (!settings::bPoiseCompatibility) {
		triggerStagger(causer, reactor, 10);
	}
	else {
		triggerPoiseReaction(causer, reactor, poiseReactionType::kLarge);
	}
}

void reactionHandler::triggerReactionMedium(RE::Actor* causer, RE::Actor* reactor) {
	if (!settings::bPoiseCompatibility) {
		triggerStagger(causer, reactor, 2);
	}
	else {
		triggerPoiseReaction(causer, reactor, poiseReactionType::kMedium);
	}
}

void reactionHandler::triggerReactionSmall(RE::Actor* causer, RE::Actor* reactor) {
	if (!settings::bPoiseCompatibility) {
		triggerStagger(causer, reactor, 1);
	}
	else {
		triggerPoiseReaction(causer, reactor, poiseReactionType::kSmall);
	}
}