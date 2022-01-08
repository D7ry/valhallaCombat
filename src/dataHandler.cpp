#include "dataHandler.h"


/*read settings from ini, and update them into game settings.*/
void dataHandler::readSettings() {
	DEBUG("loading ini settings");
	CSimpleIniA ini;
#define SETTINGFILE_PATH "Data\\MCM\\Settings\\Valhamina.ini"
	ini.LoadFile(SETTINGFILE_PATH);

	//values with "_" at the end need to be updated into game to take effect
	ReadFloatSetting(ini, "gameSetting", "fstaminaRegenMult", staminaRegenMult_);
	ReadFloatSetting(ini, "gameSetting", "fcombatStaminaRegenMult", combatStaminaRegenMult_);
	ReadFloatSetting(ini, "gameSetting", "fstaminaRegenDelay", staminaRegenDelay_);


	//read the 2 bools
	ReadBoolSetting(ini, "General", "bshieldCountAsHit", blockedHitRegenStamina);
	ReadBoolSetting(ini, "General", "bStaminaBarBlink", staminaMeterBlink);

	//read melee cost
	ReadFloatSetting(ini, "Stamina", "fmeleeCostLightMiss", meleeCostLightMiss); //this doesn't need to be divided by 100 since it's constnat.
	ReadFloatSetting(ini, "Stamina", "fmeleeCostHeavyMiss_percent", meleeCostHeavyMiss);
	ReadFloatSetting(ini, "Stamina", "fmeleeCostHeavyHit_percent", meleeCostHeavyHit);
	ReadFloatSetting(ini, "Stamina", "fmeleeRecoverLightHit_percent", meleeRecovLightHit);
	

	//read shield stamina cost
	ReadBoolSetting(ini, "Blocking", "bToggleStaminaBlock", bckToggle);
	ReadFloatSetting(ini, "Blocking", "fbckWpnStaminaPenaltyMult", bckWpnStaminaPenaltyMult);
	ReadFloatSetting(ini, "Blocking", "fbckShdStaminaPenaltyMult", bckShdStaminaPenaltyMult);


	DEBUG("ini settings loaded");
	setGameSettingf("fSprintStaminaDrainMult", 0);
	setGameSettingf("fDamagedStaminaRegenDelay", staminaRegenDelay_);
	setGameSettingf("fCombatStaminaRegenRateMult", combatStaminaRegenMult_);
	multStaminaRegen(staminaRegenMult_);
	DEBUG("game settings applied");
}

//cancels vanilla power attack stamina costs by tweaking game settings.
void dataHandler::cancelVanillaPowerStamina() {
	DEBUG("tweaking game setting");
	//negating vanilla power attack formula
	setGameSettingf("fPowerAttackDefaultBonus", 0);
	setGameSettingf("fPowerAttackStaminaPenalty", 0);
	setGameSettingf("fStaminaAttackWeaponMult", 0);
	setGameSettingf("fStaminaAttackWeaponBase", 0);
}


void dataHandler::ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting)
{
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound) {
		INFO("found {} with value {}", a_settingName, bFound);
		a_setting = static_cast<float>(a_ini.GetDoubleValue(a_sectionName, a_settingName));
	}
	else {
		INFO("failed to find {}, using default value", a_settingName);
	}
}

void dataHandler::ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting)
{
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound)
	{
		a_setting = a_ini.GetBoolValue(a_sectionName, a_settingName);
	}
}
