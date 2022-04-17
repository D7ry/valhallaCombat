#include "include/data.h"
#include "include/stunHandler.h"
using namespace Utils;

void data::loadData() {
	INFO("Loading data from game...");
	loadSound();
	loadPerk();
	loadDifficultySettings();
	loadIdle();
	loadExecutableRace();
	INFO("Data fetched.");
	
}

void data::loadSound() {
	INFO("Loading sound descriptors...");
	auto DATA = RE::TESDataHandler::GetSingleton();
	soundParryShieldD = DATA->LookupForm<RE::BGSSoundDescriptorForm>(0X433C, "ValhallaCombat.esp");
	soundParryWeaponD = DATA->LookupForm<RE::BGSSoundDescriptorForm>(0X3DD9, "ValhallaCombat.esp");
	soundParryShield_gbD = DATA->LookupForm<RE::BGSSoundDescriptorForm>(0X47720, "ValhallaCombat.esp");
	soundParryWeapon_gbD = DATA->LookupForm<RE::BGSSoundDescriptorForm>(0X47721, "ValhallaCombat.esp");
	soundStunBreakD = DATA->LookupForm<RE::BGSSoundDescriptorForm>(0X56A22, "ValhallaCombat.esp");
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

bool data::lookupIdle(RE::TESDataHandler* DATA, RE::FormID form, std::string pluginName, 
	std::vector<RE::TESIdleForm*>* idleContainer) {
	auto idle = DATA->LookupForm<RE::TESIdleForm>(form, pluginName);
	if (!idle) {
		return false;
	}
	idleContainer->push_back(idle);
	return true;
}

void data::loadIdleSection(RE::TESDataHandler* DATA, std::vector<RE::TESIdleForm*>* idleContainer, 
	CSimpleIniA& ini, const char* section) {
	INFO("Loading from section {}", section);
	CSimpleIniA::TNamesDepend keys;
	ini.GetAllKeys(section, keys);
	int idlesLoaded = 0;
	for (CSimpleIniA::TNamesDepend::iterator s_it1 = keys.begin(); s_it1 != keys.end(); s_it1++) {
		const char* idle = s_it1->pItem;
		auto line = ini.GetValue(section, idle);
		std::vector<std::string> idleConfigs = tokenize("|", line);
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
	CSimpleIniA ini;
	ini.LoadFile(kmFilePath);
	INFO("Loading idle...");
	auto DATA = RE::TESDataHandler::GetSingleton();
	loadIdleSection(DATA, &KM_Humanoid_H2H, ini ,"Humanoid-Unarmed");
	loadIdleSection(DATA, &KM_Humanoid_Dagger, ini, "Humanoid-Dagger");
	loadIdleSection(DATA, &KM_Humanoid_Sword, ini, "Humanoid-Sword");
	loadIdleSection(DATA, &KM_Humanoid_Axe, ini, "Humanoid-Axe");
	loadIdleSection(DATA, &KM_Humanoid_Mace, ini, "Humanoid-Mace");
	loadIdleSection(DATA, &KM_Humanoid_GreatSword, ini, "Humanoid-GreatSword");
	loadIdleSection(DATA, &KM_Humanoid_2hw, ini, "Humanoid-2HW");

	loadIdleSection(DATA, &KM_Humanoid_DW, ini, "Humanoid-DW");

	loadIdleSection(DATA, &KM_Humanoid_1hm_Back, ini, "Humanoid-1HM-Back");
	loadIdleSection(DATA, &KM_Humanoid_2hm_Back, ini, "Humanoid-2HM-Back");
	loadIdleSection(DATA, &KM_Humanoid_2hw_Back, ini, "Humanoid-2HW-Back");
	loadIdleSection(DATA, &KM_Humanoid_H2H_Back, ini, "Humanoid-Unarmed-Back");

	loadIdleSection(DATA, &KM_Undead_1hm, ini, "Undead-1HM");
	loadIdleSection(DATA, &KM_Undead_2hm, ini, "Undead-2HM");
	loadIdleSection(DATA, &KM_Undead_2hw, ini, "Undead-2HW");

	loadIdleSection(DATA, &KM_Falmer_1hm, ini, "Falmer-1HM");
	loadIdleSection(DATA, &KM_Falmer_2hm, ini, "Falmer-2HM");
	loadIdleSection(DATA, &KM_Falmer_2hw, ini, "Falmer-2HW");


	loadIdleSection(DATA, &KM_Spider_1hm, ini, "Spider-1HM");
	loadIdleSection(DATA, &KM_Spider_2hm, ini, "Spider-2HM");
	loadIdleSection(DATA, &KM_Spider_2hw, ini, "Spider-2HW");

	loadIdleSection(DATA, &KM_Gargoyle_1hm, ini, "Gargoyle-1HM");
	loadIdleSection(DATA, &KM_Gargoyle_2hm, ini, "Gargoyle-2HM");
	loadIdleSection(DATA, &KM_Gargoyle_2hw, ini, "Gargoyle-2HW");

	loadIdleSection(DATA, &KM_Giant_1hm, ini, "Giant-1HM");
	loadIdleSection(DATA, &KM_Giant_2hm, ini, "Giant-2HM");
	loadIdleSection(DATA, &KM_Giant_2hw, ini, "Ginat-2HW");

	loadIdleSection(DATA, &KM_Bear_1hm, ini, "Bear-1HM");
	loadIdleSection(DATA, &KM_Bear_2hm, ini, "Bear-2HM");
	loadIdleSection(DATA, &KM_Bear_2hw, ini, "Bear-2HW");

	loadIdleSection(DATA, &KM_SabreCat_1hm, ini, "SabreCat-1HM");
	loadIdleSection(DATA, &KM_SabreCat_2hm, ini, "SabreCat-2HM");
	loadIdleSection(DATA, &KM_SabreCat_2hw, ini, "SabreCat-2HW");

	loadIdleSection(DATA, &KM_Wolf_1hm, ini, "Wolf-1HM");
	loadIdleSection(DATA, &KM_Wolf_2hm, ini, "Wolf-2HM");
	loadIdleSection(DATA, &KM_Wolf_2hw, ini, "Wolf-2HW");

	loadIdleSection(DATA, &KM_Troll_1hm, ini, "Troll-1HM");
	loadIdleSection(DATA, &KM_Troll_2hm, ini, "Troll-2HM");
	loadIdleSection(DATA, &KM_Troll_2hw, ini, "Troll-2HW");


	loadIdleSection(DATA, &KM_Hagraven_1hm, ini, "Hagraven-1HM");
	loadIdleSection(DATA, &KM_Hagraven_2hm, ini, "Hagraven-2HM");
	loadIdleSection(DATA, &KM_Hagraven_2hw, ini, "Hagraven-2HW");

	loadIdleSection(DATA, &KM_Spriggan_1hm, ini, "Spriggan-1HM");
	loadIdleSection(DATA, &KM_Spriggan_2hm, ini, "Spriggan-2HM");
	loadIdleSection(DATA, &KM_Spriggan_2hw, ini, "Spriggan-2HW");

	loadIdleSection(DATA, &KM_Boar_1hm, ini, "Boar-1HM");
	loadIdleSection(DATA, &KM_Boar_2hm, ini, "Boar-2HM");
	loadIdleSection(DATA, &KM_Boar_2hw, ini, "Boar-2HW");

	loadIdleSection(DATA, &KM_Riekling_1hm, ini, "Riekling-1HM");
	loadIdleSection(DATA, &KM_Riekling_2hm, ini, "Riekling-2HM");
	loadIdleSection(DATA, &KM_Riekling_2hw, ini, "Riekling-2HW");

	loadIdleSection(DATA, &KM_AshHopper_1hm, ini, "AshHopper-1HM");
	loadIdleSection(DATA, &KM_AshHopper_2hm, ini, "AshHopper-2HM");
	loadIdleSection(DATA, &KM_AshHopper_2hw, ini, "AshHopper-2HW");

	loadIdleSection(DATA, &KM_Ballista_1hm, ini, "DwarvenBallista-1HM");
	loadIdleSection(DATA, &KM_Ballista_2hm, ini, "DwarvenBallista-2HM");
	loadIdleSection(DATA, &KM_Ballista_2hw, ini, "DwarvenBallista-2HW");

	loadIdleSection(DATA, &KM_ChaurusFlyer_1hm, ini, "ChaurusFlyer-1HM");
	loadIdleSection(DATA, &KM_ChaurusFlyer_2hm, ini, "ChaurusFlyer-2HM");
	loadIdleSection(DATA, &KM_ChaurusFlyer_2hw, ini, "ChaurusFlyer - 2HW");


	loadIdleSection(DATA, &KM_Lurker_1hm, ini, "Lurker-1HM");
	loadIdleSection(DATA, &KM_Lurker_2hm, ini, "Lurker-2HM");
	loadIdleSection(DATA, &KM_Lurker_2hw, ini, "Lurker-2HW");


	loadIdleSection(DATA, &KM_Dragon_1hm, ini, "Dragon-1HM");
	loadIdleSection(DATA, &KM_Dragon_2hm, ini, "Dragon-2HM");
	loadIdleSection(DATA, &KM_Dragon_2hw, ini, "Dragon-2HW");
	INFO("Idle loaded.");
}

bool data::pairUpRace(RE::TESDataHandler* DATA, RE::FormID form, std::string pluginName, raceCatagory raceType) {
	auto race = DATA->LookupForm<RE::TESRace>(form, pluginName);
	if (!race) {
		return false;
	}
	raceMapping.emplace(race, raceType);
	INFO("Mapped {} from {} to race catagory {}", race->GetName(), pluginName, raceType);
	return true;
}

void data::loadRaceSection(RE::TESDataHandler* DATA, raceCatagory raceType, CSimpleIniA& ini, const char* section) {
	INFO("Loading from section {}", section);
	CSimpleIniA::TNamesDepend keys;
	ini.GetAllKeys(section, keys);
	int raceLoaded = 0;
	for (CSimpleIniA::TNamesDepend::iterator s_it1 = keys.begin(); s_it1 != keys.end(); s_it1++) {
		const char* idle = s_it1->pItem;
		auto line = ini.GetValue(section, idle);
		INFO(line);
		std::vector<std::string> raceConfigs = tokenize("|", line);
		if (raceConfigs.size() != 2) {
			ERROR("Error: wrong config length");
			continue;
		}
		std::string plugin = raceConfigs[0];
		int form = 0;
		if (!Utils::ToInt32(raceConfigs[1], form)) {
			ERROR("Error: wrong formID");
			continue;
		}
		if (pairUpRace(DATA, form, plugin, raceType)) {
			raceLoaded++;
		}
		else {
			INFO("Failed to load race. FormID: {}, plugin: {}", form, plugin);
		}

	}
	INFO("Loaded {} races from section {}.", raceLoaded, section);
}

void data::loadExecutableRaceIni(RE::TESDataHandler* DATA, const char* ini_path) {
	CSimpleIniA ini;
	ini.LoadFile(ini_path);
	loadRaceSection(DATA, raceCatagory::Humanoid, ini, "Humanoid");
	loadRaceSection(DATA, raceCatagory::Undead, ini, "Undead");
	loadRaceSection(DATA, raceCatagory::Falmer, ini, "Falmer");
	loadRaceSection(DATA, raceCatagory::Spider, ini, "Spider");
	loadRaceSection(DATA, raceCatagory::Gargoyle, ini, "Gargoyle");
	loadRaceSection(DATA, raceCatagory::Giant, ini, "Giant");
	loadRaceSection(DATA, raceCatagory::Bear, ini, "Bear");
	loadRaceSection(DATA, raceCatagory::SabreCat, ini, "SabreCat");
	loadRaceSection(DATA, raceCatagory::Wolf, ini, "Wolf");
	loadRaceSection(DATA, raceCatagory::Troll, ini, "Troll");
	loadRaceSection(DATA, raceCatagory::Hagraven, ini, "Hagraven");
	loadRaceSection(DATA, raceCatagory::Spriggan, ini, "Spriggan");
	loadRaceSection(DATA, raceCatagory::Boar, ini, "Boar");
	loadRaceSection(DATA, raceCatagory::Riekling, ini, "Riekling");
	loadRaceSection(DATA, raceCatagory::AshHopper, ini, "AshHopper");
	loadRaceSection(DATA, raceCatagory::SteamCenturion, ini, "SteamCenturion");
	loadRaceSection(DATA, raceCatagory::DwarvenBallista, ini, "DwarvenBallista");
	loadRaceSection(DATA, raceCatagory::ChaurusFlyer, ini, "ChaurusFlyer");
	loadRaceSection(DATA, raceCatagory::Lurker, ini, "Lurker");
	loadRaceSection(DATA, raceCatagory::Dragon, ini, "Dragon");
}

void data::loadExecutableRace() {
	auto DATA = RE::TESDataHandler::GetSingleton();
	for (const auto& entry : std::filesystem::directory_iterator(kmRaceDir)) { //iterates through all .ini files
		std::string pathStr = entry.path().string();
		INFO("Reading from {}", pathStr);
		const char* cstr = pathStr.c_str();
		loadExecutableRaceIni(DATA, cstr);
	}
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
	INFO("Game difficulty multipliers loaded.");
}

bool data::isRaceType(RE::Actor* a_actor, raceCatagory a_catagory) {
	auto it = raceMapping.find(a_actor->GetRace());
	if (it != raceMapping.end()) {
		return it->second == a_catagory;
	}
	else {
		return false;
	}
}