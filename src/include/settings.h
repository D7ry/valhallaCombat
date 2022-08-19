#pragma once
#include "debuffHandler.h"
#include "lib/robin_hood.h"
#include "lib/SimpleIni.h"

/*All the settings of Valhalla combat*/
class settings
{
public:
	class facts
	{
	public:
		static inline bool TrueHudAPI_HasSpecialBarControl;
		static inline bool TrueHudAPI_Obtained;
		static inline bool PrecisionAPI_Obtained;
		static inline bool EldenParry_ObtainedAPI;
		static inline bool EldenCounter_EspPluginLoaded = false;
	};

	
#pragma region GlobalSettings
	static inline RE::TESGlobal* glob_TrueHudAPI;
	static inline RE::TESGlobal* glob_TrueHudAPI_SpecialMeter;
	static inline RE::TESGlobal* glob_Nemesis_EldenCounter_NPC;
	static inline RE::TESGlobal* glob_Nemesis_EldenCounter_Damage;
	static inline RE::TESGlobal* glob_EldenCounter_EspPluginLoaded;

#pragma endregion

#pragma region StaminSettings
	static inline bool bUIAlert = true;
	static inline bool bNonCombatStaminaCost = true;
	
	
	static inline float fStaminaRegenMult = 5;
	static inline float fStaminaRegenLimit = 50;
	static inline float fCombatStaminaRegenMult = 1;
	static inline float fBlockingStaminaRegenMult = 0.5;
	static inline float fStaminaRegenDelay = 2;

	static inline float bBlockedHitRegenStamina = false;

	static inline bool bBlockStaminaToggle = false;
	static inline bool bGuardBreak = true;
	static inline float fBckShdStaminaMult_PC_Block_NPC = 1;
	static inline float fBckWpnStaminaMult_PC_Block_NPC = 1;

	static inline float fBckShdStaminaMult_NPC_Block_PC = 1; //stamina penalty mult for NPCs blockign a player hit with a shield
	static inline float fBckWpnStaminaMult_NPC_Block_PC = 1;

	static inline float fBckShdStaminaMult_NPC_Block_NPC = 1;
	static inline float fBckWpnStaminaMult_NPC_Block_NPC = 1;

	static inline bool bAttackStaminaToggle = true;
	static inline bool bAttackStaminaToggle_NPC = false;

	static inline bool bStaminaDebuffToggle = true;
	static inline float fMeleeCostLightMiss_Point = 30;
	static inline float fMeleeRewardLightHit_Percent = 0.2f;
	static inline float fMeleeCostHeavyMiss_Percent = 0.4f;
	static inline float fMeleeCostHeavyHit_Percent = 0.333f;

#pragma endregion

#pragma region spiritSettings
	static inline bool bSpiritToggle = true;
	static inline bool bSpiritGuardStanceToggle = true;
	
	static inline float fHeavyAttackSpiritGain = 20.f;
	static inline float fLightAttackSpiritLoss = 5.f;
	static inline float fSpiritDamageBuff_1 = 1.2f;
	static inline float fSpiritDamageBuff_2 = 1.4f;
	static inline float fSpiritDamageBuff_3 = 1.6f;
#pragma endregion
#pragma region timedBlockSettings
	/*perfect blocking*/
	//static inline bool bPoiseCompatibility = false;
	static inline bool bBlockProjectileToggle = true;
	static inline bool bTimedBlockToggle = true;
	static inline bool bTackleToggle = false;
	static inline bool bTimedBlockProjectileToggle = true;
	
	static inline float fTimedBlockWindow = 0.3f;
	static inline float fPerfectBlockWindow = 0.15f;
	static inline float fTimedBlockCooldownTime = 0.5f;
	static inline float fTimedBlockStaminaCostMult = 0.5f;

	static inline float fTackleWindow = 0.2f;
	static inline float fTackleCooldownTime = 2.f;

	static inline float fTimedBlockMeleeExp = 10.f;
	static inline float fTimedBlockProjectileExp = 10.f;
	static inline float fBlockProjectileExp = 5.f;

	static inline bool bTimedBlockScreenShake = true;
	static inline bool bTimeBlockSFX = true;
	static inline float fTimedBlockSFXVolume = 0.5;
	static inline uint32_t uTimedBlockSparkType = 0;  //0 = fantasy, 1 = realism, 2 = none
	static inline bool bTimedBlockSlowTime = true;
	
	static inline float fTimedBlockSlowTime_Timed = 0.0f;
	static inline float fTimedBlockSlowTime_Perfect = 0.3f;

	static inline int iAltBlockKey = -1;
#pragma endregion
#pragma region StunSettings

	/*Enable/disable stun and execution.*/
	static inline bool bStunToggle = true;
	static inline bool bStunMeterToggle = true;
	static inline bool bFriendlyStunToggle = false;
	static inline bool bDownedStateToggle = false;
	
	static inline float fStunTimedBlockMult = 1.f;
	static inline float fStunParryMult = 1.f;
	static inline float fStunBashMult = 0.3f;
	static inline float fStunPowerBashMult = 1.f;
	static inline float fStunNormalAttackMult = 0.f;
	static inline float fStunPowerAttackMult = 1.f;
	static inline float fStunCounterAttackMult = 1.2f;

	static inline float fStunUnarmedMult = 1.f;
	static inline float fStunDaggerMult = 0.5f;
	static inline float fStunSwordMult = 1.f;
	static inline float fStunWarAxeMult = 1.1f;
	static inline float fStunMaceMult = 1.1f;
	static inline float fStunGreatSwordMult = 1.2f;
	static inline float fStun2HBluntMult = 1.2f;

	/*Toggle for player to be executed.*/
	static inline bool bPlayerExecution = false;
	/*Toggle for essential NPCs to be executed.*/
	static inline bool bExecutionLimit = true; //NPCs with level higher than player cannot be executed unless under certain health threshold.
	static inline bool bAutoExecution = false; //automatically execute on hit.
	static inline int iExecutionKey = -1;
#pragma endregion
#pragma region BalanceSettings
	static inline bool bBalanceToggle = true;

#pragma endregion

#pragma region AISettings
	static inline bool bExhaustionAIToggle = true;
	static inline bool bCirclingAIToggle = true;
	static inline bool bPerilousAttackAIToggle = true;
#pragma endregion
#pragma region Compatibility
	static inline bool bPoiseCompatibility = false;
#pragma endregion

	//TODO: add new settings

	static void readSettings();
	static void updateGlobals();
	static void init();
private:
	static void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting);
	static void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting);
	static void ReadIntSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, uint32_t& a_setting);
};
