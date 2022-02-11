#pragma once
#include "Utils.h"
#include "SimpleIni.h"
#include "debuffHandler.h"
using namespace Utils;

class settings
{
public:
	
	/*Attacking*/
	static inline float fStaminaRegenMult = 5;
	static inline float fStaminaRegenLimit = 50;
	static inline float fCombatStaminaRegenMult = 1;
	static inline float fStaminaRegenDelay = 2;
	static inline float fMeleeCostLightMiss_Point = 30;
	static inline float fMeleeRewardLightHit_Percent = 0.2;
	static inline float fMeleeCostHeavyMiss_Percent = 0.333;
	static inline float fMeleeCostHeavyHit_Percent = 0.199;

	/*UI*/
	static inline bool bBlockedHitRegenStamina = true;
	static inline bool bUIAlert = true;
	static inline bool bNonCombatStaminaDebuff = true;
	static inline float fMeterFlashTimeDifference = 0.5; //time difference between two meter flash, if UI alter is active.

	/*Blocking*/
	static inline bool bStaminaBlocking = true;
	static inline bool bGuardBreak = true;
	static inline float fBckShdStaminaMult_PC_Block_NPC = 2;
	static inline float fBckWpnStaminaMult_PC_Block_NPC = 2;

	static inline float fBckShdStaminaMult_NPC_Block_PC = 2; //stamina penalty mult for NPCs blockign a player hit with a shield
	static inline float fBckWpnStaminaMult_NPC_Block_PC = 2;

	static inline float fBckShdStaminaMult_NPC_Block_NPC = 2;
	static inline float fBckWpnStaminaMult_NPC_Block_NPC = 2;


	static void readSettings();

	/*Compatibility for dodge framework*/
	static inline bool bTKDodgeCompatibility = false;
	static inline bool bDMCOCompatibility = true;
	/*additional stamina*/
	static inline float fDodgeStaminaRoll = 20; //additional stamina cost for doing a dodge roll, only works for DMCO.
	static inline float fDodgeStaminaStep = 30;
private:
	static void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting);
	static void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting);

};

//struct settingsOld {


	//FIXME:One day implement stun
	/*
	static inline float stunBaseMult; //base stun multiplier. base stun damage = melee damage * stunMult. Also serves to balance difficulty.
	static inline float stunHvyMult; //stun mult for heavy attack

	static inline float stunUnarmedMult;
	static inline float stunDaggerMult;
	static inline float stunSwordMult;
	static inline float stunAxeMult;
	static inline float stunMaceMult;
	static inline float stunGreatSwordMult;
	static inline float stun2hwMult;

	static inline float stunBowMult;
	static inline float stunCrossBowMult;

	static inline float stunExecutionDamageMult; //mult for execution of stunned enemy

	*/

//};