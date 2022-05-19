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


	inline bool isPowerAttacking(RE::Actor* a_actor) {
		return
			a_actor->currentProcess && a_actor->currentProcess->high && a_actor->currentProcess->high->attackData
			&& a_actor->currentProcess->high->attackData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack);
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

	/*Try to damage this actor's actorvalue. If the actor does not have enough value, do not damage and return false;*/
	inline bool tryDamageAv(RE::Actor* a_actor, RE::ActorValue a_actorValue, float a_damage) {
		auto currentAv = a_actor->GetActorValue(a_actorValue);
		if (currentAv - a_damage <= 0) {
			return false;
		}
		damageav(a_actor, a_actorValue, a_damage);
		return true;
	}

	inline void restoreav(RE::Actor* a_actor, RE::ActorValue a_actorValue, float a_damage)
	{
		if (a_actor) {
			a_actor->As<RE::ActorValueOwner>()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, a_actorValue, a_damage);
		}
	}

	/*Slow down game time for a set period.
	@param a_duration: duration of the slow time.
	@param a_percentage: relative time speed to normal time(1).*/
	inline void slowTime(float a_duration, float a_percentage) {
		int duration_milisec = static_cast<int>(a_duration * 1000);
		RE::Offset::SGTM(a_percentage);
		/*Reset time here*/
		auto resetSlowTime = [](int a_duration) {
			std::this_thread::sleep_for(std::chrono::milliseconds(a_duration));
			RE::Offset::SGTM(1);
		};
		std::jthread resetThread(resetSlowTime, duration_milisec);
		resetThread.detach();
	}

	/*Calculate the real hit damage based on game difficulty settings, and whether the player is aggressor/victim,
	* assuming the damage is uncalculated raw damage.
	@param damage: raw damage taken from hitdata.
	@param aggressor: aggressor of this damage.
	@param victim: victim of this damage.*/
	inline void offsetRealDamage(float& damage, RE::Actor* aggressor, RE::Actor* victim) {
		if ((aggressor) && (aggressor->IsPlayerRef() || aggressor->IsPlayerTeammate())) {
			switch (RE::PlayerCharacter::GetSingleton()->getDifficultySetting()) {
			case RE::DIFFICULTY::kNovice: damage *= data::fDiffMultHPByPCVE; break;
			case RE::DIFFICULTY::kApprentice: damage *= data::fDiffMultHPByPCE; break;
			case RE::DIFFICULTY::kAdept: damage *= data::fDiffMultHPByPCN; break;
			case RE::DIFFICULTY::kExpert: damage *= data::fDiffMultHPByPCH; break;
			case RE::DIFFICULTY::kMaster: damage *= data::fDiffMultHPByPCVH; break;
			case RE::DIFFICULTY::kLegendary: damage *= data::fDiffMultHPByPCL; break;
			}
		}
		else if ((victim) && (victim->IsPlayerRef() || victim->IsPlayerTeammate())) {
			switch (RE::PlayerCharacter::GetSingleton()->getDifficultySetting()) {
			case RE::DIFFICULTY::kNovice: damage *= data::fDiffMultHPToPCVE; break;
			case RE::DIFFICULTY::kApprentice: damage *= data::fDiffMultHPToPCE; break;
			case RE::DIFFICULTY::kAdept: damage *= data::fDiffMultHPToPCN; break;
			case RE::DIFFICULTY::kExpert: damage *= data::fDiffMultHPToPCH; break;
			case RE::DIFFICULTY::kMaster: damage *= data::fDiffMultHPToPCVH; break;
			case RE::DIFFICULTY::kLegendary: damage *= data::fDiffMultHPToPCL; break;
			}
		}
	}

	inline void offsetRealDamageForPc(float& damage) {
		offsetRealDamage(damage, nullptr, RE::PlayerCharacter::GetSingleton());
	}

	inline void safeApplySpell(RE::SpellItem* a_spell, RE::Actor* a_actor) {
		if (a_actor && a_spell) {
			a_actor->AddSpell(a_spell);
		}
	}

	inline void safeRemoveSpell(RE::SpellItem* a_spell, RE::Actor* a_actor) {
		if (a_actor && a_spell) {
			a_actor->RemoveSpell(a_spell);
		}
	}

	inline void safeApplyPerk(RE::BGSPerk* a_perk, RE::Actor* a_actor) {
		if (a_actor && a_perk && !a_actor->HasPerk(a_perk)) {
			a_actor->AddPerk(a_perk);
		}
	}

	inline void safeRemovePerk(RE::BGSPerk* a_perk, RE::Actor* a_actor) {
		if (a_actor && a_perk && a_actor->HasPerk(a_perk)) {
			a_actor->RemovePerk(a_perk);
		}
	}

	/*Complete refills this actor's actor value.
	@param a_actor actor whose actorValue will be refilled.
	@param actorValue type of actor value to refill.*/
	inline void refillActorValue(RE::Actor* a_actor, RE::ActorValue a_actorValue) {
		float avToRestore = a_actor->GetPermanentActorValue(a_actorValue)
			+ a_actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, a_actorValue)
			- a_actor->GetActorValue(a_actorValue);
		restoreav(a_actor, a_actorValue, avToRestore);
	}
};

