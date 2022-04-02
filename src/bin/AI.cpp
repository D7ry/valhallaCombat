#include "include/AI.h"
#include "include/data.h"
#include "include/debuffHandler.h"
void AI::action_PerformEldenCounter(RE::Actor* a_actor) {
	a_actor->NotifyAnimationGraph(data::AnimEvent_GuardCounter);
}

void AI::calculateCounterChance(RE::Actor* actor) {
	if (debuffHandler::GetSingleton()->isInDebuff(actor)) {
		return;
	}
	auto currStamina = actor->GetActorValue(RE::ActorValue::kStamina);
	auto permStamina = actor->GetPermanentActorValue(RE::ActorValue::kStamina);
	auto num = std::rand() % 100;
	if (currStamina >= permStamina * 1 / 2) { //100% - 50% stmaina, 20% counter chance
		//if (num > 80) {
			action_PerformEldenCounter(actor);
		//}
	} 
	else if (currStamina >= permStamina * 1 / 3) { //50% - 33% stamina, 10% counter chance
		if (num > 90) {
			action_PerformEldenCounter(actor);
		}
	}
	else if (currStamina >= permStamina * 1 / 5) { //33% - 20% stamina, 5% counter chance
		if (num > 95) {
			action_PerformEldenCounter(actor);
		}
	}

}