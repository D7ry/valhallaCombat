#include "C:/Users/Ty/Documents/SKSEPlugins/Build/Plugins/valhallaCombat/CMakeFiles/valhallaCombat.dir/Debug/cmake_pch.hxx"
#include "dataHandler.h"
dataHandler::dataHandler() {
	CSimpleIniA ini;
	#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\valHallaCombat.ini"
	ini.LoadFile(SETTINGFILE_PATH);
	meleeCostLight = ini.GetLongValue("Stamina", "meleeCostLight", 30);
	meleeCostHeavy = ini.GetLongValue("Stamina", "meleeCostHeavy", 0.33); //in percent
	meleeHitStaminaCostHeavy = ini.GetLongValue("Stamina", "meleeHitStaminaCost_Heavy", 0.2);
	meleeHitStaminaRecoverLight = ini.GetLongValue("Stamina", "meleeHitStaminaRecover_Light", 0.33);
	staminaRegenMult_ = ini.GetLongValue("gameSetting", "staminaRegenMult", 5);
	combatStaminaRegenMult_ = ini.GetLongValue("gameSetting", "combatStaminaRegenMult", 1);
	staminaRegenDelay_ = ini.GetLongValue("gameSetting", "staminaRegenDelay", 3);


	debuffHandler::GetSingleton()->staminaBlink = ini.GetBoolValue("UI", "StaminaBarBlink", false);
}