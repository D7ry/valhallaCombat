#include "include/staminaHandler.h"
#include "include/spiritHandler.h"

//Cringe stuff from old Valhalla
//void attackHandler::registerAtk(RE::Actor* a_actor) {
//	checkout(a_actor);
//	//TODO:fix bash implementation
//	//DEBUG("registering attack for {}", actor->GetName());
//	if (a_actor->currentProcess && a_actor->currentProcess->high) {
//		auto attckData = a_actor->currentProcess->high->attackData;
//		if (attckData) {
//			if (attckData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
//				return; //bash attacks won't get registered
//			}
//			if (attckData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) {
//				mtx_attackerHeap.lock();
//				attackerHeap.emplace(a_actor, ATTACKTYPE::power);
//				mtx_attackerHeap.unlock();
//				//DEBUG("registered power attack");
//			}
//			else {
//				//DEBUG("registered light attack");
//				mtx_attackerHeap.lock();
//				attackerHeap.emplace(a_actor, ATTACKTYPE::light);
//				mtx_attackerHeap.unlock();
//				//DEBUG("attack heap size: {}", attackerHeap.size());
//			}
//
//		}
//	}
//}
//
//void attackHandler::registerHit(RE::Actor* a_actor) {
//	ATTACKTYPE atkType;
//	if (!getAtkType(a_actor, atkType)) {
//		return;
//	}
//	if (atkType == ATTACKTYPE::light) { //register light hit
//		actorToRegenStamina = a_actor;
//		staminaHandler::staminaLightHit(a_actor);
//		actorToRegenStamina = nullptr;
//	}
//	else { //register power hit
//		staminaHandler::staminaHeavyHit(a_actor);
//	}
//	mtx_attackerHeap.lock();
//	attackerHeap.erase(a_actor); //erase the actor from heap i.e. checking out the attack without damaging stamina.
//	mtx_attackerHeap.unlock();
//}
//
//
//void attackHandler::checkout(RE::Actor* a_actor) {
//	ATTACKTYPE atkType;
//	if (!getAtkType(a_actor, atkType)) {
//		return;
//	}
//	if (atkType == ATTACKTYPE::light) {
//		staminaHandler::staminaLightMiss(a_actor);
//	}
//	else {
//		staminaHandler::staminaHeavyMiss(a_actor);
//	}
//	mtx_attackerHeap.lock();
//	attackerHeap.erase(a_actor);
//	mtx_attackerHeap.unlock();
//}
