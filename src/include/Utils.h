#pragma once
#include "offsets.h"
#include "bin/ValhallaCombat.hpp"
#include "include/lib/robin_hood.h"
#include <cmath>
#include  <random>
#include  <iterator>
#define CONSOLELOG(msg) 	RE::ConsoleLog::GetSingleton()->Print(msg);
#define PI 3.1415926535897932384626
//TODO:clear this up a bit
namespace Utils
{
	inline void SetRotationMatrix(RE::NiMatrix3& a_matrix, float sacb, float cacb, float sb)
	{
		float cb = std::sqrtf(1 - sb * sb);
		float ca = cacb / cb;
		float sa = sacb / cb;
		a_matrix.entry[0][0] = ca;
		a_matrix.entry[0][1] = -sacb;
		a_matrix.entry[0][2] = sa * sb;
		a_matrix.entry[1][0] = sa;
		a_matrix.entry[1][1] = cacb;
		a_matrix.entry[1][2] = -ca * sb;
		a_matrix.entry[2][0] = 0.0;
		a_matrix.entry[2][1] = sb;
		a_matrix.entry[2][2] = cb;
	}

	template<typename Iter, typename RandomGenerator>
	Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
		std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
		std::advance(start, dis(g));
		return start;
	}

	template<typename Iter>
	Iter select_randomly(Iter start, Iter end) {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		return select_randomly(start, end, gen);
	}


	inline bool isPowerAttacking(RE::Actor* actor) {
		return
			actor->currentProcess && actor->currentProcess->high && actor->currentProcess->high->attackData
			&& actor->currentProcess->high->attackData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack);
	}
	inline void damageav(RE::Actor* a, RE::ActorValue av, float val)
	{
		if (val == 0) {
			return;
		}
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

	/*Calculate the real hit damage based on game difficulty settings, and whether the player is aggressor/victim,
	* assuming the damage is uncalculated raw damage.
	@param damage: raw damage taken from hitdata.
	@param aggressor: aggressor of this damage.
	@param victim: victim of this damage.*/
	inline void offsetRealDamage(float& damage, RE::Actor* aggressor, RE::Actor* victim) {
		auto difficulty = data::GetSingleton();
		if (aggressor->IsPlayerRef() || aggressor->IsPlayerTeammate()) {
			switch (RE::PlayerCharacter::GetSingleton()->getDifficultySetting()) {
			case RE::DIFFICULTY::kNovice: damage *= difficulty->fDiffMultHPByPCVE; break;
			case RE::DIFFICULTY::kApprentice: damage *= difficulty->fDiffMultHPByPCE; break;
			case RE::DIFFICULTY::kAdept: damage *= difficulty->fDiffMultHPByPCN; break;
			case RE::DIFFICULTY::kExpert: damage *= difficulty->fDiffMultHPByPCH; break;
			case RE::DIFFICULTY::kMaster: damage *= difficulty->fDiffMultHPByPCVH; break;
			case RE::DIFFICULTY::kLegendary: damage *= difficulty->fDiffMultHPByPCL; break;
			}
		}
		else if (victim->IsPlayerRef() || victim->IsPlayerTeammate()) {
			switch (RE::PlayerCharacter::GetSingleton()->getDifficultySetting()) {
			case RE::DIFFICULTY::kNovice: damage *= difficulty->fDiffMultHPToPCVE; break;
			case RE::DIFFICULTY::kApprentice: damage *= difficulty->fDiffMultHPToPCE; break;
			case RE::DIFFICULTY::kAdept: damage *= difficulty->fDiffMultHPToPCN; break;
			case RE::DIFFICULTY::kExpert: damage *= difficulty->fDiffMultHPToPCH; break;
			case RE::DIFFICULTY::kMaster: damage *= difficulty->fDiffMultHPToPCVH; break;
			case RE::DIFFICULTY::kLegendary: damage *= difficulty->fDiffMultHPToPCL; break;
			}
		}
	}
};

