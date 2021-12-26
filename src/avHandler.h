#pragma once
#include "Utils.h"
#include "dataHandler.h"
#include "debuffHandler.h"
using namespace Utils;
namespace avHandler
{
	/*causes 1h weapon stamina damage to actor A*/
	inline void damageStamina(RE::Actor* a) {
		if (!wieldingOneHanded(a)) {
			damageav(a, RE::ActorValue::kStamina, dataHandler::GetSingleton()->meleeCost2h);
		}
		else {
			damageav(a, RE::ActorValue::kStamina, dataHandler::GetSingleton()->meleeCost1h);
		}
		if (a->GetActorValue(RE::ActorValue::kStamina) == 0) {
			DEBUG("{} is exhausted!", a->GetName());
			debuffHandler::initStaminaDebuff();
		}
	}

	/*restores stamina for actor A*/
	inline void restoreStamina(RE::Actor* a) {
		if (!wieldingOneHanded(a)) {
			restoreav(a, RE::ActorValue::kStamina, dataHandler::GetSingleton()->meleeHitStaminaRecover + dataHandler::GetSingleton()->meleeCost2h);
		}
		else {
			restoreav(a, RE::ActorValue::kStamina, dataHandler::GetSingleton()->meleeHitStaminaRecover + dataHandler::GetSingleton()->meleeCost1h);
		}
	}

	inline bool canAttack = true;

};

