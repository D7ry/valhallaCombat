#include "include/actorManager.h"
#include "include/Utils.h"
#include "include/data.h"
bool actorManager::isActorManaged(RE::Actor* actor) {
	return managedActors.find(actor) != managedActors.end();
}

void actorManager::update() {
	float deltaTime = *Utils::g_deltaTimeRealTime;
	auto it = managedActors.begin();
	while (it != managedActors.end()) {
		auto actor = it->first;
		if (!actor
			|| !actor->currentProcess
			|| !actor->currentProcess->InHighProcess()) {
			it = managedActors.erase(it); continue;
		}
		if (settings::bStunToggle) {
			updateStun(it->second, deltaTime);
		}
		if (settings::bPerfectBlockToggle) {
			updateBlocking(it->second, deltaTime);
		}
		if (settings::bStaminaToggle) {
			updateDebuff(it->second, deltaTime);
		}
	}
}

void actorManager::updateBlocking(actorMetaData* metadata, float deltaTime) {
	if (metadata->isPerfectBlocking) {
		if (metadata->perfectBlockingTimer <= 0) {
			metadata->isPerfectBlockingCoolingDown = true;
			metadata->perfectBlockingCooldownTimer = settings::fPerfectBlockCoolDownTime;
		}
		metadata->perfectBlockingTimer -= deltaTime;

	}
}