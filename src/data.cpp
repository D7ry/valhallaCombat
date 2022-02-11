#include "data.h"

/*read settings from ini, and update them into game settings.*/
void settings::readSettings() {
	INFO("Reading ini settings...");
	CSimpleIniA ini;
#define SETTINGFILE_PATH "Data\\MCM\\Settings\\ValhallaCombat.ini"
	ini.LoadFile(SETTINGFILE_PATH);


	//read general settings
	ReadBoolSetting(ini, "General", "bUIalert", bUIAlert);
	ReadBoolSetting(ini, "General", "bNonCombatStaminaDebuff", bNonCombatStaminaDebuff);
	ReadFloatSetting(ini, "gameSetting", "fStaminaRegenMult", fStaminaRegenMult);
	ReadFloatSetting(ini, "gameSettings", "fStaminaRegenLimit", fStaminaRegenLimit); //FIXME:add MCM config
	ReadFloatSetting(ini, "gameSetting", "fCombatStaminaRegenMult", fCombatStaminaRegenMult);
	ReadFloatSetting(ini, "gameSetting", "fStaminaRegenDelay", fStaminaRegenDelay);

	//read attack section
	ReadBoolSetting(ini, "General", "bBlockedHitRegenStamina", bBlockedHitRegenStamina);
	ReadFloatSetting(ini, "Attack", "fMeleeCostLightMiss_Point", fMeleeCostLightMiss_Point); //this doesn't need to be divided by 100 since it's constnat.
	ReadFloatSetting(ini, "Attack", "fMeleeRewardLightHit_Percent", fMeleeRewardLightHit_Percent);
	ReadFloatSetting(ini, "Attack", "fMeleeCostHeavyMiss_Percent", fMeleeCostHeavyMiss_Percent);
	ReadFloatSetting(ini, "Attack", "fMeleeCostHeavyHit_Percent", fMeleeCostHeavyHit_Percent);

	//read block section
	ReadBoolSetting(ini, "Block", "bStaminaBlocking", bStaminaBlocking);
	ReadBoolSetting(ini, "Block", "bGuardBreak", bGuardBreak);
	ReadFloatSetting(ini, "Block", "fBckShdStaminaMult_NPC_Block_NPC", fBckShdStaminaMult_NPC_Block_NPC);
	ReadFloatSetting(ini, "Block", "fBckShdStaminaMult_NPC_Block_PC", fBckShdStaminaMult_NPC_Block_PC);
	ReadFloatSetting(ini, "Block", "fBckShdStaminaMult_PC_Block_NPC", fBckShdStaminaMult_PC_Block_NPC);
	ReadFloatSetting(ini, "Block", "fBckWpnStaminaMult_NPC_Block_NPC", fBckWpnStaminaMult_NPC_Block_NPC);
	ReadFloatSetting(ini, "Block", "fBckWpnStaminaMult_NPC_Block_PC", fBckWpnStaminaMult_NPC_Block_PC);
	ReadFloatSetting(ini, "Block", "fBckWpnStaminaMult_PC_Block_NPC", fBckWpnStaminaMult_PC_Block_NPC);

	//read dodge section
	ReadFloatSetting(ini, "Dodging", "fDodgeStaminaShort", fDodgeStaminaShort);
	ReadFloatSetting(ini, "Dodging", "fDodgeStaminaLong", fDodgeStaminaLong);
	ReadBoolSetting(ini, "Dodging", "bTKDodgeCompatibility", bTKDodgeCompatibility);
	ReadBoolSetting(ini, "Dodging", "bDMCOCompatibility", bDMCOCompatibility);
	ReadBoolSetting(ini, "Dodging", "bCGOCompatibility", bCGOCompatibility);
	ReadBoolSetting(ini, "Dodging", "bTUDMCompatibility", bTUDMCompatibility);

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
