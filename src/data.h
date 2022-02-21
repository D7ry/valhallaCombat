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

	/*Stamina*/
	static inline bool bUIAlert = true;
	static inline bool bNonCombatStaminaDebuff = true;

	static inline float fStaminaRegenMult = 5;
	static inline float fStaminaRegenLimit = 50;
	static inline float fCombatStaminaRegenMult = 1;
	static inline float fStaminaRegenDelay = 2;

	static inline bool bGuardBreak = true;
	static inline float fBckShdStaminaMult_PC_Block_NPC = 1;
	static inline float fBckWpnStaminaMult_PC_Block_NPC = 1;

	static inline float fBckShdStaminaMult_NPC_Block_PC = 1; //stamina penalty mult for NPCs blockign a player hit with a shield
	static inline float fBckWpnStaminaMult_NPC_Block_PC = 1;

	static inline float fBckShdStaminaMult_NPC_Block_NPC = 1;
	static inline float fBckWpnStaminaMult_NPC_Block_NPC = 1;

	static inline bool bBlockedHitRegenStamina = false;
	static inline float fMeleeCostLightMiss_Point = 30;
	static inline float fMeleeRewardLightHit_Percent = 0.2;
	static inline float fMeleeCostHeavyMiss_Percent = 0.333;
	static inline float fMeleeCostHeavyHit_Percent = 0.199;
	/*perfect blocking*/
	//static inline bool bPoiseCompatibility = false;

	static inline bool bPerfectBlockToggle = true;
	static inline bool bPerfectBlockScreenShake = true;
	static inline bool bPerfectBlockSFX = true;
	static inline bool bPerfectBlockVFX = true;
	static inline float fPerfectBlockTime = 0.3;
	static inline float fPerfectBlockCoolDownTime = 1;



	//TODO:WHEN READ SETTING, automatically request/release TRUEHUD meter control.
	/*Enable/disable stun and execution.*/
	static inline bool bStunToggle = true;
	static inline bool bStunMeterToggle = true;
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
	/*Toggle for player to be executed.*/
	static inline bool bPlayerExecution = false;
	/*Toggle for player teammate to be executed.*/
	static inline bool bPlayerTeammateExecution = false;
	/*Toggle for essential NPCs to be executed.*/
	static inline bool bEssentialExecution = false;
	static inline bool bExecutionLimit = true; //NPCs with level higher than player cannot be executed unless under certain health threshold.


	static void readSettings();
	
private:
	static void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting);
	static void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting);

};

/*Everything stored in game.*/
struct gameDataCache
{
public:

	static inline RE::BGSPerk* debuffPerk;
	static inline RE::BGSSoundDescriptorForm* soundParryShieldD;
	static inline RE::BGSSoundDescriptorForm* soundParryWeaponD;

	static inline float fDiffMultHPByPCVE;
	static inline float fDiffMultHPByPCE;
	static inline float fDiffMultHPByPCN;
	static inline float fDiffMultHPByPCH;
	static inline float fDiffMultHPByPCVH;
	static inline float fDiffMultHPByPCL;
	static inline float fDiffMultHPToPCVE;
	static inline float fDiffMultHPToPCE;
	static inline float fDiffMultHPToPCN;
	static inline float fDiffMultHPToPCH;
	static inline float fDiffMultHPToPCVH;
	static inline float fDiffMultHPToPCL;

	/*Fetch data from game and store them for latter use.*/
	static void fetchGameData() {
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
		fDiffMultHPByPCVE = gameSettings->GetSetting("fDiffMultHPByPCVE")->GetFloat();
		fDiffMultHPByPCE = gameSettings->GetSetting("fDiffMultHPByPCE")->GetFloat();
		fDiffMultHPByPCN = gameSettings->GetSetting("fDiffMultHPByPCN")->GetFloat();
		fDiffMultHPByPCH = gameSettings->GetSetting("fDiffMultHPByPCH")->GetFloat();
		fDiffMultHPByPCVH = gameSettings->GetSetting("fDiffMultHPByPCVH")->GetFloat();
		fDiffMultHPByPCL = gameSettings->GetSetting("fDiffMultHPByPCL")->GetFloat();

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

namespace Utils
{
	/*tweaks the value of designated game setting
	@param gameSettingStr game setting to be tweaked.
	@param val desired float value of gamesetting.*/
	inline void setGameSettingf(const char* settingStr, float val) {
		RE::Setting* setting = nullptr;
		RE::GameSettingCollection* _settingCollection = RE::GameSettingCollection::GetSingleton();
		setting = _settingCollection->GetSetting(settingStr);
		if (!setting) {
			INFO("invalid setting: {}", settingStr);
		}
		else {
			INFO("setting {} from {} to {}", settingStr, setting->GetFloat(), val);
			setting->data.f = val;
		}
	}

	/*a map of all game races' original stamina regen, in case player wants to tweak the stamina regen values again*/
	inline static std::unordered_map<std::string, float> staminaRegenMap;

	/*multiplies stamina regen of every single race by MULT.
	@param mult multiplier for stamina regen.
	@param upperLimit upper limit for stamina regen.*/
	inline void multStaminaRegen(float mult, float upperLimit) {
		for (auto& race : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESRace>()) {
			if (race) {
				std::string raceName = race->GetName();
				float staminaRegen;
				if (staminaRegenMap.find(raceName) == staminaRegenMap.end()) {
					DEBUG("recording race default stamina for {}!", raceName);
					staminaRegenMap[raceName] = race->data.staminaRegen;
					staminaRegen = race->data.staminaRegen * mult;
				}
				else {
					staminaRegen = mult * staminaRegenMap.at(raceName);
				}
				if (staminaRegen > upperLimit) {
					staminaRegen = upperLimit;
				}
				race->data.staminaRegen = staminaRegen;
				INFO("setting stamina regen rate for race {} to {}.", race->GetName(), staminaRegen);
			}
		}
	}
}