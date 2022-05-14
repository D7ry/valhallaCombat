#pragma once
#include "data.h"
#include "lib/robin_hood.h"
#include <mutex>
/*Handling enemy stun value.*/
class stunHandler {
public:
	/*Called once per frame.
	Regenerate stun for actors not in combat, by looking up actors.*/
	void update();
	/*Calculate this Actor's max(i.e. permanent) stun.
	@param actor: actor whose stun will be calculated
	@return this actor's max sun.*/
	float calcMaxStun(RE::Actor* actor);

	/*Start tracking this Actor's stun.
	@param actor: actor whose stun will be tracked.*/
	void trackStun(RE::Actor* actor);
	/*Stop tracking this Actor's stun.
	@param actor: actor whose stun will no longer be tracked.*/
	void untrackStun(RE::Actor* actor);
private:
	/*Mapping of actors whose stun values are tracked => a pair storing [0]Actor's maximum stun value, [1] Actor's current stun value.*/
	robin_hood::unordered_map <RE::Actor*, std::pair<float, float>> actorStunMap;

	/*Mapping of actors whose stun has been damaged recently => their stun regen cooldown.
	Their timer decrements on update and once the timer reaches 0, corresponding actors in actorStunMap will regenerate stun.*/
	robin_hood::unordered_map <RE::Actor*, float> stunRegenQueue;

	static inline std::mutex mtx_ActorStunMap;
	static inline std::mutex mtx_StunRegenQueue;
	static inline std::mutex mtx_StunnedActors;
	/*Erase actor from actorStunMap with mutex.*/
	inline void safeErase_ActorStunMap(RE::Actor* actor);
	/*Erase actor from stunRegenQueue with mutex.*/
	inline void safeErase_StunRegenQueue(RE::Actor* actor);
	/*Erase actor from stunnedActors with mutex.*/
	inline void safeErase_StunnedActors(RE::Actor* actor);
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
	static void async_HealthBarFlash();
	static inline std::atomic<bool> async_HealthBarFlash_b;
	static inline void flashHealthBar(RE::Actor* a_actor);

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
	static float getStun(RE::Actor* actor);

	/*Get this actor's max stun(i.e. permanent stun) from stun map.
	* If the actor does not exist on the stun map yet, start tracking this actor's stun.
	@param actor: actor whose still value will be calculated.
	@return the actor's maximum stun value.*/
	static float getMaxStun(RE::Actor* actor);


	/*Return if the actor is completely stunned and ready for execution.*/
	void isActorStunned(RE::Actor* actor, bool& isStunned);
	bool isActorStunned(RE::Actor* actor);
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
