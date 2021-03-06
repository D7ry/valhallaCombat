//#include "include/lib/robin_hood.h"
//#include <mutex>
//#pragma once
///*Keep track of all actor's stagger resistence i.e. "Balance", and deligate stagger events.*/
//class balanceHandler {
//private:
//	/*
//	/*Mapping of actors whose balance values are tracked => a pair storing [0]Actor's maximum balance value, [1] Actor's current balance value.*/
//	robin_hood::unordered_map<RE::Actor*, std::pair<float, float>> actorBalanceMap;
//	/*Set of all actors with broken balance.
//	robin_hood::unordered_set<RE::Actor*> balanceBrokenActors;
//	static inline std::mutex mtx_actorBalanceMap;
//	static inline std::mutex mtx_balanceBrokenActors; */
//
//public:
//	static balanceHandler* GetSingleton()
//	{
//		static balanceHandler singleton;
//		return  std::addressof(singleton);
//	}
//	enum DMGSOURCE
//	{
//		lightAttack = 0,
//		powerAttack,
//		bash,
//		powerBash,
//		parry
//	};
//
//	/*Called once per frame.*/
//	void update();
//	/*Calculate balance damage to be dealt to an actor based on a series of offsets.
//	@param staggerSource: type of source of balance damage.
//	@param weapon: weapon source of balance damage.
//	@param aggressor: aggressor of this balance damage.
//	@param victim: victim of this balance damage.
//	@param baseDamage: base damage to be dealt, may or may not be used depending on the type of stagger source.*/
//	void processBalanceDamage(DMGSOURCE dmgSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage);
//
//	/*Return if this actor has their balance broken.*/
//	bool isBalanceBroken(RE::Actor* actor);
//
//	/*Clean up the balance up; get rid of actors no longer loaded to free up memory.*/
//	void collectGarbage();
//
//	/*Reset all balance values by wiping all hashmap clean.*/
//	void reset();
//
//	void queueGarbageCollection();
//private:
//	/*Damage an actor's balance; actor's balance cannot go below 0.
//	Break the actor's balance if the actor's balance hits 0 and the actor's balance is not currently broken.
//	When the actor's balance is broken, each damage will result in a stagger
//	Start tracking the actor's balance if it's not yet tracked.
//	@param aggressor: actor that deals balance damage.
//	@param victim: actor that receives balance damage.
//	@param damage: balance damage.*/
//	void damageBalance(DMGSOURCE dmgSource, RE::Actor* aggressor, RE::Actor* victim, float damage);
//	/*Recovery this actor's balance.*/
//	void recoverBalance(RE::Actor* a_actor, float recovery);
//	/*Calculate this actor's max balance.
//	@param a_actor: actor whose max possible balance will be returned.
//	@return max possible balace of this actor.*/
//	inline float calculateMaxBalance(RE::Actor* a_actor);
//
//	/*Start tracking an actor's balance, putting it onto the balance map.*/
//	inline void trackBalance(RE::Actor* a_actor);
//
//	/*Stop tracking an actor's balance, removing it from 1.balance map and 2.balance broken actors.*/
//	inline void untrackBalance(RE::Actor* a_actor);
//
//
//	inline void safeErase_BalanceBrokenActors(RE::Actor* a_actor);
//	inline void safeErase_ActorBalanceMap(RE::Actor* a_actor);
//
//	struct actorBalanceData {
//	private:
//		inline float getMaxBalanceImmunity(RE::Actor* a_actor) {
//			return 1 / 5 * a_actor->GetPermanentActorValue(RE::ActorValue::kHealth);
//		}
//	public:
//		enum balanceState {
//			neutral = 0, //from neutral to one_hit, a balance wall must be broken.
//			one_hit,
//			two_hit,
//			hardened //state where only heavy attack, can stagger
//		};
//
//		actorBalanceData(RE::Actor* a_actor) {
//			_actor = a_actor;
//			_balanceState = balanceState::neutral;
//			_balanceStateRecoveryTimer = 0;
//			_balanceImmunity_max = getMaxBalanceImmunity(a_actor);
//			_balanceImmunity_current = _balanceImmunity_max;
//		}
//
//		bool damageBalanceImmunity(float a_damage) {
//			_balanceImmunity_current -= a_damage;
//			if (_balanceImmunity_current <= 0) {
//				_balanceImmunity_current = 0;
//				return true;
//			}
//		}
//
//	private:
//		RE::Actor* _actor;
//		balanceState _balanceState;
//		float _balanceStateRecoveryTimer;
//		float _balanceImmunity_current;
//		float _balanceImmunity_max;
//	};
//
//	using actorBalanceData_ptr = std::shared_ptr<balanceHandler::actorBalanceData>;
//	robin_hood::unordered_map<RE::Actor*, actorBalanceData_ptr> actorBalanceDataMap;
//
//};