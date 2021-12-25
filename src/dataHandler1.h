#pragma once
#include "Utils.h"
#include "SimpleIni.h"
using namespace Utils;

#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\valHallaCombat.ini"
namespace dataHandler
{
	inline CSimpleIniA ini;
	inline float meleeCost1h = 20;
	inline float meleeCost2h = 35;
	inline float meleeHitStaminaRecover = 50; //stamina recovered from hitting an enemy with a light attack.
	inline float staminaRegenMult_ = 5;
	inline float combatStaminaRegenMult_ = 1;
	inline float staminaRegenDelay_ = 3;

	inline void refreshData() {
		ini.LoadFile(SETTINGFILE_PATH);
		meleeCost1h = ini.GetLongValue("Consumption", "meleeCost1h", 20);
		meleeCost2h = ini.GetLongValue("Consumption", "meleeCost2h", 35);
		meleeHitStaminaRecover = ini.GetLongValue("Consumption", "meleeHitStaminaRecover", 20);
		staminaRegenMult_ = ini.GetLongValue("Consumption", "staminaRegenMult", 5);
		combatStaminaRegenMult_ = ini.GetLongValue("Consumption", "combatStaminaRegenMult", 1);
		staminaRegenDelay_ = ini.GetLongValue("Consumption", "staminaRegenDelay", 3);
	}

	inline void setupData() {
		refreshData();
		gameSettings::tweakGameSetting();
	}


}

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
		setGameSettingf(gameSettings::staminaRegenDelay, staminaRegenDelay_);
		setGameSettingf(gameSettings::atkStaminaCostMult, 0);
		setGameSettingf(gameSettings::atkStaminaCostBase, 20);
		setGameSettingf(gameSettings::combatStaminaRegenMult, combatStaminaRegenMult_);
		multStaminaRegen(staminaRegenMult_);
	}
}


