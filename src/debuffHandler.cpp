#include "debuffHandler.h"
debuffHandler::debuffHandler() {
	//AcquireHud();
	debuffPerk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(0x00000d63, "ValhallaCombat.esp");
	staminaLimitPerk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(0x00003877, "ValhallaCombat.esp");
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
		debuffThread::t1 = std::jthread(&debuffHandler::staminaDebuffCheck, this);
		debuffThread::t1.detach();
		//debuffThread::t2 = std::jthread(debug::printAttackState);
		//debuffThread::t2.detach();
		//if (staminaBlink) {
			//DEBUG("Setup stamina blink");
			//debuffThread::t2= std::jthread(debuffOps::staminaDebuffBlink);
			//debuffThread::t2.detach();
		//}
		DEBUG("thread detached");
	}
}


void debuffHandler::staminaDebuffCheck() {
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
			Utils::FlashHUDMenuMeter(RE::ActorValue::kStamina);
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}
}


/*bool debuffHandler::AcquireHud() noexcept {
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
}*/
