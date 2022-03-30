#pragma once
namespace RE
{
	namespace Offset
	{

		typedef void(_fastcall* tPushActorAway_sub_14067D4A0)(RE::AIProcess* a_causer, RE::Actor* a_target, RE::NiPoint3& a_origin, float a_magnitude);
		inline static REL::Relocation<tPushActorAway_sub_14067D4A0> pushActorAway{ REL::ID(38858) };

		inline static float* g_deltaTime = (float*)REL::ID(523660).address();                            // 2F6B948
		inline static float* g_deltaTimeRealTime = (float*)REL::ID(523661).address();                  // 2F6B94C

		typedef void(_fastcall* _shakeCamera)(float strength, RE::NiPoint3 source, float duration);
		inline static REL::Relocation<_shakeCamera> shakeCamera{ REL::ID(32275) };

	}
}



