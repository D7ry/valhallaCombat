#pragma once
#include "offsets.h"
//TODO:clear this up a bit
namespace Utils
{
	inline void damageav(RE::Actor* a, RE::ActorValue av, float val)
	{
		if (a) {
			a->As<RE::ActorValueOwner>()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, -val);
		}
	}

	inline void restoreav(RE::Actor* a, RE::ActorValue av, float val)
	{
		if (a) {
			a->As<RE::ActorValueOwner>()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, val);
		}
	}
};

class ValhallaUtils
{
public:
	/*Whether the actor's back is facing the other actor's front.
	@param actor1: actor whose facing will be returned
	@param actor2: actor whose relative location to actor1 will be calculated.*/
	static bool isBackFacing(RE::Actor* actor1, RE::Actor* actor2) {
		auto angle = actor1->GetHeadingAngle(actor2->GetPosition(), false);
		if (90 < angle || angle < -90) {
			return true;
		}
		else {
			return false;
		}
	}




	/*Send the target flying based on causer's location.
	@param magnitude: strength of a push.*/
	static void PushActorAway(RE::Actor* causer, RE::Actor* target, float magnitude) {
		RE::NiPoint3 vec = causer->GetPosition();
		RE::Offset::pushActorAway(causer->currentProcess, target, vec, magnitude);
	}

#pragma region playSound
	static inline int sub_140BEEE70(void* manager, RE::BSSoundHandle* a2, int a3, int a4)
	{
		using func_t = decltype(&sub_140BEEE70);
		REL::Relocation<func_t> func{ REL::ID(66401) };
		return func(manager, a2, a3, a4);
	}

	static inline char set_sound_position(RE::BSSoundHandle* a1, float x, float y, float z)
	{
		using func_t = decltype(&set_sound_position);
		REL::Relocation<func_t> func{ REL::ID(66370) };
		return func(a1, x, y, z);
	}

	static inline void sub_140BEDB10(RE::BSSoundHandle* a1, RE::NiAVObject* source_node)
	{
		using func_t = decltype(&sub_140BEDB10);
		REL::Relocation<func_t> func{ REL::ID(66375) };
		return func(a1, source_node);
	}

	static inline char __fastcall sub_140BED530(RE::BSSoundHandle* a1)
	{
		using func_t = decltype(&sub_140BED530);
		REL::Relocation<func_t> func{ REL::ID(66355) };
		return func(a1);
	}

	static inline void* BSAudioManager__GetSingleton()
	{
		using func_t = decltype(&BSAudioManager__GetSingleton);
		REL::Relocation<func_t> func{ RE::Offset::BSAudioManager::GetSingleton };
		return func();
	}
	/*Play sound with formid at a certain actor's position.
	@param a: actor on which to play sonud.
	@param formid: formid of the sound descriptor.*/
	static void playSound(RE::Actor* a, int formid)
	{
		RE::BSSoundHandle handle;
		handle.soundID = static_cast<uint32_t>(-1);
		handle.assumeSuccess = false;
		*(uint32_t*)&handle.state = 0;

		auto manager = BSAudioManager__GetSingleton();
		sub_140BEEE70(manager, &handle, formid, 16);
		if (set_sound_position(&handle, a->data.location.x, a->data.location.y, a->data.location.z)) {
			sub_140BEDB10(&handle, a->Get3D());
			sub_140BED530(&handle);
		}
	}

	/*Returns if actor a and actor b's relative distance does not exceed certain distance.*/
	static bool inRange(RE::Actor* a, RE::Actor* b, float radius) {
		auto aPos = a->GetPosition();
		INFO("{}'s position: {}, {}, {}", a->GetName(), aPos.x, aPos.y, aPos.z);
		auto bPos = b->GetPosition();
		INFO("{}'s position: {}, {}, {}", b->GetName(), bPos.x, bPos.y, bPos.z);
		if ((abs(aPos.x - bPos.x) < radius && abs(aPos.y - bPos.y) < radius)) {
			INFO("in range");
			return true;
		}
		else {
			INFO("not in range");
			return false;
		}
	}
#pragma endregion
};