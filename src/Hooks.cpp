#include "PCH.h"

#include "Hooks.h"

namespace {
    // DirectInput scancodes for the Shift keys (keyboard device uses DIK_* codes).
    constexpr std::uint32_t kDIK_LShift = 0x2A;
    constexpr std::uint32_t kDIK_RShift = 0x36;

    // True if any live, fully-simulated NPC currently detects the player.
    bool PlayerIsDetected() {
        auto player = RE::PlayerCharacter::GetSingleton();
        auto processLists = RE::ProcessLists::GetSingleton();
        if (!player || !processLists) {
            return false;
        }

        for (auto& handle : processLists->highActorHandles) {
            auto actor = handle.get();  // NiPointer<Actor>
            if (!actor || actor.get() == player) {
                continue;
            }
            if (actor->IsDead()) {
                continue;
            }
            // Positive detection level means this actor can see/hear the player.
            if (actor->RequestDetectionLevel(player) > 0) {
                return true;
            }
        }
        return false;
    }

    // True while either Shift key is physically held.
    bool ShiftHeld() {
        auto manager = RE::BSInputDeviceManager::GetSingleton();
        if (!manager) {
            return false;
        }
        auto keyboard = manager->GetKeyboard();
        if (!keyboard) {
            return false;
        }
        return keyboard->IsPressed(kDIK_LShift) || keyboard->IsPressed(kDIK_RShift);
    }

    // Block the activation only when: taking the crosshair target would be a theft,
    // AND the player is detected, AND Shift is not held (Shift overrides the block).
    bool ShouldBlockActivate() {
        if (ShiftHeld()) {
            return false;  // Shift always lets the steal through.
        }

        auto pick = RE::CrosshairPickData::GetSingleton();
        if (!pick) {
            return false;
        }
        auto target = pick->target.get();  // NiPointer<TESObjectREFR>
        if (!target) {
            return false;
        }
        if (!target->IsCrimeToActivate()) {
            return false;  // Not stealing -> never block.
        }
        return PlayerIsDetected();  // Stealing: block only if someone sees you.
    }

    // Hooks ActivateHandler::ProcessButton (the handler that turns an Activate
    // press into a world activation). When a blocked theft is in progress we just
    // don't forward to the original, so the activation never happens.
    struct ProcessButtonHook {
        static void thunk(RE::ActivateHandler*     a_this,
                          RE::ButtonEvent*         a_event,
                          RE::PlayerControlsData*  a_data) {
            if (a_event && ShouldBlockActivate()) {
                // Notify once per press (on the down edge), not every held frame.
                if (a_event->IsDown()) {
                    RE::DebugNotification("I can't steal this, I'm being watched");
                }
                return;  // swallow the activation
            }
            func(a_this, a_event, a_data);
        }

        static inline REL::Relocation<decltype(thunk)> func;
        static constexpr std::size_t                   idx = 0x4;  // ProcessButton
    };
}

namespace Hooks {
    void Install() {
        REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_ActivateHandler[0] };
        ProcessButtonHook::func = vtbl.write_vfunc(ProcessButtonHook::idx, ProcessButtonHook::thunk);
        logger::info("ActivateHandler::ProcessButton hook installed.");
    }
}
