#include "debuffHandler.h"
#include "data.h"
/*Initialize a stmaina debuff for actor, giving them exhaustion perk, and putting them into the debuff map.
If the actor is already in the debuff map(i.e. they are already experiencing debuff), do nothing.
@param actor actor who will receive debuff.*/
void debuffHandler::initStaminaDebuff(RE::Actor* actor) {
	DEBUG("Init stamina debuff for {}", actor->GetName());
	if (!actor->IsInCombat() && !settings::bNonCombatStaminaDebuff) {
		DEBUG("{} is not in combat, no stamina debuff will be applied.", actor->GetName());
		return;
	}
	if (actorsInDebuff.contains(actor)) {
		DEBUG("{} is already in debuff", actor->GetName());
		return;
	}
	actorsInDebuff.emplace(actor); 
	//addDebuffSpell();
	//TODO:add back debuff perk
	//TODO:use truehud API to grey out corresponding meters.
	if (g_trueHUD->OverridePlayerWidgetBarColor(SKSE::GetPluginHandle(), TRUEHUD_API::PlayerWidgetBarType::StaminaBar, TRUEHUD_API::BarColorType::BackgroundColor, 0x111111) == TRUEHUD_API::APIResult::OK) {
		DEBUG("greyed out player stmaina background!");
	}
	else {
		DEBUG("wtf ersh");
	}
	if (g_trueHUD->OverridePlayerWidgetBarColor(SKSE::GetPluginHandle(), TRUEHUD_API::PlayerWidgetBarType::StaminaBar, TRUEHUD_API::BarColorType::BarColor, 0x333333) == TRUEHUD_API::APIResult::OK) {
		DEBUG("greyed out player stmaina bar!");
	}
	else {
		DEBUG("wtf ersh");
	}
}

void debuffHandler::stopStaminaDebuff(RE::Actor* actor) {
	DEBUG("Stopping stamina debuff for {}", actor->GetName());
	//rmDebuffSpell();
	//TODO:truehud STuff
	if (g_trueHUD->RevertPlayerWidgetBarColor(SKSE::GetPluginHandle(), TRUEHUD_API::PlayerWidgetBarType::StaminaBar, TRUEHUD_API::BarColorType::BackgroundColor) == TRUEHUD_API::APIResult::OK) {
		DEBUG("reverted player stamina background!");
	}
	else {
		DEBUG("wtf ersh");
	}
	if (g_trueHUD->RevertPlayerWidgetBarColor(SKSE::GetPluginHandle(), TRUEHUD_API::PlayerWidgetBarType::StaminaBar, TRUEHUD_API::BarColorType::BarColor) == TRUEHUD_API::APIResult::OK) {
		DEBUG("reverted player stamina bar!");
	}
	else {
		DEBUG("wtf ersh");
	}
}


