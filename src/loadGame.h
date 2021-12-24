#pragma once
#include "animEventHandler.h"
#include "onHitEventHandler.h"
#include "Utils.h"
namespace gameSettings {
	inline const char* powerAtkStaminaCostMultiplier = "fPowerAttackStaminaPenalty";
	inline const char* staminaRegenDelay = "fDamagedStaminaRegenDelay";
	inline const char* atkStaminaCostBase = "fStaminaAttackWeaponBase";
	inline const char* atkStaminaCostMult = "fStaminaAttackWeaponMult";
	inline const char* blockStaminaRegen = "fBlockStaminaRate";
	/*power atk stamina formula:
	(wpn weight * fStaminaAttackWeaponMult + 1 * fStaminaAttackWeaponBase ) * fPowerAttackStaminaPenalty
	*/
}

namespace loadGame
{
	//static void registerAnimEvents();

	void EventCallBACK(SKSE::MessagingInterface::Message* msg);
	inline void tweakGameSetting(); //tweaks game setting to desired values
	void init(); //sets up variables and shenanigans

};

