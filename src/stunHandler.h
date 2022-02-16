#pragma once
#include "data.h"

/*Handling enemy stun value.*/
class stunHandler {
public:
	static stunHandler* GetSingleton()
	{
		static stunHandler singleton;
		return  std::addressof(singleton);
	}

	/*Called once per frame to check if the actors are either unloaded or exhausted.*/
	void update();


	/*Calculate stun damage onto an actor and apply it, after, check if the actor is ready for execution.
	*If so, let execution handler handle the rest.
	@param actor actor whose stun will be damaged.
	@param baseDamage base damage to actor's stun.
	@param weapon weapon used to damage actor's stun.
	*/
	void processStunDamage(RE::Actor* actor, float baseDamage, RE::TESObjectWEAP weapon, RE::AttackData::AttackFlag, ) {

	}
	/*Returns the current stun value of this actor.*/
	void getStun(RE::Actor* actor);
	/*Returns the maximum stun value of this actor.*/
	void getMaxStun(RE::Actor* actor);

private:
	/*Calculate this actor's maximum stun value based on various parameters.
	* Current algorithm: Health + Stamina / 2
	@param actor: actor whose still value will be calculated.
	@return the actor's maximum stun value.*/
	inline float calculateStun(RE::Actor* actor);

	/*Start tracking this Actor's stun.
	@param actor: actor whose stun will be tracked.*/
	inline void trackStun(RE::Actor* actor);
	/*Stop tracking this Actor's stun.
	@param actor: actor whose stun will no longer be tracked.*/
	inline void untrackStun(RE::Actor* actor);

	/*Mapping of actors whose stun values are tracked => a pair storing [0]Actor's maximum stun value, [1] Actor's current stun value.*/
	boost::unordered_map <RE::Actor*, std::pair<float, float>> actorStunMap;

	/*Mapping of actors whose stun values are less than 0 =>their meter blinking timer.*/
	boost::unordered_map <RE::Actor*, float> actorsStunned;

	/*Damage this actor's stun. If the actor does not exist on the stunmap, track their stun first.
	@param actor: actor whose stun will be damaged.
	@param damage: stun damage applied onto this actor.*/
	void damageStun(RE::Actor* actor, float damage);




	/*currently stun meter uses stamina*/

	/*applies stun damage to the actor
	@param baseDamage the total health damage applied onto actor, used as a base for calculation
	@param a_actor actor whose stun meter is to be damaged.
	@param isPowerAtk whether the attack is a power attack. Be ware, power attack itself already has more damage.*/
	/*
	static bool stunActor(float baseDamage, RE::ActorPtr a_actor, RE::TESObjectWEAP* weapon, bool isPowerAtk) {
		baseDamage *= settings::stunBaseMult;
		if (isPowerAtk) {
			baseDamage *= settings::stunHvyMult;
		}
		switch (weapon->GetWeaponType())
		{
		case RE::WEAPON_TYPE::kBow: baseDamage *= settings::stunBowMult; break;
		case RE::WEAPON_TYPE::kCrossbow: baseDamage *= settings::stunCrossBowMult; break;
		case RE::WEAPON_TYPE::kHandToHandMelee: baseDamage *= settings::stunUnarmedMult; break;
		case RE::WEAPON_TYPE::kOneHandDagger: baseDamage *= settings::stunDaggerMult; break;
		case RE::WEAPON_TYPE::kOneHandSword: baseDamage *= settings::stunSwordMult; break;
		case RE::WEAPON_TYPE::kOneHandAxe: baseDamage *= settings::stunAxeMult; break;
		case RE::WEAPON_TYPE::kOneHandMace: baseDamage *= settings::stunMaceMult; break;
		case RE::WEAPON_TYPE::kTwoHandAxe: baseDamage *= settings::stun2hwMult; break;
		case RE::WEAPON_TYPE::kTwoHandSword: baseDamage *= settings::stunGreatSwordMult; break;
		default: break;
		}
		Utils::damageav(a_actor.get(), RE::ActorValue::kStamina, baseDamage);
		return true;
	}
	static bool isStunMeterEmpty(RE::ActorPtr a_actor) {
		return a_actor->GetActorValue(RE::ActorValue::kStamina) <= 0;
	}

	static void execute(RE::ActorPtr aggressor, RE::ActorPtr target) {
		DEBUG("{} has executed {}!", aggressor->GetName(), target->GetName());
		//target->push
		//Utils::push::pushActor(aggressor, target, std::to_string(3));
		reFillStun(target);
	}

	/*refills actor's stun meter
	static void reFillStun(RE::ActorPtr a_actor) {
		Utils::restoreav(a_actor.get(), RE::ActorValue::kStamina, a_actor->GetPermanentActorValue(RE::ActorValue::kStamina));
		DEBUG("{}'s stun refilled.", a_actor->GetName());
	}*/
}; 

