#pragma once
#include "data.h"
#include "lib/robin_hood.h"
#include <mutex>
#include <shared_mutex>

/*Handling enemy stun value.*/

class stunHandler {


public:
	/*Mini-class containing an actor's stun data.*/
	struct actorStunData {
		/*Calculate this Actor's max(i.e. permanent) stun.
		@param actor: actor whose stun will be calculated
		@return this actor's max sun.*/
		inline float calculateMaxStun(RE::Actor* a_actor) {
			return (a_actor->GetPermanentActorValue(RE::ActorValue::kHealth) + a_actor->GetPermanentActorValue(RE::ActorValue::kStamina)) / 2;
		}
		 
	public:
		//The constructor assumes param a_actor is currently loaded.
		actorStunData(RE::Actor* a_actor) {
			_actor = a_actor;
			_maxStun = calculateMaxStun(a_actor);
			_currentStun = _maxStun;
		}
		/*Damage this actor's stun. Stun cannot go below 0
		@return if the stun is depleted(sub 0) after this damage.*/
		bool damageStun(float a_damage) {
			_currentStun -= a_damage;
			if (_currentStun <= 0) {
				_currentStun = 0;
				return true;
			}
		}
		/*Recovery this actor's stun, Stun cannot go above max stun.
		@return if the stun is fully recovered after this recovery.*/
		bool recoverStun(float a_recovery) {
			_currentStun += a_recovery;
			if (_currentStun >= _maxStun) {
				_currentStun = _maxStun;
				return true;
			}
		}
		/*Refills current stun to full.*/
		void refillStun() {
			_currentStun = _maxStun;
		}
		float getCurrentStun() {
			return _currentStun;
		}
		float getMaxStun() {
			return _maxStun;
		}
	private:
		RE::Actor* _actor;
		float _maxStun;
		float _currentStun;
	};
	/*Called once per frame.
	Regenerate stun for actors not in combat, by looking up actors.*/
	void update();


	/*Start tracking this Actor's stun. Return this actor's max stun if the tracking is successful.
	@param actor: actor whose stun will be tracked.
	@return maximum stun of this actor.
	*/
	std::shared_ptr<stunHandler::actorStunData> trackStun(RE::Actor* a_actor);
	/*Stop tracking this Actor's stun.
	@param actor: actor whose stun will no longer be tracked.*/
	void untrackStun(RE::Actor* a_actor);

	void queueGarbageCollection();

	

private:
	bool garbageCollectionQueued = false;
	/*Mapping of actors whose stun values are tracked => a pair storing [0]Actor's maximum stun value, [1] Actor's current stun value.*/
	robin_hood::unordered_map <RE::Actor*, std::pair<float, float>> actorStunMap;

	robin_hood::unordered_map <RE::Actor*, std::shared_ptr<actorStunData>> actorStunDataMap;
	/*Mapping of actors whose stun has been damaged recently => their stun regen cooldown.
	Their timer decrements on update and once the timer reaches 0, corresponding actors in actorStunMap will regenerate stun.*/
	robin_hood::unordered_map <RE::Actor*, float> stunRegenQueue;

	mutable std::shared_mutex mtx_ActorStunMap;
	mutable std::shared_mutex mtx_StunRegenQueue;
	mutable std::shared_mutex mtx_StunnedActors;
	mutable std::shared_mutex mtx_ActorStunDataMap;
 /*Erase actor from actorStunMap with mutex.*/
	inline void safeErase_ActorStunDataMap(RE::Actor* actor);
	/*Erase actor from stunRegenQueue with mutex.*/
	inline void safeErase_StunRegenQueue(RE::Actor* actor);
	/*Erase actor from stunnedActors with mutex.*/
	inline void safeErase_StunnedActors(RE::Actor* actor);

	inline void safeInsert_StunRegenQueue(RE::Actor* a_actor);
	inline void safeInsert_StunnedActors(RE::Actor* a_actor);

	inline std::shared_ptr<actorStunData> safeGet_ActorStunData(RE::Actor* a_actor);
	inline bool safeGet_isStunned(RE::Actor* a_actor);
	/*Reset this actor's stun back to full.
	@param actor: actor whose stun will be recovered fully.*/
	void refillStun(RE::Actor* actor);

	/*Damage this actor's stun. Actor's stun won't get damaged below zero.
	If the actor does not exist on the stunmap, track their stun first.
	If the damage gets the actor's stun below 0, and the actor is not yet stunned, play a special stun sound effect; for humanoid actors, trigger their downed state.
	@param aggressor: actor who will apply stun damage.
	@param actor: actor whose stun will be damaged.
	@param damage: stun damage applied onto this actor.*/
	void damageStun(RE::Actor* aggressor, RE::Actor* actor, float damage);

	/*Asynchronous function to constantly flash the actor's stun.*/
	void async_launchHealthBarFlashThread();
	static inline std::atomic<bool> async_HealthBarFlash;


public:
	/*Mapping of actors who are completely stunned => their stun meter blinking timer.*/
	robin_hood::unordered_set <RE::Actor*> stunnedActors;

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
		parry
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
	bool getIsStunned(RE::Actor* actor);
	/*Clears all records from StunMap.*/
	void reset();

	/*Iterate over actor stun map and clears off all unloaded actors. To make sure stun map contains recent loaded actors.
	*/
	void collectGarbage();

	/*Calculate a stun damage for the actor, and immediately apply the stun damage.
	Stun calculation go as follows:
	for light/power attack: base damage * mult.
	for bash/powerbash:  blocking level * mult.
	for parry: damage the aggressor would receive if they were to attack themselves * mult.
	*/
	void processStunDamage(STUNSOURCE stunSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage);


};
