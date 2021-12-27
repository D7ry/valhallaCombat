#pragma once
#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include "spdlog/async_logger.h"

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
	std::atomic<bool> isPlayerExhausted = false;
	RE::GFxMovieView* Hud = nullptr;


	void staminaBlinkOnce() noexcept;

private:
	bool AcquireHud() noexcept;





public:
	void addDebuffPerk() {
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (debuffPerk && !pc->HasPerk(debuffPerk)) {
			RE::PlayerCharacter::GetSingleton()->AddPerk(debuffPerk);
		}
	}

	void rmDebuffPerk() {
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (debuffPerk && pc->HasPerk(debuffPerk)) {
			pc->RemovePerk(debuffPerk);
		}
	}


	void initStaminaDebuff();
};

namespace debuffOps {
	inline void staminaDebuffCheck() {
		auto _debuffHandler = debuffHandler::GetSingleton();
		auto pc = RE::PlayerCharacter::GetSingleton();
		while (_debuffHandler->isPlayerExhausted && pc != nullptr) {
			if (pc->GetActorValue(RE::ActorValue::kStamina) == pc->GetPermanentActorValue(RE::ActorValue::kStamina)) {
				_debuffHandler->rmDebuffPerk();
				_debuffHandler->isPlayerExhausted = false;
				debuffThread::t2.request_stop();
				break;
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
		}
	}

	inline void staminaDebuffBlink() {
		auto _debuffHandler = debuffHandler::GetSingleton();
		while (_debuffHandler->isPlayerExhausted) {
			_debuffHandler->staminaBlinkOnce();
			std::this_thread::sleep_for(std::chrono::milliseconds(800));
		}
	}
}