#pragma once
#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include "spdlog/async_logger.h"
#include "Utils.h"
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
	
	


private:

	void staminaDebuffOp();

	void addDebuffSpell() {
		Utils::applySpell(debuffSpell, RE::PlayerCharacter::GetSingleton());
	}

	void rmDebuffSpell() {
		Utils::removeSpell(debuffSpell, RE::PlayerCharacter::GetSingleton());
	}



	RE::BGSPerk* debuffPerk;

	RE::SpellItem* debuffSpell;

	std::atomic<bool> isPlayerExhausted = false;

};


