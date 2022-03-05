#pragma once
#include "Utils.h"

#include "debuffHandler.h"
#include "lib/robin_hood.h"
#include "lib/SimpleIni.h"

/*Everything stored in game.*/
class data
{
public:
	static data* GetSingleton()
	{
		static data singleton;
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
#pragma region ExecutionRaces
	/*Mapping of all executable races -> Vector of killmove idles suitable for those races.*/
	robin_hood::unordered_map<RE::TESRace*, std::vector<RE::TESIdleForm*>> ExecutionRaceMap;
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