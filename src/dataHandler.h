#pragma once
#include "Utils.h"
using namespace Utils;
namespace dataHandler
{

	inline float meleeCost1h = 20;
	inline float meleeCost2h = 35;
	inline float meleeHitStaminaRecover = 50; //stamina recovered from hitting an enemy with a light attack.

	namespace gameSettings {
		inline const char* powerAtkStaminaCostMultiplier = "fPowerAttackStaminaPenalty";
		inline const char* staminaRegenDelay = "fDamagedStaminaRegenDelay";
		inline const char* atkStaminaCostBase = "fStaminaAttackWeaponBase";
		inline const char* atkStaminaCostMult = "fStaminaAttackWeaponMult";
		inline const char* combatStaminaRegenMult = "fCombatStaminaRegenRateMult";
		/*power atk stamina formula:
		(wpn weight * fStaminaAttackWeaponMult + 1 * fStaminaAttackWeaponBase ) * fPowerAttackStaminaPenalty
		*/

		inline void tweakGameSetting() {
			DEBUG("tweaking game setting");
			setGameSettingf(gameSettings::powerAtkStaminaCostMultiplier, 1);
			setGameSettingf(gameSettings::staminaRegenDelay, 3);
			setGameSettingf(gameSettings::atkStaminaCostMult, 0);
			setGameSettingf(gameSettings::atkStaminaCostBase, 20);
			setGameSettingf(gameSettings::combatStaminaRegenMult, 1);
			multStaminaRegen(5);
		}

	}

	inline void refreshData() {
		meleeCost1h = 20;
		meleeCost2h = 35;
	}

	inline void setupData() {
		refreshData();
		gameSettings::tweakGameSetting();
	}
};


