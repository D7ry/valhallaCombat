#include "data.h"
#include "stunHandler.h"
using namespace Utils;
void settings::ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting)
{
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound) {
		INFO("found {} with value {}", a_settingName, bFound);
		a_setting = static_cast<float>(a_ini.GetDoubleValue(a_sectionName, a_settingName));
	}
	else {
		INFO("failed to find {}, using default value", a_settingName);
	}
}

void settings::ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting)
{
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound)
	{
		INFO("found {} with value {}", a_settingName, bFound);
		a_setting = a_ini.GetBoolValue(a_sectionName, a_settingName);
	}
	else {
		INFO("failed to find {}, using default value", a_settingName);
	}
}
/*read settings from ini, and update them into game settings.*/
void settings::readSettings() {
	INFO("Reading ini settings...");
	CSimpleIniA ini;
#define SETTINGFILE_PATH "Data\\MCM\\Settings\\ValhallaCombat.ini"
	ini.LoadFile(SETTINGFILE_PATH);


	/*Read stamina section*/
	ReadBoolSetting(ini, "Stamina", "bUIalert", bUIAlert);
	ReadBoolSetting(ini, "Stamina", "bNonCombatStaminaDebuff", bNonCombatStaminaDebuff);
	ReadFloatSetting(ini, "Stamina", "fStaminaRegenMult", fStaminaRegenMult);
	ReadFloatSetting(ini, "Stamina", "fStaminaRegenLimit", fStaminaRegenLimit);
	ReadFloatSetting(ini, "Stamina", "fCombatStaminaRegenMult", fCombatStaminaRegenMult);
	ReadFloatSetting(ini, "Stamina", "fStaminaRegenDelay", fStaminaRegenDelay);

	ReadBoolSetting(ini, "Stamina", "bGuardBreak", bGuardBreak);
	ReadFloatSetting(ini, "Stamina", "fBckShdStaminaMult_NPC_Block_NPC", fBckShdStaminaMult_NPC_Block_NPC);
	ReadFloatSetting(ini, "Stamina", "fBckShdStaminaMult_NPC_Block_PC", fBckShdStaminaMult_NPC_Block_PC);
	ReadFloatSetting(ini, "Stamina", "fBckShdStaminaMult_PC_Block_NPC", fBckShdStaminaMult_PC_Block_NPC);
	ReadFloatSetting(ini, "Stamina", "fBckWpnStaminaMult_NPC_Block_NPC", fBckWpnStaminaMult_NPC_Block_NPC);
	ReadFloatSetting(ini, "Stamina", "fBckWpnStaminaMult_NPC_Block_PC", fBckWpnStaminaMult_NPC_Block_PC);
	ReadFloatSetting(ini, "Stamina", "fBckWpnStaminaMult_PC_Block_NPC", fBckWpnStaminaMult_PC_Block_NPC);

	ReadBoolSetting(ini, "Stamina", "bBlockedHitRegenStamina", bBlockedHitRegenStamina);
	ReadFloatSetting(ini, "Stamina", "fMeleeCostLightMiss_Point", fMeleeCostLightMiss_Point); 
	ReadFloatSetting(ini, "Stamina", "fMeleeRewardLightHit_Percent", fMeleeRewardLightHit_Percent);
	ReadFloatSetting(ini, "Stamina", "fMeleeCostHeavyMiss_Percent", fMeleeCostHeavyMiss_Percent);
	ReadFloatSetting(ini, "Stamina", "fMeleeCostHeavyHit_Percent", fMeleeCostHeavyHit_Percent);

	/*Read perfect blocking section*/
	ReadBoolSetting(ini, "Parrying", "bPerfectBlockToggle", bPerfectBlockToggle);
	ReadBoolSetting(ini, "Parrying", "bPerfectBlockScreenShake", bPerfectBlockScreenShake);
	ReadBoolSetting(ini, "Parrying", "bPerfectBlockSFX", bPerfectBlockSFX);
	ReadBoolSetting(ini, "Blocking", "bPerfectBlockVFX", bPerfectBlockVFX);
	ReadFloatSetting(ini, "Parrying", "fPerfectBlockTime", fPerfectBlockTime);
	ReadFloatSetting(ini, "Parrying", "fPerfectBlockCoolDownTime", fPerfectBlockCoolDownTime);

	/*Read stun section*/
	ReadBoolSetting(ini, "Stun", "bStunToggle", bStunToggle);
	ReadBoolSetting(ini, "Stun", "bStunMeterToggle", bStunMeterToggle);
	ReadBoolSetting(ini, "Stun", "bPlayerExecution", bPlayerExecution);
	ReadBoolSetting(ini, "Stun", "bEssentialExecution", bEssentialExecution);
	ReadBoolSetting(ini, "Stun", "bExecutionLimit", bExecutionLimit);

	ReadFloatSetting(ini, "Stun", "fStunParryMult", fStunParryMult);
	ReadFloatSetting(ini, "Stun", "fStunBashMult", fStunBashMult);
	ReadFloatSetting(ini, "Stun", "fStunPowerBashMult", fStunPowerBashMult);
	ReadFloatSetting(ini, "Stun", "fStunPowerAttackMult", fStunPowerAttackMult);

	ReadFloatSetting(ini, "Stun", "fStunUnarmedMult", fStunUnarmedMult);
	ReadFloatSetting(ini, "Stun", "fStunDaggerMult", fStunDaggerMult);
	ReadFloatSetting(ini, "Stun", "fStunSwordMult", fStunSwordMult);
	ReadFloatSetting(ini, "Stun", "fStunWarAxeMult", fStunWarAxeMult);
	ReadFloatSetting(ini, "Stun", "fStunMaceMult", fStunMaceMult);
	ReadFloatSetting(ini, "Stun", "fStunGreatSwordMult", fStunGreatSwordMult);
	ReadFloatSetting(ini, "Stun", "fStun2HBluntMult", fStun2HBluntMult);


	ReadBoolSetting(ini, "Compatibility", "bPoiseCompatibility", bPoiseCompatibility);
	INFO("Ini settings read.");
	setGameSettingf("fDamagedStaminaRegenDelay", fStaminaRegenDelay);
	setGameSettingf("fCombatStaminaRegenRateMult", fCombatStaminaRegenMult);
	multStaminaRegen(fStaminaRegenMult, fStaminaRegenLimit);
	/*Release truehud meter if set so.*/
	if (bStunToggle && bStunMeterToggle) {
		stunHandler::GetSingleton()->initTrueHUDStunMeter();
	}
	else {
		stunHandler::GetSingleton()->releaseTrueHUDStunMeter();
	}
	INFO("Game settings applied.");
}


