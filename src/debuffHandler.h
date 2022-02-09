#pragma once
#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include <boost/container/set.hpp>
#include <boost/foreach.hpp>
#include "spdlog/async_logger.h"
#include "Utils.h"
#include "TrueHUDAPI.h"
//TODO:threading no longer works for NPC as there might be more NPCs to count debuff than threads. Need another workaround.
#define foreach_         BOOST_FOREACH
namespace debuffThread {
	inline std::jthread t1;
}
class debuffHandler
{

	debuffHandler() {
		debuffSpell = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0x00001827, "ValhallaCombat.esp");
		//debuffPerk = 
	}


public:
	/*Set of actors experiencing debuff.*/
	boost::container::set<RE::Actor*> actorsInDebuff;

	static debuffHandler* GetSingleton()
	{
		static debuffHandler singleton;
		return  std::addressof(singleton);
	}

	/*called along with the main update hook.*/
	void update() {
		/*Iterate through the set of actors debuffing,
		checking their stamina.*/
		auto it = actorsInDebuff.begin();
		while (it != actorsInDebuff.end()) {
			if ((*it)->GetActorValue(RE::ActorValue::kStamina) >= (*it)->GetPermanentActorValue(RE::ActorValue::kStamina)) {
				DEBUG("{}'s stamina has fully recovered", (*it)->GetName());
				debuffHandler::stopStaminaDebuff((*it));
				it = actorsInDebuff.erase(it); //erase actor from debuff set.
				DEBUG("actor erased from debuff set");
			}
			else {
				++it;
			}
		}
	}

	void initStaminaDebuff(RE::Actor* actor);
	void stopStaminaDebuff(RE::Actor* actor);
	/*reset debuff state on game load*/
	void refresh() {
		//isPlayerExhausted = false;
		rmDebuffSpell();
	}

	static inline std::atomic<bool> isPlayerExhausted = false;
	static inline TRUEHUD_API::IVTrueHUD1* g_trueHUD = nullptr;

private:


	void addDebuffSpell() {
		Utils::safeApplySpell(debuffSpell, RE::PlayerCharacter::GetSingleton());
	}

	void rmDebuffSpell() {
		Utils::safeRemoveSpell(debuffSpell, RE::PlayerCharacter::GetSingleton());
	}

	RE::BGSPerk* debuffPerk;

	RE::SpellItem* debuffSpell;


};


