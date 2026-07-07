#include "PCH.h"

#include "Hooks.h"

namespace {
    // Route SKSE/CommonLib logging to Documents/My Games/Skyrim.../SKSE/MySKSEPlugin.log
    void SetupLog() {
        auto path = SKSE::log::log_directory();
        if (!path) {
            SKSE::stl::report_and_fail("Failed to find SKSE log directory."sv);
        }
        *path /= std::format("{}.log", SKSE::PluginDeclaration::GetSingleton()->GetName());

        auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
        auto logger = std::make_shared<spdlog::logger>("global", std::move(sink));
        logger->set_level(spdlog::level::info);
        logger->flush_on(spdlog::level::info);

        spdlog::set_default_logger(std::move(logger));
        spdlog::set_pattern("[%H:%M:%S.%e] [%L] %v");
    }

    // Fired once the game has finished loading its data. Safe place to touch forms,
    // register for events, patch things, etc.
    void OnMessage(SKSE::MessagingInterface::Message* a_msg) {
        switch (a_msg->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            logger::info("Data loaded — {} is running.",
                         SKSE::PluginDeclaration::GetSingleton()->GetName());
            Hooks::Install();
            break;
        default:
            break;
        }
    }
}

// Entry point. CommonLibSSE-NG calls this after loading the DLL.
SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
    SetupLog();

    SKSE::Init(a_skse);

    logger::info("{} v{} loaded.",
                 SKSE::PluginDeclaration::GetSingleton()->GetName(),
                 SKSE::PluginDeclaration::GetSingleton()->GetVersion());

    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);

    return true;
}
