#include "include/stunHandler.h"
#include "include/settings.h"
using namespace Utils;
void settings::ReadIntSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, uint32_t& a_setting) {
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound) {
		INFO("found {} with value {}", a_settingName, bFound);
		a_setting = static_cast<int>(a_ini.GetDoubleValue(a_sectionName, a_settingName));
	}
	else {
		INFO("failed to find {}, using default value", a_settingName);
	}
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
/*read settings from ini, and update them into game settings.*/
void settings::readSettings() {
	INFO("Reading ini settings...");
	CSimpleIniA ini;
#define SETTINGFILE_PATH "Data\\MCM\\Settings\\ValhallaCombat.ini"
	ini.LoadFile(SETTINGFILE_PATH);
	std::list<CSimpleIniA::Entry> ls;
	/*Read stamina section*/
	ReadBoolSetting(ini, "Stamina", "bUIalert", bUIAlert);
	ReadBoolSetting(ini, "Stamina", "bNonCombatStaminaDebuff", bNonCombatStaminaDebuff);
	ReadFloatSetting(ini, "Stamina", "fStaminaRegenMult", fStaminaRegenMult);
	ReadFloatSetting(ini, "Stamina", "fStaminaRegenLimit", fStaminaRegenLimit);
	ReadFloatSetting(ini, "Stamina", "fCombatStaminaRegenMult", fCombatStaminaRegenMult);
	ReadFloatSetting(ini, "Stamina", "fStaminaRegenDelay", fStaminaRegenDelay);

	ReadBoolSetting(ini, "Stamina", "bBlockStaminaToggle", bBlockStaminaToggle);
	ReadBoolSetting(ini, "Stamina", "bGuardBreak", bGuardBreak);
	ReadFloatSetting(ini, "Stamina", "fBckShdStaminaMult_NPC_Block_NPC", fBckShdStaminaMult_NPC_Block_NPC);
	ReadFloatSetting(ini, "Stamina", "fBckShdStaminaMult_NPC_Block_PC", fBckShdStaminaMult_NPC_Block_PC);
	ReadFloatSetting(ini, "Stamina", "fBckShdStaminaMult_PC_Block_NPC", fBckShdStaminaMult_PC_Block_NPC);
	ReadFloatSetting(ini, "Stamina", "fBckWpnStaminaMult_NPC_Block_NPC", fBckWpnStaminaMult_NPC_Block_NPC);
	ReadFloatSetting(ini, "Stamina", "fBckWpnStaminaMult_NPC_Block_PC", fBckWpnStaminaMult_NPC_Block_PC);
	ReadFloatSetting(ini, "Stamina", "fBckWpnStaminaMult_PC_Block_NPC", fBckWpnStaminaMult_PC_Block_NPC);

	ReadBoolSetting(ini, "Stamina", "bAttackStaminaToggle", bAttackStaminaToggle);
	ReadBoolSetting(ini, "Stamina", "bBlockedHitRegenStamina", bBlockedHitRegenStamina);
	ReadFloatSetting(ini, "Stamina", "fMeleeCostLightMiss_Point", fMeleeCostLightMiss_Point); 
	ReadFloatSetting(ini, "Stamina", "fMeleeRewardLightHit_Percent", fMeleeRewardLightHit_Percent);
	ReadFloatSetting(ini, "Stamina", "fMeleeCostHeavyMiss_Percent", fMeleeCostHeavyMiss_Percent);
	ReadFloatSetting(ini, "Stamina", "fMeleeCostHeavyHit_Percent", fMeleeCostHeavyHit_Percent);

	/*Read perfect blocking section*/
	ReadBoolSetting(ini, "Parrying", "bPerfectBlockToggle", bPerfectBlockToggle);
	ReadBoolSetting(ini, "Parrying", "bPerfectBlockScreenShake", bPerfectBlockScreenShake);
	ReadBoolSetting(ini, "Parrying", "bPerfectBlockSFX", bPerfectBlockSFX);
	ReadBoolSetting(ini, "Blocking", "bPerfectBlockVFX", bPerfectBlockVFX);
	ReadFloatSetting(ini, "Parrying", "fPerfectBlockTime", fPerfectBlockTime);
	ReadFloatSetting(ini, "Parrying", "fPerfectBlockCoolDownTime", fPerfectBlockCoolDownTime);

	/*Read stun section*/
	ReadBoolSetting(ini, "Stun", "bStunToggle", bStunToggle);
	ReadBoolSetting(ini, "Stun", "bStunMeterToggle", bStunMeterToggle);
	ReadBoolSetting(ini, "Stun", "bExecutionLimit", bExecutionLimit);

	ReadFloatSetting(ini, "Stun", "fStunParryMult", fStunParryMult);
	ReadFloatSetting(ini, "Stun", "fStunBashMult", fStunBashMult);
	ReadFloatSetting(ini, "Stun", "fStunPowerBashMult", fStunPowerBashMult);
	ReadFloatSetting(ini, "Stun", "fStunNormalAttackMult", fStunNormalAttackMult);
	ReadFloatSetting(ini, "Stun", "fStunPowerAttackMult", fStunPowerAttackMult);

	ReadFloatSetting(ini, "Stun", "fStunUnarmedMult", fStunUnarmedMult);
	ReadFloatSetting(ini, "Stun", "fStunDaggerMult", fStunDaggerMult);
	ReadFloatSetting(ini, "Stun", "fStunSwordMult", fStunSwordMult);
	ReadFloatSetting(ini, "Stun", "fStunWarAxeMult", fStunWarAxeMult);
	ReadFloatSetting(ini, "Stun", "fStunMaceMult", fStunMaceMult);
	ReadFloatSetting(ini, "Stun", "fStunGreatSwordMult", fStunGreatSwordMult);
	ReadFloatSetting(ini, "Stun", "fStun2HBluntMult", fStun2HBluntMult);

	ReadBoolSetting(ini, "Stun", "bAutoExecution", bAutoExecution);
	ReadIntSetting(ini, "Stun", "uExecutionKey", uExecutionKey);
	ReadBoolSetting(ini, "Compatibility", "bPoiseCompatibility", bPoiseCompatibility);
	INFO("Ini settings read.");

	/*Set some game settings*/
	setGameSettingf("fDamagedStaminaRegenDelay", fStaminaRegenDelay);
	setGameSettingf("fCombatStaminaRegenRateMult", fCombatStaminaRegenMult);
	multStaminaRegen(fStaminaRegenMult, fStaminaRegenLimit);

	/*Release truehud meter if set so.*/
	if (bStunToggle && bStunMeterToggle) {
		stunHandler::GetSingleton()->initTrueHUDStunMeter();
	}
	else {
		stunHandler::GetSingleton()->releaseTrueHUDStunMeter();
	}
	INFO("Game settings applied.");
}

