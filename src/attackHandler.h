#pragma once
#include "avHandler.h"
namespace attackHandler
{
	/* whether player's stamina should be damaged at the moment of checking out.*/
	inline bool shouldDamageStamina = false; 

	/* when a player has already had a hitframe in player's current attacking animation. */
	inline bool attackFired = false;

	/*whether current attack is a light attack.*/
	inline bool isLightAtk = true;

	/* decide whether the hitframe counts as an attack.
	also checks whether the hitframe is fired when player is not attacking. If not,
	attack won't be registered.*/
	inline void registerAtk() {
		if (!attackFired) {
			INFO("first attack in the animation, registering attack");
			shouldDamageStamina = true;
			attackFired = true;
			INFO("fetching attack data");
			//FIXME:
			auto attackData = RE::PlayerCharacter::GetSingleton()->currentProcess->high->attackData;
			if (attackData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) {
				INFO("power attack registered");
				isLightAtk = false;
			} 
		}
	}

	/* when player hits a living enemy, a hit is registered.*/
	inline void registerHit() {
		DEBUG("registering a successful hit");
		shouldDamageStamina = false;
		DEBUG("pc gained stamina, and should not receive stamina damage for the following checkout!");
	}

	/* attack is checked out when the attack is either:
	* 1. interrupted -> when attackStop event is called
	* 2. finished -> when attackStop event is called
	* 3. transitioning into next attack - when Skysa_AttackWin is called.
	*/
	inline void checkout() {
		DEBUG("checking out attack");
		if (shouldDamageStamina) {
			auto pc = RE::PlayerCharacter::GetSingleton();
			DEBUG("attack failed to hit anyone, damaging playe stamina!");
			if (isLightAtk) {
				avHandler::staminaLightMiss(pc);
			}
			else {
				avHandler::staminaHeavyMiss(pc);
			}
		}
		attackFired = false;
		shouldDamageStamina = false;
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

