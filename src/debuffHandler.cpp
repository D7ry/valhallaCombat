#include "debuffHandler.h"
#include "data.h"
void debuffHandler::initStaminaDebuff() {
	DEBUG("initializing stamina debuff!");
	if (isPlayerExhausted) {
		DEBUG("player already exhausted!");
		return;
	}
	if (!RE::PlayerCharacter::GetSingleton()->IsInCombat() && !settings::bNonCombatStaminaDebuff) {
		DEBUG("out of combat, no stamina debuff");
		return;
	}
	DEBUG("initializing player exhaustion!");
	this->isPlayerExhausted = true;
	//addDebuffSpell();
	//TODO:add back debuff perk
	if (g_trueHUD->RequestPlayerWidgetBarColorsControl(SKSE::GetPluginHandle()) == TRUEHUD_API::APIResult::OK) {
		DEBUG("greying out stamina bar!");
		//g_trueHUD->OverridePlayerWidgetBarColor(SKSE::GetPluginHandle(), TRUEHUD_API::PlayerWidgetBarType::StaminaBar, TRUEHUD_API::PlayerWidgetBarColorType::BarColor, 0x333333);
		//g_trueHUD->OverridePlayerWidgetBarColor(SKSE::GetPluginHandle(), TRUEHUD_API::PlayerWidgetBarType::StaminaBar, TRUEHUD_API::PlayerWidgetBarColorType::BackgroundColor, 0x111111);
	}
	else {
		DEBUG("true hud doesn't lemme control!");
	}
	debuffThread::t1 = std::jthread(&debuffHandler::staminaDebuffOp, this);
	debuffThread::t1.detach();
}


void debuffHandler::staminaDebuffOp() {
	auto _debuffHandler = debuffHandler::GetSingleton();
	auto pc = RE::PlayerCharacter::GetSingleton();
	while (_debuffHandler->isPlayerExhausted && pc) {
		if (pc->GetActorValue(RE::ActorValue::kStamina) >= pc->GetPermanentActorValue(RE::ActorValue::kStamina)) {
			//_debuffHandler->rmDebuffPerk();
			_debuffHandler->isPlayerExhausted = false;
			rmDebuffSpell();
			if (g_trueHUD->RequestPlayerWidgetBarColorsControl(SKSE::GetPluginHandle()) == TRUEHUD_API::APIResult::OK) {
				DEBUG("reverting stamina bar color!");
				//g_trueHUD->RevertPlayerWidgetBarColor(SKSE::GetPluginHandle(), TRUEHUD_API::PlayerWidgetBarType::StaminaBar, TRUEHUD_API::PlayerWidgetBarColorType::BarColor);
				//g_trueHUD->RevertPlayerWidgetBarColor(SKSE::GetPluginHandle(), TRUEHUD_API::PlayerWidgetBarType::StaminaBar, TRUEHUD_API::PlayerWidgetBarColorType::BackgroundColor);
			}
			break;
		}
		if (settings::bUIAlert) {
			Utils::FlashHUDMenuMeter(RE::ActorValue::kStamina);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

