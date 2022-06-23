#pragma once
#include "staminaHandler.h"
#include "lib/robin_hood.h"
#include <mutex>
/*Handle attack registration, stamina consumption, and stamina reward.*/
class attackHandler
{

public:
	static inline std::mutex mtx_attackerHeap;
	static attackHandler* GetSingleton()
	{
		static attackHandler singleton;
		return  std::addressof(singleton);
	}

	RE::Actor* actorToRegenStamina;
	/*the type of attack.*/
	enum ATTACKTYPE
	{
		light = 0,
		power = 1,
	};
	/*mapping of all actors who:
	1. are attacking（swinging a weapon)
	2. have not hit any target.*/
	robin_hood::unordered_map<RE::Actor*, ATTACKTYPE> attackerHeap;
	/*registers a light/heavy attack.
	    checks iff there is a previous attack not checked out, if so,
		check out the attack.*/
	void registerAtk(RE::Actor* actor);

	/*register a successful hit for an actor. Automatically detects if the hit is light/power.
	* The actor will no longer receive any stamina debuff for this attack.
	* If the actor has not received any stamina regen/any stamina damage
	* due to the attack, do stamina regen/stamina damage.
	@param actor: actor whose hit will be registered*/
	void registerHit(RE::Actor* actor);

	/* attack is checked out when the attack is either:
	* 1. interrupted -> when attackStop event is called
	* 2. finished -> when attackStop event is called
	* 3. transitioning into next attack - when Skysa_AttackWin is called.
	* @param actor: actor whose attack will be checked out.
	*/
	void checkout(RE::Actor* actor);

	/*try to obtain an actor's attack type stored in the map.
iff the actor is not stored in the map, return false.
@param actor: actor whose info to obtain.
@param atkType: address to store attack type*/
	inline bool getAtkType(RE::Actor* actor, attackHandler::ATTACKTYPE& atkType) {
		//DEBUG("Getting {}'s attack from attack heap", actor->GetName());
		mtx_attackerHeap.lock();
		auto it = attackerHeap.find(actor); //check if the actor's attack is registered
		if (it == attackerHeap.end()) {
			mtx_attackerHeap.unlock();
			//DEBUG("{} not found in attackState map", actor->GetName());
			return false;
		}
		atkType = it->second;
		mtx_attackerHeap.unlock();
		return true;
	}
};

