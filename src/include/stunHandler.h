#pragma once
#include "data.h"
#include "offsets.h"
#include "lib/robin_hood.h"
#include <mutex>
#include <shared_mutex>
#include <unordered_set>

/*Handling enemy stun value.*/

class stunHandler {

private:
	/*Mini-class containing an actor's stun data.*/
	struct actorStunData {

		private:
		RE::Actor* _actor;
		float _maxStun;
		float _currentStun;
		bool _combatRegeneration;  //flipped when stun goes to 0 and actor start regeneration
		float _regenCountDown; //timer for regen. 
		float _regenCountDownMax;
		/*Calculate this Actor's max(i.e. permanent) stun.
		@param actor: actor whose stun will be calculated
		@return this actor's max sun.*/
		inline float calculateMaxStun(RE::Actor* a_actor) {
			return (a_actor->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kHealth) + a_actor->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kStamina)) / 2;
		}

	public:
		//The constructor assumes param a_actor is currently loaded.
		actorStunData(RE::ActorHandle a_actorHandle, float a_regenCountdown = 3) {
			if (!a_actorHandle) {
				logger::error("Error: Actor not loaded while attempting to construct stun data.");
			}
			_actor = a_actorHandle.get().get();
			if (!_actor) {
				logger::error("Error: Actor obtained from handle is a null ref when constructing stun data.");
			}
			_maxStun = calculateMaxStun(_actor);
			_currentStun = _maxStun;
			_regenCountDownMax = a_regenCountdown;
			_regenCountDown = _regenCountDownMax;
			_combatRegeneration = false;
		}
		/*Damage this actor's stun. Stun cannot go below 0
		@return if the stun is depleted(sub 0) after this damage.*/
		bool damageStun(float a_damage) {
			_regenCountDown = _regenCountDownMax;
			_currentStun -= a_damage;
			if (_currentStun <= 0) {
				_currentStun = 0;
				return true;
			}
			return false;
		}
		
		/*Recovery this actor's stun, Stun cannot go above max stun.
		@return if the stun is fully recovered after this recovery.*/
		bool recoverStun(float a_recovery) {
			_currentStun += a_recovery;
			if (_currentStun >= _maxStun) {
				_currentStun = _maxStun;
				return true;
			}
			return false;
		}

		/*Regenerates this actor's stun for the amount of a tick.
		Returns true if the stun is fully regenerated.*/
		bool regenStun() {
			return recoverStun(*RE::Offset::g_deltaTime * 1 / 7 * _maxStun);
		}
		
		/*Refills current stun to full.*/
		void refillStun() {
			_currentStun = _maxStun;
		}

		/*Simply modify this actor's stun value.*/
		void modStun(float a_val) {
			_currentStun += a_val;
			if (_currentStun > _maxStun) {
				_currentStun = _maxStun;
			}
			if (_currentStun < 0) {
				_currentStun = 0;
			}
		}
		
		float getCurrentStun() {
			return _currentStun;
		}
		
		float getMaxStun() {
			return _maxStun;
		}

		bool canRegen() {
			return !_actor->IsInCombat() || _combatRegeneration;
		}

		float getRegenCountDown() {
			return _regenCountDown;
		}

		void modRegenCountDown(float a_mod) {
			_regenCountDown += a_mod;
		}

		void setRegenCountDown(float a_countDown) {
			_regenCountDown = a_countDown;
		}

		void setCombatRegeneration(bool a_bool) {
			_combatRegeneration = a_bool;
		}
	};
	
public:
	using actorStunData_ptr = std::shared_ptr<stunHandler::actorStunData>;
	/*Called once per frame.
	Regenerate stun for actors not in combat, by looking up actors.*/
	void update();


	/*Start tracking this Actor's stun, emplacing it into the stun map.
	@param actor: actor whose stun will be tracked.
	@return stun data object of this actor.
	*/
	actorStunData_ptr trackStun(RE::Actor* a_actor);
	actorStunData_ptr trackStun(RE::ActorHandle a_actorHandle);
	/*Stop tracking this Actor's stun.
	@param actor: actor whose stun will no longer be tracked.*/
	void untrackStun(RE::Actor* a_actor);

	

