#pragma once
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

	inline bool wieldingOneHanded(RE::Actor* a) {
		if (!a->GetEquippedObject(false)) {
			return true;
		}
		RE::WEAPON_TYPE wpnType = a->GetEquippedObject(false)->As<RE::TESObjectWEAP>()->GetWeaponType();
		if (wpnType >= RE::WEAPON_TYPE::kHandToHandMelee && wpnType <= RE::WEAPON_TYPE::kOneHandMace) {
			DEBUG("player wielding one handed weapon!");
			return true;
		}
		else {
			DEBUG("player wielding two handed weapon!");
			return false;
		}
	}

	typedef void(_fastcall* _shakeCamera)(float strength, RE::NiPoint3 source, float duration);
	inline static REL::Relocation<_shakeCamera> shakeCamera{ REL::ID(32275) };


	typedef void(_fastcall* tPushActorAway_sub_14067D4A0)(RE::AIProcess* a_causer, RE::Actor* a_target, RE::NiPoint3& a_origin, float a_magnitude);
	inline static REL::Relocation<tPushActorAway_sub_14067D4A0> pushActorAway{ REL::ID(38858) };

	inline static float* g_deltaTime = (float*)REL::ID(523660).address();                            // 2F6B948
	inline static float* g_deltaTimeRealTime = (float*)REL::ID(523661).address();                  // 2F6B94C

	/*get the weapon the actor is most likely wielding. If the actor is empty handed, return nullptr.*/
	inline RE::TESObjectWEAP* getWieldingWeapon(RE::Actor* actor) {
		if (actor) {
			if (actor->GetAttackingWeapon() && actor->GetAttackingWeapon()->object) {
				return actor->GetAttackingWeapon()->object->As<RE::TESObjectWEAP>();
			}
			auto weapon = actor->GetEquippedObject(false);
			if (weapon && weapon->GetFormType() == RE::FormType::Weapon) {
				return weapon->As<RE::TESObjectWEAP>();
			}
			weapon = actor->GetEquippedObject(true);
			if (weapon && weapon->GetFormType() == RE::FormType::Weapon) {
				return weapon->As<RE::TESObjectWEAP>();
			}
		}
		return nullptr;
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


	typedef void(_fastcall* tPushActorAway_sub_14067D4A0)(RE::AIProcess* a_causer, RE::Actor* a_target, RE::NiPoint3& a_origin, float a_magnitude);
	inline static REL::Relocation<tPushActorAway_sub_14067D4A0> pushActorAway{ REL::ID(38858) };

	/*Send the target flying based on causer's location.
	@param magnitude: strength of a push.*/
	static void PushActorAway(RE::Actor* causer, RE::Actor* target, float magnitude) {
		RE::NiPoint3 vec = causer->GetPosition();
		pushActorAway(causer->currentProcess, target, vec, magnitude);
	}

};