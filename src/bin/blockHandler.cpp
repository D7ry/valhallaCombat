#include "include/blockHandler.h"
#include "include/data.h"
#include "include/settings.h"
#include "include/Utils.h"
#include "include/stunHandler.h"
#include "include/balanceHandler.h"
#include "include/reactionHandler.h"
#include "include/AI.h"
#include "include/lib/BlockSpark.h"
#include "include/offsets.h"
#include "ValhallaCombat.hpp"
using HITFLAG = RE::HitData::Flag;
/*Called every frame.
Decrement the timer for actors either perfect blocking or cooling down.*/
void blockHandler::update() {
	if (!isPcTimedBlocking && !isPcBlockingCoolDown && !bKeyUpTimeBuffer && !isPcTackling && !isPcTackleCooldown) {
		ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::HANDLER::blockHandler);
	}

	if (bKeyUpTimeBuffer) {
		pcTimedBlockKeyUpTimer -= *RE::Offset::g_deltaTime;
		if (pcTimedBlockKeyUpTimer <= 0) {
			bKeyUpTimeBuffer = false;
			if (isPcTimedBlocking) {
				onPcTimedBlockEnd();
			}
		}
	}
	if (isPcTimedBlocking) {
		pcTimedBlockTimer -= *RE::Offset::g_deltaTime;
		if (pcTimedBlockTimer <= 0 && isPcTimedBlocking) {
			onPcTimedBlockEnd();
		}
	}
	if (isPcBlockingCoolDown) {
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (pc && !pc->IsBlocking()) {
			pcTimedBlockCooldownTimer -= *RE::Offset::g_deltaTime;
			if (pcTimedBlockCooldownTimer <= 0) {
				switch (pcBlockWindowPenalty) {
				case blockWindowPenaltyLevel::light:
					pcBlockWindowPenalty = none;
					isPcBlockingCoolDown = false;
					break;
				case blockWindowPenaltyLevel::medium:
					pcBlockWindowPenalty = light;
					break;
				case blockWindowPenaltyLevel::heavy:
					pcBlockWindowPenalty = medium;
					break;
				}
				pcTimedBlockCooldownTimer = settings::fTimedBlockCooldownTime;
			}
		}
		
	}

	if (isPcTackling) {
		pcTackleTimer -= *RE::Offset::g_deltaTime;
		if (pcTackleTimer <= 0) {
			isPcTackling = false;
			isPcTackleCooldown = true;
			pcTackleCooldownTimer = settings::fTackleCooldownTime;
		}
	}

	if (isPcTackleCooldown) {
		pcTackleCooldownTimer -= *RE::Offset::g_deltaTime;
		if (pcTackleCooldownTimer <= 0) {
			isPcTackleCooldown = false;
		}
	}
	
}
void blockHandler::onPcTimedBlockEnd() {
	if (!isPcTimedBlockSuccess) {//apply penalty only if the current block is not a successful timed block.
		isPcBlockingCoolDown = true; //not a successful timed block, start penalty cool down.
		pcTimedBlockCooldownTimer = settings::fTimedBlockCooldownTime;
		//increment penalty.
		switch (pcBlockWindowPenalty) {
		case blockWindowPenaltyLevel::none: pcBlockWindowPenalty = light; break;
		case blockWindowPenaltyLevel::light: pcBlockWindowPenalty = medium; break;
		case blockWindowPenaltyLevel::medium: pcBlockWindowPenalty = heavy; break;
		}
		ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::HANDLER::blockHandler);

	}
	else {//previous timed block is successful
		isPcTimedBlockSuccess = false; //reset success state.
	}
	isPcTimedBlocking = false;
	bKeyUpTimeBuffer = false;
}

inline bool blockHandler::isTimedBlockElapsedTimeLessThan(float a_in)
{
	return settings::fTimedBlockWindow - pcTimedBlockTimer <= a_in;
}

