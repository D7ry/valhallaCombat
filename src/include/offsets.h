#pragma once
namespace RE
{
	namespace Offset
	{

		typedef void(_fastcall* tPushActorAway_sub_14067D4A0)(RE::AIProcess* a_causer, RE::Actor* a_target, RE::NiPoint3& a_origin, float a_magnitude);
		inline static REL::Relocation<tPushActorAway_sub_14067D4A0> pushActorAway{ REL::ID(38858) };



		typedef void(_fastcall* _shakeCamera)(float strength, RE::NiPoint3 source, float duration);
		inline static REL::Relocation<_shakeCamera> shakeCamera{ REL::ID(32275) };

		typedef void(_fastcall* _destroyProjectile)(RE::Projectile* a_projectile);
		inline static REL::Relocation<_destroyProjectile> destroyProjectile{ REL::ID(42930) };

		typedef RE::TESObjectREFR* (_fastcall* _getEquippedShield)(RE::Actor* a_actor);
		inline static REL::Relocation< _getEquippedShield> getEquippedShield{ REL::ID(37624) };

		inline void SGTM(float a_in) {
#if ANNIVERSARY_EDITION
			static float* g_SGTM = (float*)REL::ID(388443).address();
			*g_SGTM = a_in;
			using func_t = decltype(SGTM);
			REL::Relocation<func_t> func{ REL::ID(68246) };
#else
			static float* g_SGTM = (float*)REL::ID(511883).address();
			*g_SGTM = a_in;
			using func_t = decltype(SGTM);
			REL::Relocation<func_t> func{ REL::ID(66989) };
#endif
			return;
		}


		static float* g_worldScaleInverse = (float*)REL::ID(230692).address();
		inline static float* g_deltaTime = (float*)REL::ID(523660).address();                            // 2F6B948
		inline static float* g_deltaTimeRealTime = (float*)REL::ID(523661).address();                  // 2F6B94C
		
	}
}



