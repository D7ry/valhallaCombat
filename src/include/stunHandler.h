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
private:
	static inline std::mutex mtx_ActorStunMap;
	static inline std::mutex mtx_StunRegenQueue;
	static inline std::mutex mtx_StunnedActors;
	/*Start tracking this Actor's stun.
	@param actor: actor whose stun will be tracked.*/
	void trackStun(RE::Actor* actor);
	/*Stop tracking this Actor's stun.
	@param actor: actor whose stun will no longer be tracked.*/
	void untrackStun(RE::Actor* actor);
	/*Reset this actor's stun back to full.
	@param actor: actor whose stun will be recovered fully.*/
	void refillStun(RE::Actor* actor);
	/*Damage this actor's stun. If the actor does not exist on the stunmap, track their stun first.
	@param aggressor: actor who will apply stun damage.
	@param actor: actor whose stun will be damaged.
	@param damage: stun damage applied onto this actor.*/
	void damageStun(RE::Actor* aggressor, RE::Actor* actor, float damage);

	/*An actor is knocked down when simultaneously out of stun and stamina, and either:
	gets their attack parried OR
	failed to block a heavy attack*/
	void knockDown(RE::Actor* aggressor, RE::Actor* victim);

	inline void greyOutStunMeter(RE::Actor* a_actor);

	inline void revertStunMeter(RE::Actor* a_actor);

	inline void flashHealthBar(RE::Actor* a_actor);
	/*Mapping of actors whose stun values are tracked => a pair storing [0]Actor's maximum stun value, [1] Actor's current stun value.*/
	robin_hood::unordered_map <RE::Actor*, std::pair<float, float>> actorStunMap;

	/*Mapping of actors whose stun has been damaged recently => their stun regen cooldown.
	Their timer decrements on update and once the timer reaches 0, corresponding actors in actorStunMap will regenerate stun.*/
	robin_hood::unordered_map <RE::Actor*, float> stunRegenQueue;

	/*Mapping of actors who are completely stunned => their stun meter blinking timer.*/
	robin_hood::unordered_set <RE::Actor*> stunnedActors;
	float timer_StunMeterFlash;
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
	bool isActorStunned(RE::Actor* actor);
	/*Clears all records from StunMap.*/
	void refreshStun();

	/*Iterate over actor stun map and clears off all unloaded actors. To make sure stun map contains recent loaded actors.
	*/
	void cleanUpStunMap();

	/*Launch the cleaner thread that cleans up stun map every 5 minutes.*/
	static void launchStunMapCleaner();

	/*Perform a clean up every 5 minutes in a separate thread.*/
	static void stunMapCleanUpTask();

	/*Calculate a stun damage for the actor, and immediately apply the stun damage.
	Stun calculation go as follows:
	for light/power attack: base damage * mult.
	for bash/powerbash:  blocking level * mult.
	for parry: damage the aggressor would receive if they were to attack themselves * mult.
	*/
	void calculateStunDamage(STUNSOURCE stunSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage);


};
