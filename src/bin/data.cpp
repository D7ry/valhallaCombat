#include "include/data.h"
#include "include/stunHandler.h"
using namespace Utils;

void data::loadData() {
	INFO("Loading data from game...");
	loadSound();
	loadPerk();
	loadDifficultySettings();
	loadIdle();
	INFO("Data fetched.");
}

void data::loadSound() {
	INFO("Loading sound descriptors...");
	soundParryShieldD = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0X433C, "ValhallaCombat.esp");
	soundParryWeaponD = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0X3DD9, "ValhallaCombat.esp");
	soundParryShield_gbD = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0X47720, "ValhallaCombat.esp");
	soundParryWeapon_gbD = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0X47721, "ValhallaCombat.esp");
	if (soundParryShieldD && soundParryWeaponD && soundParryShield_gbD && soundParryWeapon_gbD) {
		INFO("Sound descriptors successfully loaded!");
	}
}

void data::loadPerk() {
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
bool data::lookupIdle(RE::TESDataHandler* DATA, RE::FormID form, std::string pluginName, std::vector<RE::TESIdleForm*>* idleContainer) {
	auto idle = DATA->LookupForm<RE::TESIdleForm>(form, pluginName);
	if (!idle) {
		return false;
	}
	idleContainer->push_back(idle);
	return true;
}

void data::loadIdleSection(RE::TESDataHandler* DATA, std::vector<RE::TESIdleForm*>* idleContainer, const char* section) {
	//FIXME:this is a temporary janky workaround for simpleIni
	CSimpleIniA ini;
#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\ValhallaCombat_Killmoves\\ValhallaCombat_Killmoves.ini"
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

void data::loadIdle() {
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

void data::loadDifficultySettings() {
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