#pragma once
#include "staminaHandler.h"
#include <boost/unordered_map.hpp>
#include <boost/container/set.hpp>
class attackHandler
{

public:
	static RE::Actor* actorToRegenStamina;
	/*the type of attack.*/
	enum ATTACKTYPE
	{
		light = 0,
		power = 1,
	};
	/*mapping of all actors who are attacking i.e. playing the attacking animation, to
	their attacking type.*/
	static inline boost::unordered_map<RE::Actor*, ATTACKTYPE> activeAttackers;
	/*set of all actors who are still attacking but have already hit someone.
	This is to prevent actor from regenerating stamina twice in a single hit.*/
	static inline boost::container::set<RE::Actor*> actor_who_hit;
	
	/* registers a light/heavy attack.
	    checks iff there is a previous attack not checked out, if so,
		check out the attack.*/
	static void registerAtk(RE::Actor* actor) {
		checkout(actor);
		//TODO:fix bash implementation
		/*if (nextIsBashAtk) {
			DEBUG("bash attack won't get registered");
			nextIsBashAtk = false;
			return;
		}*/
		DEBUG("registering attack for {}", actor->GetName());
		if (actor->currentProcess && actor->currentProcess->high) {
			auto attckData = actor->currentProcess->high->attackData;
			if (attckData) {
				if (attckData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
					DEBUG("skipped shield bash");
					return; //bash attacks won't get registered
				}
				if (attckData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) {
					activeAttackers.emplace(actor, ATTACKTYPE::power);
					DEBUG("registered power attack");
					return;
				}
			}
		}
		else {
			activeAttackers.emplace(actor, ATTACKTYPE::light);
		}
	}

	/*register a successful hit for an actor. 
	* The actor will no longer receive any stamina debuff for this attack.
	* if the actor has not received any stamina regen/any stamina damage
	* due to the attack, do stamina regen/stamina damage.
	@param actor actor whose hit will be registered*/
	static void registerHit(RE::Actor* actor) {
		DEBUG("registering successful hit for {}", actor->GetName());
		if (actor_who_hit.contains(actor)) {
			/*This happens when the actor has already hit something in the current attack.
			in this case we don't want to regenerate/damage their stamina twice.*/
			DEBUG("{} has already hit!", actor);
			return;
		}
		ATTACKTYPE atkType;
		if (getAtkType(actor, atkType)) {
			return;
		}
		if (atkType == ATTACKTYPE::light) { //register light hit
			actorToRegenStamina = actor;
			staminaHandler::staminaLightHit(actor);
			actorToRegenStamina = nullptr;
		}
		else { //register power hit
			staminaHandler::staminaHeavyHit(actor);
		}
		actor_who_hit.emplace(actor); //put it in the set so when the actor hits again, the function doesn't run.
	}

	/* attack is checked out when the attack is either:
	* 1. interrupted -> when attackStop event is called
	* 2. finished -> when attackStop event is called
	* 3. transitioning into next attack - when Skysa_AttackWin is called.
	* @param actor actor whose attack will be checked out.
	*/
	static void checkout(RE::Actor* actor) {
		DEBUG("checking out attack for {}", actor->GetName());
		//no need to perform a nullptr check, which is already done in animEventHandler.
		if (actor_who_hit.contains(actor)) {
			actor_who_hit.erase(actor);
		}
		ATTACKTYPE atkType;
		if (!getAtkType(actor, atkType)) {
			return;
		}
		if (atkType == ATTACKTYPE::light) {
			staminaHandler::staminaLightMiss(actor);
		}
		else {
			staminaHandler::staminaHeavyMiss(actor);
		}
		activeAttackers.erase(actor);
	}
private:
	/*try to obtain an actor's attack type stored in the map.
	iff the actor is not stored in the map, return false.
	@param actor actor whose info to obtain.
	@param atkType address to store attack type*/
	inline static bool getAtkType(RE::Actor* actor, ATTACKTYPE& atkType) {
		boost::unordered::iterator_detail::iterator atkTypeItr = activeAttackers.find(actor); //check if the actor's attack is registered
		if (atkTypeItr == activeAttackers.end()) {
			DEBUG("Error: {} not found in attackState map", actor->GetName());
			return false;
		}
		atkType = atkTypeItr->second;
		return true;
	}

};

