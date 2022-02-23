#pragma once
#include "include/robin_hood.h"
/*Handling block stamina damage and perfect blocking.*/
class blockHandler {
public:

	/*Mapping of all actors in perfect blocking state =>> effective time of their perfect blocks.*/
	robin_hood::unordered_map <RE::Actor*, float> actorsPerfectBlocking;

	/*Mapping of all actors in perfect blocking cool down =>> remaining time of the cool down.*/
	robin_hood::unordered_map <RE::Actor*, float> actorsInBlockingCoolDown;

	/*Set of all actors who have successfully perfect blocked an attack.*/
	robin_hood::unordered_set <RE::Actor*> actorsPerfectblockSuccessful;

	static blockHandler* GetSingleton()
	{
		static blockHandler singleton;
		return  std::addressof(singleton);
	}

	void update();

	void registerPerfectBlock(RE::Actor* actor);

	/*Process a single block.
	@param blocker: Actor who blocks.
	@param aggressor: Actor whose attack gets blocked.
	@param iHitflag: hitflag of the blocked hit, in integer.
	@param hitData: hitdata of the blocked hit.
	@param realDamage: real damage of this blocked hit.
	@return if the block is a perfect block.*/
	bool processBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData, float realDamage);

private:
	inline void guardBreakLarge(RE::Actor* actor, RE::Actor* actorToPush);
	inline void guardBreakMedium(RE::Actor* actor);
	inline void guardBreakSmall(RE::Actor* deflector, RE::Actor* deflected);
	inline void processStaminaBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData, float realDamage);



	/*Process perfect block does not take in real damage, as parry damage is re-adjusted.*/
	inline void processPerfectBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData);

};