private:
	/*Mapping of actors whose stun is actively tracked => data structure storing their stun.*/
	std::unordered_map<RE::ActorHandle, actorStunData_ptr> actorStunDataMap;


	/*Set of all actors whose stun are broken.*/
	std::unordered_set<RE::ActorHandle> stunBrokenActors;

	mutable std::shared_mutex mtx_ActorStunMap;
	mutable std::shared_mutex mtx_StunBrokenActors;
	mutable std::shared_mutex mtx_ActorStunDataMap;

	inline void safeErase_ActorStunDataMap(RE::Actor* actor);
	inline void safeErase_StunBrokenActors(RE::Actor* actor);
	inline void safeInsert_StunBrokenActors(RE::Actor* a_actor);
	inline actorStunData_ptr safeInsert_ActorStunDataMap(RE::Actor* a_actor);
	inline actorStunData_ptr safeGet_ActorStunData(RE::Actor* a_actor);
	inline bool safeGet_isStunBroken(RE::Actor* a_actor);

	inline void safeErase_ActorStunDataMap(RE::ActorHandle a_handle);
	inline void safeErase_StunBrokenActors(RE::ActorHandle a_handle);
	inline void safeInsert_StunBrokenActors(RE::ActorHandle a_handle);
	inline actorStunData_ptr safeInsert_ActorStunDataMap(RE::ActorHandle a_handle);
	inline actorStunData_ptr safeGet_ActorStunData(RE::ActorHandle a_handle);
	inline bool safeGet_isStunBroken(RE::ActorHandle a_handle);

	/*Reset this actor's stun back to full.
	@param actor: actor whose stun will be recovered fully.*/
	void refillStun(RE::Actor* actor);

	/*Damage this actor's stun. Actor's stun won't get damaged below zero.
	If the actor does not exist on the stunmap, track their stun first.
	@param aggressor: actor who will apply stun damage.
	@param actor: actor whose stun will be damaged.
	@param damage: stun damage applied onto this actor.*/
	void damageStun(RE::Actor* aggressor, RE::Actor* actor, float damage);

	/*Stun the actor*/
	void onStunBroken(RE::Actor* a_actor);

	/*Launch a separate thread constantly flashing all stun broken actor's health bar.
	The thread terminates itself once there are no stun broken actors.*/
	void async_launchHealthBarFlashThread();

	std::atomic<bool> async_HealthBarFlashThreadActive;
	
	RE::BSFixedString gv_bStunned = "val_bStunned";

public:


	static stunHandler* GetSingleton()
	{
		static stunHandler singleton;
		return  std::addressof(singleton);
	}

	
	/*Source from which a stun damage can be applied.*/
	enum STUNSOURCE
	{
		lightAttack,
		powerAttack,
		bash,
		powerBash,
		timedBlock,
		parry,
		counterAttack
	};


	/*Get this actor's current stun from stun map.
	If the actor does not exist on the stun map yet, start tracking this actor's stun.
	@param actor: actor whose stun will be returned.
	@return the actor's current stun value.*/
	float getCurrentStun(RE::Actor* actor);

	static float getCurrentStun_static(RE::Actor* a_actor);

	/*Get this actor's max stun(i.e. permanent stun) from stun map.
	* If the actor does not exist on the stun map yet, start tracking this actor's stun.
	@param actor: actor whose still value will be calculated.
	@return the actor's maximum stun value.*/
	float getMaxStun(RE::Actor* actor);

	static float getMaxStun_static(RE::Actor* a_actor);
	/*Return if the actor is completely stunned and ready for execution.*/
	bool getIsStunBroken(RE::Actor* actor);
	/*Clears all records from StunMap.*/
	void reset();


	/*Calculate a stun damage for the actor, and immediately apply the stun damage.
	*/
	void processStunDamage(STUNSOURCE stunSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage);

	void modStun(RE::Actor* a_actor, float a_mod);
	/*Getter for a copy of stun broken actors.*/
	std::unordered_set<RE::ActorHandle> getStunBrokenActors();

};