void blockHandler::onBlockKeyDown() {
	if (isPcTimedBlocking) {
		onPcTimedBlockEnd();
	}

	isPcTimedBlocking = true;
	isBlockButtonPressed = true;
	isPcTimedBlockSuccess = false; //reset success state
	float blockWindow = settings::fTimedBlockWindow;
	
	switch (pcBlockWindowPenalty) {
	case blockWindowPenaltyLevel::none: break;
	case blockWindowPenaltyLevel::light: blockWindow *= 0.8; break;
	case blockWindowPenaltyLevel::medium: blockWindow *= 0.5; break;
	case blockWindowPenaltyLevel::heavy: blockWindow *= 0.3; break;
	}
	pcTimedBlockTimer = blockWindow;
	ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::HANDLER::blockHandler);
}

void blockHandler::onTackleKeyDown() {
	auto pc = RE::PlayerCharacter::GetSingleton();
	if (!pc || !inlineUtils::isPowerAttacking(pc)) {
		return;
	}
	if (isPcTackling || isPcTackleCooldown) {
		return;
	}
	isPcTackling = true;
	pcTackleTimer = settings::fTackleWindow;
	ValhallaUtils::playSound(pc, data::soundTackleStart);
	ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::HANDLER::blockHandler);
}

void blockHandler::onBlockKeyUp() {
	isBlockButtonPressed = false;
	auto pc = RE::PlayerCharacter::GetSingleton();
	if (pc && isPcTimedBlocking) {
		pcTimedBlockKeyUpTimer = 0.1;
		bKeyUpTimeBuffer = true;
	}
}

void blockHandler::onBlockStop() {
	bKeyUpTimeBuffer = false;
	onPcTimedBlockEnd();
}

void blockHandler::OnPcSuccessfulTimedBlock() {
	isPcTimedBlockSuccess = true;
	pcBlockWindowPenalty = blockWindowPenaltyLevel::none;
}

bool blockHandler::isInBlockAngle(RE::Actor* blocker, RE::TESObjectREFR* a_obj) 
{
	auto angle = blocker->GetHeadingAngle(a_obj->GetPosition(), false);
	return (angle <= data::fCombatHitConeAngle && angle >= -data::fCombatHitConeAngle);
}

bool blockHandler::tryParryProjectile_Spell(RE::Actor* a_blocker, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable)
{
	if (getIsPcTimedBlocking()) {
		OnPcSuccessfulTimedBlock();
		if (a_blocker->IsPlayerRef()) {
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (pc) {
				pc->AddSkillExperience(RE::ActorValue::kBlock, settings::fTimedBlockProjectileExp);
			}
		}
		float cost = a_projectile->spell->CalculateMagickaCost(a_blocker);
		if (inlineUtils::tryDamageAv(a_blocker, RE::ActorValue::kMagicka, cost)) {  //parry only happnens when there's enough magicka.
			deflectProjectile(a_blocker, a_projectile, a_projectile_collidable);
		} else {
			destroyProjectile(a_projectile);
		}
		if (a_blocker->IsBlocking()) {
			a_blocker->NotifyAnimationGraph("BlockHitStart");
		}
		playBlockEffects(a_blocker, nullptr, blockHandler::blockType::timed);
		return true;
	}
	return false;
}

bool blockHandler::tryParryProjectile_Arrow(RE::Actor* a_blocker, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable) 
{
	if (getIsPcTimedBlocking()) {
		OnPcSuccessfulTimedBlock();
		if (a_blocker->IsPlayerRef()) {
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (pc) {
				pc->AddSkillExperience(RE::ActorValue::kBlock, settings::fTimedBlockProjectileExp);
			}
		}
		auto launcher = a_projectile->weaponSource;
		auto ammo = a_projectile->ammoSource;
		float cost = 0;
		if (launcher) {
			cost += launcher->GetAttackDamage();
		}
		if (ammo) {
			cost += ammo->data.damage;
		}
		inlineUtils::offsetRealDamageForPc(cost);
		if (inlineUtils::tryDamageAv(a_blocker, RE::ActorValue::kMagicka, cost)) { //parry only happens when there's enough magicka
			deflectProjectile(a_blocker, a_projectile, a_projectile_collidable);
		} else {
			destroyProjectile(a_projectile);
		}
		if (a_blocker->IsBlocking()) {
			a_blocker->NotifyAnimationGraph("BlockHitStart");
		}
		playBlockEffects(a_blocker, nullptr, blockHandler::blockType::timed);
		return true;
	}
	return false;
}


