#pragma once
namespace RE
{

	enum DIFFICULTY
	{
		kNovice = 0,
		kApprentice = 1,
		kAdept = 2,
		kExpert = 3,
		kMaster = 4,
		kLegendary = 5
	};
	namespace Offset
	{

		typedef void(_fastcall* tPushActorAway_sub_14067D4A0)(RE::AIProcess* a_causer, RE::Actor* a_target, RE::NiPoint3& a_origin, float a_magnitude);
		inline static REL::Relocation<tPushActorAway_sub_14067D4A0> pushActorAway{ RELOCATION_ID(38858, 39895) };

		typedef void(_fastcall* _shakeCamera)(float strength, RE::NiPoint3 source, float duration);
		inline static REL::Relocation<_shakeCamera> shakeCamera{ RELOCATION_ID(32275, 33012) };

		typedef void(_fastcall* _destroyProjectile)(RE::Projectile* a_projectile);
		inline static REL::Relocation<_destroyProjectile> destroyProjectile{ RELOCATION_ID(42930, 44110) };

		typedef RE::TESObjectREFR* (_fastcall* _getEquippedShield)(RE::Actor* a_actor);
		inline static REL::Relocation<_getEquippedShield> getEquippedShield{ RELOCATION_ID(37624, 38577) };

		inline void SGTM(float a_in) {
			static float* g_SGTM = (float*)RELOCATION_ID(511883, 388443).address();
			*g_SGTM = a_in;
			using func_t = decltype(SGTM);
			REL::Relocation<func_t> func{ RELOCATION_ID(66989, 68246) };
			return;
		}
		
		static float* g_worldScaleInverse = (float*)RELOCATION_ID(230692, 187407).address();
		inline static float* g_deltaTime = (float*)RELOCATION_ID(523660, 410199).address();                                                                    // 2F6B948
		inline static float* g_deltaTimeRealTime = (float*)RELOCATION_ID(523661, 410200).address();  // 2F6B94C

		inline bool playPairedIdle(RE::AIProcess* proc, RE::Actor* attacker, RE::DEFAULT_OBJECT smth, RE::TESIdleForm* idle, bool a5, bool a6, RE::TESObjectREFR* target)
		{
			using func_t = decltype(&playPairedIdle);
			REL::Relocation<func_t> func{ RELOCATION_ID(38290, 39256) };
			return func(proc, attacker, smth, idle, a5, a6, target);
		}

		inline int soundHelper_a(void* manager, RE::BSSoundHandle* a2, int a3, int a4)  //sub_140BEEE70
		{
			using func_t = decltype(&soundHelper_a);
			REL::Relocation<func_t> func{ RELOCATION_ID(66401, 67663) };
			return func(manager, a2, a3, a4);
		}

		inline void soundHelper_b(RE::BSSoundHandle* a1, RE::NiAVObject* source_node)  //sub_140BEDB10
		{
			using func_t = decltype(&soundHelper_b);
			REL::Relocation<func_t> func{ RELOCATION_ID(66375, 67636) };
			return func(a1, source_node);
		}

		inline char __fastcall soundHelper_c(RE::BSSoundHandle* a1)  //sub_140BED530
		{
			using func_t = decltype(&soundHelper_c);
			REL::Relocation<func_t> func{ RELOCATION_ID(66355, 67616) };
			return func(a1);
		}

		inline char set_sound_position(RE::BSSoundHandle* a1, float x, float y, float z)
		{
			using func_t = decltype(&set_sound_position);
			REL::Relocation<func_t> func{ RELOCATION_ID(66370, 67631) };
			return func(a1, x, y, z);
		}

		inline RE::TESObjectREFR* PlaceAtMe(RE::TESObjectREFR* self, RE::TESForm* a_form, std::uint32_t count, bool forcePersist, bool initiallyDisabled)
		{
			using func_t = RE::TESObjectREFR*(RE::BSScript::Internal::VirtualMachine*, RE::VMStackID, RE::TESObjectREFR*, RE::TESForm*, std::uint32_t, bool, bool);
			RE::VMStackID frame = 0;  

			REL::Relocation<func_t> func{ RELOCATION_ID(55672, 56203) };
			auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();

			return func(vm, frame, self, a_form, count, forcePersist, initiallyDisabled);
		};
		
		
	}
}