namespace TrueHUDUtils
{
	inline void flashHealthMeter(robin_hood::unordered_set<RE::Actor*> actors) {
		if (!settings::TrueHudAPI_Obtained) {
			return;
		}
		auto ersh = ValhallaCombat::GetSingleton()->ersh;
		auto temp_set = actors;
		for (auto a_actor : actors) {
			if (!a_actor->IsDead()) {
				ersh->FlashActorValue(a_actor->GetHandle(), RE::ActorValue::kHealth, false);
			}
		}
	}

	inline void flashActorValue(RE::Actor* a_actor, RE::ActorValue actorValue) {
		if (!settings::TrueHudAPI_Obtained) {
			return;
		}
		if (a_actor) {
			ValhallaCombat::GetSingleton()->ersh->FlashActorValue(a_actor->GetHandle(), actorValue, true);
		}
		
	}

	inline void greyoutAvMeter(RE::Actor* a_actor, RE::ActorValue actorValue) {
		if (!settings::TrueHudAPI_Obtained) {
			return;
		}
		auto ersh = ValhallaCombat::GetSingleton()->ersh;
		ersh->OverrideBarColor(a_actor->GetHandle(), actorValue, TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
		ersh->OverrideBarColor(a_actor->GetHandle(), actorValue, TRUEHUD_API::BarColorType::BarColor, 0x7d7e7d);
		ersh->OverrideBarColor(a_actor->GetHandle(), actorValue, TRUEHUD_API::BarColorType::PhantomColor, 0xb30d10);
	}

	inline void revertAvMeter(RE::Actor* a_actor, RE::ActorValue actorValue) {
		if (!settings::TrueHudAPI_Obtained) {
			return;
		}
		auto ersh = ValhallaCombat::GetSingleton()->ersh;
		ersh->RevertBarColor(a_actor->GetHandle(), actorValue, TRUEHUD_API::BarColorType::FlashColor);
		ersh->RevertBarColor(a_actor->GetHandle(), actorValue, TRUEHUD_API::BarColorType::BarColor);
		ersh->RevertBarColor(a_actor->GetHandle(), actorValue, TRUEHUD_API::BarColorType::PhantomColor);
	}

	inline void greyOutSpecialMeter(RE::Actor* a_actor) {
		if (!settings::TrueHudAPI_Obtained) {
			return;
		}
		auto ersh = ValhallaCombat::GetSingleton()->ersh;
		ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
		ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::BarColor, 0x7d7e7d);
		ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::PhantomColor, 0xb30d10);
		ersh->OverrideBarColor(a_actor->GetHandle(), RE::ActorValue::kHealth, TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
	}

