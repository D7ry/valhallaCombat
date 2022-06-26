#pragma once

#include "SKSE/Trampoline.h"
#include "include/blockHandler.h"
#include "include/Utils.h"
class Hook_OnGetAttackStaminaCost //Actor__sub_140627930+16E	call ActorValueOwner__sub_1403BEC90
{
	/*to cancel out vanilla power attack stamina consumption.*/
public:
	static void install() {
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(37650) };

		_getHeavyAttackStaminaCost = trampoline.write_call<5>(hook.address() + 0x16E, getAttackStaminaCost);
		logger::info("Heavy attack stamina hook installed.");
	}
private:
	static float getAttackStaminaCost(uintptr_t avOwner, RE::BGSAttackData* atkData);
	static inline REL::Relocation<decltype(getAttackStaminaCost)> _getHeavyAttackStaminaCost;
};

class Hook_CacheAttackStaminaCost //sub_14063CFB0+BB	call    ActorValueOwner__sub_1403BEC90 //this thing is useless
{
public:
	static void install() {
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(38047) };

		_cacheAttackStaminaCost = trampoline.write_call<5>(hook.address() + 0xBB, cacheAttackStaminaCost);
		logger::info("Heavy bash stamina hook installed.");
	}
private:
	static float cacheAttackStaminaCost(uintptr_t avOwner, RE::BGSAttackData* atkData);
	static inline REL::Relocation<decltype(cacheAttackStaminaCost)> _cacheAttackStaminaCost;
};

class Hook_GetBlockChance
{
public:
	static void install() { //Down	p	sub_140845C60+2E	call    Actor__GetEquippedShield_140625FA0
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(49751) };

		_getBlockChance = trampoline.write_call<5>(hook.address() + 0x2E, getBlockChance);
		logger::info("Get block chance hook installed.");
	}
private:
	static uintptr_t getBlockChance(RE::Actor* actor);
	static inline REL::Relocation<decltype(getBlockChance)> _getBlockChance;
};

class Hook_GetWantBlock
{
public:

	static void install()
	{
		REL::Relocation<std::uintptr_t> GetWantBlockBase{ REL::ID(37376) };

		auto& trampoline = SKSE::GetTrampoline();
		_GetWantBlock = trampoline.write_call<5>(GetWantBlockBase.address() + 0x23, GetWantBlock);


		logger::info("GetWantBlock hook installed.");
	}
private:
	static std::int32_t& GetWantBlock(void* unk_ptr, const RE::BSFixedString& a_channelName, std::uint8_t unk_int, RE::Actor* a_actor, std::int32_t& a_result);

	static inline REL::Relocation<decltype(GetWantBlock)> _GetWantBlock;
};

/*Returns NPC attack chance. Return 0 to deny NPC attack.*/
class Hook_GetAttackChance1 {
public:
	static void install() { //Up	p	sub_14042F810+157A	call    Character__sub_140845B30
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(28629) };

		_getAttackChance = trampoline.write_call<5>(hook.address() + 0x157A, getAttackChance);
		logger::info("Get block chance hook installed.");
	}
private:
	static uintptr_t getAttackChance(RE::Actor* a1, RE::Actor* a2, RE::BGSAttackData* atkData);
	static inline REL::Relocation<decltype(getAttackChance)> _getAttackChance;
};
/*Returns NPC attack chance. Return 0 to deny NPC attack.*/
class Hook_GetAttackChance2 {
public:
	static void install() { //Up	p	sub_14080C020+2AE	call    Character__sub_140845B30
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(48139) };

		_getAttackChance = trampoline.write_call<5>(hook.address() + 0x2AE, getAttackChance);
		logger::info("Get block chance hook installed.");
	}
private:
	static uintptr_t getAttackChance(RE::Actor* a1, RE::Actor* a2, RE::BGSAttackData* atkData);
	static inline REL::Relocation<decltype(getAttackChance)> _getAttackChance;
};

class Hook_OnStaminaRegen //block stamina regen during weapon swing
{
public:
	static void install() {
		REL::Relocation<uintptr_t> hook{ REL::ID(37510) };  // 620690 - a function that regenerates stamina
		auto& trampoline = SKSE::GetTrampoline();
		_HasFlags1 = trampoline.write_call<5>(hook.address() + 0x62, HasFlags1);
		logger::info("Stamina Regen hook installed");
	}

private:
    static bool HasFlags1(RE::ActorState* a_this, uint16_t a_flags);
    static inline REL::Relocation<decltype(HasFlags1)> _HasFlags1;
};

class Hook_OnPhysicalHit
{
public:
	static void install() {
		REL::Relocation<uintptr_t> hook{ REL::ID(37673) };
		auto& trampoline = SKSE::GetTrampoline();
		_ProcessHit = trampoline.write_call<5>(hook.address() + 0x3C0, processHit);
		logger::info("Physical Hit hook installed.");
	}
private:
    static void processHit(RE::Actor* victim, RE::HitData& hitData);
    static inline REL::Relocation<decltype(processHit)> _ProcessHit;

};

/*Remove vanilla stagger by setting return value of all vanilla stagger magnitude calculations to 0.*/
class Hook_OnGetStaggerMagnitude
{
public:
	static void install() {
		auto& trampoline = SKSE::GetTrampoline();
		REL::Relocation<uintptr_t> hook1{ REL::ID(42839) }; //Down	p	Character__sub_1407431D0+81	call    ActorValueOwner__sub_1403BEB10
		_getStaggerMagnitude_Weapon = trampoline.write_call<5>(hook1.address() + 0x81, getStaggerMagnitude_Weapon);

		REL::Relocation<uintptr_t> hook2{ REL::ID(42839) };	//Down	p	Character__sub_1407431D0+5B	call    sub_1403BE760
		_getStaggerManitude_Bash = trampoline.write_call<5>(hook2.address() + 0x5B, getStaggerManitude_Bash);
		logger::info("Stagger magnitude hook installed.");
	}
private:
	static float getStaggerMagnitude_Weapon(RE::ActorValueOwner* a1, RE::ActorValueOwner* a2, RE::TESObjectWEAP* a3, float a4); 
	static inline REL::Relocation<decltype(getStaggerMagnitude_Weapon)> _getStaggerMagnitude_Weapon;

