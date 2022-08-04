#include "include/staminaHandler.h"
#include "include/attackHandler.h"

void attackHandler::cacheAttack(RE::Actor* a_actor) {
	checkout(a_actor);
	//TODO:fix bash implementation
	//DEBUG("registering attack for {}", actor->GetName());
	if (a_actor->currentProcess && a_actor->currentProcess->high) {
		auto attckData = a_actor->currentProcess->high->attackData;
		if (!attckData) {
			return;
		}
		if (attckData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
			return;
		}
		if (attckData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) {
			return;
		}
		else {
			//DEBUG("registered light attack");
			mtx.lock();
			cachedAttackers.insert(a_actor);
			mtx.unlock();
			//DEBUG("attack heap size: {}", attackerHeap.size());
		}

	}
}

void attackHandler::OnLightHit(RE::Actor* a_actor) {

	actorToRegenStamina = a_actor;
	staminaHandler::staminaLightHit(a_actor);
	actorToRegenStamina = nullptr;

	mtx.lock();
	cachedAttackers.erase(a_actor);
	mtx.unlock();
}


void attackHandler::checkout(RE::Actor* a_actor) {
	auto it = cachedAttackers.find(a_actor);
	if (it == cachedAttackers.end()) {
		return;
	}
	
	staminaHandler::staminaLightMiss(a_actor);
	mtx.lock();
	cachedAttackers.erase(it);
	mtx.unlock();
}
