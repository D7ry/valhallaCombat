#pragma once


// AdditionalInclude


// CommonLib
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

using namespace std::literals;

// Version
//#include "Version.h"
#define INFO(msg) logger::info(msg);
#define DEBUG(msg) logger::debug(msg);

#define DLLEXPORT __declspec(dllexport)