void gameDataCache::loadData() {
	INFO("Loading data from game...");
	loadSound();
	loadPerk();
	loadDifficultySettings();
	loadIdle();
	INFO("Loading fetched.");
}

void gameDataCache::loadSound() {
	INFO("Loading sound descriptors...");
	soundParryShieldD = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0X433C, "ValhallaCombat.esp");
	soundParryWeaponD = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0X3DD9, "ValhallaCombat.esp");
	soundParryShield_gbD = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0X47720, "ValhallaCombat.esp");
	soundParryWeapon_gbD = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0X47721, "ValhallaCombat.esp");
	if (soundParryShieldD && soundParryWeaponD && soundParryShield_gbD && soundParryWeapon_gbD) {
		INFO("Sound descriptors successfully loaded!");
	}
}

void gameDataCache::loadPerk() {
	INFO("Loading perk...");
	debuffPerk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(0x2DB2, "ValhallaCombat.esp");
	if (debuffPerk) {
		INFO("Debuff perk successfully loaded!");
	}
}

/*Lookup an idle from a certain esp, and store it in an idleContainer. Errors if the idle does not exist.
@param DATA: pointer to TESDataHandler's singleton.
@param form: formID of the idle.
@param pluginName: name of the plugin storing idle.
@praam idleContainer: Vector to store loaded idle.*/
bool gameDataCache::lookupIdle(RE::TESDataHandler* DATA, RE::FormID form, std::string pluginName, std::vector<RE::TESIdleForm*>* idleContainer) {
	auto idle = DATA->LookupForm<RE::TESIdleForm>(form, pluginName);
	if (!idle) {
		return false;
	}
	idleContainer->push_back(idle);
	return true;
}

