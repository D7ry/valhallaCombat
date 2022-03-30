#include "include/staminaHandler.h"
#include "include/attackHandler.h"
void attackHandler::registerAtk(RE::Actor* actor) {
	checkout(actor);
	//TODO:fix bash implementation
	//DEBUG("registering attack for {}", actor->GetName());
	if (actor->currentProcess && actor->currentProcess->high) {
		auto attckData = actor->currentProcess->high->attackData;
		if (attckData) {
			if (attckData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
				return; //bash attacks won't get registered
			}
			if (attckData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) {
				mtx.lock();
				attackerHeap.emplace(actor, ATTACKTYPE::power);
				mtx.unlock();
				//DEBUG("registered power attack");
			}
			else {
				//DEBUG("registered light attack");
				mtx.lock();
				attackerHeap.emplace(actor, ATTACKTYPE::light);
				mtx.unlock();
				//DEBUG("attack heap size: {}", attackerHeap.size());
			}

		}
	}
}

void attackHandler::registerHit(RE::Actor* actor) {
	ATTACKTYPE atkType;
	if (!getAtkType(actor, atkType)) {
		return;
	}
	if (atkType == ATTACKTYPE::light) { //register light hit
		actorToRegenStamina = actor;
		staminaHandler::staminaLightHit(actor);
		actorToRegenStamina = nullptr;
	}
	else { //register power hit
		staminaHandler::staminaHeavyHit(actor);
	}
	mtx.lock();
	attackerHeap.erase(actor); //erase the actor from heap i.e. checking out the attack without damaging stamina.
	mtx.unlock();
}


void attackHandler::checkout(RE::Actor* actor) {
	ATTACKTYPE atkType;
	if (!getAtkType(actor, atkType)) {
		return;
	}
	if (atkType == ATTACKTYPE::light) {
		staminaHandler::staminaLightMiss(actor);
	}
	else {
		staminaHandler::staminaHeavyMiss(actor);
	}
	mtx.lock();
	attackerHeap.erase(actor);
	mtx.unlock();
}