#include "include/data.h"
#include "include/stunHandler.h"
#include "include/Utils.h"

using namespace inlineUtils;
void data::loadData() {
	logger::info("Loading data from game...");
	auto data = RE::TESDataHandler::GetSingleton();
	if (!data) {
		logger::critical("Error: TESDataHandler not found.");
		return;
	}
	loadBlockFX(data);
	loadSound(data);
	loadPerk(data);
	loadIdle(data);
	loadExecutableRace(data);
	

	auto gameSettings = RE::GameSettingCollection::GetSingleton();
	if (!gameSettings) {
		logger::critical("Error: GameSettingCollection not found.");
		return;
	}
	loadDifficultySettings(gameSettings);

	logger::info("Data fetched.");
	
}

void data::loadSound(RE::TESDataHandler* a_data) {
	logger::info("Loading sound descriptors...");
	DtryUtils::formLoader loader("ValhallaCombat.esp");
	loader.load(soundParryShield, 0x433c);
	loader.load(soundParryShield_perfect, 0x60c2e);
	loader.load(soundParryShield_gb, 0X47720);
	loader.load(soundParryWeapon, 0X3DD9);
	loader.load(soundParryWeapon_perfect, 0X60C2D);
	loader.load(soundParryWeapon_gb, 0X47721);
	loader.load(soundStunBreak, 0X56A22);
	loader.load(soundTackleStart, 0x60c36);
	loader.log();
	logger::info("...done");
}

void data::loadPerk(RE::TESDataHandler* a_data) {
	logger::info("Loading perk...");
	DtryUtils::formLoader loader("ValhallaCombat.esp");
	loader.load(debuffPerk, 0x2DB2);
	loader.log();
	logger::info("...done");
}

void data::loadBlockFX(RE::TESDataHandler* a_data) {
	logger::info("Loading block FX...");
	DtryUtils::formLoader loader("ValhallaCombat.esp");
	loader.load(BlockFX, 0x60c2f);
	loader.load(BlockSpark, 0x60c30);
	loader.load(BlockSparkFlare, 0x60c31);
	loader.load(BlockSparkRing, 0x60c32);
	loader.log();
	logger::info("...done");
}


bool data::lookupIdle(RE::TESDataHandler* data, RE::FormID form, std::string pluginName, 
	std::vector<RE::TESIdleForm*>* idleContainer) {
	auto idle = data->LookupForm<RE::TESIdleForm>(form, pluginName);
	if (!idle) {
		return false;
	}
	idleContainer->push_back(idle);
	return true;
}

void data::loadIdleSection(RE::TESDataHandler* a_data, std::vector<RE::TESIdleForm*>* idleContainer, 
	CSimpleIniA& ini, const char* section) {
	//logger::info("Loading from section {}", section);
	CSimpleIniA::TNamesDepend keys;
	ini.GetAllKeys(section, keys);
	int idlesLoaded = 0;
	for (CSimpleIniA::TNamesDepend::iterator s_it1 = keys.begin(); s_it1 != keys.end(); s_it1++) {
		const char* idle = s_it1->pItem;
		auto line = ini.GetValue(section, idle);
		std::vector<std::string> idleConfigs = tokenize("|", line);
		if (idleConfigs.size() != 2) {
			//ERROR("Error: wrong config length");
			continue;
		}
		std::string plugin = idleConfigs[0];
		int form = 0;
		if (!inlineUtils::ToInt32(idleConfigs[1], form)) {
			//ERROR("Error: wrong formID");
			continue;
		}
		if (lookupIdle(a_data, form, plugin, idleContainer)) {
			idlesLoaded++;
			//logger::info("Loaded idle. FormID: {}, plugin: {}", form, plugin);
		}
		else {
			logger::info("Error: Failed to load idle. FormID: {}, plugin: {}", form, plugin);
		}

	}
	logger::info("Loaded {} idles from section {}.", idlesLoaded, section);
}

void data::loadIdle(RE::TESDataHandler* a_actor) {
	CSimpleIniA ini;
	ini.LoadFile(kmFilePath);
	logger::info("Loading idle...");
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

	loadIdleSection(DATA, &KM_Centurion_1hm, ini, "SteamCenturion-1HM");
	loadIdleSection(DATA, &KM_Centurion_2hm, ini, "SteamCenturion-2HM");
	loadIdleSection(DATA, &KM_Centurion_2hw, ini, "SteamCenturion-2HW");

	loadIdleSection(DATA, &KM_ChaurusFlyer_1hm, ini, "ChaurusFlyer-1HM");
	loadIdleSection(DATA, &KM_ChaurusFlyer_2hm, ini, "ChaurusFlyer-2HM");
	loadIdleSection(DATA, &KM_ChaurusFlyer_2hw, ini, "ChaurusFlyer-2HW");


	loadIdleSection(DATA, &KM_Lurker_1hm, ini, "Lurker-1HM");
	loadIdleSection(DATA, &KM_Lurker_2hm, ini, "Lurker-2HM");
	loadIdleSection(DATA, &KM_Lurker_2hw, ini, "Lurker-2HW");


	loadIdleSection(DATA, &KM_Dragon_1hm, ini, "Dragon-1HM");
	loadIdleSection(DATA, &KM_Dragon_2hm, ini, "Dragon-2HM");
	loadIdleSection(DATA, &KM_Dragon_2hw, ini, "Dragon-2HW");
	logger::info("Idle loaded.");
}

bool data::pairUpRace(RE::TESDataHandler* a_actor, RE::FormID a_formID, std::string a_plugin, raceCatagory a_raceType) {
	auto race = a_actor->LookupForm<RE::TESRace>(a_formID, a_plugin);
	if (!race) {
		return false;
	}
	raceMapping.emplace(race, a_raceType);
	//logger::info("Mapped {} from {} to race catagory {}", race->GetName(), pluginName, raceType);
	return true;
}

