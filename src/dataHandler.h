#pragma once
#include "Utils.h"
#include "SimpleIni.h"
#include "debuffHandler.h"
using namespace Utils;

class dataHandler
{

public:

	void readSettings();


	static dataHandler* GetSingleton()
	{
		static dataHandler singleton;
		return  std::addressof(singleton);
	}

	//cancels vanilla power stamina. 
	void cancelVanillaPowerStamina();
private:
	void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting);
	void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting);

};

namespace settings {
	//attacking
	static float staminaRegenMult_ = 5;
	static float combatStaminaRegenMult_ = 1;
	static float staminaRegenDelay_ = 3;
	static float meleeCostLightMiss = 30;
	static float meleeRecovLightHit = 20;
	static float meleeCostHeavyMiss = 33.3;
	static float meleeCostHeavyHit = 19.9;
	static bool blockedHitRegenStamina = true;
	static bool staminaMeterBlink = true;

	//blocking
	static bool bckToggle = true;
	static float bckShdStaminaPenaltyMult = 1;
	static float bckWpnStaminaPenaltyMult = 1;
};