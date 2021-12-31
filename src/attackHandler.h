#pragma once
#include "avHandler.h"
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

};

