#include "dataHandler.h"

namespace settings {
	//attacking
	float staminaRegenMult_ = 5;
	float combatStaminaRegenMult_ = 1;
	float staminaRegenDelay_ = 3;
	float meleeCostLightMiss = 30;
	float meleeRecovLightHit = 20;
	float meleeCostHeavyMiss = 33.3;
	float meleeCostHeavyHit = 19.9;
	bool blockedHitRegenStamina = false;
	bool staminaMeterBlink = true;
	bool NonCombatStaminaDebuff = true;
	//blocking
	bool bckToggle = true;
	bool guardBreak = true;

	float bckShdStaminaPenaltyMultPlayer = 1;
	float bckWpnStaminaPenaltyMultPlayer = 1;

	float bckShdStaminaPenaltyMultNPC = 1;
	float bckWpnStaminaPenaltyMultNPC = 1;

	//stun
	float stunBaseMult = 1; //base stun multiplier. base stun damage = melee damage * stunMult.;	//stun mult for light attack
	float stunHvyMult = 1; //stun mult for heavy attack

	float stunUnarmedMult = 1;
	float stunDaggerMult = 1;
	float stunSwordMult = 1;
	float stunAxeMult = 1;
	float stunMaceMult = 1;
	float stunGreatSwordMult = 1;
	float stun2hwMult = 1;
	float stunBowMult = 1;
	float stunCrossBowMult = 1;

	float stunExecutionDamageMult = 1; //mult for execution of stunned enemy


}
/*read settings from ini, and update them into game settings.*/
void dataHandler::readSettings() {
	DEBUG("loading ini settings");
	CSimpleIniA ini;
#define SETTINGFILE_PATH "Data\\MCM\\Settings\\ValhallaCombat.ini"
	ini.LoadFile(SETTINGFILE_PATH);
	using namespace settings;

	//values with "_" at the end need to be updated into game to take effect
	ReadFloatSetting(ini, "gameSetting", "fstaminaRegenMult", staminaRegenMult_);
	ReadFloatSetting(ini, "gameSetting", "fcombatStaminaRegenMult", combatStaminaRegenMult_);
	ReadFloatSetting(ini, "gameSetting", "fstaminaRegenDelay", staminaRegenDelay_);


	//read the 2 bools
	ReadBoolSetting(ini, "General", "bshieldCountAsHit", blockedHitRegenStamina);
	ReadBoolSetting(ini, "General", "bStaminaBarBlink", staminaMeterBlink);
	ReadBoolSetting(ini, "General", "bNonCombatStaminaDebuff", NonCombatStaminaDebuff);

	//read melee cost
	ReadFloatSetting(ini, "Stamina", "fmeleeCostLightMiss", meleeCostLightMiss); //this doesn't need to be divided by 100 since it's constnat.
	ReadFloatSetting(ini, "Stamina", "fmeleeCostHeavyMiss_percent", meleeCostHeavyMiss);
	ReadFloatSetting(ini, "Stamina", "fmeleeCostHeavyHit_percent", meleeCostHeavyHit);
	ReadFloatSetting(ini, "Stamina", "fmeleeRecoverLightHit_percent", meleeRecovLightHit);
	

	//read shield stamina cost
	ReadBoolSetting(ini, "Blocking", "bToggleStaminaBlock", bckToggle);
	ReadBoolSetting(ini, "Blocking", "bToggleGuardBreak", guardBreak);
	ReadFloatSetting(ini, "Blocking", "fbckWpnStaminaPenaltyMultPlayer", bckWpnStaminaPenaltyMultPlayer);
	ReadFloatSetting(ini, "Blocking", "fbckShdStaminaPenaltyMultPlayer", bckShdStaminaPenaltyMultPlayer);
	ReadFloatSetting(ini, "Blocking", "fbckWpnStaminaPenaltyMultNPC", bckWpnStaminaPenaltyMultNPC);
	ReadFloatSetting(ini, "Blocking", "fbckShdStaminaPenaltyMultNPC", bckShdStaminaPenaltyMultNPC);


	DEBUG("ini settings loaded");
	setGameSettingf("fSprintStaminaDrainMult", 0);
	//setGameSettingf("fStaminaBashBase", 0);
	setGameSettingf("fStaminaPowerBashBase", 0);
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
		DEBUG("found {} with value {}", a_settingName, bFound);
		a_setting = a_ini.GetBoolValue(a_sectionName, a_settingName);
	}
	else {
		INFO("failed to find {}, using default value", a_settingName);
	}
}
