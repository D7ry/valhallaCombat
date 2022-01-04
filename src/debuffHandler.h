#pragma once
#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include "spdlog/async_logger.h"
#include "Utils.h"
namespace debuffThread {
	inline std::jthread t1;

	inline std::jthread t2;

}

class debuffHandler
{
	debuffHandler();


public:
	static debuffHandler* GetSingleton()
	{
		static debuffHandler singleton;
		return  std::addressof(singleton);
	}

	bool staminaBlink = false;
	RE::BGSPerk* debuffPerk;
	RE::BGSPerk* staminaLimitPerk;
	std::atomic<bool> isPlayerExhausted = false;
	//RE::GFxMovieView* Hud = nullptr;

	//void staminaBlinkOnce() noexcept;

private:
	//bool AcquireHud() noexcept;
	void staminaDebuffCheck();

public:
	void addDebuffPerk() {
		Utils::addPerkToPc(debuffPerk);
	}

	void rmDebuffPerk() {
		Utils::rmPerkFromPc(debuffPerk);
	}

	void addStaminaLimitPerk() {
		Utils::addPerkToPc(staminaLimitPerk);
	}

	void initStaminaDebuff();
};


