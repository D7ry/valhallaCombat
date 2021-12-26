#pragma once
#include <thread>
#include <iostream>
#include <chrono>
#include "spdlog/async_logger.h"
namespace debuffHandler
{
	inline bool staminaBlink = false;

	inline std::atomic<bool> isPlayerExhausted = false;

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
			Hud->InvokeNoReturn("_level0.HUDMovieBaseInstance.StartStaminaBlinking", nullptr, 0);
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

	inline std::jthread t2; //testing

	inline void staminaDebuffBlink() {
		while (!isPlayerExhausted && Hud) {
			startStaminaBlinking();
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		}
	}

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
		while (isPlayerExhausted && pc && pc->GetActorValue(RE::ActorValue::kStamina) && pc->GetPermanentActorValue(RE::ActorValue::kStamina)) {
			if (pc->GetActorValue(RE::ActorValue::kStamina) == pc->GetPermanentActorValue(RE::ActorValue::kStamina)) {
				rmDebuffPerk();
				isPlayerExhausted = false;
				break;
			} else {
				std::this_thread::sleep_for(std::chrono::milliseconds(800));
			}
		}
	}

	static inline void initStaminaDebuff() {
		if (isPlayerExhausted) {
			DEBUG("player already exhausted!");
		}
		else {
			addDebuffPerk();
			isPlayerExhausted = true;
			DEBUG("delegating threads");
			t1 = std::jthread(staminaDebuffCheck);
			t1.detach();
			if (staminaBlink) {
				DEBUG("setup stamina blink");
				t2 = std::jthread(staminaDebuffBlink);
				t2.detach();
			}
			DEBUG("threads detached");
		}
	}



};

