#include "C:/Users/Ty/Documents/SKSEPlugins/Build/Plugins/valhallaCombat/CMakeFiles/valhallaCombat.dir/Debug/cmake_pch.hxx"
#include "dataHandler.h"



dataHandler::dataHandler() {
	DEBUG("initializing data handler");
	DEBUG("loading ini settings");
	CSimpleIniA ini;
	#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\valHallaCombat.ini"
	ini.LoadFile(SETTINGFILE_PATH);

	staminaRegenMult_ = ini.GetDoubleValue("gameSetting", "staminaRegenMult", 5);
	combatStaminaRegenMult_ = ini.GetDoubleValue("gameSetting", "combatStaminaRegenMult", 1);
	staminaRegenDelay_ = ini.GetDoubleValue("gameSetting", "staminaRegenDelay", 3);

	meleeCostLightMiss = ini.GetDoubleValue("Stamina", "meleeCostLightMiss", 30); //one and only not in percent
	meleeCostHeavyMiss = ini.GetDoubleValue("Stamina", "meleeCostHeavyMiss_percent", 0.33);
	meleeCostHeavyHit = ini.GetDoubleValue("Stamina", "meleeCostHeavyHit_percent", 0.19);
	meleeRecovLightHit = ini.GetDoubleValue("Stamina", "meleeRecoverLightHit_percent", 0.3);

	shieldCountAsHit = ini.GetBoolValue("General", "shieldCountAsHit", false);

	DEBUG("ini settings loaded");
	//debuffHandler::GetSingleton()->staminaBlink = ini.GetBoolValue("UI", "StaminaBarBlink", false);
	DEBUG("ini settings loaded2");
}