void gameDataCache::loadIdleSection(RE::TESDataHandler* DATA, std::vector<RE::TESIdleForm*>* idleContainer, const char* section) {
	//FIXME:this is a temporary janky workaround for simpleIni
	CSimpleIniA ini;
#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\ValhallaCombat_Killmoves.ini"
	ini.LoadFile(SETTINGFILE_PATH);
	CSimpleIniA::TNamesDepend keys;
	INFO("Loading from section {}", section);
	ini.GetAllKeys(section, keys);
	int idlesLoaded = 0;
	for (CSimpleIniA::TNamesDepend::iterator s_it1 = keys.begin(); s_it1 != keys.end(); s_it1++) {
		const char* idle = s_it1->pItem;
		auto line = ini.GetValue(section, idle);
		INFO(line);
		std::vector<std::string> idleConfigs = parseStr("|", line);
		if (idleConfigs.size() != 2) {
			ERROR("Error: wrong config length");
			continue;
		}
		std::string plugin = idleConfigs[0];
		int form = 0;
		if (!Utils::ToInt32(idleConfigs[1], form)) {
			ERROR("Error: wrong formID");
			continue;
		}
		if (lookupIdle(DATA, form, plugin, idleContainer)) {
			idlesLoaded++;
			INFO("Loaded idle. FormID: {}, plugin: {}", form, plugin);
		}
		else {
			INFO("Failed to load idle. FormID: {}, plugin: {}", form, plugin);
		}

	}
	INFO("Loaded {} idles from section {}.", idlesLoaded, section);
}

