#include "include/debuffHandler.h"
#include "include/data.h"
#include "include/settings.h"
#include "include/offsets.h"
#include "valhallaCombat.hpp"
//TODO:make sure to resume the debuff state on reloading the actor i.e. the actor has debuff perk, but it's no longer in the debuff map, so they have to be put back onto the map.
/*Called every frame.
Iterate through the set of actors debuffing.
Check the actors' stamina. If the actor's stamina has fully recovered, remove the actor from the set.
Check the actor's UI counter, if the counter is less than 0, flash the actor's UI.*/
void debuffHandler::update() {
	mtx_actorInDebuff.lock();
	auto it = actorInDebuff.begin();
	while (it != actorInDebuff.end()) {
		auto actor = *it;
		if (!actor || !actor->currentProcess || !actor->currentProcess->InHighProcess()) {//actor no longer loaded
			//DEBUG("Actor no longer loaded");
			it = actorInDebuff.erase(it);//erase actor from debuff set.
			if (actorInDebuff.size() == 0) {
				ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::debuffHandler);
			}
			continue;
		}
		if (actor->GetActorValue(RE::ActorValue::kStamina) >= 
			actor->GetPermanentActorValue(RE::ActorValue::kStamina) 
			+ actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, RE::ActorValue::kStamina)) { //offset max stamina based on modifier
			//DEBUG("{}'s stamina has fully recovered.", actor->GetName());
			stopStaminaDebuff(actor);
			//DEBUG("erasing actor");
			it = actorInDebuff.erase(it);
			if (actorInDebuff.size() == 0) {
				ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::debuffHandler);
			}
			continue;
		}
		++it;
	}

	mtx_actorInDebuff.unlock();
	
}

/*Initialize a stmaina debuff for actor, giving them exhaustion perk, and putting them into the debuff map.
If the actor is already in the debuff map(i.e. they are already experiencing debuff), do nothing.
@param actor actor who will receive debuff.*/
void debuffHandler::initStaminaDebuff(RE::Actor* actor) {
	mtx_actorInDebuff.lock();
	if (actorInDebuff.contains(actor)) {
		//DEBUG("{} is already in debuff", actor->GetName());
		mtx_actorInDebuff.unlock();
		return;
	}
	actorInDebuff.insert(actor);
	mtx_actorInDebuff.unlock();
	if (actor->IsPlayerRef()) {
		addDebuffPerk(actor);
	}
	if (settings::bUIAlert && settings::TrueHudAPI_Obtained) {
		greyOutStaminaMeter(actor);
		if (actor->IsPlayerRef()) {
			std::jthread t(async_pcStaminaMeterFlash);
			t.detach();
			async_pcStaminaMeterFlash_b = true;
		}
	}
	ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::HANDLER::debuffHandler);
}

/*Stamina the actor's stamina debuff, remove their debuff perk, and revert their UI meter.
@param actor actor whose stamina debuff will stop.*/
void debuffHandler::stopStaminaDebuff(RE::Actor* actor) {
	//DEBUG("Stopping stamina debuff for {}", actor->GetName());
	removeDebuffPerk(actor);
	if (settings::bUIAlert && settings::TrueHudAPI_Obtained) {
		if (actor->IsPlayerRef()) {
			async_pcStaminaMeterFlash_b = false;
		}
		revertStaminaMeter(actor);
	}
}

void debuffHandler::quickStopStaminaDebuff(RE::Actor* actor) {
	mtx_actorInDebuff.lock();
	if (!actorInDebuff.contains(actor)) {
		mtx_actorInDebuff.unlock();
		return;
	}
	actorInDebuff.erase(actor);
	if (actorInDebuff.size() == 0) {
		ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::debuffHandler);
	}
	mtx_actorInDebuff.unlock();
	stopStaminaDebuff(actor);
}

/*Attach stamina debuff perk to actor.
@param a_actor actor who will receive the debuff perk.*/
void debuffHandler::addDebuffPerk(RE::Actor* a_actor) {
	Utils::safeApplyPerk(data::debuffPerk, a_actor);
}

/*Remove stamina debuff perk from actor.
@param a_actor actor who will gets the perk removed.*/
void debuffHandler::removeDebuffPerk(RE::Actor* a_actor) {
	Utils::safeRemovePerk(data::debuffPerk, a_actor);
}
	

bool debuffHandler::isInDebuff(RE::Actor* a_actor) {
	mtx_actorInDebuff.lock();
	if (actorInDebuff.contains(a_actor)) {
		mtx_actorInDebuff.unlock();
		return true;
	}
	else {
		mtx_actorInDebuff.unlock();
		return false;
	}
} 

#pragma region staminaBarTweak

/*Turn flashColor red, turn barcolor and phantom color grey.*/
void debuffHandler::greyOutStaminaMeter(RE::Actor* actor) {
	auto ersh = ValhallaCombat::GetSingleton()->ersh;
	ersh->OverrideBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
	ersh->OverrideBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::BarColor, 0x7d7e7d);
	ersh->OverrideBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::PhantomColor, 0xb30d10);
}

void debuffHandler::revertStaminaMeter(RE::Actor* actor) {
	auto ersh = ValhallaCombat::GetSingleton()->ersh;
	ersh->RevertBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::FlashColor);
	ersh->RevertBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::BarColor);
	ersh->RevertBarColor(actor->GetHandle(), RE::ActorValue::kStamina, TRUEHUD_API::BarColorType::PhantomColor);
	//DEBUG("{}s stamia meter reverted", actor->GetName());
}

void debuffHandler::async_pcStaminaMeterFlash() {
	while (true) {
		if (async_pcStaminaMeterFlash_b) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			ValhallaCombat::GetSingleton()->ersh->FlashActorValue(RE::PlayerCharacter::GetSingleton()->GetHandle(), RE::ActorValue::kStamina, true);
		}
		else {
			return;
		}
	}
}

#pragma endregion