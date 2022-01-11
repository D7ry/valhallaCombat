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
		debuffPerk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(0x000012C5, "Valhamina.esp");
	}


public:
	static debuffHandler* GetSingleton()
	{
		static debuffHandler singleton;
		return  std::addressof(singleton);
	}

	void addDebuffPerk() {
		Utils::addPerkToPc(debuffPerk);
	}

	void rmDebuffPerk() {
		Utils::rmPerkFromPc(debuffPerk);
	}

	void initStaminaDebuff();

	/*reset debuff state on game load*/
	void refresh() {
		//isPlayerExhausted = false;
		rmDebuffPerk();
	}
	
	


private:
	void staminaDebuffOp();


	RE::BGSPerk* debuffPerk;

	std::atomic<bool> isPlayerExhausted = false;

};


