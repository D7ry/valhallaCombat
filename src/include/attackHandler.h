#pragma once
#include "staminaHandler.h"
#include <mutex>
#include <unordered_set>
/*Handle attack registration, stamina consumption, and stamina reward.*/
class attackHandler
{
private:
	std::mutex mtx;
public:
	static attackHandler* GetSingleton()
	{
		static attackHandler singleton;
		return  std::addressof(singleton);
	}

	RE::Actor* actorToRegenStamina;

	/*mapping of all actors who:
	1. are attacking（swinging a weapon)
	2. have not hit any target.*/
	std::unordered_set<RE::Actor*> cachedAttackers;
	/*registers a light/heavy attack.
	    checks iff there is a previous attack not checked out, if so,
		check out the attack.*/
	void cacheAttack(RE::Actor* actor);

	/*register a successful light hit for an actor. 
	* The actor will no longer receive any stamina debuff for this attack.
	* If the actor has not received any stamina regen/any stamina damage
	* due to the attack, do stamina regen/stamina damage.
	@param actor: actor whose hit will be registered*/
	void OnLightHit(RE::Actor* actor);

	/* attack is checked out when the attack is either:
	* 1. interrupted -> when attackStop event is called
	* 2. finished -> when attackStop event is called
	* 3. transitioning into next attack - when Skysa_AttackWin is called.
	* @param actor: actor whose attack will be checked out.
	*/
	void checkout(RE::Actor* actor);

};

