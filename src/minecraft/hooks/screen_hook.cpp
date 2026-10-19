#include "minecraft/hooks/screen_hook.h"
#include "minecraft/sdk/client/client_instance.h"
#include "input/input_manager.h"
#include "events/event_bus.h"
#include "events/screen_change_event.h"
#include "utils/logger.h"

#include <MinHook.h>
#include <mutex>

namespace {
    using GetScreenNameFn = std::string&(__fastcall*)(ClientInstance* self, std::string& out);
    GetScreenNameFn g_originalGetScreenName = nullptr;
    void* g_screenTarget = nullptr;

    std::string s_currentScreenName;
    std::string s_overrideScreenName;
    std::mutex s_screenMutex;

    static bool call_get_screen_name_raw(ClientInstance* self, GetScreenNameFn fn, std::string* out) {
        __try {
            fn(self, *out);
            return true;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    std::string& __fastcall hook_client_instance_get_screen_name(ClientInstance* self, std::string& out) {
        if (!g_originalGetScreenName) {
            return out;
        }

        std::string& result = g_originalGetScreenName(self, out);

        std::string prevScreen;
        bool changed = false;

        {
            std::lock_guard<std::mutex> lock(s_screenMutex);
            if (result != s_currentScreenName) {
                prevScreen = s_currentScreenName;
                s_currentScreenName = result;
                changed = true;
            }
        }

        if (changed) {
            ScreenChangeEvent event(self, prevScreen, result);
            EventBus::instance().publish(event);

            if (event.isOverridden()) {
                std::lock_guard<std::mutex> lock(s_screenMutex);
                result = event.overrideScreen;
                s_currentScreenName = event.overrideScreen;
            }
        } else {
            std::lock_guard<std::mutex> lock(s_screenMutex);
            if (!s_overrideScreenName.empty()) {
                result = s_overrideScreenName;
            }
        }

        return result;
    }
}

namespace minecraft::hooks {
    bool install_screen_hook(std::uintptr_t screenNameAddr) {
        if (!screenNameAddr) {
            LOG_ERROR("Invalid ClientInstance::getScreenName address provided!");
            return false;
        }

        LOG_INFO("Hooking ClientInstance::getScreenName at address: {:#x}", screenNameAddr);

        g_screenTarget = reinterpret_cast<void*>(screenNameAddr);
        if (MH_CreateHook(g_screenTarget, reinterpret_cast<LPVOID>(&hook_client_instance_get_screen_name), reinterpret_cast<void**>(&g_originalGetScreenName)) != MH_OK) {
            LOG_ERROR("Failed to create ClientInstance::getScreenName hook!");
            g_screenTarget = nullptr;
            return false;
        }

        if (MH_EnableHook(g_screenTarget) != MH_OK) {
            LOG_ERROR("Failed to enable ClientInstance::getScreenName hook!");
            return false;
        }

        LOG_INFO("ClientInstance getScreenName hook enabled.");
        return true;
    }

    void remove_screen_hook() {
        {
            std::lock_guard<std::mutex> lock(s_screenMutex);
            s_currentScreenName.clear();
            s_overrideScreenName.clear();
        }

        if (g_screenTarget) {
            MH_DisableHook(g_screenTarget);
            MH_RemoveHook(g_screenTarget);
            g_screenTarget = nullptr;
            g_originalGetScreenName = nullptr;
            LOG_INFO("ClientInstance getScreenName hook removed.");
        }
    }

    void update_screen_name(ClientInstance* self) {
        if (!self || !g_screenTarget) return;

        GetScreenNameFn fn = g_originalGetScreenName ? g_originalGetScreenName : reinterpret_cast<GetScreenNameFn>(g_screenTarget);
        if (!fn) return;

        std::string temp;
        if (!call_get_screen_name_raw(self, fn, &temp)) {
            return;
        }

        std::string prevScreen;
        bool changed = false;

        {
            std::lock_guard<std::mutex> lock(s_screenMutex);
            if (!temp.empty() && temp != s_currentScreenName) {
                prevScreen = s_currentScreenName;
                s_currentScreenName = temp;
                changed = true;
            }
        }

        if (changed) {
            ScreenChangeEvent event(self, prevScreen, temp);
            EventBus::instance().publish(event);

            InputManager::instance().clear_key_states();
        }
    }

    std::string get_current_screen_name() {
        std::lock_guard<std::mutex> lock(s_screenMutex);
        return s_currentScreenName;
    }

    bool is_game_ui_open() {
        // If there is no local player, we are in menus
        if (!ClientInstance::getLocalPlayer()) {
            return true;
        }

        const std::string screen = get_current_screen_name();
        if (screen.empty()) {
            return false;
        }
        return screen != "hud_screen";
    }

    void set_screen_override(const std::string& screenName) {
        std::lock_guard<std::mutex> lock(s_screenMutex);
        s_overrideScreenName = screenName;
    }

    void clear_screen_override() {
        std::lock_guard<std::mutex> lock(s_screenMutex);
        s_overrideScreenName.clear();
    }
}