class TrueHUDUtils
{
public:
	static void flashHealthMeter(robin_hood::unordered_set<RE::Actor*> actors) {
		auto ersh = ValhallaCombat::GetSingleton()->ersh;
		auto temp_set = actors;
		for (auto a_actor : actors) {
			if (!a_actor->IsDead()) {
				ersh->FlashActorValue(a_actor->GetHandle(), RE::ActorValue::kHealth, false);
			}
		}
	}
	static void flashHealthMeter(RE::Actor* a_actor) {
		ValhallaCombat::GetSingleton()->ersh->FlashActorValue(a_actor->GetHandle(), RE::ActorValue::kHealth, false);
	}
	static void flashStaminaMeter(RE::Actor* a_actor) {

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
	static inline int soundHelper_a(void* manager, RE::BSSoundHandle* a2, int a3, int a4) //sub_140BEEE70
	{
		using func_t = decltype(&soundHelper_a);
		REL::Relocation<func_t> func{ REL::ID(66401) };
		return func(manager, a2, a3, a4);
	}


	static inline void soundHelper_b(RE::BSSoundHandle* a1, RE::NiAVObject* source_node) //sub_140BEDB10
	{
		using func_t = decltype(&soundHelper_b);
		REL::Relocation<func_t> func{ REL::ID(66375) };
		return func(a1, source_node);
	}

	static inline char __fastcall soundHelper_c(RE::BSSoundHandle* a1) //sub_140BED530
	{
		using func_t = decltype(&soundHelper_c);
		REL::Relocation<func_t> func{ REL::ID(66355) };
		return func(a1);
	}

	static inline char set_sound_position(RE::BSSoundHandle* a1, float x, float y, float z)
	{
		using func_t = decltype(&set_sound_position);
		REL::Relocation<func_t> func{ REL::ID(66370) };
		return func(a1, x, y, z);
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
	static void playSound(RE::Actor* a, RE::BGSSoundDescriptorForm* a_descriptor)
	{

		RE::BSSoundHandle handle;
		handle.soundID = static_cast<uint32_t>(-1);
		handle.assumeSuccess = false;
		*(uint32_t*)&handle.state = 0;

		auto manager = BSAudioManager__GetSingleton();
		soundHelper_a(manager, &handle, a_descriptor->GetFormID(), 16);
		if (set_sound_position(&handle, a->data.location.x, a->data.location.y, a->data.location.z)) {
			soundHelper_b(&handle, a->Get3D());
			soundHelper_c(&handle);
		}
	}

	static void playSound(RE::Actor* a, std::vector<RE::BGSSoundDescriptorForm*> sounds)
	{
		playSound(a, *Utils::select_randomly(sounds.begin(), sounds.end()));
	}

	/*
	@return actor a and actor b's absolute distance, if the radius is bigger than distance.
	@return -1, if the distance exceeds radius.*/
	static float getInRange(RE::Actor* a, RE::Actor* b, float maxRange) {
		float dist = a->GetPosition().GetDistance(b->GetPosition());
		if (dist <= maxRange) {
			return dist;
		}
		else {
			return -1;
		}
		/*
		auto aPos = a->GetPosition();
		auto bPos = b->GetPosition();
		float xDiff = abs(aPos.x - bPos.x);
		float yDiff = abs(aPos.y - bPos.y);
		float zDiff = abs(aPos.z - bPos.z);
		float dist = sqrt(
			pow(xDiff, 2) + pow(yDiff, 2) //horizontal dist ^ 2
			+ pow(zDiff, 2) //vertical dist ^ 2
		);
		if (dist <= maxRange) {
			return dist;
		}
		else {
			return -1;
		}*/
	}

	static void queueMessageBox(RE::BSFixedString a_message) {
		auto factoryManager = RE::MessageDataFactoryManager::GetSingleton();
		auto uiStrHolder = RE::InterfaceStrings::GetSingleton();
		auto factory = factoryManager->GetCreator<RE::MessageBoxData>(uiStrHolder->messageBoxData);
		auto messageBox = factory->Create();
		messageBox->unk4C = 4;
		messageBox->unk38 = 10;
		messageBox->bodyText = a_message;
		auto gameSettings = RE::GameSettingCollection::GetSingleton();
		auto sOk = gameSettings->GetSetting("sOk");
		messageBox->buttonText.push_back(sOk->GetString());
		messageBox->QueueMessage();
	}
#pragma endregion
	/*Clamp the raw damage to be no more than the aggressor's max raw melee damage output.*/
	static void clampDmg(float& dmg, RE::Actor* aggressor) {
		auto a_weapon = aggressor->getWieldingWeapon();
		if (a_weapon) {
			//DEBUG("weapon to clamp damage: {}", a_weapon->GetName());
			dmg = min(dmg, a_weapon->GetAttackDamage());
		}
	}

	static bool isCasting(RE::Actor* a_actor) {
		for (int i = 0; i < 3; i++)
		{
			auto caster = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource(i));
			if (!caster)
				continue;
			if (caster->state) {
				return true;
			}
		}
		return false;
	}

	/*Set the projectile's cause to a new actor; reset the projectile's collision mask.
	@param a_projectile: projectile to be reset.
	@param a_actor: new owner of the projectile.
	@param a_projectile_collidable: pointer to the projectile collidable to rset its collision mask.*/
	static void setProjectileCause(RE::Projectile* a_projectile, RE::Actor* a_actor, RE::hkpCollidable* a_projectile_collidable) {
		a_projectile->actorCause.get()->actor = a_actor->GetHandle();
		a_projectile->desiredTarget = a_actor->currentCombatTarget;
		a_projectile->shooter = a_actor->GetHandle();
		uint32_t a_collisionFilterInfo;
		a_actor->GetCollisionFilterInfo(a_collisionFilterInfo);
		a_projectile_collidable->broadPhaseHandle.collisionFilterInfo &= (0x0000FFFF);
		a_projectile_collidable->broadPhaseHandle.collisionFilterInfo |= (a_collisionFilterInfo << 16);

	}
	static void DeflectProjectile(RE::Projectile* a_projectile) {
		a_projectile->linearVelocity *= -1.f;
		auto projectileNode = a_projectile->Get3D2();
		if (projectileNode)
		{
			RE::NiPoint3 direction = a_projectile->linearVelocity;
			direction.Unitize();

			a_projectile->data.angle.x = asin(direction.z);
			a_projectile->data.angle.z = atan2(direction.x, direction.y);

			if (a_projectile->data.angle.z < 0.0) {
				a_projectile->data.angle.z += PI;
			}

			if (direction.x < 0.0) {
				a_projectile->data.angle.z += PI;
			}

			Utils::SetRotationMatrix(projectileNode->local.rotate, -direction.x, direction.y, direction.z);
		}
	}
	static void ReflectProjectile(RE::Projectile* a_projectile)
	{
		a_projectile->linearVelocity *= -1.f;

		// rotate model
		auto projectileNode = a_projectile->Get3D2();
		if (projectileNode)
		{
			RE::NiPoint3 direction = a_projectile->linearVelocity;
			direction.Unitize();

			a_projectile->data.angle.x = asin(direction.z);
			a_projectile->data.angle.z = atan2(direction.x, direction.y);

			if (a_projectile->data.angle.z < 0.0) {
				a_projectile->data.angle.z += PI;
			}

			if (direction.x < 0.0) {
				a_projectile->data.angle.z += PI;
			}

			Utils::SetRotationMatrix(projectileNode->local.rotate, -direction.x, direction.y, direction.z);
		}
	}
};