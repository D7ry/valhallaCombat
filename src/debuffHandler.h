#pragma once
#include <thread>
#include <iostream>
#include <chrono>
using namespace std;

namespace debuffHandler
{
	inline thread t1;

	inline boolean isPlayerExhausted = false;

	static inline void addDebuffPerk() {
		auto debuffPerk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(0x00000d63, "ValhallaCombat.esp");
		auto pc = RE::PlayerCharacter::GetSingleton();
		DEBUG("adding {}", debuffPerk->GetName());
		if (debuffPerk && !pc->HasPerk(debuffPerk)) {
			RE::PlayerCharacter::GetSingleton()->AddPerk(debuffPerk);
			DEBUG("debuff perk added successfully");
		}
	}

	static inline void rmDebuffPerk() {
		auto debuffPerk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(0x00000d63, "ValhallaCombat.esp");
		auto pc = RE::PlayerCharacter::GetSingleton();
		DEBUG("removing {}", debuffPerk->GetName());
		if (debuffPerk && pc->HasPerk(debuffPerk)) {
			pc->RemovePerk(debuffPerk);
			DEBUG("debuff perk removed successfully");
		}
	}

	static inline void staminaDebuffCheck() {
		auto pc = RE::PlayerCharacter::GetSingleton();
		while (isPlayerExhausted) {
			this_thread::sleep_for(chrono::milliseconds(400));
			if (pc->GetActorValue(RE::ActorValue::kStamina) == pc->GetPermanentActorValue(RE::ActorValue::kStamina)) {
				DEBUG("player has recovered!");
				isPlayerExhausted = false;
				rmDebuffPerk();
			} else {
				DEBUG(pc->GetActorValue(RE::ActorValue::kStamina));
			}
		}
		t1.detach();
	}

	static inline void initStaminaDebuff() {
		if (isPlayerExhausted) {
			DEBUG("player already exhausted!");
		}
		else {
			addDebuffPerk();
			DEBUG("delegating thread");
			isPlayerExhausted = true;
			t1 = thread(staminaDebuffCheck);
		}
	}



};

