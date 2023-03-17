#pragma once
using EventResult = RE::BSEventNotifyControl;
class animEventHandler {
public:
    
    public:
	static void InstallHook()
	{
		logger::info("Installing animation event hook...");
		REL::Relocation<uintptr_t> AnimEventVtbl_NPC{ RE::VTABLE_Character[2] };
		REL::Relocation<uintptr_t> AnimEventVtbl_PC{ RE::VTABLE_PlayerCharacter[2] };
		
		_ProcessEvent_NPC = AnimEventVtbl_NPC.write_vfunc(0x1, ProcessEvent_NPC);
		_ProcessEvent_PC = AnimEventVtbl_PC.write_vfunc(0x1, ProcessEvent_PC);
	}

private:
	static inline void ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource);
	
	static EventResult ProcessEvent_NPC(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource);
	static EventResult ProcessEvent_PC(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource);

	static inline REL::Relocation<decltype(ProcessEvent_NPC)> _ProcessEvent_NPC;
	static inline REL::Relocation<decltype(ProcessEvent_PC)> _ProcessEvent_PC;
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
			logger::error("Script event source not found");
			return false;
		}

		ScriptEventSource->AddEventSink(&singleton);

		logger::info("Registered {}.", typeid(singleton).name());

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


class events
{
public:
	static void registerAllEventHandlers() {
		animEventHandler::InstallHook();
		inputEventHandler::Register();
	}
};
