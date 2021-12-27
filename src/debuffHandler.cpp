#include "debuffHandler.h"
debuffHandler::debuffHandler() {
	AcquireHud();
	debuffPerk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(0x00000d63, "ValhallaCombat.esp");
}

void debuffHandler::initStaminaDebuff() {
	if (isPlayerExhausted) {
		DEBUG("player already exhausted!");
	}
	else {
		this->addDebuffPerk();
		this->isPlayerExhausted = true;
		DEBUG("Delegating threads");
		DEBUG("Setup stamina check");
		debuffThread::t1 = std::jthread(debuffOps::staminaDebuffCheck);
		debuffThread::t1.detach();
		if (staminaBlink) {
			DEBUG("Setup stamina blink");
			debuffThread::t2= std::jthread(debuffOps::staminaDebuffBlink);
			debuffThread::t2.detach();
		}
		DEBUG("threads detached");
	}
}


bool debuffHandler::AcquireHud() noexcept {
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

void debuffHandler::staminaBlinkOnce() noexcept {
	if (Hud) {
		Hud->InvokeNoReturn("_level0.HUDMovieBaseInstance.StartStaminaBlinking", nullptr, 0);
	}
}
