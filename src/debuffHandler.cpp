#include "debuffHandler.h"
#include "data.h"

void debuffHandler::update() {
	/*Iterate through the set of actors debuffing,
	checking their stamina.*/
	auto it = actorsInDebuff.begin();
	while (it != actorsInDebuff.end()) {
		if (!(*it) //actor is no longer loaded.
			|| ((*it)->GetActorValue(RE::ActorValue::kStamina) >= (*it)->GetPermanentActorValue(RE::ActorValue::kStamina))) {
			DEBUG("{}'s stamina has fully recovered, or they're no longer loaded", (*it)->GetName());
			debuffHandler::stopStaminaDebuff((*it));
			it = actorsInDebuff.erase(it); //erase actor from debuff set.
			DEBUG("actor erased from debuff set");
		}
		else {
			++it;
		}
	}
	/*check iff player is in debuff state, if so, flash the stamina meter*/
	if (playerInDebuff && settings::bUIAlert) {
		if (playerMeterFlashTimer <= 0) {
			Utils::FlashHUDMenuMeter(RE::ActorValue::kStamina);
			playerMeterFlashTimer = 0.5;
		}
		else {
			playerMeterFlashTimer -= *Utils::g_deltaTimeRealTime;
		}
	}
	
}

/*Initialize a stmaina debuff for actor, giving them exhaustion perk, and putting them into the debuff map.
If the actor is already in the debuff map(i.e. they are already experiencing debuff), do nothing.
@param actor actor who will receive debuff.*/
void debuffHandler::initStaminaDebuff(RE::Actor* actor) {
	//DEBUG("Init stamina debuff for {}", actor->GetName());
	if (!actor->IsInCombat() && !settings::bNonCombatStaminaDebuff) {
		DEBUG("{} is not in combat, no stamina debuff will be applied.", actor->GetName());
		return;
	}
	if (actorsInDebuff.find(actor) != actorsInDebuff.end()) {
		DEBUG("{} is already in debuff", actor->GetName());
		return;
	}
	addDebuffPerk(actor);
	actorsInDebuff.emplace(actor); 
	if (settings::bUIAlert) {
		greyOutStaminaMeter(actor);
	}
	if (actor->IsPlayerRef()) {
		playerInDebuff = true;
	}
}

void debuffHandler::stopStaminaDebuff(RE::Actor* actor) {
	DEBUG("Stopping stamina debuff for {}", actor->GetName());
	//rmDebuffSpell();
	if (actor) {
		removeDebuffPerk(actor);
		if (actor->IsPlayerRef()) {
			playerInDebuff = false;
		}
		if (settings::bUIAlert) {
			revertStaminaMeter(actor);
		}
	}
}
void debuffHandler::greyOutStaminaMeter(RE::Actor* actor) {
	g_trueHUD->OverrideBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
	g_trueHUD->OverrideBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::BarColor, 0x7d7e7d);
	g_trueHUD->OverrideBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::PhantomColor, 0xb30d10);
}

void debuffHandler::revertStaminaMeter(RE::Actor* actor) {
	g_trueHUD->RevertBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::FlashColor);
	g_trueHUD->RevertBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::BarColor);
	g_trueHUD->RevertBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::PhantomColor);
}