bool blockHandler::tryBlockProjectile_Spell(RE::Actor* a_blocker, RE::Projectile* a_projectile) 
{
	auto cost = a_projectile->spell->CalculateMagickaCost(a_blocker);
	if (inlineUtils::tryDamageAv(a_blocker, RE::ActorValue::kMagicka, cost)) {
		if (a_blocker->IsPlayerRef()) {
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (pc) {
				pc->AddSkillExperience(RE::ActorValue::kBlock, settings::fBlockProjectileExp);
			}
		}
		destroyProjectile(a_projectile);
		if (a_blocker->IsBlocking()) {
			a_blocker->NotifyAnimationGraph("BlockHitStart");
		}
		playBlockEffects(a_blocker, nullptr, blockHandler::blockType::timed);
		return true;
	}
	return false;
}

bool blockHandler::tryBlockProjectile_Arrow(RE::Actor* a_blocker, RE::Projectile* a_projectile) 
{
	auto launcher = a_projectile->weaponSource;
	auto ammo = a_projectile->ammoSource;
	float cost = 0;
	if (launcher) {
		cost += launcher->GetAttackDamage();
	}
	if (ammo) {
		cost += ammo->data.damage;
	}
	
	inlineUtils::offsetRealDamageForPc(cost);
	if (inlineUtils::tryDamageAv(a_blocker, RE::ActorValue::kMagicka, cost)) {
		if (a_blocker->IsPlayerRef()) {
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (pc) {
				pc->AddSkillExperience(RE::ActorValue::kBlock, settings::fBlockProjectileExp);
			}
		}
		if (a_blocker->IsBlocking()) {
			a_blocker->NotifyAnimationGraph("BlockHitStart");
		}
		playBlockEffects(a_blocker, nullptr, blockHandler::blockType::timed);
		destroyProjectile(a_projectile);
		return true;
	}
	return false;
}


bool blockHandler::processProjectileBlock(RE::Actor* a_blocker, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable) {
	if (a_blocker->IsPlayerRef()) {
		if (isInBlockAngle(a_blocker, a_projectile) && a_blocker->IsBlocking()) {
			
			//try timed block
			if (settings::bTimedBlockProjectileToggle) {
				if (a_projectile->spell && tryParryProjectile_Spell(a_blocker, a_projectile, a_projectile_collidable)) {
					return true;
				} else if (tryParryProjectile_Arrow(a_blocker, a_projectile, a_projectile_collidable)) {
					return true;
				}
			}  
			
			//try projectile parry
			if (settings::bBlockProjectileToggle) {
				if (a_projectile->spell) {
					return tryBlockProjectile_Spell(a_blocker, a_projectile);
				} else if (!inlineUtils::actor::isEquippedShield(a_blocker)) {  //physical projectile blocking only applies to none-shield.
					return tryBlockProjectile_Arrow(a_blocker, a_projectile);
				}
			}
		}
		
	}
	return false;
}

void blockHandler::deflectProjectile(RE::Actor* a_deflector, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable) 
{
	RE::TESObjectREFR* shooter = nullptr;
	if (a_projectile->shooter && a_projectile->shooter.get()) {
		shooter = a_projectile->shooter.get().get();
	}
	ValhallaUtils::resetProjectileOwner(a_projectile, a_deflector, a_projectile_collidable);

	if (shooter && shooter->Is3DLoaded()) {
		ValhallaUtils::RetargetProjectile(a_projectile, shooter);
	} else {
		ValhallaUtils::ReflectProjectile(a_projectile);
	}

}

void blockHandler::destroyProjectile(RE::Projectile* a_projectile) 
{
	RE::Offset::destroyProjectile(a_projectile);
}

