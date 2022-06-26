#include "include/debuffHandler.h"
#include "include/data.h"
#include "include/settings.h"
#include "include/offsets.h"
#include "include/Utils.h"
#include "valhallaCombat.hpp"
using uniqueLocker = std::unique_lock<std::shared_mutex>;
using sharedLocker = std::shared_lock<std::shared_mutex>;
/*Called every frame.
Iterate through the set of actors debuffing.
Check the actors' stamina. If the actor's stamina has fully recovered, remove the actor from the set.
Check the actor's UI counter, if the counter is less than 0, flash the actor's UI.*/
void debuffHandler::update() {
	uniqueLocker lock(mtx_actorInDebuff);
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
}

/*Initialize a stmaina debuff for actor, giving them exhaustion perk, and putting them into the debuff map.
If the actor is already in the debuff map(i.e. they are already experiencing debuff), do nothing.
@param actor actor who will receive debuff.*/
void debuffHandler::initStaminaDebuff(RE::Actor* a_actor) {	
	if (!settings::bStaminaDebuffToggle) {
		return;
	}
	{
		uniqueLocker lock(mtx_actorInDebuff);
		if (actorInDebuff.contains(a_actor)) {
			return;
		}
		actorInDebuff.insert(a_actor);	
	}


	if (a_actor->IsPlayerRef()) {
		addDebuffPerk(a_actor);
	}
	if (settings::bUIAlert) {
		TrueHUDUtils::greyoutAvMeter(a_actor, RE::ActorValue::kStamina);
		if (a_actor->IsPlayerRef()) {
			std::jthread t(async_pcStaminaMeterFlash);
			t.detach();
			async_pcStaminaMeterFlash_b = true;
		}
	}
	ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::HANDLER::debuffHandler);
}

/*Stamina the actor's stamina debuff, remove their debuff perk, and revert their UI meter.
@param actor actor whose stamina debuff will stop.*/
void debuffHandler::stopStaminaDebuff(RE::Actor* a_actor) {
	//DEBUG("Stopping stamina debuff for {}", actor->GetName());
	removeDebuffPerk(a_actor);
	if (settings::bUIAlert) {
		if (a_actor->IsPlayerRef()) {
			async_pcStaminaMeterFlash_b = false;
		}
		TrueHUDUtils::revertAvMeter(a_actor, RE::ActorValue::kStamina);
	}
}

void debuffHandler::quickStopStaminaDebuff(RE::Actor* a_actor) {
	{
		uniqueLocker lock(mtx_actorInDebuff);
		if (!actorInDebuff.contains(a_actor)) {
			return;
		}
		actorInDebuff.erase(a_actor);
		stopStaminaDebuff(a_actor);
		if (actorInDebuff.size() == 0) {
			ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::debuffHandler);
		}
	}
	
}

/*Attach stamina debuff perk to actor.
@param a_actor actor who will receive the debuff perk.*/
void debuffHandler::addDebuffPerk(RE::Actor* a_actor) {
	inlineUtils::safeApplyPerk(data::debuffPerk, a_actor);
}

/*Remove stamina debuff perk from actor.
@param a_actor actor who will gets the perk removed.*/
void debuffHandler::removeDebuffPerk(RE::Actor* a_actor) {
	inlineUtils::safeRemovePerk(data::debuffPerk, a_actor);
}
	

bool debuffHandler::isInDebuff(RE::Actor* a_actor) {
	sharedLocker lock(mtx_actorInDebuff);
	return actorInDebuff.contains(a_actor);
} 


void debuffHandler::async_pcStaminaMeterFlash() {
	while (true) {
		if (async_pcStaminaMeterFlash_b) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			TrueHUDUtils::flashActorValue(RE::PlayerCharacter::GetSingleton(), RE::ActorValue::kStamina);
		}
		else {
			return;
		}
	}
}
