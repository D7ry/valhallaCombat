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

	//attacking
	float staminaRegenMult_ = 5;
	float combatStaminaRegenMult_ = 1;
	float staminaRegenDelay_ = 3;
	float meleeCostLightMiss = 30;
	float meleeRecovLightHit = 20;
	float meleeCostHeavyMiss = 33.3;
	float meleeCostHeavyHit = 19.9;
	bool blockedHitRegenStamina = true;
	bool staminaMeterBlink = true;

	//blocking
	bool bckToggle = true;
	float bckShdStaminaPenaltyMult = 1; 
	float bckWpnStaminaPenaltyMult = 1; 

	//cancels vanilla power stamina. 
	void cancelVanillaPowerStamina();
private:
	void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting);
	void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting);

};

