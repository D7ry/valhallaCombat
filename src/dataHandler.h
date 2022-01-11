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
	extern float staminaRegenMult_;
	extern float combatStaminaRegenMult_;
	extern float staminaRegenDelay_;
	extern float meleeCostLightMiss;
	extern float meleeRecovLightHit;
	extern float meleeCostHeavyMiss;
	extern float meleeCostHeavyHit;
	extern bool blockedHitRegenStamina;
	extern bool staminaMeterBlink;

	//blocking
	extern bool bckToggle;
	extern bool guardBreak;
	extern float bckShdStaminaPenaltyMultPlayer;
	extern float bckWpnStaminaPenaltyMultPlayer;
	extern float bckShdStaminaPenaltyMultNPC;
	extern float bckWpnStaminaPenaltyMultNPC;

	//stun
	extern float stunBaseMult; //base stun multiplier. base stun damage = melee damage * stunMult. Also serves to balance difficulty.
	extern float stunHvyMult; //stun mult for heavy attack

	extern float stunUnarmedMult;
	extern float stunDaggerMult;
	extern float stunSwordMult;
	extern float stunAxeMult;
	extern float stunMaceMult;
	extern float stunGreatSwordMult;
	extern float stun2hwMult;

	extern float stunBowMult;
	extern float stunCrossBowMult;

	extern float stunExecutionDamageMult; //mult for execution of stunned enemy

};