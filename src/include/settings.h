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
		static inline bool EldenParry_ObtainedAPI;
	};

	class settingsLoader
	{
	private:
		std::shared_ptr<CSimpleIniA> _ini;
		const char* _section;
	public:
		settingsLoader(const char* settingsFile){
			_ini = std::make_shared<CSimpleIniA>();
			_ini->LoadFile(settingsFile);
			if (_ini->IsEmpty()) {
				logger::info("Warning: {} is empty.", settingsFile);
			}
		};

		void loadSection(const char* section) {
			_section = section;
		}
#define NAME(setting) #setting
		void load(bool& settingRef, const char* key)
		{
			if (_ini->GetValue(_section, key)) {
				bool val = _ini->GetBoolValue(_section, key);
				settingRef = val;
			}
		}
		void load(float& settingRef, const char* key)
		{
			if (_ini->GetValue(_section, key)) {
				float val = static_cast<float>(_ini->GetDoubleValue(_section, key));
				settingRef = val;
			}
		}
		void load(uint32_t& settingRef, const char* key)
		{
			if (_ini->GetValue(_section, key)) {
				uint32_t val = static_cast<uint32_t>(_ini->GetDoubleValue(_section, key));
				settingRef = val;
			}
		}
		
	};
#pragma region GlobalSettings
	static inline RE::TESGlobal* glob_TrueHudAPI;
	static inline RE::TESGlobal* glob_TrueHudAPI_SpecialMeter;
	static inline RE::TESGlobal* glob_Nemesis_EldenCounter_NPC;
	static inline RE::TESGlobal* glob_Nemesis_EldenCounter_Damage;

#pragma endregion

#pragma region StaminSettings
	static inline bool bUIAlert = true;
	static inline bool bNonCombatStaminaCost = true;
	
	
	static inline float fStaminaRegenMult = 5;
	static inline float fStaminaRegenLimit = 50;
	static inline float fCombatStaminaRegenMult = 1;
	static inline float fStaminaRegenDelay = 2;

	static inline bool bBlockStaminaToggle = true;
	static inline bool bGuardBreak = true;
	static inline float fBckShdStaminaMult_PC_Block_NPC = 1;
	static inline float fBckWpnStaminaMult_PC_Block_NPC = 1;

	static inline float fBckShdStaminaMult_NPC_Block_PC = 1; //stamina penalty mult for NPCs blockign a player hit with a shield
	static inline float fBckWpnStaminaMult_NPC_Block_PC = 1;

	static inline float fBckShdStaminaMult_NPC_Block_NPC = 1;
	static inline float fBckWpnStaminaMult_NPC_Block_NPC = 1;

	static inline bool bStaminaDebuffToggle = true;
	static inline bool bAttackStaminaToggle = false;
	static inline bool bBlockedHitRegenStamina = false;

	
	static inline float fMeleeCostLightMiss_Point = 30;
	static inline float fMeleeRewardLightHit_Percent = 0.2f;
	static inline float fMeleeCostHeavyMiss_Percent = 0.4f;
	static inline float fMeleeCostHeavyHit_Percent = 0.333f;

#pragma endregion
#pragma region PerfectBlockingSettings
	/*perfect blocking*/
	//static inline bool bPoiseCompatibility = false;
	static inline bool bBlockProjectileToggle = true;
	static inline bool bTimedBlockToggle = true;
	static inline bool bTimedBlockProjectileToggle = true;
	
	static inline float fTimedBlockWindow = 0.5;
	static inline float fTimedBlockCooldownTime = 0.5;
	static inline float fTimedBlockStaminaCostMult = 0.5;

	static inline bool bTimedBlockScreenShake = true;
	static inline bool bTimeBlockSFX = true;
	static inline bool bTimedBlockVFX = true;
	static inline bool bTimedBlockSlowTime = true;

	static inline uint32_t uAltBlockKey = -1;
#pragma endregion
#pragma region StunSettings

	/*Enable/disable stun and execution.*/
	static inline bool bStunToggle = true;
	static inline bool bStunMeterToggle = true;
	static inline float fStunTimedBlockMult = 1;
	static inline float fStunParryMult = 1;
	static inline float fStunBashMult = 0.3;
	static inline float fStunPowerBashMult = 1;
	static inline float fStunNormalAttackMult = 0;
	static inline float fStunPowerAttackMult = 1;


	static inline float fStunUnarmedMult = 1;
	static inline float fStunDaggerMult = 0.5;
	static inline float fStunSwordMult = 1;
	static inline float fStunWarAxeMult = 1.1;
	static inline float fStunMaceMult = 1.1;
	static inline float fStunGreatSwordMult = 1.2;
	static inline float fStun2HBluntMult = 1.2;

	/*Toggle for player to be executed.*/
	static inline bool bPlayerExecution = false;
	/*Toggle for essential NPCs to be executed.*/
	static inline bool bExecutionLimit = true; //NPCs with level higher than player cannot be executed unless under certain health threshold.
	static inline bool bAutoExecution = false; //automatically execute on hit.
	static inline uint32_t uExecutionKey = -1;
#pragma endregion
#pragma region BalanceSettings
	static inline bool bBalanceToggle = true;

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

namespace inlineUtils
{
	/*tweaks the value of designated game setting
	@param gameSettingStr game setting to be tweaked.
	@param val desired float value of gamesetting.*/
	static void setGameSettingf(const char* a_setting, float a_value) {
		RE::Setting* setting = nullptr;
		RE::GameSettingCollection* _settingCollection = RE::GameSettingCollection::GetSingleton();
		setting = _settingCollection->GetSetting(a_setting);
		if (!setting) {
			logger::info("Error: invalid setting: {}", a_setting);
		}
		else {
			//logger::info("setting {} from {} to {}", settingStr, setting->GetFloat(), val);
			setting->data.f = a_value;
		}
	}

	static void setGameSettingb(const char* a_setting, bool a_value) {
		RE::Setting* setting = nullptr;
		RE::GameSettingCollection* _settingCollection = RE::GameSettingCollection::GetSingleton();
		setting = _settingCollection->GetSetting(a_setting);
		if (!setting) {
			logger::info("invalid setting: {}", a_setting);
		}
		else {
			logger::info("setting {} from {} to {}", a_setting, setting->GetFloat(), a_value);
			setting->data.b = false;
		}
	}

	/*a map of all game races' original stamina regen, in case player wants to tweak the stamina regen values again*/
	static inline robin_hood::unordered_map<RE::TESRace*, float> staminaRegenMap;

	/*multiplies stamina regen of every single race by MULT.
	@param mult multiplier for stamina regen.
	@param upperLimit upper limit for stamina regen.*/
	static void multStaminaRegen(float mult, float upperLimit) {
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
				//logger::info("setting stamina regen rate for race {} to {}.", race->GetName(), staminaRegen);
			}
		}
	}

}
