#include "PCH.h"

#include "Hooks.h"

namespace {
    // DirectInput scancodes for the Shift keys 
    constexpr std::uint32_t kDIK_LShift = 0x2A;
    constexpr std::uint32_t kDIK_RShift = 0x36;

    // True if any live NPCs can currently detect the player.
    bool PlayerIsDetected() {
        auto player = RE::PlayerCharacter::GetSingleton();
        auto processLists = RE::ProcessLists::GetSingleton();
        if (!player || !processLists) {
            return false;
        }

        for (auto& handle : processLists->highActorHandles) {
            auto actor = handle.get();  
            if (!actor || actor.get() == player) {
                continue;
            }
            if (actor->IsDead()) {
                continue;
            }
            // Positive detection level means this actor can detect the player.
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

    /// Reads the translation files for the current language the game is on and returns its string.
    std::optional<std::string> ReadWatchedFromFile(const std::string& a_language) {
        const auto path =
            std::format("Interface\\Translations\\DynamicStealingBlockingSKSE_{}.txt", a_language);

        RE::BSResourceNiBinaryStream stream{ path.c_str() };
        if (!stream.good()) {
            return std::nullopt;
        }

        std::u16string text;
        char16_t ch{};
        while (stream.read(&ch, 1)) {
            text.push_back(ch);
        }
        if (!text.empty() && static_cast<std::uint16_t>(text.front()) == 0xFEFFu) {
            text.erase(text.begin());          }

        static constexpr std::u16string_view kKey = u"$WTCS_Watched";
        std::size_t start = 0;
        while (start <= text.size()) {
            const auto nl = text.find(u'\n', start);
            auto line = std::u16string_view{ text }.substr(
                start, nl == std::u16string::npos ? std::u16string::npos : nl - start);
            if (!line.empty() && line.back() == u'\r') {
                line.remove_suffix(1);
            }
            const auto tab = line.find(u'\t');
            if (tab != std::u16string_view::npos && line.substr(0, tab) == kKey) {
                const auto value = line.substr(tab + 1);
                std::wstring_view wide{ reinterpret_cast<const wchar_t*>(value.data()), value.size() };
                if (auto utf8 = SKSE::stl::utf16_to_utf8(wide)) {
                    return *utf8;
                }
            }
            if (nl == std::u16string::npos) {
                break;
            }
            start = nl + 1;
        }
        return std::nullopt;
    }

    // The localized "being watched" message, resolved once.
    std::string WatchedMessage() {
        constexpr const char* kFallback = "I can't steal this, I'm being watched";

        std::string language = "english";
        if (auto setting = RE::GetINISetting("sLanguage:General")) {
            if (auto str = setting->GetString(); str && *str) {
                language = str;
                for (auto& c : language) {
                    if (c >= 'A' && c <= 'Z') {
                        c = static_cast<char>(c + ('a' - 'A'));
                    }
                }
            }
        }

        if (auto msg = ReadWatchedFromFile(language)) {
            return *msg;
        }
        if (language != "english") {
            if (auto msg = ReadWatchedFromFile("english")) {
                return *msg;
            }
        }
        return kFallback;
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

    /// If the process (stealign) is blocked then we force stop the input.
    struct ProcessButtonHook {
        static void thunk(RE::ActivateHandler*     a_this,
                          RE::ButtonEvent*         a_event,
                          RE::PlayerControlsData*  a_data) {
            if (a_event && ShouldBlockActivate()) {
                // Notify once per press, not every held frame.
                if (a_event->IsDown()) {
                    static const std::string msg = WatchedMessage();
                    RE::DebugNotification(msg.c_str());
                }
                return; 
            }
            func(a_this, a_event, a_data);
        }

        static inline REL::Relocation<decltype(thunk)> func;
        static constexpr std::size_t idx = 0x4;  
    };
}

namespace Hooks {
    void Install() {
        REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_ActivateHandler[0] };
        ProcessButtonHook::func = vtbl.write_vfunc(ProcessButtonHook::idx, ProcessButtonHook::thunk);
        logger::info("ActivateHandler::ProcessButton hook installed.");
    }
}
