#pragma once
using EventResult = RE::BSEventNotifyControl;
class animEventHandler {
public:
    template<class Ty>
    static Ty SafeWrite64Function(uintptr_t addr, Ty data) {
        DWORD oldProtect;
        void* _d[2];
        memcpy(_d, &data, sizeof(data));
        size_t len = sizeof(_d[0]);

        VirtualProtect((void*)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
        Ty olddata;
        memset(&olddata, 0, sizeof(Ty));
        memcpy(&olddata, (void*)addr, len);
        memcpy((void*)addr, &_d[0], len);
        VirtualProtect((void*)addr, len, oldProtect, &oldProtect);
        return olddata;
    }

    typedef RE::BSEventNotifyControl(animEventHandler::* FnProcessEvent)(RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* dispatcher);

    RE::BSEventNotifyControl HookedProcessEvent(RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* src);

    static void HookSink(uintptr_t ptr) {
        FnProcessEvent fn = SafeWrite64Function(ptr + 0x8, &animEventHandler::HookedProcessEvent);
        fnHash.insert(std::pair<uint64_t, FnProcessEvent>(ptr, fn));
    }

    /*Hook anim event sink too all actors. Player and NPC.*/
    static void hookAllActors() {
        INFO("Sinking animation event hook for all actors...");
        REL::Relocation<uintptr_t> npcPtr{ REL::ID(261399) };
        REL::Relocation<uintptr_t> pcPtr{ REL::ID(261918) };
        HookSink(pcPtr.address());
        HookSink(npcPtr.address());
        INFO("Sinking complete.");
    }

protected:
    static std::unordered_map<uint64_t, FnProcessEvent> fnHash;
};

class cellLoadEventHandler : public RE::BSTEventSink<RE::TESCellFullyLoadedEvent>
{
public:

	virtual EventResult ProcessEvent(const RE::TESCellFullyLoadedEvent* a_event, RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource);


	static bool Register()
	{
		static cellLoadEventHandler singleton;
		auto ScriptEventSource = RE::ScriptEventSourceHolder::GetSingleton();

		if (!ScriptEventSource) {
			ERROR("Script event source not found");
			return false;
		}

		ScriptEventSource->AddEventSink(&singleton);

		INFO("Registered {}.", typeid(singleton).name());

		return true;
	}

};

class inputEventHandler : public RE::BSTEventSink<RE::InputEvent*>
{
public:
    virtual EventResult ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;

    static void Register()
    {
        auto deviceManager = RE::BSInputDeviceManager::GetSingleton();
        deviceManager->AddEventSink(inputEventHandler::GetSingleton());
    }

private:
    enum : std::uint32_t
    {
        kInvalid = static_cast<std::uint32_t>(-1),
        kKeyboardOffset = 0,
        kMouseOffset = 256,
        kGamepadOffset = 266
    };
    static inputEventHandler* GetSingleton()
    {
        static inputEventHandler singleton;
        return std::addressof(singleton);
    }

    static std::uint32_t GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key);
};


