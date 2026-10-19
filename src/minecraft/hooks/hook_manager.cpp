#include "minecraft/hooks/hook_manager.h"
#include "minecraft/hooks/client_instance_hook.h"
#include "minecraft/hooks/actor_tick_hook.h"
#include "minecraft/hooks/actor_set_rot_hook.h"
#include "minecraft/hooks/perspective_hook.h"
#include "minecraft/hooks/attack_hook.h"
#include "minecraft/hooks/level_render_hook.h"
#include "minecraft/hooks/hook_memory.h"
#include "minecraft/bedrock_memory.h"
#include "minecraft/sdk/client/client_instance.h"
#include "events/event_bus.h"
#include "events/client_instance_update_event.h"
#include "utils/logger.h"

#include <MinHook.h>

namespace minecraft::hooks {

    namespace {
        std::uintptr_t s_getLocalPlayerAddr = 0;

        void on_client_update(ClientInstanceUpdateEvent& event) {
            if (!s_getLocalPlayerAddr) return;

            void* localPlayer = hook_memory::call_function<void*>(
                s_getLocalPlayerAddr,
                event.clientInstance);

            if (localPlayer) {
                ClientInstance::setLocalPlayer(static_cast<LocalPlayer*>(localPlayer));
                install_actor_tick_hook(localPlayer, event.clientInstance);
                install_attack_hook(static_cast<LocalPlayer*>(localPlayer));
            }
        }
    }

    bool initialize() {
        LOG_INFO("Initializing MinHook framework...");
        if (MH_Initialize() != MH_OK) {
            LOG_ERROR("Failed to initialize MinHook!");
            return false;
        }

        LOG_INFO("Resolving game signatures...");

        const std::uintptr_t updateAddr = hook_memory::resolve_signature(
            "ClientInstance::update",
            BedrockSignatures::clientInstance_Update);

        s_getLocalPlayerAddr = hook_memory::resolve_signature(
            "ClientInstance::getLocalPlayer",
            BedrockSignatures::clientInstance_getLocalPlayer);

        const std::uintptr_t actorTickAddr = hook_memory::resolve_signature(
            "Actor::normalTick",
            BedrockSignatures::actor_normalTick);

        const std::uintptr_t actorSetRotAddr = hook_memory::resolve_signature(
            "Actor::setRotation",
            BedrockSignatures::actor_setRotation);

        const std::uintptr_t perspectiveAddr = hook_memory::resolve_signature(
            "Options::getPerspective",
            BedrockSignatures::options_getPerspective);

        const std::uintptr_t levelRenderCallAddr = hook_memory::resolve_signature(
            "LevelRenderer::renderLevel",
            BedrockSignatures::render_levelRender);

        if (!updateAddr || !s_getLocalPlayerAddr || !actorTickAddr || !levelRenderCallAddr) {
            LOG_FATAL("One or more required signatures failed to resolve!");
            return false;
        }

        set_actor_tick_address(actorTickAddr);

        EventBus::instance().subscribe<ClientInstanceUpdateEvent>(on_client_update);

        if (!install_client_instance_hook(updateAddr)) {
            LOG_ERROR("Failed to install ClientInstance::update hook!");
            return false;
        }

        if (actorSetRotAddr) {
            if (!install_actor_set_rot_hook(actorSetRotAddr)) {
                LOG_ERROR("Failed to install Actor::setRotation hook!");
            }
        }

        if (perspectiveAddr) {
            if (!install_perspective_hook(perspectiveAddr)) {
                LOG_ERROR("Failed to install Options::getPerspective hook!");
            }
        }

        if (!install_level_render_hook(levelRenderCallAddr)) {
            LOG_ERROR("Failed to install LevelRenderer::renderLevel hook!");
            return false;
        }

        LOG_INFO("All game hooks initialized successfully.");
        return true;
    }

    void shutdown() {
        LOG_INFO("Shutting down game hooks...");
        ClientInstance::setLocalPlayer(nullptr);
        remove_level_render_hook();
        remove_perspective_hook();
        remove_actor_set_rot_hook();
        remove_attack_hook();
        remove_actor_tick_hook();
        remove_client_instance_hook();
        MH_Uninitialize();
    }

}