void gameDataCache::loadIdle() {
	INFO("Loading idle...");
	auto DATA = RE::TESDataHandler::GetSingleton();
	loadIdleSection(DATA, &KM_Humanoid_H2H, "Humanoid-Unarmed");
	loadIdleSection(DATA, &KM_Humanoid_Dagger, "Humanoid-Dagger");
	loadIdleSection(DATA, &KM_Humanoid_Sword, "Humanoid-Sword");
	loadIdleSection(DATA, &KM_Humanoid_Axe, "Humanoid-Axe");
	loadIdleSection(DATA, &KM_Humanoid_Mace, "Humanoid-Mace");
	loadIdleSection(DATA, &KM_Humanoid_GreatSword, "Humanoid-GreatSword");
	loadIdleSection(DATA, &KM_Humanoid_2hw, "Humanoid-2HW");

	loadIdleSection(DATA, &KM_Humanoid_DW, "Humanoid-DW");

	loadIdleSection(DATA, &KM_Humanoid_1hm_Back, "Humanoid-1HM-Back");
	loadIdleSection(DATA, &KM_Humanoid_2hm_Back, "Humanoid-2HM-Back");
	loadIdleSection(DATA, &KM_Humanoid_2hw_Back, "Humanoid-2HW-Back");
	loadIdleSection(DATA, &KM_Humanoid_H2H_Back, "Humanoid-Unarmed-Back");

	loadIdleSection(DATA, &KM_Undead_1hm, "Undead-1HM");
	loadIdleSection(DATA, &KM_Undead_2hm, "Undead-2HM");
	loadIdleSection(DATA, &KM_Undead_2hw, "Undead-2HW");

	loadIdleSection(DATA, &KM_Falmer_1hm, "Falmer-1HM");
	loadIdleSection(DATA, &KM_Falmer_2hm, "Falmer-2HM");
	loadIdleSection(DATA, &KM_Falmer_2hw, "Falmer-2HW");


	loadIdleSection(DATA, &KM_Spider_1hm, "Spider-1HM");
	loadIdleSection(DATA, &KM_Spider_2hm, "Spider-2HM");
	loadIdleSection(DATA, &KM_Spider_2hw, "Spider-2HW");

	loadIdleSection(DATA, &KM_Gargoyle_1hm, "Gargoyle-1HM");
	loadIdleSection(DATA, &KM_Gargoyle_2hm, "Gargoyle-2HM");
	loadIdleSection(DATA, &KM_Gargoyle_2hw, "Gargoyle-2HW");

	loadIdleSection(DATA, &KM_Giant_1hm, "Giant-1HM");
	loadIdleSection(DATA, &KM_Giant_2hm, "Giant-2HM");
	loadIdleSection(DATA, &KM_Giant_2hw, "Ginat-2HW");

	loadIdleSection(DATA, &KM_Bear_1hm, "Bear-1HM");
	loadIdleSection(DATA, &KM_Bear_2hm, "Bear-2HM");
	loadIdleSection(DATA, &KM_Bear_2hw, "Bear-2HW");

	loadIdleSection(DATA, &KM_SabreCat_1hm, "SabreCat-1HM");
	loadIdleSection(DATA, &KM_SabreCat_2hm, "SabreCat-2HM");
	loadIdleSection(DATA, &KM_SabreCat_2hw, "SabreCat-2HW");

	loadIdleSection(DATA, &KM_Wolf_1hm, "Wolf-1HM");
	loadIdleSection(DATA, &KM_Wolf_2hm, "Wolf-2HM");
	loadIdleSection(DATA, &KM_Wolf_2hw, "Wolf-2HW");

	loadIdleSection(DATA, &KM_Troll_1hm, "Troll-1HM");
	loadIdleSection(DATA, &KM_Troll_2hm, "Troll-2HM");
	loadIdleSection(DATA, &KM_Troll_2hw, "Troll-2HW");


	loadIdleSection(DATA, &KM_Hagraven_1hm, "Hagraven-1HM");
	loadIdleSection(DATA, &KM_Hagraven_2hm, "Hagraven-2HM");
	loadIdleSection(DATA, &KM_Hagraven_2hw, "Hagraven-2HW");

	loadIdleSection(DATA, &KM_Spriggan_1hm, "Spriggan-1HM");
	loadIdleSection(DATA, &KM_Spriggan_2hm, "Spriggan-2HM");
	loadIdleSection(DATA, &KM_Spriggan_2hw, "Spriggan-2HW");

	loadIdleSection(DATA, &KM_Boar_1hm, "Boar-1HM");
	loadIdleSection(DATA, &KM_Boar_2hm, "Boar-2HM");
	loadIdleSection(DATA, &KM_Boar_2hw, "Boar-2HW");

	loadIdleSection(DATA, &KM_Riekling_1hm, "Riekling-1HM");
	loadIdleSection(DATA, &KM_Riekling_2hm, "Riekling-2HM");
	loadIdleSection(DATA, &KM_Riekling_2hw, "Riekling-2HW");

	loadIdleSection(DATA, &KM_AshHopper_1hm, "AshHopper-1HM");
	loadIdleSection(DATA, &KM_AshHopper_2hm, "AshHopper-2HM");
	loadIdleSection(DATA, &KM_AshHopper_2hw, "AshHopper-2HW");

	loadIdleSection(DATA, &KM_Ballista_1hm, "DwarvenBallista-1HM");
	loadIdleSection(DATA, &KM_Ballista_2hm, "DwarvenBallista-2HM");
	loadIdleSection(DATA, &KM_Ballista_2hw, "DwarvenBallista-2HW");

	loadIdleSection(DATA, &KM_ChaurusFlyer_1hm, "ChaurusFlyer-1HM");
	loadIdleSection(DATA, &KM_ChaurusFlyer_2hm, "ChaurusFlyer-2HM");
	loadIdleSection(DATA, &KM_ChaurusFlyer_2hw, "ChaurusFlyer-2HW");


	loadIdleSection(DATA, &KM_Lurker_1hm, "Lurker-1HM");
	loadIdleSection(DATA, &KM_Lurker_2hm, "Lurker-2HM");
	loadIdleSection(DATA, &KM_Lurker_2hw, "Lurker-2HW");


	loadIdleSection(DATA, &KM_Dragon_1hm, "Dragon-1HM");
	loadIdleSection(DATA, &KM_Dragon_2hm, "Dragon-2HM");
	loadIdleSection(DATA, &KM_Dragon_2hw, "Dragon-2HW");
	INFO("Idle loaded.");
}

void gameDataCache::loadDifficultySettings() {
	INFO("Loading difficulty settings...");
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
}