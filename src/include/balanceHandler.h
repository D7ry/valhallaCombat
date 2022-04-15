#include "include/lib/robin_hood.h"
#include <mutex>
#pragma once
/*Keep track of all actor's stagger resistence i.e. "Balance", and deligate stagger events.*/
class balanceHandler {
private:
	/*Mapping of actors whose balance values are tracked => a pair storing [0]Actor's maximum balance value, [1] Actor's current balance value.*/
	robin_hood::unordered_map<RE::Actor*, std::pair<float, float>> actorBalanceMap;
	/*Set of all actors with broken balance.*/
	robin_hood::unordered_set<RE::Actor*> balanceBrokenActors;
	static inline std::mutex mtx_actorBalanceMap;
	static inline std::mutex mtx_balanceBrokenActors;
public:
	static balanceHandler* GetSingleton()
	{
		static balanceHandler singleton;
		return  std::addressof(singleton);
	}
	enum DMGSOURCE
	{
		lightAttack,
		powerAttack,
		bash,
		powerBash,
		parry
	};

	/*Called once per frame.*/
	void update();
	/*Calculate balance damage to be dealt to an actor based on a series of offsets.
	@param staggerSource: type of source of balance damage.
	@param weapon: weapon source of balance damage.
	@param aggressor: aggressor of this balance damage.
	@param victim: victim of this balance damage.
	@param baseDamage: base damage to be dealt, may or may not be used depending on the type of stagger source.*/
	void calculateBalanceDamage(DMGSOURCE dmgSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage);
private:
	/*Damage an actor's balance; actor's balance cannot go below 0.
	Break the actor's balance if the actor's balance hits 0 and the actor's balance is not currently broken.
	When the actor's balance is broken, each damage will result in a stagger
	Start tracking the actor's balance if it's not yet tracked.
	@param aggressor: actor that deals balance damage.
	@param victim: actor that receives balance damage.
	@param damage: balance damage.*/
	void damageBalance(RE::Actor* aggressor, RE::Actor* victim, float damage);

	/*Calculate this actor's max balance.
	@param a_actor: actor whose max possible balance will be returned.
	@return max possible balace of this actor.*/
	inline float calculateMaxBalance(RE::Actor* a_actor);

	/*Start tracking an actor's balance, putting it onto the balance map.*/
	inline void trackBalance(RE::Actor* a_actor);

	/*Stop tracking an actor's balance, removing it from 1.balance map and 2.balance broken actors.*/
	inline void untrackBalance(RE::Actor* a_actor);

};