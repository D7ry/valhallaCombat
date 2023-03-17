#include "include/Utils.h"


bool Utils::Actor::isHumanoid(RE::Actor* a_actor)
{
	auto bodyPartData = a_actor->GetRace() ? a_actor->GetRace()->bodyPartData : nullptr;
	return bodyPartData && bodyPartData->GetFormID() == 0x1d;
}

RE::TESObjectWEAP* Utils::Actor::getWieldingWeapon(RE::Actor* a_actor)
{
	auto weapon = a_actor->GetAttackingWeapon();
	if (weapon) {
		return weapon->object->As<RE::TESObjectWEAP>();
	}
	auto rhs = a_actor->GetEquippedObject(false);
	if (rhs && rhs->IsWeapon()) {
		return rhs->As<RE::TESObjectWEAP>();
	}
	auto lhs = a_actor->GetEquippedObject(true);
	if (lhs && lhs->IsWeapon()) {
		return lhs->As<RE::TESObjectWEAP>();
	}
	return nullptr;
}

bool Utils::Actor::isDualWielding(RE::Actor* a_actor)
{
	auto lhs = a_actor->GetEquippedObject(true);
	auto rhs = a_actor->GetEquippedObject(false);
	if (lhs && rhs && lhs->IsWeapon() && rhs->IsWeapon()) {
		auto weaponType = rhs->As<RE::TESObjectWEAP>()->GetWeaponType();
		return weaponType != RE::WEAPON_TYPE::kTwoHandAxe && weaponType != RE::WEAPON_TYPE::kTwoHandSword;  //can't be two hand sword.
	}
	return false;
}

bool Utils::Actor::isEquippedShield(RE::Actor* a_actor)
{
	return RE::Offset::getEquippedShield(a_actor);
}

bool Utils::Actor::isPowerAttacking(RE::Actor* a_actor)
{
	auto currentProcess = a_actor->GetActorRuntimeData().currentProcess;
	if (currentProcess) {
		auto highProcess = currentProcess->high;
		if (highProcess) {
			auto attackData = highProcess->attackData;
			if (attackData) {
				auto flags = attackData->data.flags;
				return flags.any(RE::AttackData::AttackFlag::kPowerAttack) && !flags.any(RE::AttackData::AttackFlag::kBashAttack);
			}
		}
	}
	return false;
}

void Utils::Actor::damageav(RE::Actor* a, RE::ActorValue av, float val)
{
	if (val == 0) {
		return;
	}
	if (a) {
		a->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, -val);
	}
}

/*Try to damage this actor's actorvalue. If the actor does not have enough value, do not damage and return false;*/
bool Utils::Actor::tryDamageAv(RE::Actor* a_actor, RE::ActorValue a_actorValue, float a_damage)
{
	auto currentAv = a_actor->AsActorValueOwner()->GetActorValue(a_actorValue);
	if (currentAv - a_damage <= 0) {
		return false;
	}
	damageav(a_actor, a_actorValue, a_damage);
	return true;
}

void Utils::Actor::restoreav(RE::Actor* a_actor, RE::ActorValue a_actorValue, float a_value)
{
	if (a_value == 0) {
		return;
	}
	if (a_actor) {
		a_actor->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, a_actorValue, a_value);
	}
}

/*Complete refills this actor's actor value.
		@param a_actor actor whose actorValue will be refilled.
		@param actorValue type of actor value to refill.*/
void Utils::Actor::refillActorValue(RE::Actor* a_actor, RE::ActorValue a_actorValue)
{
	float av = a_actor->AsActorValueOwner()->GetActorValue(a_actorValue);
	float pav = a_actor->AsActorValueOwner()->GetPermanentActorValue(a_actorValue);
	if (av >= pav) {
		return;
	}
	float avToRestore = pav - av;
	restoreav(a_actor, a_actorValue, avToRestore);
}

void Utils::Actor::safeApplySpell(RE::SpellItem* a_spell, RE::Actor* a_actor)
{
	if (a_actor && a_spell) {
		a_actor->AddSpell(a_spell);
	}
}

void Utils::Actor::safeRemoveSpell(RE::SpellItem* a_spell, RE::Actor* a_actor)
{
	if (a_actor && a_spell) {
		a_actor->RemoveSpell(a_spell);
	}
}
void Utils::Actor::safeApplyPerk(RE::BGSPerk* a_perk, RE::Actor* a_actor)
{
	if (a_actor && a_perk && !a_actor->HasPerk(a_perk)) {
		a_actor->AddPerk(a_perk);
	}
}

void Utils::Actor::safeRemovePerk(RE::BGSPerk* a_perk, RE::Actor* a_actor)
{
	if (a_actor && a_perk && a_actor->HasPerk(a_perk)) {
		a_actor->RemovePerk(a_perk);
	}
}

bool Utils::Actor::isBackFacing(RE::Actor* actor1, RE::Actor* actor2)
{
	auto angle = actor1->GetHeadingAngle(actor2);
	if (90 < angle || angle < -90) {
		return true;
	} else {
		return false;
	}
}
inline bool isJumping(RE::Actor* a_actor)
{
	bool result = false;
	return a_actor->GetGraphVariableBool("bInJumpState", result) && result;
}

/* Whether the actor is blocking or is in a state at which a block input transfer the actor into blocking state.*/
bool Utils::Actor::canBlock(RE::Actor* a_actor)
{
	if (a_actor->IsBlocking()) {
		return true;
	}
	RE::ActorState* state = a_actor->AsActorState();
	return !isJumping(a_actor) && state->GetKnockState() == RE::KNOCK_STATE_ENUM::kNormal && state->GetAttackState() == RE::ATTACK_STATE_ENUM::kNone && !state->IsSwimming() && state->IsWeaponDrawn() && !state->IsSprinting() && !state->IsStaggered();
}

bool Utils::Actor::getGraphVariable(bool& r_gv, RE::Actor* a_actor, const RE::BSFixedString& a_variableName)
{
	if (!a_actor) {
		return false;
	}
	return a_actor->GetGraphVariableBool(a_variableName, r_gv);
}

bool Utils::Actor::getGraphVariable(float& r_gv, RE::Actor* a_actor, const RE::BSFixedString& a_variableName)
{
	if (!a_actor) {
		return false;
	}
	return a_actor->GetGraphVariableFloat(a_variableName, r_gv);
}

bool Utils::Actor::getGraphVariable(int& r_gv, RE::Actor* a_actor, const RE::BSFixedString& a_variableName)
{
	if (!a_actor) {
		return false;
	}
	return a_actor->GetGraphVariableInt(a_variableName, r_gv);
}


bool Utils::Actor::isBashing(RE::Actor* a_actor)
{
	if (a_actor->AsActorState()->GetAttackState() == RE::ATTACK_STATE_ENUM::kBash) {
		return true;
	}
	auto currentProcess = a_actor->GetActorRuntimeData().currentProcess;
	if (currentProcess) {
		auto highProcess = currentProcess->high;
		if (highProcess) {
			auto attackData = highProcess->attackData;
			if (attackData) {
				auto flags = attackData->data.flags;
				return flags.any(RE::AttackData::AttackFlag::kBashAttack);
			}
		}
	}
	return false;
}
