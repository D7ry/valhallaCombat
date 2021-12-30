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

	/* decide whether the hitframe counts as an attack.
	also checks whether the hitframe is fired when player is not attacking. If not,
	attack won't be registered.*/
	inline void registerAtk() {
		if (!attackFired) {
			DEBUG("first attack in the animation, registering attack");
			DEBUG("Fetching attack data");
			auto pc = RE::PlayerCharacter::GetSingleton();
			RE::NiPointer<RE::BGSAttackData> attackData;
			if (!pc->currentProcess || !pc->currentProcess->high || !pc->currentProcess->high->attackData) {
				DEBUG("failed to fetch attack data! detaching fetcher thread");
			} else {
				attackData = RE::PlayerCharacter::GetSingleton()->currentProcess->high->attackData;
			}
			if (!attackData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
				if (attackData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) {
					INFO("power attack registered");
					isLightAtk = false;
				}
				else {
					DEBUG("light attack registered");
				}
				shouldDamageStamina = true;
				attackFired = true;
			}
			else {
				DEBUG("bash attack does not count");
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
			DEBUG("attack failed to hit anyone, damaging playe stamina!");
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (pc) {
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