	static float getStaggerManitude_Bash(uintptr_t a1, uintptr_t a2);
	static inline REL::Relocation<decltype(getStaggerManitude_Bash)> _getStaggerManitude_Bash;
	/*
	static void initStagger1(uintptr_t a1, RE::Actor* a2, uintptr_t a3, float a4, float a5);
	static inline REL::Relocation<decltype(initStagger1)> _initStagger1;

	static void initStagger2(uintptr_t a1, RE::Actor* a2, uintptr_t a3, float a4, float a5);
	static inline REL::Relocation<decltype(initStagger2)> _initStagger2;

	static void initStagger3(uintptr_t a1, RE::Actor* a2, uintptr_t a3, float a4, float a5);
	static inline REL::Relocation<decltype(initStagger3)> _initStagger3;*/
};

class Hook_MagicHit {
public:
	static void install()
	{
		auto& trampoline = SKSE::GetTrampoline();
		REL::Relocation<uintptr_t> hook{ REL::ID(43015) };  // magic hit process
		_processMagicHit = trampoline.write_call<5>(hook.address() + 0x216, processMagicHit);
		logger::debug("Magic hit hook installed");
	}
private:
	static void __fastcall processMagicHit([[maybe_unused]] RE::ActorMagicCaster* attacker, [[maybe_unused]] RE::NiPoint3* rdx0, [[maybe_unused]] RE::Projectile* a_projectile, [[maybe_unused]] RE::TESObjectREFR* victim, [[maybe_unused]] float a5, [[maybe_unused]] float a6, [[maybe_unused]] char a7, [[maybe_unused]] char a8)
	{
		if (!attacker || !victim) {
			_processMagicHit(attacker, rdx0, a_projectile, victim, a5, a6, a7, a8);
		}
		if (victim && victim->IsPlayerRef()) {
			
			if (blockHandler::GetSingleton()->processRegularSpellBlock(victim->As<RE::Actor>(), a_projectile->spell, a_projectile)) {

				return;
			}
		}
		_processMagicHit(attacker, rdx0, a_projectile, victim, a5, a6, a7, a8);
	}
	static inline REL::Relocation<decltype(processMagicHit)> _processMagicHit;
};


class Hook_MainUpdate
{
public:
	static void install()
	{
		auto& trampoline = SKSE::GetTrampoline();
		REL::Relocation<uintptr_t> hook{ RELOCATION_ID(35551, 36544) };  // 5AF3D0, 5D29F0, main loop

#ifdef SKYRIM_SUPPORT_AE
		_Update = trampoline.write_call<5>(hook.address() + 0x160, Update);
#else
		_Update = trampoline.write_call<5>(hook.address() + 0x11F, Update);
#endif
		logger::info("Main update hook installed");
	}

private:
	static void Update(RE::Main* a_this, float a2);
	static inline REL::Relocation<decltype(Update)> _Update;

};

class Hook_OnPlayerUpdate
{
public:
	static void install() {

		REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };

		_Update = PlayerCharacterVtbl.write_vfunc(0xAD, Update);
		logger::info("Player update hook installed");
	}
private:

	static void Update(RE::PlayerCharacter* a_this, float a_delta) {
		//DEBUG("PLAYER update");
		ValhallaCombat::GetSingleton()->update();
		_Update(a_this, a_delta);
	}
	static inline REL::Relocation<decltype(Update)> _Update;
};

class Hook_OnProjectileCollision {
public:
	static void install() {

		REL::Relocation<std::uintptr_t> arrowProjectileVtbl{ RE::VTABLE_ArrowProjectile[0] };
		REL::Relocation<std::uintptr_t> missileProjectileVtbl{ RE::VTABLE_MissileProjectile[0] };

		_arrowCollission = arrowProjectileVtbl.write_vfunc(190, OnArrowCollision);
		_missileCollission = missileProjectileVtbl.write_vfunc(190, OnMissileCollision);
	};
private:
	static void OnArrowCollision(RE::Projectile* a_this, RE::hkpAllCdPointCollector* a_AllCdPointCollector);

	static void OnMissileCollision(RE::Projectile* a_this, RE::hkpAllCdPointCollector* a_AllCdPointCollector);
	static inline REL::Relocation<decltype(OnArrowCollision)> _arrowCollission;
	static inline REL::Relocation<decltype(OnMissileCollision)> _missileCollission;
};

class Hooks {
public:
	static void install() {
		logger::info("Installing hooks...");
		SKSE::AllocTrampoline(1 << 8);
		Hook_OnGetAttackStaminaCost::install();
		//Hook_CacheAttackStaminaCost::install();
		//Hook_GetBlockChance::install();
		//Hook_GetAttackChance1::install();
		//Hook_GetAttackChance2::install();
		Hook_OnStaminaRegen::install();
		//Hook_GetWantBlock::install();
		Hook_OnPhysicalHit::install();
		//Hook_MagicHit::install();
		//Hook_MainUpdate::install();
		Hook_OnPlayerUpdate::install();
		Hook_OnProjectileCollision::install();
		//Hook_OnGetStaggerMagnitude::install();
		//TODO:Re apply stagger hook
	}
};