#pragma region Process Block
void blockHandler::processPhysicalBlock(RE::Actor* blocker, RE::Actor* aggressor, SKSE::stl::enumeration<RE::HitData::Flag, std::uint32_t> a_hitFlag, RE::HitData& hitData)
{
	if (settings::bBlockStaminaToggle) {
		processStaminaBlock(blocker, aggressor, a_hitFlag, hitData);
	}
}


/// <returns>The stamina cost multiplier for a stamina block, based on the plugin settings.</returns>
inline float getBlockStaminaCostMult(RE::Actor* blocker, RE::Actor* aggressor, SKSE::stl::enumeration<RE::HitData::Flag, std::uint32_t> a_hitFlag)
{

	if (a_hitFlag.any(HITFLAG::kBlockWithWeapon)) {
		//DEBUG("hit blocked with weapon");
		if (blocker->IsPlayerRef()) {
			return settings::fBckWpnStaminaMult_PC_Block_NPC;
		}
		else {
			if (aggressor->IsPlayerRef()) {
				return settings::fBckWpnStaminaMult_NPC_Block_PC;
			}
			else {
				return settings::fBckWpnStaminaMult_NPC_Block_NPC;
			}
		}
	}
	else {
		//DEBUG("hit blocked with shield");
		if (blocker->IsPlayerRef()) {
			return settings::fBckShdStaminaMult_PC_Block_NPC;
		}
		else {
			if (aggressor->IsPlayerRef()) {
				return settings::fBckShdStaminaMult_NPC_Block_PC;
			}
			else {
				return settings::fBckShdStaminaMult_NPC_Block_NPC;
			}
		}
	}
}

void blockHandler::processStaminaBlock(RE::Actor* a_blocker, RE::Actor* a_aggressor, SKSE::stl::enumeration<RE::HitData::Flag, std::uint32_t> a_hitFlag, RE::HitData& a_hitData)
{
	using HITFLAG = RE::HitData::Flag;	
	float staminaDamage = a_hitData.totalDamage;

	inlineUtils::offsetRealDamage(staminaDamage, a_aggressor, a_blocker);
	float staminaDamageMult = getBlockStaminaCostMult(a_blocker, a_aggressor, a_hitFlag);
	staminaDamage *= staminaDamageMult;
	float targetStamina = a_blocker->GetActorValue(RE::ActorValue::kStamina);

	//check whether there's enough stamina to block incoming attack
	if (targetStamina < staminaDamage) {
		if (settings::bGuardBreak) {
			if (a_hitFlag.any(HITFLAG::kPowerAttack)) {
				reactionHandler::triggerStagger(a_aggressor, a_blocker, reactionHandler::kLarge);
			}
			else {
				reactionHandler::triggerStagger(a_aggressor, a_blocker, reactionHandler::kMedium);
			}
		}
		a_hitData.totalDamage =
			(staminaDamage - (targetStamina / staminaDamageMult))  //real damage actor will be receiving.
			* (a_hitData.totalDamage) / staminaDamage;             //offset real damage back into raw damage to be converted into real damage again later.
		
		inlineUtils::damageav(a_blocker, RE::ActorValue::kStamina,targetStamina);
		debuffHandler::GetSingleton()->initStaminaDebuff(a_blocker); //initialize debuff for the failed blocking attempt
	}
	else {
		a_hitData.totalDamage = 0;
		inlineUtils::damageav(a_blocker, RE::ActorValue::kStamina,
			staminaDamage);
	}
}
bool blockHandler::getIsPcTimedBlocking() {
	return isPcTimedBlocking;
}

bool blockHandler::getIsPcPerfectBlocking() {
	return isTimedBlockElapsedTimeLessThan(settings::fPerfectBlockWindow);
}


