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
	static inline float fStunBashMult = 1;
	static inline float fStunPowerBashMult = 1;
	static inline float fStunPowerAttackMult = 1;

	static inline float fStunUnarmedMult = 1;
	static inline float fStunDaggerMult = 1;
	static inline float fStunSwordMult = 1;
	static inline float fStunWarAxeMult = 1;
	static inline float fStunMaceMult = 1;
	static inline float fStunGreatSwordMult = 1;
	static inline float fStun2HBluntMult = 1;


	//execute
	static inline bool bExecutionToggle = true; //enable/disable execution
	/*Toggle for player to be executed.*/
	static inline bool bPlayerExecution = false;
	/*Toggle for player teammate to be executed.*/
	static inline bool bPlayerTeammateExecution = false;
	/*Toggle for essential NPCs to be executed.*/
	static inline bool bEssentialExecution = false;
	static inline bool bExecutionLevelLimit = true; //NPCs with level higher than player cannot be executed unless under certain health threshold.
	static void readSettings();

private:
	static void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting);
	static void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting);

};

/*Everything stored in game.*/
struct data
{
public:

	static data* GetSingleton()
	{
		static data singleton;
		return  std::addressof(singleton);
	}
	RE::BGSPerk* debuffPerk = nullptr;
	RE::BGSSoundDescriptorForm* soundParryShieldD = nullptr;
	RE::BGSSoundDescriptorForm* soundParryWeaponD = nullptr;

	float fDiffMultHPByPCVE;
	float fDiffMultHPByPCE;
	float fDiffMultHPByPCN;
	float fDiffMultHPByPCH;
	float fDiffMultHPByPCVH;
	float fDiffMultHPByPCL;
	float fDiffMultHPToPCVE;
	float fDiffMultHPToPCE;
	float fDiffMultHPToPCN;
	float fDiffMultHPToPCH;
	float fDiffMultHPToPCVH;
	float fDiffMultHPToPCL;

	/*Fetch data from game and store them for latter use.*/
	void fetchGameData() {
		INFO("Fetching data from game...");
		debuffPerk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(0x2DB2, "ValhallaCombat.esp");
		if (debuffPerk) {
			INFO("Debuff perk successfully loaded!");
		}
		soundParryShieldD = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0X433C, "ValhallaCombat.esp");
		soundParryWeaponD = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0X3DD9, "ValhallaCombat.esp");
		if (soundParryShieldD && soundParryWeaponD) {
			INFO("SoundD successfully loaded!");
		}
		auto gameSettings = RE::GameSettingCollection::GetSingleton();
		fDiffMultHPByPCVE = gameSettings->GetSetting("fDiffMultHPByPCVE")->GetFloat(); DEBUG(fDiffMultHPByPCVE);
		fDiffMultHPByPCE = gameSettings->GetSetting("fDiffMultHPByPCE")->GetFloat(); DEBUG(fDiffMultHPByPCE);
		fDiffMultHPByPCN = gameSettings->GetSetting("fDiffMultHPByPCN")->GetFloat(); DEBUG(fDiffMultHPByPCN);
		fDiffMultHPByPCH = gameSettings->GetSetting("fDiffMultHPByPCH")->GetFloat(); DEBUG(fDiffMultHPByPCH);
		fDiffMultHPByPCVH = gameSettings->GetSetting("fDiffMultHPByPCVH")->GetFloat(); DEBUG(fDiffMultHPByPCVH);
		fDiffMultHPByPCL = gameSettings->GetSetting("fDiffMultHPByPCL")->GetFloat(); DEBUG(fDiffMultHPByPCL);

		fDiffMultHPToPCVE = gameSettings->GetSetting("fDiffMultHPToPCVE")->GetFloat();
		fDiffMultHPToPCE = gameSettings->GetSetting("fDiffMultHPToPCE")->GetFloat();
		fDiffMultHPToPCN = gameSettings->GetSetting("fDiffMultHPToPCN")->GetFloat();
		fDiffMultHPToPCH = gameSettings->GetSetting("fDiffMultHPToPCH")->GetFloat();
		fDiffMultHPToPCVH = gameSettings->GetSetting("fDiffMultHPToPCVH")->GetFloat();
		fDiffMultHPToPCL = gameSettings->GetSetting("fDiffMultHPToPCL")->GetFloat();
		INFO("Difficulty multipliers loaded!");
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