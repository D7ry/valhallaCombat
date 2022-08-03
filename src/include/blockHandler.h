#pragma once
#include "lib/robin_hood.h"
#include "lib/PrecisionAPI.h"
#include <mutex>
/*Handling block stamina damage and perfect blocking.
All timed block features are only usable by player at this point.*/
class blockHandler {
public:
	static blockHandler* GetSingleton()
	{
		static blockHandler singleton;
		return  std::addressof(singleton);
	}

	void update();

	bool isBlockButtonPressed;

	enum blockType {
		regular = 1,
		timed,
		perfect,
		guardBreaking,
		tackle
	};
	
	bool getIsPcTimedBlocking();
	bool getIsPcPerfectBlocking();

	void onTackleKeyDown();
	void onBlockKeyDown();
	void onBlockKeyUp();
	void onBlockStop();

	
private:
	enum blockWindowPenaltyLevel {
		none = 0,
		light, //70% of regular timing
		medium, //50% of regular timing
		heavy //20% of regular timing
	};

	blockWindowPenaltyLevel pcBlockWindowPenalty;
	
	inline bool isTimedBlockElapsedTimeLessThan(float a_in);

	float pcTimedBlockTimer = 0;
	float pcTimedBlockCooldownTimer = 0;
	float pcTimedBlockKeyUpTimer = 0;

	bool isPcTackling = false;
	bool isPcTackleCooldown = false;
	float pcTackleTimer = 0;
	float pcTackleCooldownTimer = 0;
	
	bool isPcTimedBlocking;
	bool isPcBlockingCoolDown;
	bool isPcTimedBlockSuccess;
	bool bKeyUpTimeBuffer;
	
	void OnPcSuccessfulTimedBlock();
	void onPcTimedBlockEnd();

	inline bool isInBlockAngle(RE::Actor* blocker, RE::TESObjectREFR* a_obj);

	inline void playBlockVFX(RE::Actor* blocker, blockType blockType, bool blockedWithWeapon);
	inline void playBlockSFX(RE::Actor* blocker, blockType blockType, bool blockedWithWeapon);
	inline void playBlockScreenShake(RE::Actor* blocker, blockType blockType);
	inline void playBlockSlowTime(blockType blockType);
	
	void playBlockEffects(RE::Actor* blocker, RE::Actor* aggressor, blockType blockType);

public:	


	/// <summary>
	/// Process an attempt to physically block an attack.
	/// </summary>
	void processPhysicalBlock(RE::Actor* blocker, RE::Actor* aggressor, SKSE::stl::enumeration<RE::HitData::Flag, std::uint32_t>, RE::HitData& hitData);
	

	/// <summary>
	/// Initialize an attempt for the blocker to block, parry, or deflect an incoming projectile. Result varies depending on game settings, types
	/// of block, and the blocker's status.
	/// </summary>
	/// <returns>Whether the projectile is blocked, parried, or deflected.</returns>
	bool processProjectileBlock(RE::Actor* a_blocker, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable);


	/// <summary>
	/// Process a melee timed block attempt by the blocker.
	/// If the timed block is successful:
	/// 1. damage the attacker's stun and balance value.
	/// 2. play block spark effects & screen shake effects.
	/// </summary>
	/// <param name="a_blocker"></param>
	/// <param name="a_attacker"></param>
	/// <returns>Whether a timed block is successfully performed.</returns>
	bool processMeleeTimedBlock(RE::Actor* a_blocker, RE::Actor* a_attacker);

	
	/// <summary>
	/// Process a melee tackle attempt by an actor.
	/// If the tackle is successful:
	/// 1. recoil the attacker being tackled.
	/// 2. play block spark effects.
	/// </summary>
	/// <param name="a_tackler">Actor attempting to tackle the attack.</param>
	/// <param name="a_attacker">The attacker.</param>
	/// <returns></returns>
	bool processMeleeTackle(RE::Actor* a_tackler, RE::Actor* a_attacker);
	