bool blockHandler::processMeleeTimedBlock(RE::Actor* a_blocker, RE::Actor* a_attacker) {
	if (!a_blocker->IsPlayerRef()) {
		return false;
	}
	if (!isPcTimedBlocking) {
		return false;
	}

	if (!isInBlockAngle(a_blocker, a_attacker)) {
		return false;
	}
	bool isPerfectblock = this->getIsPcPerfectBlocking();

	if (a_blocker->IsPlayerRef()) {
		OnPcSuccessfulTimedBlock();
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (pc) {
			pc->AddSkillExperience(RE::ActorValue::kBlock, settings::fTimedBlockMeleeExp);
		}
	}
	
	float reflectedDamage = 0;
	auto blockerWeapon = inlineUtils::actor::getWieldingWeapon(a_blocker);
	if (blockerWeapon) {
		reflectedDamage = blockerWeapon->GetAttackDamage();//get attack damage of blocker's weapon
	}
	inlineUtils::offsetRealDamage(reflectedDamage, a_blocker, a_attacker);
	float stunDmg = reflectedDamage;
	stunHandler::GetSingleton()->processStunDamage(stunHandler::STUNSOURCE::timedBlock, nullptr, a_blocker, a_attacker, stunDmg);


	if (isPerfectblock) {
		playBlockEffects(a_blocker, a_attacker, blockType::perfect);
	} else {
		playBlockEffects(a_blocker, a_attacker, blockType::timed);
	}


	if (isPerfectblock) {//stagger opponent immediately on perfect block.
		reactionHandler::triggerStagger(a_blocker, a_attacker, reactionHandler::reactionType::kLarge);
		debuffHandler::GetSingleton()->quickStopStaminaDebuff(a_blocker);
		inlineUtils::refillActorValue(a_blocker, RE::ActorValue::kStamina); //perfect blocking completely restores actor value.
	}
	else {
		RE::HitData hitData;
		RE::InventoryEntryData* attackerWeapon = a_attacker->GetAttackingWeapon();
		
		hitData.Populate(a_attacker, a_blocker, attackerWeapon);
		inlineUtils::damageav(a_blocker, RE::ActorValue::kStamina,
			hitData.totalDamage * getBlockStaminaCostMult(a_blocker, a_attacker, hitData.flags) * settings::fTimedBlockStaminaCostMult);
	}
	inlineUtils::damageav(a_attacker, RE::ActorValue::kStamina, a_attacker->GetPermanentActorValue(RE::ActorValue::kStamina) * 0.333);

	return true;
}
bool blockHandler::processMeleeTackle(RE::Actor* a_tackler, RE::Actor* a_attacker) {
	if (!a_tackler->IsPlayerRef()) {
		return false;
	}

	if (!isPcTackling) {
		return false;
	}
	
	if (!inlineUtils::isPowerAttacking(a_tackler)) { //tackle only happens in power attacks.
		return false;
	}

	a_attacker->NotifyAnimationGraph("recoillargestart");
	playBlockEffects(a_tackler, a_attacker, blockHandler::tackle);
	
	return true;
}
PRECISION_API::PreHitCallbackReturn blockHandler::precisionPrehitCallbackFunc(const PRECISION_API::PrecisionHitData& a_precisionHitData) {
	PRECISION_API::PreHitCallbackReturn ret;
	if (!settings::bTimedBlockToggle) {
		return ret;
	}
	auto attacker = a_precisionHitData.attacker;
	if (!attacker) {
		return ret;
	}
	auto target = a_precisionHitData.target;
	if (!target) {
		return ret;
	}
	if (target->GetFormType() != RE::FormType::ActorCharacter) {
		return ret;
	}
	RE::Actor* targetActor = target->As<RE::Actor>();
	if (targetActor->GetAttackState() == RE::ATTACK_STATE_ENUM::kNone) {
		if (settings::bTimedBlockToggle && blockHandler::GetSingleton()->processMeleeTimedBlock(targetActor, attacker)) {
			ret.bIgnoreHit = true;
		}
	} else {
		if (settings::bTackleToggle && blockHandler::GetSingleton()->processMeleeTackle(targetActor, attacker)) {
			ret.bIgnoreHit = true;
		}
	}

	return ret;
}
#pragma endregion

