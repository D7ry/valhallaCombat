#pragma once
#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include <boost/container/set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>
#include "spdlog/async_logger.h"
#include "Utils.h"
#include "TrueHUDAPI.h"
#include "data.h"
//TODO:threading no longer works for NPC as there might be more NPCs to count debuff than threads. Need another workaround.
#define foreach_         BOOST_FOREACH
namespace debuffThread {
	inline std::jthread t1;
}
class debuffHandler
{

	debuffHandler() {
		//debuffSpell = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0x00001827, "ValhallaCombat.esp");
		//debuffPerk = 
		playerMeterFlashTimer = 0;
		playerInDebuff = false;
		debuffPerk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(0x2DB2, "ValhallaCombat.esp");
	}


public:
	/*Set of actors experiencing debuff.*/
	//boost::unordered_map<RE::Actor*, float> actorsInDebuff;
	boost::container::set<RE::Actor*> actorsInDebuff;

	/*special bool to check if player is in debuff. If so, flash the player's meter*/
	bool playerInDebuff;
	float playerMeterFlashTimer;

	static debuffHandler* GetSingleton()
	{
		static debuffHandler singleton;
		return  std::addressof(singleton);
	}

	/*called along with the main update hook.*/
	void update();
	void greyOutStaminaMeter(RE::Actor* actor);
	void revertStaminaMeter(RE::Actor* actor);
	void initStaminaDebuff(RE::Actor* actor);
	void stopStaminaDebuff(RE::Actor* actor);

	static inline TRUEHUD_API::IVTrueHUD2* g_trueHUD = nullptr;

private:

	void addDebuffPerk(RE::Actor* a_actor) {
		Utils::safeApplyPerk(debuffPerk, a_actor);
	}

	void removeDebuffPerk(RE::Actor* a_actor) {
		Utils::safeRemovePerk(debuffPerk, a_actor);
	}

	RE::BGSPerk* debuffPerk;

	RE::SpellItem* debuffSpell;

};


