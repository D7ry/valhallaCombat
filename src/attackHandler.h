#pragma once
#include "staminaHandler.h"
#include <boost/unordered_map.hpp>
#include <boost/container/set.hpp>
/*Handle attack registration, stamina consumption, and stamina reward.*/
class attackHandler
{

public:
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
	1. are attacking��swinging a weapon)
	2. have not hit any target.*/
	boost::unordered_map<RE::Actor*, ATTACKTYPE> attackerHeap;

	/* registers a light/heavy attack.
	    checks iff there is a previous attack not checked out, if so,
		check out the attack.*/
	void registerAtk(RE::Actor* actor) {
		checkout(actor);
		//TODO:fix bash implementation
		//DEBUG("registering attack for {}", actor->GetName());
		if (actor->currentProcess && actor->currentProcess->high) {
			auto attckData = actor->currentProcess->high->attackData;
			if (attckData) {
				if (attckData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
					//DEBUG("skipped shield bash");
					return; //bash attacks won't get registered
				}
				if (attckData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) {
					attackerHeap.emplace(actor, ATTACKTYPE::power);
					//DEBUG("registered power attack");
					return;
				}
				else {
					//DEBUG("registered light attack");
					attackerHeap.emplace(actor, ATTACKTYPE::light);
					//DEBUG("attack heap size: {}", attackerHeap.size());
				}
			}
		}
		else {
			//DEBUG("ERROR: actor has no high process");
		}
	}

	/*register a successful hit for an actor. Automatically detects if the hit is light/power.
	* The actor will no longer receive any stamina debuff for this attack.
	* If the actor has not received any stamina regen/any stamina damage
	* due to the attack, do stamina regen/stamina damage.
	@param actor: actor whose hit will be registered*/
	void registerHit(RE::Actor* actor) {
		//DEBUG("registering successful hit for {}", actor->GetName());
		ATTACKTYPE atkType;
		if (!getAtkType(actor, atkType)) {
			return;
		}
		//DEBUG("attack type is: ");
		//DEBUG(atkType);
		if (atkType == ATTACKTYPE::light) { //register light hit
			actorToRegenStamina = actor;
			staminaHandler::staminaLightHit(actor);
			actorToRegenStamina = nullptr;
		}
		else { //register power hit
			staminaHandler::staminaHeavyHit(actor);
		}
		Utils::flashStaminaMeter(actor);
		//DEBUG("erasing {} from attaker heap", actor->GetName());
		attackerHeap.erase(actor); //erase the actor from heap i.e. checking out the attack without damaging stamina.
		//DEBUG("current attacker heap size: {}", attackerHeap.size());
	}

	/* attack is checked out when the attack is either:
	* 1. interrupted -> when attackStop event is called
	* 2. finished -> when attackStop event is called
	* 3. transitioning into next attack - when Skysa_AttackWin is called.
	* @param actor: actor whose attack will be checked out.
	*/
	void checkout(RE::Actor* actor) {
		//DEBUG("checking out attack for {}", actor->GetName());
		//no need to perform a nullptr check, which is already done in animEventHandler.
		ATTACKTYPE atkType;
		if (!getAtkType(actor, atkType)) {
		//	DEBUG("{} has no attack to check out", actor->GetName());
			return;
		}
		if (atkType == ATTACKTYPE::light) {
			staminaHandler::staminaLightMiss(actor);
		}
		else {
			staminaHandler::staminaHeavyMiss(actor);
		}
		attackerHeap.erase(actor);
	}
private:
	/*try to obtain an actor's attack type stored in the map.
	iff the actor is not stored in the map, return false.
	@param actor: actor whose info to obtain.
	@param atkType: address to store attack type*/
	inline bool getAtkType(RE::Actor* actor, ATTACKTYPE& atkType) {
		DEBUG("Getting {}'s attack from attack heap", actor->GetName());
		boost::unordered::iterator_detail::iterator atkTypeItr = attackerHeap.find(actor); //check if the actor's attack is registered
		if (atkTypeItr == attackerHeap.end()) {
			//DEBUG("{} not found in attackState map", actor->GetName());
			return false;
		}
		else {
			//DEBUG("found attack type!");
		}
		atkType = atkTypeItr->second;
		return true;
	}

};

