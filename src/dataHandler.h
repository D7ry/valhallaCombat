#pragma once
#include "Utils.h"
#include "SimpleIni.h"
using namespace Utils;

class dataHandler
{
public:
	dataHandler();
	float meleeCost1h;
	float meleeCost2h;
	float meleeHitStaminaRecover; //stamina recovered from hitting an enemy with a light attack.
	float staminaRegenMult_;
	float combatStaminaRegenMult_;
	float staminaRegenDelay_;

	static dataHandler* GetSingleton()
	{
		static dataHandler singleton;
		return  std::addressof(singleton);
	}

	void updateMCMchanges() {

	}
};

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
		setGameSettingf(gameSettings::staminaRegenDelay, dataHandler::GetSingleton()->staminaRegenDelay_);
		setGameSettingf(gameSettings::atkStaminaCostMult, 0);
		setGameSettingf(gameSettings::atkStaminaCostBase, 20);
		setGameSettingf(gameSettings::combatStaminaRegenMult, dataHandler::GetSingleton()->combatStaminaRegenMult_);
		multStaminaRegen(dataHandler::GetSingleton()->staminaRegenMult_);
	}
}

