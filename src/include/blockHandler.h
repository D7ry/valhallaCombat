#pragma once
#include "lib/robin_hood.h"
#include "lib/PrecisionAPI.h"
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

	bool getIsPcTimedBlocking();
	bool getIsPcPerfectBlocking();

	/*Register perfect block but only for pc.*/
	void onBlockKeyDown();
	void onBlockKeyUp();
	void onBlockStop();

	/*Register a tackle.*/
	void onTackleKeyDown();
	
	/*Process a single block.
	@param blocker: Actor who blocks.
	@param aggressor: Actor whose attack gets blocked.
	@param iHitflag: hitflag of the blocked hit, in integer.
	@param hitData: hitdata of the blocked hit.
	@param realDamage: real damage of this blocked hit.*/
	void processPhysicalBlock(RE::Actor* blocker, RE::Actor* aggressor, SKSE::stl::enumeration<RE::HitData::Flag, std::uint32_t>, RE::HitData& hitData, float realDamage);
	
	bool isBlockButtonPressed;

	enum blockType {
		regular = 1,
		timed,
		perfect,
		guardBreaking,
		tackle
	};

	
private:
	enum blockWindowPenaltyLevel {
		none = 0,
		light, //70% of regular timing
		medium, //50% of regular timing
		heavy //20% of regular timing
	};

	blockWindowPenaltyLevel pcBlockWindowPenalty;
	float pcTimedBlockTimer = 0;
	float pcTimedBlockCooldownTimer = 0;
	float pcTimedBlockKeyUpTimer = 0;

	bool isPcTackling = false;
	bool isPcTackleCooldown = false;
	float pcTackleTimer = 0;
	float pcTackleCooldownTimer = 0;
	
	inline void onSuccessfulTimedBlock();

	/*Mapping of all actors in perfect blocking state =>> effective time of their perfect blocks.*/
	//robin_hood::unordered_map <RE::Actor*, float> actors_PerfectBlocking;
	bool isPcTimedBlocking;
	bool isPcBlockingCoolDown;
	bool isPcTimedBlockSuccess;
	bool bKeyUpTimeBuffer;
	inline void onPcTimedBlockEnd();
	inline bool isElapsedTimedLessThan(float a_in);
	/*Mapping of all actors in perfect blocking cool down =>> remaining time of the cool down.*/
	robin_hood::unordered_map <RE::Actor*, float> actors_BlockingCoolDown;

	/*Set of all actors who have successfully perfect blocked an attack.*/
	//robin_hood::unordered_set <RE::Actor*> actors_PrevTimeBlockingSuccessful;
	static inline std::mutex mtx;
	static inline std::mutex mtx_actors_PerfectBlocking;
	static inline std::mutex mtx_actors_BlockingCoolDown;
	static inline std::mutex mtx_actors_PrevTimeBlockingSuccessful;
	/*Process a stamina block.
	Actor with enough stamina can negate all incoming health damage with stamina. 
	Actor without enough stamina will triggerStagger and receive partial damage.*/
	void processStaminaBlock(RE::Actor* blocker, RE::Actor* aggressor, SKSE::stl::enumeration<RE::HitData::Flag, std::uint32_t> a_hitFlag, RE::HitData& hitData, float realDamage);

	/*Return: whether the object is within the blocker's block angle and thus can be blocked.*/
	inline bool isInBlockAngle(RE::Actor* blocker, RE::TESObjectREFR* a_obj);

	/*Play VFX, SFX and screenShake for successful perfect block.*/
	inline void playBlockVFX(RE::Actor* blocker, blockType blockType, bool blockedWithWeapon);
	inline void playBlockSFX(RE::Actor* blocker, blockType blockType, bool blockedWithWeapon);
	inline void playBlockScreenShake(RE::Actor* blocker, blockType blockType);
	inline void playBlockSlowTime(blockType blockType);

public:
	void playBlockEffects(RE::Actor* blocker, RE::Actor* aggressor, blockType blockType);

	bool processRegularSpellBlock(RE::Actor* a_blocker, RE::MagicItem* a_spell, RE::Projectile* a_projectile);
	/*Initialize an attempt to block the incoming projectile. If the deflection is successful, return true.
	@param a_blocker: a_blocker: actor attempting to deflect the projectlile
	@param a_projectile: projectile to be deflected.
	@param a_projectile_collidable: the collidable of the projectile, whose collision group will be reset if the deflection
	is successful.
	@return whether the deflection is successful.*/
	bool processProjectileBlock(RE::Actor* a_blocker, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable);
	/*Process a timed block.
	* Damage the attacker's stun and balance
	Play block spark effects & screen shake effects
	The blocker will not receive any block cooldown once the block timer ends, and may initialize another perfect block as they wish.
	@param blocker: actor performing the perfect block.
	@param attacker: actor whose attack gets perfect blocked.
	@return whether a timed block is successfully performed.*/
	bool processMeleeTimedBlock(RE::Actor* blocker, RE::Actor* attacker);
	
	bool processMeleeTackle(RE::Actor* a_tackler, RE::Actor* a_attacker);
	
	static PRECISION_API::PreHitCallbackReturn precisionPrehitCallbackFunc(const PRECISION_API::PrecisionHitData& a_precisionHitData);

private:
	bool tryBlockProjectile_Arrow(RE::Actor* a_blocker, RE::Projectile* a_projectile);
	bool tryBlockProjectile_Spell(RE::Actor* a_blocker, RE::Projectile* a_projectile, RE::MagicItem* a_spell);
	bool tryParryProjectile(RE::Actor* a_blocker, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable);
	/*Deflect the projectile back at the sender of the projectile.*/
	inline void parryProjectile(RE::Actor* a_blocker, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable);
	/*Destroy the projectile by parrying.*/
	inline void blockProjectile(RE::Actor* a_blocker, RE::Projectile* a_projectile);
};
