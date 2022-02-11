#include "data.h"

/*read settings from ini, and update them into game settings.*/
void settings::readSettings() {
	INFO("Reading ini settings...");
	CSimpleIniA ini;
#define SETTINGFILE_PATH "Data\\MCM\\Settings\\ValhallaCombat.ini"
	ini.LoadFile(SETTINGFILE_PATH);

	//values with "_" at the end need to be updated into game to take effect
	ReadFloatSetting(ini, "gameSetting", "fStaminaRegenMult", fStaminaRegenMult);
	ReadFloatSetting(ini, "gameSettings", "fStaminaRegenLimit", fStaminaRegenLimit); //FIXME:add MCM config
	ReadFloatSetting(ini, "gameSetting", "fCombatStaminaRegenMult", fCombatStaminaRegenMult);
	ReadFloatSetting(ini, "gameSetting", "fStaminaRegenDelay", fStaminaRegenDelay);


	//read the 3 bools
	ReadBoolSetting(ini, "General", "bBlockedHitRegenStamina", bBlockedHitRegenStamina);
	ReadBoolSetting(ini, "General", "bUIalert", bUIAlert);
	ReadBoolSetting(ini, "General", "bNonCombatStaminaDebuff", bNonCombatStaminaDebuff);

	//read melee cost
	ReadFloatSetting(ini, "Stamina", "fMeleeCostLightMiss_Point", fMeleeCostLightMiss_Point); //this doesn't need to be divided by 100 since it's constnat.
	ReadFloatSetting(ini, "Stamina", "fMeleeRewardLightHit_Percent", fMeleeRewardLightHit_Percent);
	ReadFloatSetting(ini, "Stamina", "fMeleeCostHeavyMiss_Percent", fMeleeCostHeavyMiss_Percent);
	ReadFloatSetting(ini, "Stamina", "fMeleeCostHeavyHit_Percent", fMeleeCostHeavyHit_Percent);

	//read shield stamina cost
	ReadBoolSetting(ini, "Blocking", "bStaminaBlocking", bStaminaBlocking);
	ReadBoolSetting(ini, "Blocking", "bGuardBreak", bGuardBreak);
	ReadFloatSetting(ini, "Blocking", "fBckShdStaminaMult_NPC_Block_NPC", fBckShdStaminaMult_NPC_Block_NPC);
	ReadFloatSetting(ini, "Blocking", "fBckShdStaminaMult_NPC_Block_PC", fBckShdStaminaMult_NPC_Block_PC);
	ReadFloatSetting(ini, "Blocking", "fBckShdStaminaMult_PC_Block_NPC", fBckShdStaminaMult_PC_Block_NPC);
	ReadFloatSetting(ini, "Blocking", "fBckWpnStaminaMult_NPC_Block_NPC", fBckWpnStaminaMult_NPC_Block_NPC);
	ReadFloatSetting(ini, "Blocking", "fBckWpnStaminaMult_NPC_Block_PC", fBckWpnStaminaMult_NPC_Block_PC);
	ReadFloatSetting(ini, "Blocking", "fBckWpnStaminaMult_PC_Block_NPC", fBckWpnStaminaMult_PC_Block_NPC);


	INFO("Ini settings read.");
	setGameSettingf("fDamagedStaminaRegenDelay", fStaminaRegenDelay);
	setGameSettingf("fCombatStaminaRegenRateMult", fCombatStaminaRegenMult);
	multStaminaRegen(fStaminaRegenMult, fStaminaRegenLimit);
	INFO("Game settings applied.");
}



void settings::ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting)
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

void settings::ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting)
{
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound)
	{
		INFO("found {} with value {}", a_settingName, bFound);
		a_setting = a_ini.GetBoolValue(a_sectionName, a_settingName);
	}
	else {
		INFO("failed to find {}, using default value", a_settingName);
	}
}
