#include "include/ModAPI.h"
#include "include/stunHandler.h"
#include "include/blockHandler.h"
void ModAPI::VALInterface::processStunDamage(VAL_API::STUNSOURCE stunSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage) noexcept
{
	stunHandler::GetSingleton()->processStunDamage(static_cast<stunHandler::STUNSOURCE>(stunSource), weapon, aggressor, victim, baseDamage);
}

bool ModAPI::VALInterface::getIsPCTimedBlocking() noexcept
{
	return blockHandler::GetSingleton()->getIsPcTimedBlocking();
}

bool ModAPI::VALInterface::getIsPCPerfectBlocking() noexcept
{
	return blockHandler::GetSingleton()->getIsPcPerfectBlocking();
}

void ModAPI::VALInterface::triggerPcTimedBlockSuccess() noexcept
{
	blockHandler::GetSingleton()->OnPcSuccessfulTimedBlock();
}

bool ModAPI::VALInterface::isActorStunned(RE::Actor* a_actor) noexcept
{
	if (!a_actor) {
		return false;
	}
	return stunHandler::GetSingleton()->getIsStunBroken(a_actor);
}


bool ModAPI::VALInterface::isActorExhausted(RE::Actor* a_actor) noexcept
{
	if (!a_actor) {
		return false;
	}
	return debuffHandler::GetSingleton()->isInDebuff(a_actor);
}