void data::loadRaceSection(RE::TESDataHandler* a_data, raceCatagory a_raceType, CSimpleIniA& a_ini, const char* a_section) {
	//logger::info("Loading from section {}", section);
	CSimpleIniA::TNamesDepend keys;
	a_ini.GetAllKeys(a_section, keys);
	int raceLoaded = 0;
	for (CSimpleIniA::TNamesDepend::iterator s_it1 = keys.begin(); s_it1 != keys.end(); s_it1++) {
		const char* idle = s_it1->pItem;
		auto line = a_ini.GetValue(a_section, idle);
		//logger::info(line);
		std::vector<std::string> raceConfigs = tokenize("|", line);
		if (raceConfigs.size() != 2) {
			logger::critical("Error: wrong config length");
			continue;
		}
		std::string plugin = raceConfigs[0];
		int form = 0;
		if (!inlineUtils::ToInt32(raceConfigs[1], form)) {
			logger::critical("Error: wrong formID");
			continue;
		}
		if (pairUpRace(a_data, form, plugin, a_raceType)) {
			raceLoaded++;
		}
		else {
			logger::info("Failed to load race. FormID: {}, plugin: {}", form, plugin);
		}

	}
	logger::info("Loaded {} races from section {}.", raceLoaded, a_section);
}

void data::loadExecutableRaceIni(RE::TESDataHandler* a_data, const char* a_path) {
	CSimpleIniA ini;
	ini.LoadFile(a_path);
	loadRaceSection(a_data, raceCatagory::Humanoid, ini, "Humanoid");
	loadRaceSection(a_data, raceCatagory::Undead, ini, "Undead");
	loadRaceSection(a_data, raceCatagory::Falmer, ini, "Falmer");
	loadRaceSection(a_data, raceCatagory::Spider, ini, "Spider");
	loadRaceSection(a_data, raceCatagory::Gargoyle, ini, "Gargoyle");
	loadRaceSection(a_data, raceCatagory::Giant, ini, "Giant");
	loadRaceSection(a_data, raceCatagory::Bear, ini, "Bear");
	loadRaceSection(a_data, raceCatagory::SabreCat, ini, "SabreCat");
	loadRaceSection(a_data, raceCatagory::Wolf, ini, "Wolf");
	loadRaceSection(a_data, raceCatagory::Troll, ini, "Troll");
	loadRaceSection(a_data, raceCatagory::Hagraven, ini, "Hagraven");
	loadRaceSection(a_data, raceCatagory::Spriggan, ini, "Spriggan");
	loadRaceSection(a_data, raceCatagory::Boar, ini, "Boar");
	loadRaceSection(a_data, raceCatagory::Riekling, ini, "Riekling");
	loadRaceSection(a_data, raceCatagory::AshHopper, ini, "AshHopper");
	loadRaceSection(a_data, raceCatagory::SteamCenturion, ini, "SteamCenturion");
	loadRaceSection(a_data, raceCatagory::DwarvenBallista, ini, "DwarvenBallista");
	loadRaceSection(a_data, raceCatagory::ChaurusFlyer, ini, "ChaurusFlyer");
	loadRaceSection(a_data, raceCatagory::Lurker, ini, "Lurker");
	loadRaceSection(a_data, raceCatagory::Dragon, ini, "Dragon");
}

void data::loadExecutableRace(RE::TESDataHandler* a_data) {
	for (const auto& entry : std::filesystem::directory_iterator(kmRaceDir)) { //iterates through all .ini files
		std::string pathStr = entry.path().string();
		logger::info("Reading race config from {}", pathStr);
		const char* cstr = pathStr.c_str();
		loadExecutableRaceIni(a_data, cstr);
	}
}



void data::loadDifficultySettings(RE::GameSettingCollection* a_gameSettingCollection) {
	logger::info("Loading difficulty settings...");
	fDiffMultHPByPCVE = a_gameSettingCollection->GetSetting("fDiffMultHPByPCVE")->GetFloat();
	fDiffMultHPByPCE = a_gameSettingCollection->GetSetting("fDiffMultHPByPCE")->GetFloat();
	fDiffMultHPByPCN = a_gameSettingCollection->GetSetting("fDiffMultHPByPCN")->GetFloat();
	fDiffMultHPByPCH = a_gameSettingCollection->GetSetting("fDiffMultHPByPCH")->GetFloat();
	fDiffMultHPByPCVH = a_gameSettingCollection->GetSetting("fDiffMultHPByPCVH")->GetFloat();
	fDiffMultHPByPCL = a_gameSettingCollection->GetSetting("fDiffMultHPByPCL")->GetFloat();

	fDiffMultHPToPCVE = a_gameSettingCollection->GetSetting("fDiffMultHPToPCVE")->GetFloat();
	fDiffMultHPToPCE = a_gameSettingCollection->GetSetting("fDiffMultHPToPCE")->GetFloat();
	fDiffMultHPToPCN = a_gameSettingCollection->GetSetting("fDiffMultHPToPCN")->GetFloat();
	fDiffMultHPToPCH = a_gameSettingCollection->GetSetting("fDiffMultHPToPCH")->GetFloat();
	fDiffMultHPToPCVH = a_gameSettingCollection->GetSetting("fDiffMultHPToPCVH")->GetFloat();
	fDiffMultHPToPCL = a_gameSettingCollection->GetSetting("fDiffMultHPToPCL")->GetFloat();

	fCombatHitConeAngle = a_gameSettingCollection->GetSetting("fCombatHitConeAngle")->GetFloat();
	logger::info("...done");
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
