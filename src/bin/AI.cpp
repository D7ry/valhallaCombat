#include "include/AI.h"
#include "include/data.h"
void AI::action_PerformEldenCounter(RE::Actor* a_actor) {
	a_actor->NotifyAnimationGraph(data::AnimEvent_GuardCounter);
}

void AI::calculateCounterChance(RE::Actor* actor) {
	
}