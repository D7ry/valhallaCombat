#pragma once
#include "Utils.h"
#include "dataHandler.h"
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

	inline boolean canAttack = true;

};

