#include "C:/Users/Ty/Documents/SKSEPlugins/Build/Plugins/valhallaCombat/CMakeFiles/valhallaCombat.dir/Debug/cmake_pch.hxx"
#include "dataHandler.h"
dataHandler::dataHandler() {
	DEBUG("initializing data handler");
	DEBUG("loading ini settings");
	CSimpleIniA ini;
	#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\valHallaCombat.ini"
	ini.LoadFile(SETTINGFILE_PATH);

	staminaRegenMult_ = ini.GetDoubleValue("Stamina", "staminaRegenMult", 5);
	combatStaminaRegenMult_ = ini.GetDoubleValue("Stamina", "combatStaminaRegenMult", 1);
	staminaRegenDelay_ = ini.GetDoubleValue("Stamina", "staminaRegenDelay", 3);

	meleeCostLightMiss = ini.GetDoubleValue("Stamina", "meleeCostLightMiss", 30); //one and only not in percent
	meleeCostHeavyMiss = ini.GetDoubleValue("Stamina", "meleeCostHeavyMiss_percent", 0.33);
	meleeCostHeavyHit = ini.GetDoubleValue("Stamina", "meleeCostHeavyHit_percent", 0.19);
	meleeRecovLightHit = ini.GetDoubleValue("Stamina", "meleeRecovLightHit_percent", 0.3);

	DEBUG("ini settings loaded");
	//debuffHandler::GetSingleton()->staminaBlink = ini.GetBoolValue("UI", "StaminaBarBlink", false);
	DEBUG("ini settings loaded2");
}