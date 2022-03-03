#include "include/actorManager.h"
#include "include/Utils.h"
#include "include/data.h"
#include "include/stunHandler.h"
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
			metadata->isPerfectBlocking = false;
			metadata->isPerfectBlockingCoolingDown = true;
			metadata->perfectBlockingCooldownTimer = settings::fPerfectBlockCoolDownTime;
		}
		else {
			metadata->perfectBlockingTimer -= deltaTime;
		}
	}
	if (metadata->isPerfectBlockingCoolingDown) {
		if (metadata->perfectBlockingCooldownTimer <= 0) {
			metadata->isPerfectBlockingCoolingDown = false;
		}
		else {
			metadata->perfectBlockingCooldownTimer -= deltaTime;
		}
	}
}

void actorManager::updateStun(actorMetaData* metadata, float deltaTime) {
	//metadata->maxStun = stunHandler::calcMaxStun(metadata->actor);

}