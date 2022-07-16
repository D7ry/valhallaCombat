#include "include/stunHandler.h"
#include "include/settings.h"
#include "include/Utils.h"
#include "ValhallaCombat.hpp"
using namespace inlineUtils;


void settings::init() {
	logger::info("Initilize settings...");
	DtryUtils::formLoader loader("ValhallaCombat.esp");
	loader.load(glob_Nemesis_EldenCounter_Damage, 0x56A23);
	loader.load(glob_Nemesis_EldenCounter_NPC, 0x56A24);
	loader.load(glob_TrueHudAPI, 0x56A25);
	loader.load(glob_TrueHudAPI_SpecialMeter, 0x56A26);
	loader.log();
	logger::info("...done");
}
void settings::updateGlobals() {
	logger::info("Update globals...");
	if (glob_TrueHudAPI) {
		glob_TrueHudAPI->value = ValhallaCombat::GetSingleton()->ersh != nullptr ? 1.f : 0.f;
	}
	if (glob_TrueHudAPI_SpecialMeter) {
		glob_TrueHudAPI_SpecialMeter->value = facts::TrueHudAPI_HasSpecialBarControl ? 1.f : 0.f;
	}
	auto pc = RE::PlayerCharacter::GetSingleton();
	if (pc) {
		if (glob_Nemesis_EldenCounter_Damage) {
			bool bDummy;
			glob_Nemesis_EldenCounter_Damage->value = pc->GetGraphVariableBool("is_Guard_Countering", bDummy);
		}
		if (glob_Nemesis_EldenCounter_NPC) {
			bool bDummy;
			glob_Nemesis_EldenCounter_NPC->value = pc->GetGraphVariableBool("IsValGC", bDummy);
		}
	}
	logger::info("...done");
}
#define SETTINGFILE_PATH "Data\\MCM\\Settings\\ValhallaCombat.ini"
/*Load the setting from ini file with the same name as the variable in code.*/
#define FETCH(setting) load(setting, #setting)
/*read settings from ini, and update them into game settings.*/
void settings::readSettings() {
	logger::info("Read ini settings...");
	DtryUtils::settingsLoader loader(SETTINGFILE_PATH);

	loader.setActiveSection("Stamina");
	loader.FETCH(bUIAlert);
	loader.FETCH(bNonCombatStaminaCost);
	loader.FETCH(fStaminaRegenMult);
	loader.FETCH(fStaminaRegenLimit);
	loader.FETCH(fCombatStaminaRegenMult);
	loader.FETCH(fStaminaRegenDelay);
	loader.FETCH(bBlockStaminaToggle);
	loader.FETCH(bGuardBreak);
	loader.FETCH(fBckShdStaminaMult_NPC_Block_NPC);
	loader.FETCH(fBckShdStaminaMult_NPC_Block_PC);
	loader.FETCH(fBckShdStaminaMult_PC_Block_NPC);
	loader.FETCH(bStaminaDebuffToggle);
	loader.FETCH(bAttackStaminaToggle);
	loader.FETCH(bBlockedHitRegenStamina);
	loader.FETCH(fMeleeCostLightMiss_Point);
	loader.FETCH(fMeleeRewardLightHit_Percent);
	loader.FETCH(fMeleeCostHeavyMiss_Percent);
	loader.FETCH(fMeleeCostHeavyHit_Percent);
	
	loader.setActiveSection("TimedBlocking");
	loader.FETCH(bBlockProjectileToggle);
	loader.FETCH(bTimedBlockToggle);
	loader.FETCH(bTimedBlockProjectileToggle);
	loader.FETCH(bTimedBlockScreenShake);
	loader.FETCH(bTimeBlockSFX);
	loader.FETCH(bTimedBlockVFX);
	loader.FETCH(bTimedBlockSlowTime);
	loader.FETCH(fTimedBlockWindow);
	loader.FETCH(fTimedBlockCooldownTime);
	loader.FETCH(fTimedBlockStaminaCostMult);
	loader.FETCH(uAltBlockKey);
	
	loader.setActiveSection("Stun");
	loader.FETCH(bStunToggle);
	loader.FETCH(bStunMeterToggle);
	loader.FETCH(bDownedStateToggle);
	loader.FETCH(bExecutionLimit);
	loader.FETCH(fStunTimedBlockMult);
	loader.FETCH(fStunBashMult);
	loader.FETCH(fStunPowerBashMult);
	loader.FETCH(fStunNormalAttackMult);
	loader.FETCH(fStunPowerAttackMult);
	loader.FETCH(fStunParryMult);
	loader.FETCH(fStunUnarmedMult);
	loader.FETCH(fStunDaggerMult);
	loader.FETCH(fStunSwordMult);
	loader.FETCH(fStunWarAxeMult);
	loader.FETCH(fStunMaceMult);
	loader.FETCH(fStunGreatSwordMult);
	loader.FETCH(fStun2HBluntMult);
	loader.FETCH(bAutoExecution);
	loader.FETCH(uExecutionKey);

	loader.setActiveSection("Compatibility");
	loader.FETCH(bPoiseCompatibility);

	loader.log();
	logger::info("...done");

	logger::info("Apply game settings...");
	/*Set some game settings*/
	setGameSettingf("fDamagedStaminaRegenDelay", fStaminaRegenDelay);
	setGameSettingf("fCombatStaminaRegenRateMult", fCombatStaminaRegenMult);
	multStaminaRegen(fStaminaRegenMult, fStaminaRegenLimit);
	
	/*Release truehud meter if set so.*/
	if (bStunMeterToggle && bStunToggle){
		ValhallaCombat::GetSingleton()->requestTrueHudSpecialBarControl();
	}
	else {
		ValhallaCombat::GetSingleton()->releaseTrueHudSpecialBarControl();
	}
	logger::info("...done");
}

