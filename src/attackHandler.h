#pragma once
#include "avHandler.h"
namespace attackHandler
{
	/* whether player's stamina should be damaged at the moment of checking out.*/
	inline bool shouldDamageStamina = false; 

	/* when a player has already had a hitframe in player's current attacking animation. */
	inline bool attackFired = false;

	inline void alterPcStamina(bool damage) {
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (pc) {
			if (damage) {
				avHandler::damageStamina(pc);
			}
			else {
				avHandler::restoreStamina(pc);
			}
		}
	}
	


	/* decide whether the hitframe counts as an attack.
	also checks whether the hitframe is fired when player is not attacking. If not,
	attack won't be registered.*/
	//TODO:iff player is not attacking, the attack does not count.
	inline void registerAtk() {
		if (!attackFired) {
			DEBUG("first attack in the animation, registering attack");
			shouldDamageStamina = true;
			attackFired = true;
		}
	}

	/* when player hits a living enemy, a hit is registered.*/
	inline void registerHit() {
		DEBUG("registering a successful hit");
		shouldDamageStamina = false;
		alterPcStamina(false);
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
			DEBUG("attack failed to hit anyone, damaging playe stamina!");
			alterPcStamina(true);
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