	inline void revertSpecialMeter(RE::Actor* a_actor) {
		if (!settings::TrueHudAPI_Obtained) {
			return;
		}
		auto ersh = ValhallaCombat::GetSingleton()->ersh;
		ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::FlashColor);
		ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::BarColor);
		ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::PhantomColor);
		ersh->RevertBarColor(a_actor->GetHandle(), RE::ActorValue::kHealth, TRUEHUD_API::BarColorType::FlashColor);
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
	inline static void PushActorAway(RE::Actor* causer, RE::Actor* target, float magnitude) {
		auto targetPoint = causer->GetNodeByName(causer->race->bodyPartData->parts[0]->targetName.c_str());
		RE::NiPoint3 vec = targetPoint->world.translate;
		//RE::NiPoint3 vec = causer->GetPosition();
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
	static void resetProjectileOwner(RE::Projectile* a_projectile, RE::Actor* a_actor, RE::hkpCollidable* a_projectile_collidable) {
		a_projectile->actorCause.get()->actor = a_actor->GetHandle();
		a_projectile->desiredTarget = a_actor->currentCombatTarget;
		a_projectile->shooter = a_actor->GetHandle();
		uint32_t a_collisionFilterInfo;
		a_actor->GetCollisionFilterInfo(a_collisionFilterInfo);
		a_projectile_collidable->broadPhaseHandle.collisionFilterInfo &= (0x0000FFFF);
		a_projectile_collidable->broadPhaseHandle.collisionFilterInfo |= (a_collisionFilterInfo << 16);
	}

	static inline bool ApproximatelyEqual(float A, float B)
	{
		return ((A - B) < FLT_EPSILON) && ((B - A) < FLT_EPSILON);
	}

	static bool PredictAimProjectile(RE::NiPoint3 a_projectilePos, RE::NiPoint3 a_targetPosition, RE::NiPoint3 a_targetVelocity, float a_gravity, RE::NiPoint3& a_projectileVelocity)
	{
		// http://ringofblades.com/Blades/Code/PredictiveAim.cs

		float projectileSpeedSquared = a_projectileVelocity.SqrLength();
		float projectileSpeed = std::sqrtf(projectileSpeedSquared);

		if (projectileSpeed <= 0.f || a_projectilePos == a_targetPosition) {
			return false;
		}

		float targetSpeedSquared = a_targetVelocity.SqrLength();
		float targetSpeed = std::sqrtf(targetSpeedSquared);
		RE::NiPoint3 targetToProjectile = a_projectilePos - a_targetPosition;
		float distanceSquared = targetToProjectile.SqrLength();
		float distance = std::sqrtf(distanceSquared);
		RE::NiPoint3 direction = targetToProjectile;
		direction.Unitize();
		RE::NiPoint3 targetVelocityDirection = a_targetVelocity;
		targetVelocityDirection.Unitize();

		float cosTheta = (targetSpeedSquared > 0)
			? direction.Dot(targetVelocityDirection)
			: 1.0f;

		bool bValidSolutionFound = true;
		float t;

		if (ApproximatelyEqual(projectileSpeedSquared, targetSpeedSquared)) {
			// We want to avoid div/0 that can result from target and projectile traveling at the same speed
			//We know that cos(theta) of zero or less means there is no solution, since that would mean B goes backwards or leads to div/0 (infinity)
			if (cosTheta > 0) {
				t = 0.5f * distance / (targetSpeed * cosTheta);
			}
			else {
				bValidSolutionFound = false;
				t = 1;
			}
		}
		else {
			float a = projectileSpeedSquared - targetSpeedSquared;
			float b = 2.0f * distance * targetSpeed * cosTheta;
			float c = -distanceSquared;
			float discriminant = b * b - 4.0f * a * c;

			if (discriminant < 0) {
				// NaN
				bValidSolutionFound = false;
				t = 1;
			}
			else {
				// a will never be zero
				float uglyNumber = sqrtf(discriminant);
				float t0 = 0.5f * (-b + uglyNumber) / a;
				float t1 = 0.5f * (-b - uglyNumber) / a;

				// Assign the lowest positive time to t to aim at the earliest hit
				t = min(t0, t1);
				if (t < FLT_EPSILON) {
					t = max(t0, t1);
				}

				if (t < FLT_EPSILON) {
					// Time can't flow backwards when it comes to aiming.
					// No real solution was found, take a wild shot at the target's future location
					bValidSolutionFound = false;
					t = 1;
				}
			}
		}

		a_projectileVelocity = a_targetVelocity + (-targetToProjectile / t);

		if (!bValidSolutionFound)
		{
			a_projectileVelocity.Unitize();
			a_projectileVelocity *= projectileSpeed;
		}

		if (!ApproximatelyEqual(a_gravity, 0.f))
		{
			float netFallDistance = (a_projectileVelocity * t).z;
			float gravityCompensationSpeed = (netFallDistance + 0.5f * a_gravity * t * t) / t;
			a_projectileVelocity.z = gravityCompensationSpeed;
		}

		return bValidSolutionFound;
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
	/*Deflect this projectile, aiming it at a_target.*/
	static void DeflectProjectile(RE::Actor* a_actor, RE::Projectile* a_projectile, RE::Actor* a_target) {
		auto projectileNode = a_projectile->Get3D2();
		auto target = a_target->GetHandle();

		RE::BGSBodyPart* bodyPart = a_target->race->bodyPartData->parts[0];
		
		auto targetPoint = a_target->GetNodeByName(bodyPart->targetName.c_str());
		if (!targetPoint) {
			ReflectProjectile(a_projectile);
		}

		RE::NiPoint3 targetPos = targetPoint->world.translate;
		RE::NiPoint3 targetVelocity;
		target.get()->GetLinearVelocity(targetVelocity);

		float projectileGravity = 0.f;
		if (auto ammo = a_projectile->ammoSource) {
			if (auto bgsProjectile = ammo->data.projectile) {
				projectileGravity = bgsProjectile->data.gravity;
				if (auto bhkWorld = a_projectile->parentCell->GetbhkWorld()) {
					if (auto hkpWorld = bhkWorld->GetWorld1()) {
						auto vec4 = hkpWorld->gravity;
						float quad[4];
						_mm_store_ps(quad, vec4.quad);
						float gravity = -quad[2] * *RE::Offset::g_worldScaleInverse;
						projectileGravity *= gravity;
					}
				}
			}
		}

		PredictAimProjectile(a_projectile->data.location, targetPos, targetVelocity, projectileGravity, a_projectile->linearVelocity);

		// rotate
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
};