#include "include/ModAPI.h"
#include "include/stunHandler.h"
#include "include/blockHandler.h"
void ModAPI::VALInterface::processStunDamage(VAL_API::STUNSOURCE stunSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage) noexcept
{
	stunHandler::GetSingleton()->processStunDamage(static_cast<stunHandler::STUNSOURCE>(stunSource), weapon, aggressor, victim, baseDamage);
}

bool ModAPI::VALInterface::getIsPCTimedBlocking() 
{
	return blockHandler::GetSingleton()->getIsPcTimedBlocking();
}

bool ModAPI::VALInterface::getIsPCPerfectBlocking() 
{
	return blockHandler::GetSingleton()->getIsPcPerfectBlocking();
}
