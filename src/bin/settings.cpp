#include "include/stunHandler.h"
#include "include/settings.h"
#include "include/Utils.h"
#include "ValhallaCombat.hpp"
using namespace inlineUtils;
/*tweaks the value of designated game setting
	@param gameSettingStr game setting to be tweaked.
	@param val desired float value of gamesetting.*/
static void setGameSettingf(const char* a_setting, float a_value)
{
	RE::Setting* setting = nullptr;
	RE::GameSettingCollection* _settingCollection = RE::GameSettingCollection::GetSingleton();
	setting = _settingCollection->GetSetting(a_setting);
	if (!setting) {
		logger::info("Error: invalid setting: {}", a_setting);
	} else {
		//logger::info("setting {} from {} to {}", settingStr, setting->GetFloat(), val);
		setting->data.f = a_value;
	}
}

static void setGameSettingb(const char* a_setting, bool a_value)
{
	RE::Setting* setting = nullptr;
	RE::GameSettingCollection* _settingCollection = RE::GameSettingCollection::GetSingleton();
	setting = _settingCollection->GetSetting(a_setting);
	if (!setting) {
		logger::info("invalid setting: {}", a_setting);
	} else {
		logger::info("setting {} from {} to {}", a_setting, setting->GetFloat(), a_value);
		setting->data.b = false;
	}
}

/*a map of all game races' original stamina regen, in case player wants to tweak the stamina regen values again*/
static inline robin_hood::unordered_map<RE::TESRace*, float> staminaRegenMap;

/*offset stamina regen of every single race. Multiplying values by MULT and clamping the final value.
	@param mult multiplier for stamina regen.
	@param upperLimit upper limit for stamina regen.*/
static void offsetStaminaRegen(float mult, float upperLimit, float lowerLimit = 10)
{
	for (auto& race : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESRace>()) {
		if (race) {
			float staminaRegen;
			if (!staminaRegenMap.contains(race)) {
				float val = race->data.staminaRegen;
				staminaRegenMap[race] = val;
				staminaRegen = val * mult;
			} else {
				staminaRegen = mult * staminaRegenMap[race];
			}
			/* Clamp regen val.*/
			staminaRegen = min(staminaRegen, upperLimit);
			staminaRegen = max(staminaRegen, lowerLimit);
			race->data.staminaRegen = staminaRegen;
			//logger::info("setting stamina regen rate for race {} to {}.", race->GetName(), staminaRegen);
		}
	}
}

