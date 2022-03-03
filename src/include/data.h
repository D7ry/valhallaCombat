#pragma once
#include "Utils.h"

#include "debuffHandler.h"
#include "lib/robin_hood.h"
#include "lib/SimpleIni.h"


/*All the settings of Valhalla combat*/
class settings
{
public:

#pragma region StaminSettings
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

	static inline uint32_t uExecutionKey = 35; //H for now
#pragma endregion
#pragma region PerfectBlockingSettings
	/*perfect blocking*/
	//static inline bool bPoiseCompatibility = false;

	static inline bool bPerfectBlockToggle = true;
	static inline bool bPerfectBlockScreenShake = true;
	static inline bool bPerfectBlockSFX = true;
	static inline bool bPerfectBlockVFX = true;
	static inline float fPerfectBlockTime = 0.3;
	static inline float fPerfectBlockCoolDownTime = 1;

#pragma endregion
#pragma region StunSettings
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
#pragma endregion
#pragma region Compatibility
	static inline bool bPoiseCompatibility = false;
#pragma endregion
	static void readSettings();
	
private:
	static void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting);
	static void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting);

};

/*Everything stored in game.*/
class gameDataCache
{
public:
	static gameDataCache* GetSingleton()
	{
		static gameDataCache singleton;
		return  std::addressof(singleton);
	}
	/*Load all game data and store their pointers for later use.*/
	void loadData();

#pragma region Perk
	RE::BGSPerk* debuffPerk;
#pragma endregion
#pragma region ExecutionIdles
	using KM = std::vector<RE::TESIdleForm*>;
	KM KM_Humanoid_H2H = std::vector<RE::TESIdleForm*>();
	KM KM_Humanoid_Dagger = std::vector<RE::TESIdleForm*>();
	KM KM_Humanoid_Sword = std::vector<RE::TESIdleForm*>();
	KM KM_Humanoid_Axe = std::vector<RE::TESIdleForm*>();
	KM KM_Humanoid_Mace = std::vector<RE::TESIdleForm*>();
	KM KM_Humanoid_GreatSword = std::vector<RE::TESIdleForm*>();
	KM KM_Humanoid_2hw = std::vector<RE::TESIdleForm*>();
	KM KM_Humanoid_DW = std::vector<RE::TESIdleForm*>();
	KM KM_Humanoid_1hm_Back = std::vector<RE::TESIdleForm*>();
	KM KM_Humanoid_2hm_Back = std::vector<RE::TESIdleForm*>();
	KM KM_Humanoid_H2H_Back = std::vector<RE::TESIdleForm*>();
	KM KM_Humanoid_2hw_Back = std::vector<RE::TESIdleForm*>();
	KM KM_Falmer_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Falmer_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Falmer_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Undead_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Undead_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Undead_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Spider_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Spider_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Spider_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Gargoyle_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Gargoyle_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Gargoyle_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Giant_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Giant_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Giant_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Bear_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Bear_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Bear_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_SabreCat_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_SabreCat_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_SabreCat_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Wolf_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Wolf_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Wolf_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Troll_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Troll_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Troll_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Hagraven_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Hagraven_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Hagraven_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Spriggan_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Spriggan_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Spriggan_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Boar_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Boar_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Boar_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Riekling_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Riekling_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Riekling_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_AshHopper_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_AshHopper_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_AshHopper_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Centurion_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Centurion_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Centurion_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Ballista_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Ballista_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Ballista_2hw= std::vector<RE::TESIdleForm*>(); 
	KM KM_ChaurusFlyer_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_ChaurusFlyer_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_ChaurusFlyer_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Lurker_1hm= std::vector<RE::TESIdleForm*>();
	KM KM_Lurker_2hm= std::vector<RE::TESIdleForm*>();
	KM KM_Lurker_2hw= std::vector<RE::TESIdleForm*>();
	KM KM_Dragon_1hm = std::vector<RE::TESIdleForm*>();
	KM KM_Dragon_2hm = std::vector<RE::TESIdleForm*>();
	KM KM_Dragon_2hw = std::vector<RE::TESIdleForm*>();

#pragma endregion
#pragma region sounds
	RE::BGSSoundDescriptorForm* soundParryShieldD;
	RE::BGSSoundDescriptorForm* soundParryWeaponD;
	RE::BGSSoundDescriptorForm* soundParryShield_gbD;
	RE::BGSSoundDescriptorForm* soundParryWeapon_gbD;
#pragma endregion
#pragma region gameDifficultySettings
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

#pragma endregion
private:


	bool lookupIdle(RE::TESDataHandler* DATA, RE::FormID form, std::string pluginName, std::vector<RE::TESIdleForm*>* idleContainer);
	void loadIdleSection(RE::TESDataHandler* DATA, std::vector<RE::TESIdleForm*>* idleContainer, const char* section);
	void loadIdle();

	void loadSound();
	void loadPerk();
	void loadDifficultySettings();



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
	inline static robin_hood::unordered_map<RE::TESRace*, float> staminaRegenMap;

	/*multiplies stamina regen of every single race by MULT.
	@param mult multiplier for stamina regen.
	@param upperLimit upper limit for stamina regen.*/
	inline void multStaminaRegen(float mult, float upperLimit) {
		for (auto& race : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESRace>()) {
			if (race && race->data.staminaRegen) {
				float staminaRegen;
				if (staminaRegenMap.find(race) == staminaRegenMap.end()) {
					staminaRegenMap[race] = race->data.staminaRegen;
					staminaRegen = staminaRegenMap[race] * mult;
				}
				else {
					staminaRegen = mult * staminaRegenMap[race];
				}
				if (staminaRegen > upperLimit) {
					staminaRegen = upperLimit;
				}
				race->data.staminaRegen = staminaRegen;
				INFO("setting stamina regen rate for race {} to {}.", race->GetName(), staminaRegen);
			}
		}
	}

	/*Parse a string into a vector of tokens.*/
	static std::vector<std::string> parseStr(const std::string& delimiter, const std::string& str) {
		std::vector<std::string> arr;
		int strleng = str.length();
		int delleng = delimiter.length();
		if (delleng == 0)
			return arr;//no change

		int i = 0;
		int k = 0;
		while (i < strleng)
		{
			int j = 0;
			while (i + j < strleng && j < delleng && str[i + j] == delimiter[j])
				j++;
			if (j == delleng)//found delimiter
			{
				arr.push_back(str.substr(k, i - k));
				i += delleng;
				k = i;
			}
			else
			{
				i++;
			}
		}
		arr.push_back(str.substr(k, i - k));
		return arr;
	}

	/*Turn a formID into a uint32. Useful when reading from ini.*/
	static bool ToInt32(std::string str, int& value)
	{
		const char* strVal = str.c_str();
		char* endVal = NULL;
		long ret = strtol(strVal, &endVal, 0);
		if (ret == LONG_MAX || ret == LONG_MIN || (int)endVal != (int)strVal + strlen(strVal))
			return false;
		value = ret;
		return true;
	}
}