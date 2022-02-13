#include "debuffHandler.h"

#include "data.h"
#include "valhallaCombat.hpp"


void debuffHandler::update() {
	/*Iterate through the set of actors debuffing,
	Check the actors' stamina. If the actor's stamina has fully recovered, remove the actor from the set.
	Check the actor's UI counter, if the counter is less than 0, flash the actor's UI.*/
	auto it = actorsInDebuff.begin();
	while (it != actorsInDebuff.end()) {
		auto actor = it->first;
		if (!actor) {//actor no longer loaded
			DEBUG("Actor no longer loaded");
			it = actorsInDebuff.erase(it);//erase actor from debuff set.
		}
		else if (actor->GetActorValue(RE::ActorValue::kStamina) >= actor->GetPermanentActorValue(RE::ActorValue::kStamina)) { //actor loaded and recovered
			DEBUG("{}'s stamina has fully recovered.", actor->GetName());
			debuffHandler::stopStaminaDebuff(actor);
			DEBUG("erasing actor");
			it = actorsInDebuff.erase(it);
			if (it == actorsInDebuff.end()) {
				break;
			}
		}
		else if (settings::bUIAlert){ //flash the actor's meter
			if (it->second <= 0) {
				Utils::flashStaminaMeter(actor);
				it->second = 0.5;
			}
			else {
				it->second -= *Utils::g_deltaTimeRealTime;
				DEBUG(it->second);
			}
		}
		++it;
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
	actorsInDebuff.emplace(actor, 0); 
	if (settings::bUIAlert) {
		greyOutStaminaMeter(actor);
	}
}

void debuffHandler::stopStaminaDebuff(RE::Actor* actor) {
	DEBUG("Stopping stamina debuff for {}", actor->GetName());
	removeDebuffPerk(actor);
	if (settings::bUIAlert) {
		revertStaminaMeter(actor);
	}
}

#pragma region debuffPerks
void debuffHandler::addDebuffPerk(RE::Actor* a_actor) {
	Utils::safeApplyPerk(debuffPerk, a_actor);
}

void debuffHandler::removeDebuffPerk(RE::Actor* a_actor) {
	Utils::safeRemovePerk(debuffPerk, a_actor);
}
	

#pragma endregion
#pragma region staminaBarTweak

/*Turn flashColor red, turn barcolor and phantom color grey.*/
void debuffHandler::greyOutStaminaMeter(RE::Actor* actor) {
	ValhallaCombat::GetSingleton()->g_trueHUD->OverrideBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
	ValhallaCombat::GetSingleton()->g_trueHUD->OverrideBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::BarColor, 0x7d7e7d);
	ValhallaCombat::GetSingleton()->g_trueHUD->OverrideBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::PhantomColor, 0xb30d10);
}

void debuffHandler::revertStaminaMeter(RE::Actor* actor) {
	ValhallaCombat::GetSingleton()->g_trueHUD->RevertBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::FlashColor);
	ValhallaCombat::GetSingleton()->g_trueHUD->RevertBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::BarColor);
	ValhallaCombat::GetSingleton()->g_trueHUD->RevertBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::PhantomColor);
	DEBUG("{}s stamia meter reverted", actor->GetName());
}

#pragma endregion