#pragma once
#include "Utils.h"
#include "SimpleIni.h"
#include "debuffHandler.h"
using namespace Utils;


/*All the settings of Valhalla combat*/
class settings
{
public:
	/*General*/
	static inline float fStaminaRegenMult = 5;
	static inline float fStaminaRegenLimit = 50;
	static inline float fCombatStaminaRegenMult = 1;
	static inline float fStaminaRegenDelay = 2;
	/*Attacking*/
	static inline bool bBlockedHitRegenStamina = false;
	static inline float fMeleeCostLightMiss_Point = 30;
	static inline float fMeleeRewardLightHit_Percent = 0.2;
	static inline float fMeleeCostHeavyMiss_Percent = 0.333;
	static inline float fMeleeCostHeavyHit_Percent = 0.199;

	/*UI*/
	static inline bool bUIAlert = true;
	static inline bool bNonCombatStaminaDebuff = true;

	/*Blocking*/
	static inline bool bStaminaBlocking = true;
	static inline bool bGuardBreak = true;
	static inline float fBckShdStaminaMult_PC_Block_NPC = 2;
	static inline float fBckWpnStaminaMult_PC_Block_NPC = 2;

	static inline float fBckShdStaminaMult_NPC_Block_PC = 2; //stamina penalty mult for NPCs blockign a player hit with a shield
	static inline float fBckWpnStaminaMult_NPC_Block_PC = 2;

	static inline float fBckShdStaminaMult_NPC_Block_NPC = 2;
	static inline float fBckWpnStaminaMult_NPC_Block_NPC = 2;


	/*Compatibility for dodge framework*/
	static inline bool bTKDodgeCompatibility = false;
	static inline bool bDMCOCompatibility = false;
	static inline bool bCGOCompatibility = false;
	static inline bool bTUDMCompatibility = false;
	/*additional stamina override. This override does not limit dodging.*/
	static inline float fDodgeStaminaLong = 0; //additional stamina cost for doing a dodge roll, only works for DMCO.
	static inline float fDodgeStaminaShort = 0;



	//perfect blocking
	//FIXME: add MCM helper

	//TODO:Settings to add to MCM and readSettings()
	static inline bool bPoiseCompatibility = false;
	static inline bool bPerfectBlocking = true;
	static inline bool bPerfectBlockingScreenShake = true;
	static inline bool bPerfectBlockingSFX = true;
	static inline bool bPerfectBlockingVFX = true;
	static inline float fPerfectBlockTime = 0.3;
	static inline float fPerfectBlockCoolDownTime = 1;


	//Stun
	static inline float fStunParryMult = 1;
	static void readSettings();

private:
	static void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting);
	static void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting);

};

struct data
{
public:

	static data* GetSingleton()
	{
		static data singleton;
		return  std::addressof(singleton);
	}
	RE::TESSound* soundParryShield = nullptr;
	RE::TESSound* soundParryWeapon = nullptr;
	RE::BGSPerk* debuffPerk = nullptr;
	RE::BGSSoundDescriptorForm* soundParryShieldD = nullptr;
	RE::BGSSoundDescriptorForm* soundParryWeaponD = nullptr;
	/*Fetch data from game and store them for latter use.*/
	void fetchGameData() {
		INFO("Fetching data from game...");
		debuffPerk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(0x2DB2, "ValhallaCombat.esp");
		if (debuffPerk) {
			INFO("Debuff perk successfully loaded!");
		}
		soundParryShield = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESSound>(0X433D, "ValhallaCombat.esp");
		soundParryWeapon = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESSound>(0X433E, "ValhallaCombat.esp");
		if (soundParryShield && soundParryWeapon) {
			INFO("Sound successfully loaded!");
		}
		soundParryShieldD = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0X433C, "ValhallaCombat.esp");
		soundParryWeaponD = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0X3DD9, "ValhallaCombat.esp");
		if (soundParryShieldD && soundParryWeaponD) {
			INFO("SoundD successfully loaded!");
		}
		INFO("Data fetched.");
	}
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