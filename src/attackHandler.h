#pragma once
#include "staminaHandler.h"
namespace attackHandler
{
	/* whether player's stamina should be damaged at the moment of checking out.*/
	inline bool shouldDamageStamina = false; 
	/* when a player has already had a hitframe in player's current attacking animation. */
	inline bool attackFired = false;

	inline bool isLightAtk = true;

	inline bool nextIsLightAtk = true;

	inline bool nextIsBashing = false;

	inline float pcStaminaRate = 0;

	/* decide whether the hitframe counts as an attack.
	also checks whether the hitframe is fired when player is not attacking. If not,
	attack won't be registered.*/
	inline void registerAtk() {
		if (nextIsBashing) {
			DEBUG("bashing attack");
			nextIsBashing = false;
			return;
		}
		if (!attackFired) {
			DEBUG("first attack in the animation, registering attack");
			if (!nextIsLightAtk) {
				DEBUG("power attack registered");
				isLightAtk = false;
				nextIsLightAtk = true;
			}
			else {
				DEBUG("light attack registered");
			}
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (pc) {
				pc->SetActorValue(RE::ActorValue::KStaminaRate, 0);
				DEBUG("pc stamina rate set to 0");
			}
			shouldDamageStamina = true;
			attackFired = true;
		}
	}

	/* when player hits a living enemy, a hit is registered.*/
	inline void registerHit() {
		auto pc = RE::PlayerCharacter::GetSingleton();
		INFO("registering a successful hit");
		if (isLightAtk) {
			staminaHandler::staminaLightHit(pc);
		}
		else if (shouldDamageStamina) {
			staminaHandler::staminaHeavyHit(pc); //heavy attack's second hit should damage more stamina
		}
		shouldDamageStamina = false;
	}

	/* attack is checked out when the attack is either:
	* 1. interrupted -> when attackStop event is called
	* 2. finished -> when attackStop event is called
	* 3. transitioning into next attack - when Skysa_AttackWin is called.
	*/
	inline void checkout() {
		DEBUG("checking out attack");
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (pc) {
			if (shouldDamageStamina) {
				DEBUG("attack failed to hit anyone, damaging player stamina!");
				if (isLightAtk) {
					staminaHandler::staminaLightMiss(pc);
				}
				else {
					staminaHandler::staminaHeavyMiss(pc);
				}
			}
			DEBUG("resetting pc stamina rate to normal");
			pc->SetActorValue(RE::ActorValue::KStaminaRate, pcStaminaRate);
		}
		attackFired = false;
		shouldDamageStamina = false;
		isLightAtk = true;
	}

	/*proceed does two things:
	* 1. iff there's an attack not checked out, it'll be checked out.
	* 2. register a new attack.
	*/
	inline void proceed() {
		if (attackFired) {
			checkout();
		}
		registerAtk();
	}

	/*updates the original stamina rate of pc*/
	inline void updatePcStaminaRate() {
		pcStaminaRate = RE::PlayerCharacter::GetSingleton()->GetActorValue(RE::ActorValue::KStaminaRate);
		DEBUG("player's stamina rate has been updated to {}", pcStaminaRate);
	}


};

