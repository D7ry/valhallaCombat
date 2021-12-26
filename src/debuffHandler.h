#pragma once
#include <thread>
#include <iostream>
#include <chrono>

namespace debuffHandler
{
	inline RE::GFxMovieView* Hud = nullptr;

	inline bool AcquireHud() noexcept {
		const auto* ui = RE::UI::GetSingleton();
		if (!ui) {
			return false;
		}
		const auto& map = ui->menuMap;
		for (auto& [name, entry] : map) {
			if (name == RE::HUDMenu::MENU_NAME) {
				Hud = entry.menu->uiMovie.get();
					Hud->AddRef();
			}
		}
		return Hud;
	}

	inline void startStaminaBlinking() noexcept {
		if (Hud) {
			DEBUG("blink");
			Hud->InvokeNoReturn("_level0.HUDMovieBaseInstance.StartStaminaBlinking", nullptr, 0);
		}
		else {
			ERROR("hud not found");
		}
		
	}

	inline auto getDebuffPerk() {
		auto perk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(0x00000d63, "ValhallaCombat.esp");
		if (perk) {
			return perk;
		} else {
			ERROR("debuff perk not found. Make sure you have ValhallaCombat.esp enabled");
		}
	}

	inline std::jthread t1;

	inline std::atomic<bool> isPlayerExhausted = false;

	static inline void addDebuffPerk() {
		auto debuffPerk = getDebuffPerk();
		auto pc = RE::PlayerCharacter::GetSingleton();
		DEBUG("adding {}", debuffPerk->GetName());
		if (debuffPerk && !pc->HasPerk(debuffPerk)) {
			RE::PlayerCharacter::GetSingleton()->AddPerk(debuffPerk);
			DEBUG("debuff perk added successfully");
		}
	}

	static inline void rmDebuffPerk() {
		auto debuffPerk = getDebuffPerk();
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
			if (pc->GetActorValue(RE::ActorValue::kStamina) == pc->GetPermanentActorValue(RE::ActorValue::kStamina)) {
				DEBUG("player has recovered!");
				rmDebuffPerk();
				isPlayerExhausted = false;
				break;
			} else {
				//startStaminaBlinking();
				DEBUG(pc->GetActorValue(RE::ActorValue::kStamina));
				std::this_thread::sleep_for(std::chrono::milliseconds(800));
			}
		}
		return;
	}

	static inline void initStaminaDebuff() {
		if (isPlayerExhausted) {
			DEBUG("player already exhausted!");
		}
		else {
			DEBUG("delegating thread");
			addDebuffPerk();
			isPlayerExhausted = true;
			t1 = std::jthread(staminaDebuffCheck);
			t1.detach();
		}
	}



};

