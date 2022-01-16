#include "debuffHandler.h"
#include "dataHandler.h"
void debuffHandler::initStaminaDebuff() {
	DEBUG("initializing stamina debuff!");
	if (isPlayerExhausted) {
		DEBUG("player already exhausted!");
		return;
	}
	if (!RE::PlayerCharacter::GetSingleton()->IsInCombat() && !settings::NonCombatStaminaDebuff) {
		DEBUG("out of combat, no stamina debuff");
		return;
	}
	DEBUG("initializing player exhaustion!");
	this->isPlayerExhausted = true;
	addDebuffSpell();
	RE::PlayerCharacter::GetSingleton()->SetActorValue(RE::ActorValue::kAttackDamageMult, 0.5);
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
			break;
		}
		if (settings::staminaMeterBlink) {
			Utils::FlashHUDMenuMeter(RE::ActorValue::kStamina);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

