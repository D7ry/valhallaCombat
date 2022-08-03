#pragma once

#include "SKSE/Trampoline.h"
#include "include/blockHandler.h"
#include "include/Utils.h"
namespace Hooks
{
	class Hook_OnGetAttackStaminaCost  //Actor__sub_140627930+16E	call ActorValueOwner__sub_1403BEC90
	{
		/*to cancel out vanilla power attack stamina consumption.*/
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();

			REL::Relocation<uintptr_t> hook{ RELOCATION_ID(37650, 38603) };  //SE:627930 + 16E => 3BEC90 AE:64D350 + 171 => 3D6720
#ifdef SKYRIM_SUPPORT_AE
			_getAttackStaminaCost = trampoline.write_call<5>(hook.address() + 0x171, getAttackStaminaCost);
#else
			_getAttackStaminaCost = trampoline.write_call<5>(hook.address() + 0x16E, getAttackStaminaCost);
#endif
			logger::info("hook:OnGetAttackStaminaCost");
		}

	private:
		static float getAttackStaminaCost(uintptr_t avOwner, RE::BGSAttackData* atkData);
		static inline REL::Relocation<decltype(getAttackStaminaCost)> _getAttackStaminaCost;
	};


	class Hook_OnStaminaRegen  //block stamina regen during weapon swing
	{
	public:
		static void install()
		{
			REL::Relocation<uintptr_t> hook{ RELOCATION_ID(37510, 38452) };  // 140620690       140645AA0
			auto& trampoline = SKSE::GetTrampoline();
#ifdef SKYRIM_SUPPORT_AE
			int offset = 0x6F;
#else
			int offset = 0x62;
#endif
			_HasFlags1 = trampoline.write_call<5>(hook.address() + offset, HasFlags1);
			logger::info("hook:OnStaminaRegen");
		}

	private:
		static bool HasFlags1(RE::ActorState* a_this, uint16_t a_flags);
		static inline REL::Relocation<decltype(HasFlags1)> _HasFlags1;  //14063C330       140662930
	};

	class Hook_OnMeleeHit
	{
	public:
		static void install()
		{
			REL::Relocation<uintptr_t> hook{ RELOCATION_ID(37673, 38627) };  //140628C20       14064E760
#ifdef SKYRIM_SUPPORT_AE
			int offset = 0x4A8;
#else
			int offset = 0x3C0;
#endif
			auto& trampoline = SKSE::GetTrampoline();
			_ProcessHit = trampoline.write_call<5>(hook.address() + offset, processHit);
			logger::info("hook:OnMeleeHit");
		}

	private:
		static void processHit(RE::Actor* victim, RE::HitData& hitData);
		static inline REL::Relocation<decltype(processHit)> _ProcessHit;  //140626400       14064BAB0
	};

	/*Remove vanilla stagger by setting return value of all vanilla stagger magnitude calculations to 0.*/
	class Hook_OnGetStaggerMagnitude
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<uintptr_t> hook1{ REL::ID(42839) };  //Down	p	Character__sub_1407431D0+81	call    ActorValueOwner__sub_1403BEB10
			_getStaggerMagnitude_Weapon = trampoline.write_call<5>(hook1.address() + 0x81, getStaggerMagnitude_Weapon);

			REL::Relocation<uintptr_t> hook2{ REL::ID(42839) };  //Down	p	Character__sub_1407431D0+5B	call    sub_1403BE760
			_getStaggerManitude_Bash = trampoline.write_call<5>(hook2.address() + 0x5B, getStaggerManitude_Bash);
			logger::info("hook:OnGetStaggerMagnitude");
		}

	private:
		static float getStaggerMagnitude_Weapon(RE::ActorValueOwner* a1, RE::ActorValueOwner* a2, RE::TESObjectWEAP* a3, float a4);
		static inline REL::Relocation<decltype(getStaggerMagnitude_Weapon)> _getStaggerMagnitude_Weapon;

		static float getStaggerManitude_Bash(uintptr_t a1, uintptr_t a2);
		static inline REL::Relocation<decltype(getStaggerManitude_Bash)> _getStaggerManitude_Bash;
	};

	class Hook_MagicHit
	{  //not used now
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<uintptr_t> hook{ REL::ID(43015) };  // magic hit process
			_processMagicHit = trampoline.write_call<5>(hook.address() + 0x216, processMagicHit);
			logger::debug("hook:OnMagicHit");
		}

	private:
		static void __fastcall processMagicHit([[maybe_unused]] RE::ActorMagicCaster* attacker, [[maybe_unused]] RE::NiPoint3* rdx0, [[maybe_unused]] RE::Projectile* a_projectile, [[maybe_unused]] RE::TESObjectREFR* victim, [[maybe_unused]] float a5, [[maybe_unused]] float a6, [[maybe_unused]] char a7, [[maybe_unused]] char a8)
		{
			if (!attacker || !victim) {
				_processMagicHit(attacker, rdx0, a_projectile, victim, a5, a6, a7, a8);
			}
			if (victim && victim->IsPlayerRef()) {
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
			logger::info("hook:OnMainUpdate");
		}

	private:
		static void Update(RE::Main* a_this, float a2);
		static inline REL::Relocation<decltype(Update)> _Update;
	};

	class Hook_OnPlayerUpdate  //no longer used
	{
	public:
		static void install()
		{
			REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };

			_Update = PlayerCharacterVtbl.write_vfunc(0xAD, Update);
			logger::info("hook:OnPlayerUpdate");
		}

	private:
		static void Update(RE::PlayerCharacter* a_this, float a_delta)
		{
			//DEBUG("PLAYER update");
			ValhallaCombat::GetSingleton()->update();
			_Update(a_this, a_delta);
		}
		static inline REL::Relocation<decltype(Update)> _Update;
	};

	class Hook_OnProjectileCollision
	{
	public:
		static void install()
		{
			REL::Relocation<std::uintptr_t> arrowProjectileVtbl{ RE::VTABLE_ArrowProjectile[0] };
			REL::Relocation<std::uintptr_t> missileProjectileVtbl{ RE::VTABLE_MissileProjectile[0] };

			_arrowCollission = arrowProjectileVtbl.write_vfunc(190, OnArrowCollision);
			_missileCollission = missileProjectileVtbl.write_vfunc(190, OnMissileCollision);
			logger::info("hook:OnProjectileCollision");
		};

	private:
		static void OnArrowCollision(RE::Projectile* a_this, RE::hkpAllCdPointCollector* a_AllCdPointCollector);

		static void OnMissileCollision(RE::Projectile* a_this, RE::hkpAllCdPointCollector* a_AllCdPointCollector);
		static inline REL::Relocation<decltype(OnArrowCollision)> _arrowCollission;
		static inline REL::Relocation<decltype(OnMissileCollision)> _missileCollission;
	};

	class Hook_OnMeleeCollision
	{
	public:
		static void install()
		{
			REL::Relocation<uintptr_t> hook{ RELOCATION_ID(37650, 38603) };  //SE:627930 + 38B AE:64D350 + 40A / 45A
			auto& trampoline = SKSE::GetTrampoline();
#ifdef SKYRIM_SUPPORT_AE
			_ProcessHit = trampoline.write_call<5>(hook.address() + 0x45A, processHit);
#else
			_ProcessHit = trampoline.write_call<5>(hook.address() + 0x38B, processHit);
#endif
			logger::info("hook:OnMeleeCollision");
		}
	private:
		static void processHit(RE::Actor* a_aggressor, RE::Actor* a_victim, std::int64_t a_int1, bool a_bool, void* a_unkptr);
		
		static inline REL::Relocation<decltype(processHit)> _ProcessHit;
	};

	class Hook_OnAttackAction
	{
	public:
		static void install()
		{
#if SKYRIM_SUPPORT_AE
			std::uint32_t offset = 0x435; 
#else
			std::uint32_t offset = 0x4D7;  
#endif
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<std::uintptr_t> AttackActionBase{ RELOCATION_ID(48139, 49170) };
			_PerformAttackAction = trampoline.write_call<5>(AttackActionBase.address() + offset, PerformAttackAction);
			logger::info("hook:OnAttackAction");
		}

	private:
		static bool PerformAttackAction(RE::TESActionData* a_actionData);

		static inline REL::Relocation<decltype(PerformAttackAction)> _PerformAttackAction;
	};

	static void install()
	{
		logger::info("Installing hooks...");
		SKSE::AllocTrampoline(1 << 8);
		Hook_OnGetAttackStaminaCost::install();
		Hook_OnStaminaRegen::install();
		Hook_OnMeleeHit::install();
		Hook_OnPlayerUpdate::install();
		Hook_OnProjectileCollision::install();
		Hook_OnMeleeCollision::install();
		Hook_OnAttackAction::install();
		logger::info("...done");
	}
}