	static PRECISION_API::PreHitCallbackReturn precisionPrehitCallbackFunc(const PRECISION_API::PrecisionHitData& a_precisionHitData);

private:
	/// <summary>
	/// Process a stamina blocking attempt similar to "shield of stamina".
	/// Actor with enough stamina can negate all incoming health damage with stamina.
	/// Actor without enough stamina will triggerStagger and receive partial damage.
	/// </summary>
	/// <param name="a_blocker"></param>
	/// <param name="a_aggressor"></param>
	/// <param name="a_hitFlag"></param>
	/// <param name="a_hitData"></param>
	/// <param name="a_realDamage"></param>
	void processStaminaBlock(RE::Actor* a_blocker, RE::Actor* a_aggressor, SKSE::stl::enumeration<RE::HitData::Flag, std::uint32_t> a_hitFlag, RE::HitData& a_hitData);
	
	/// <summary>
	/// Attempt to block an incoming arrow projectile for the blocker.
	/// Block is successful if the blocker has enough magicka.
	/// </summary>
	/// <param name="a_blocker">actor blocking the projectile</param>
	/// <param name="a_projectile">incoming projectile</param>
	/// <returns>Whether the block is successful.</returns>
	bool tryBlockProjectile_Arrow(RE::Actor* a_blocker, RE::Projectile* a_projectile);

	/// <summary>
	/// Attempt to block an incoming spell projectile for the blocker.
	/// Block is successful if the blocker has enough magicka.
	/// </summary>
	/// <param name="a_blocker">actor blocking the projectile</param>
	/// <param name="a_projectile">incoming projectile</param>
	/// <returns>Whether the block is successful.</returns>
	bool tryBlockProjectile_Spell(RE::Actor* a_blocker, RE::Projectile* a_projectile);
	
	/// <summary>
	/// Attempt to parry(timed-block) an incoming spell projectile for the blocker. Conditionally deflect the projectile if the blocker has enough magicka.
	/// Parry is successful if:
	/// 1. the blocker is timed blocking.
	/// 2. the projectile is in the blocker's block angle.
	/// Projectile deflection is successful if:
	/// 1. the blocker has enough magicka.
	/// On successful projectile deflection, blocker loses magicka required to deflect.
	/// </summary>
	/// <param name="a_blocker">the actor blocking the projectile</param>
	/// <param name="a_projectile">the incoming projectile</param>
	/// <param name="a_projectile_collidable">havok collidable of the projectile</param>
	/// <returns>Whether the parry is successful, regardless of whether deflection is successful.</returns>
	bool tryParryProjectile_Spell(RE::Actor* a_blocker, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable);
	
	/// <summary>
	/// Attempt to parry(timed-block) an incoming arrow projectile for the blocker. Conditionally deflect the projectile if the blocker has enough magicka.
	/// Parry is successful if:
	/// 1. the blocker is timed blocking.
	/// 2. the projectile is in the blocker's block angle.
	/// Projectile deflection is successful if:
	/// 1. the blocker has enough magicka.
	/// On successful projectile deflection, blocker loses magicka required to deflect.
	/// </summary>
	/// <param name="a_blocker">the actor blocking the projectile</param>
	/// <param name="a_projectile">the incoming projectile</param>
	/// <param name="a_projectile_collidable">havok collidable of the projectile</param>
	/// <returns>Whether the parry is successfu,  regardless of whether deflection is successful.</returns>
	bool tryParryProjectile_Arrow(RE::Actor* a_blocker, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable);
	
	/// <summary>
	/// Deflect an incoming projectile back.
	/// If the projectile has a sender, the projectile will be aimed at the sender.
	/// </summary>
	/// <param name="a_deflector">Actor deflecting the projectile</param>
	/// <param name="a_projectile"></param>
	/// <param name="a_projectile_collidable"></param>
	inline void deflectProjectile(RE::Actor* a_deflector, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable);
	
	/// <summary>
	/// Destroy the projectile, removing it from the world.
	/// </summary>
	/// <param name="a_projectile">Projectile to be destroyed.</param>
	inline void destroyProjectile(RE::Projectile* a_projectile);
};
