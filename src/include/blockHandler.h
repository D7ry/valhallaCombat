#pragma once
#include "lib/robin_hood.h"
#include <mutex>
/*Handling block stamina damage and perfect blocking.*/
class blockHandler {
public:


	static blockHandler* GetSingleton()
	{
		static blockHandler singleton;
		return  std::addressof(singleton);
	}

	void update();

	/*Register a perfect block when an actor tries to block. Put the blocker into the active perfect blocker set and start timing.
	@param actor actor whose block is registered as a perfect block.*/
	void registerPerfectBlock(RE::Actor* actor);

	/*Process a single block.
	@param blocker: Actor who blocks.
	@param aggressor: Actor whose attack gets blocked.
	@param iHitflag: hitflag of the blocked hit, in integer.
	@param hitData: hitdata of the blocked hit.
	@param realDamage: real damage of this blocked hit.
	@return if the block is a perfect block.*/
	bool processBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData, float realDamage);

	bool isBlockButtonPressed;
private:
	

	enum blockType {
		regular = 1,
		timed,
		perfect,
		guardBreaking
	};
	/*Mapping of all actors in perfect blocking state =>> effective time of their perfect blocks.*/
	robin_hood::unordered_map <RE::Actor*, float> actors_PerfectBlocking;

	/*Mapping of all actors in perfect blocking cool down =>> remaining time of the cool down.*/
	robin_hood::unordered_map <RE::Actor*, float> actors_BlockingCoolDown;

	/*Set of all actors who have successfully perfect blocked an attack.*/
	robin_hood::unordered_set <RE::Actor*> actors_PrevTimeBlockingSuccessful;
	static inline std::mutex mtx;
	static inline std::mutex mtx_actors_PerfectBlocking;
	static inline std::mutex mtx_actors_BlockingCoolDown;
	static inline std::mutex mtx_actors_PrevTimeBlockingSuccessful;
	/*Process a stamina block.
	Actor with enough stamina can negate all incoming health damage with stamina. 
	Actor without enough stamina will triggerStagger and receive partial damage.*/
	void processStaminaBlock(RE::Actor* blocker, RE::Actor* aggressor, int iHitflag, RE::HitData& hitData, float realDamage);

	/*Process a timed block.
	* Damage the attacker's stun and balance, enabled.
	Play block spark effects & screen shake effects if enabled.
	The blocker will not receive any block cooldown once the block timer ends, and may initialize another perfect block as they wish.
	@param blocker: actor performing the perfect block.
	@param attacker: actor whose attack gets perfect blocked.
	@param hitData: reference to the hitData of the blocked attack.*/
	void processTimedBlock(RE::Actor* blocker, RE::Actor* attacker, int iHitflag, RE::HitData& hitData, float realDamage);

	/*Play VFX, SFX and screenShake for successful perfect block.*/
	inline void playBlockVFX(RE::Actor* blocker, RE::Actor* aggressor, int iHitFlag, blockType blockType);
	inline void playBlockSFX(RE::Actor* blocker, int iHitFlag, blockType blockType);
	inline void playBlockScreenShake(RE::Actor* blocker, int iHitFlag, blockType blockType);
	inline void playBlockSlowTime(blockType blockType);

	void playBlockEffects(RE::Actor* blocker, RE::Actor* aggressor, int iHitFlag, blockType blockType);

};

namespace Utils
{
	namespace sound
	{

	}
}