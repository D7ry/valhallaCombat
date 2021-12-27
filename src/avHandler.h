#pragma once
#include "Utils.h"
#include "dataHandler.h"
#include "debuffHandler.h"
using namespace Utils;
namespace avHandler
{
	/*causes weapon stamina damage to actor A*/
	inline void damageStamina(RE::Actor* a) {
		if (!wieldingOneHanded(a)) {
			DEBUG("damaging {} stamina", dataHandler::GetSingleton()->meleeCost2h);
			damageav(a, RE::ActorValue::kStamina, dataHandler::GetSingleton()->meleeCost2h);
		}
		else {
			DEBUG("damaging {} stamina", dataHandler::GetSingleton()->meleeCost1h);
			damageav(a, RE::ActorValue::kStamina, dataHandler::GetSingleton()->meleeCost1h);
		}
		if (a->GetActorValue(RE::ActorValue::kStamina) == 0) {
			DEBUG("{} is exhausted!", a->GetName());
			debuffHandler::GetSingleton()->initStaminaDebuff();
		}
	}

	/*restores stamina for actor A*/
	inline void restoreStamina(RE::Actor* a) {
		if (!wieldingOneHanded(a)) {
			restoreav(a, RE::ActorValue::kStamina, dataHandler::GetSingleton()->meleeHitStaminaRecover1h);
		}
		else {
			restoreav(a, RE::ActorValue::kStamina, dataHandler::GetSingleton()->meleeHitStaminaRecover2h);
		}
	}

	inline bool canAttack = true;

};

