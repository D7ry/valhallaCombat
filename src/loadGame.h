#pragma once
#include "events/animEventHandler.h"
#include "events/onHitEventHandler.h"
#include "events/actionEventHandler.h"
#include "events/actorLoadEventHandler.h"
#include "Utils.h"


namespace loadGame
{
	//static void registerAnimEvents();

	void EventCallBACK(SKSE::MessagingInterface::Message* msg);

};

