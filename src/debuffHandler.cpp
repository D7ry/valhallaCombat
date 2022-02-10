#include "debuffHandler.h"
#include "data.h"
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
	actorsInDebuff.emplace(actor); 
	addDebuffSpell();
	//TODO:debuffPerk
	greyOutStaminaMeter(actor);
	if (actor->IsPlayerRef()) {
		playerInDebuff = true;
	}
}

void debuffHandler::stopStaminaDebuff(RE::Actor* actor) {
	DEBUG("Stopping stamina debuff for {}", actor->GetName());
	//rmDebuffSpell();
	if (actor) {
		revertStaminaMeter(actor);
		if (actor->IsPlayerRef()) {
			playerInDebuff = false;
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