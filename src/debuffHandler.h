#pragma once
#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include "spdlog/async_logger.h"
#include "Utils.h"
#include "TrueHUDAPI.h"
namespace debuffThread {
	inline std::jthread t1;
}
class debuffHandler
{
	debuffHandler() {
		debuffSpell = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0x00001827, "ValhallaCombat.esp");
	}


public:
	static debuffHandler* GetSingleton()
	{
		static debuffHandler singleton;
		return  std::addressof(singleton);
	}


	void initStaminaDebuff();

	/*reset debuff state on game load*/
	void refresh() {
		//isPlayerExhausted = false;
		rmDebuffSpell();
	}

	static inline std::atomic<bool> isPlayerExhausted = false;
	static inline TRUEHUD_API::IVTrueHUD1* g_trueHUD = nullptr;

private:

	void staminaDebuffOp();

	void addDebuffSpell() {
		Utils::safeApplySpell(debuffSpell, RE::PlayerCharacter::GetSingleton());
	}

	void rmDebuffSpell() {
		Utils::safeRemoveSpell(debuffSpell, RE::PlayerCharacter::GetSingleton());
	}

	RE::BGSPerk* debuffPerk;

	RE::SpellItem* debuffSpell;


};


