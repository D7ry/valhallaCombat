#include "C:/Users/Ty/Documents/SKSEPlugins/Build/Plugins/valhallaCombat/CMakeFiles/valhallaCombat.dir/Debug/cmake_pch.hxx"
#include "dataHandler.h"
dataHandler::dataHandler() {
	CSimpleIniA ini;
	#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\valHallaCombat.ini"
	ini.LoadFile(SETTINGFILE_PATH);
	meleeCost1h = ini.GetLongValue("Consumption", "meleeCost1h", 20);
	meleeCost2h = ini.GetLongValue("Consumption", "meleeCost2h", 35);
	meleeHitStaminaRecover2h = ini.GetLongValue("Consumption", "meleeHitStaminaRecover2h", 50);
	meleeHitStaminaRecover1h = ini.GetLongValue("Consumption", "meleeHitStaminaRecover1h", 40);
	staminaRegenMult_ = ini.GetLongValue("gameSetting", "staminaRegenMult", 5);
	combatStaminaRegenMult_ = ini.GetLongValue("gameSetting", "combatStaminaRegenMult", 1);
	staminaRegenDelay_ = ini.GetLongValue("gameSetting", "staminaRegenDelay", 3);
	debuffHandler::staminaBlink = ini.GetBoolValue("UI", "StaminaBarBlink", true);
}