void blockHandler::playBlockSFX(RE::Actor* blocker, blockType blockType, bool blockedWithWeapon) {
	if (blockedWithWeapon) {
		switch (blockType) {
		case blockType::guardBreaking: ValhallaUtils::playSound(blocker, data::soundParryWeapon_gb); break;
		case blockType::perfect: ValhallaUtils::playSound(blocker, data::soundParryWeapon_perfect); break;
		case blockType::timed: ValhallaUtils::playSound(blocker, data::soundParryWeapon); break;
		case blockType::tackle:
			ValhallaUtils::playSound(blocker, data::soundParryWeapon);
			break;
		}
	}
	else {
		switch (blockType) {
		case blockType::guardBreaking: ValhallaUtils::playSound(blocker, data::soundParryShield_gb); break;
		case blockType::perfect: ValhallaUtils::playSound(blocker, data::soundParryShield_perfect); break;
		case blockType::timed: ValhallaUtils::playSound(blocker, data::soundParryShield); break;
		case blockType::tackle:
			ValhallaUtils::playSound(blocker, data::soundParryWeapon);
			break;
		}
	}
}
void blockHandler::playBlockVFX(RE::Actor* blocker, blockType blockType, bool blockedWithWeapon){
	//
	auto blockFXNode = RE::Offset::PlaceAtMe(blocker, data::BlockFX, 1, false, false);
	std::string node;
	if (blockedWithWeapon) {
		node = "WEAPON";
	} else {
		node = "SHIELD";
	}
	blockFXNode->MoveToNode(blocker, node);
	RE::Offset::PlaceAtMe(blockFXNode, data::BlockSpark, 1, false, false);
	RE::Offset::PlaceAtMe(blockFXNode, data::BlockSparkFlare, 1, false, false);
	switch (blockType) {
	case blockType::guardBreaking:
	case blockType::perfect:
		RE::Offset::PlaceAtMe(blockFXNode, data::BlockSparkRing, 1, false, false);
	}
	blockFXNode->SetDelete(true);
}
void blockHandler::playBlockScreenShake(RE::Actor* blocker, blockType blockType) {
	switch (blockType) {
	case blockType::guardBreaking:RE::Offset::shakeCamera(1.7, RE::PlayerCharacter::GetSingleton()->GetPosition(), 0.8f); break;
	case blockType::timed: RE::Offset::shakeCamera(1.5, RE::PlayerCharacter::GetSingleton()->GetPosition(), 0.3f); break;
	case blockType::perfect: RE::Offset::shakeCamera(1.5, RE::PlayerCharacter::GetSingleton()->GetPosition(), 0.4f); break;
	}
}

void blockHandler::playBlockSlowTime(blockType blockType) {
	float slowDuration = 0.0f;
	switch (blockType) {
	case blockType::timed:
	case blockType::tackle:
		slowDuration = settings::fTimedBlockSlowTime_Timed;
		break;
	case blockType::guardBreaking:
	case blockType::perfect:
		slowDuration = settings::fTimedBlockSlowTime_Perfect;
		break;
	}
	if (slowDuration == 0) {
		return;
	}
	inlineUtils::slowTime(slowDuration, 0.1);
}

void blockHandler::playBlockEffects(RE::Actor* blocker, RE::Actor* attacker, blockType blockType) {
	//DEBUG("playing effects");
	bool shieldEquipped = RE::Offset::getEquippedShield(blocker);
	if (settings::bTimeBlockSFX) {
		playBlockSFX(blocker, blockType, !shieldEquipped);
	}
	if (settings::bTimedBlockScreenShake
		&&(blocker->IsPlayerRef() || (attacker && attacker->IsPlayerRef()))) {
		playBlockScreenShake(blocker, blockType);
	}

	if (settings::bTimedBlockSlowTime
		&&((attacker && attacker->IsPlayerRef()) || blocker->IsPlayerRef())) {
		playBlockSlowTime(blockType);
	}
	switch (settings::uTimedBlockSparkType) {
	case 0:
		if (blockType == blockType::tackle) {
			playBlockVFX(attacker, blockType, true);
		} else {
			playBlockVFX(blocker, blockType, !shieldEquipped);
		}
		break;
	case 1:
		if (blockType == blockType::tackle) {
			MaxsuBlockSpark::blockSpark::playPerfectBlockSpark(attacker);
		} else {
			MaxsuBlockSpark::blockSpark::playPerfectBlockSpark(blocker);
		}
		break;
	}
}
