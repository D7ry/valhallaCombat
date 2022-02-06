#pragma once
#include "staminaHandler.h"

class attackHandler
{

public:
	enum ATTACKTYPE 
	{
		light = 0,
		power
	};
	static inline enum ATTACKTYPE attackType = ATTACKTYPE::light;

	static inline bool shouldDamageStamina = false;

	static inline bool attackFired = false;

	static inline bool nextIsLightAtk = true;
	static inline bool nextIsBashAtk = false;

	static inline bool meleeHitRegen = false;

	static inline bool attackPreSwing = true; //whether the player is in "pre-swing" stage


	/* registers a light/heavy attack.
	    checks iff there is a previous attack not checked out, if so,
		check out the attack.*/
	static void registerAtk() {
		if (attackFired) {
			checkout();
		}
		if (nextIsBashAtk) {
			DEBUG("bash attack won't get registered");
			nextIsBashAtk = false;
			return;
		}
		DEBUG("registering attack");
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (!pc || !pc->currentProcess) {
			ERROR("PC PROCESS NOT FOUND!");
			return;
		}
		auto highProcess = pc->currentProcess->high;
		while (!highProcess) {
			DEBUG("high process not found, trying again");
			highProcess = pc->currentProcess->high;
		}
		auto atkData = highProcess->attackData;
		if (!atkData) {
			DEBUG("atk data not found, trying again!");
			atkData = highProcess->attackData;
		}
		if (atkData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) {
			DEBUG("attack data detected power attack!");
			nextIsLightAtk = false;
		}
		if (nextIsLightAtk) {
			DEBUG("registered light attack!");
			attackType = ATTACKTYPE::light;
		}
		else {
			DEBUG("registered power attack!");
			attackType = ATTACKTYPE::power;
			nextIsLightAtk = true;
		}
		shouldDamageStamina = true;
		attackFired = true;
	}

	/* when player hits a living enemy, a hit is registered.*/
	static void registerHit() {
		auto pc = RE::PlayerCharacter::GetSingleton();
		INFO("registering a successful hit");
		if (attackType == ATTACKTYPE::light) {
			meleeHitRegen = true;
			staminaHandler::staminaLightHit(pc);
			meleeHitRegen = false;
		}
		else if (shouldDamageStamina) {
			staminaHandler::staminaHeavyHit(pc); //heavy attack's second hit should not damage more stamina
		}
		shouldDamageStamina = false;
	}

	/* attack is checked out when the attack is either:
	* 1. interrupted -> when attackStop event is called
	* 2. finished -> when attackStop event is called
	* 3. transitioning into next attack - when Skysa_AttackWin is called.
	*/
	static void checkout() {
		DEBUG("checking out attack");
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (pc) {
			if (shouldDamageStamina) {
				DEBUG("attack failed to hit anyone, damaging player stamina!");
				if (attackType == ATTACKTYPE::light) {
					staminaHandler::staminaLightMiss(pc);
				}
				else {
					staminaHandler::staminaHeavyMiss(pc);
				}
			}
		}
		attackFired = false;
		shouldDamageStamina = false;
		attackPreSwing = true;
	}


};