void settings::updateGameSettings() {
	logger::info("Update game settings...");
	auto gsc = RE::GameSettingCollection::GetSingleton();
	if (!gsc) {
		return;
	}
	RE::Setting* bVal_TrueHudAPI_Acq = gsc->GetSetting("bVal_TrueHudAPI_Acq");
	if (bVal_TrueHudAPI_Acq) {
		bVal_TrueHudAPI_Acq->SetBool(ValhallaCombat::GetSingleton()->ersh_TrueHUD != nullptr);
	}
	RE::Setting* bVal_TrueHUDAPI_SpecialMeter_Acq = gsc->GetSetting("bVal_TrueHUDAPI_SpecialMeter_Acq");
	if (bVal_TrueHUDAPI_SpecialMeter_Acq) {
		bVal_TrueHUDAPI_SpecialMeter_Acq->SetBool(facts::TrueHudAPI_HasSpecialBarControl);
	}
	logger::info("...done");
}
#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\ValhallaCombat\\Settings.ini"
/*Load the setting from ini file with the same name as the variable in code.*/
#define FETCH(setting) load(setting, #setting)
/*read settings from ini, and update them into game settings.*/
void settings::readSettings() {
	logger::info("Read ini settings...");
	DtryUtils::settingsLoader MCM_Settings(SETTINGFILE_PATH);

	MCM_Settings.setActiveSection("Stamina");
	MCM_Settings.FETCH(bUIAlert);
	MCM_Settings.FETCH(bNonCombatStaminaCost);
	MCM_Settings.FETCH(fStaminaRegenMult);
	MCM_Settings.FETCH(fStaminaRegenLimit);
	MCM_Settings.FETCH(fStaminaRegenMin);
	MCM_Settings.FETCH(fCombatStaminaRegenMult);
	MCM_Settings.FETCH(fBlockingStaminaRegenMult);
	MCM_Settings.FETCH(fStaminaRegenDelay);
	MCM_Settings.FETCH(bBlockStaminaToggle);
	MCM_Settings.FETCH(bBlockedHitRegenStamina);
	MCM_Settings.FETCH(bGuardBreak);
	MCM_Settings.FETCH(fBckShdStaminaMult_NPC_Block_NPC);
	MCM_Settings.FETCH(fBckShdStaminaMult_NPC_Block_PC);
	MCM_Settings.FETCH(fBckShdStaminaMult_PC_Block_NPC);
	MCM_Settings.FETCH(bStaminaDebuffToggle);
	
	
	MCM_Settings.FETCH(bAttackStaminaToggle);
	MCM_Settings.FETCH(bAttackStaminaToggle_NPC);
	MCM_Settings.FETCH(fMeleeCostLightMiss_Point);
	MCM_Settings.FETCH(fMeleeRewardLightHit_Percent);
	//loader.FETCH(fMeleeCostHeavyMiss_Percent);
	//loader.FETCH(fMeleeCostHeavyHit_Percent);
	
	MCM_Settings.setActiveSection("TimedBlocking");
	MCM_Settings.FETCH(bBlockCommitmentToggle);
	MCM_Settings.FETCH(fBlockCommitmentTime);
	MCM_Settings.FETCH(bBlockProjectileToggle);
	MCM_Settings.FETCH(bTimedBlockToggle);
	MCM_Settings.FETCH(bTimedBlockProjectileToggle);
	MCM_Settings.FETCH(bTimedBlockScreenShake);
	MCM_Settings.FETCH(bTimeBlockSFX);
	MCM_Settings.FETCH(fTimedBlockSFXVolume);
	MCM_Settings.FETCH(uTimedBlockSparkType);
	MCM_Settings.FETCH(bTimedBlockSlowTime);
	MCM_Settings.FETCH(fTimedBlockWindow);
	MCM_Settings.FETCH(uPerfectBlockAccum);
	MCM_Settings.FETCH(fTimedBlockSlowTime_Timed);
	MCM_Settings.FETCH(fTimedBlockSlowTime_Perfect);
	MCM_Settings.FETCH(fTimedBlockCooldownTime);
	MCM_Settings.FETCH(fTimedBlockStaminaCostMult);
	MCM_Settings.FETCH(fBlockProjectileExp);
	MCM_Settings.FETCH(fTimedBlockProjectileExp);
	MCM_Settings.FETCH(fTimedBlockMeleeExp);
	MCM_Settings.FETCH(bTackleToggle);
	
	MCM_Settings.FETCH(iAltBlockKey);
	
	MCM_Settings.setActiveSection("Stun");
	MCM_Settings.FETCH(bStunToggle);
	MCM_Settings.FETCH(bStunMeterToggle);
	MCM_Settings.FETCH(bDownedStateToggle);
	MCM_Settings.FETCH(bFriendlyStunToggle);
	MCM_Settings.FETCH(bExecutionLimit);
	MCM_Settings.FETCH(fStunTimedBlockMult);
	MCM_Settings.FETCH(fStunBashMult);
	MCM_Settings.FETCH(fStunPowerBashMult);
	MCM_Settings.FETCH(fStunNormalAttackMult);
	MCM_Settings.FETCH(fStunPowerAttackMult);
	MCM_Settings.FETCH(fStunParryMult);
	MCM_Settings.FETCH(fStunCounterAttackMult);
	MCM_Settings.FETCH(fStunUnarmedMult);
	MCM_Settings.FETCH(fStunDaggerMult);
	MCM_Settings.FETCH(fStunSwordMult);
	MCM_Settings.FETCH(fStunWarAxeMult);
	MCM_Settings.FETCH(fStunMaceMult);
	MCM_Settings.FETCH(fStunGreatSwordMult);
	MCM_Settings.FETCH(fStun2HBluntMult);
	MCM_Settings.FETCH(bAutoExecution);
	MCM_Settings.FETCH(iExecutionKey);

	MCM_Settings.setActiveSection("Compatibility");
	MCM_Settings.FETCH(bPoiseCompatibility);

	MCM_Settings.log();
	logger::info("...done");

	logger::info("Apply game settings...");
	/*Set some game settings*/
	setGameSettingf("fDamagedStaminaRegenDelay", fStaminaRegenDelay);
	setGameSettingf("fCombatStaminaRegenRateMult", fCombatStaminaRegenMult);
	
	
	offsetStaminaRegen(fStaminaRegenMult, fStaminaRegenLimit, fStaminaRegenMin);
	
	/*Release truehud meter if set so.*/
	if (bStunMeterToggle && bStunToggle){
		ValhallaCombat::GetSingleton()->requestTrueHudSpecialBarControl();
	}
	else {
		ValhallaCombat::GetSingleton()->releaseTrueHudSpecialBarControl();
	}
	logger::info("...done");
}

