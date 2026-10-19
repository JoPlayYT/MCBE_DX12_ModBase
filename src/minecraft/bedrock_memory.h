#pragma once

#include <cstdint>

namespace minecraft {

    struct BedrockVtableSlots {
        static constexpr int gameMode_Attack = 15; //26.52
    };

    struct BedrockOffsets {
        static constexpr std::uintptr_t localPlayer_gameMode = 0xAA0; //26.52
        static constexpr std::uintptr_t clientInstance_levelRenderer = 0x1C0; //26.52
        static constexpr std::uintptr_t clientInstance_glMatrix = 0x420; //26.52
        static constexpr std::uintptr_t clientInstance_guiData = 0x650; //26.52
        static constexpr std::uintptr_t levelRenderer_renderPlayer = 0x468; //26.52
        static constexpr std::uintptr_t renderPlayer_cameraPos = 0x660; //26.52
        static constexpr std::uintptr_t renderPlayer_fovX = 0xF58; //26.52
        static constexpr std::uintptr_t renderPlayer_fovY = 0xF6C; //26.52
        static constexpr std::uintptr_t guiData_windowSize = 0x40; //26.52
        static constexpr std::uintptr_t screenContext_shaderColor = 0x30; //26.52
        static constexpr std::uintptr_t screenContext_tessellator = 0xB8; //26.52
    };

    struct BedrockSignatures {
        static constexpr const char* clientInstance_Update = //26.52
            "55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 85 ? ? ? ? FE FF FF FF 89 D3 48 89 CE 48 8B 01 48 8B 80 ? ? ? ? 48 8D 95 ? ? ? ? FF 15 ? ? ? ?";
        static constexpr const char* clientInstance_grabMouse = //26.52
            "56 48 83 EC 20 48 89 CE 48 8B 01 48 8B 80 10 05 00 00 FF 15 ? ? ? ? 84 C0 74 ? 48 8B 8E A8 01 00 00 48 8B 01 48 8B 80 38 04 00 00";
        static constexpr const char* clientInstance_releaseMouse = //26.52
            "56 48 83 EC 20 48 89 CE 48 8B 01 48 8B 80 10 05 00 00 FF 15 ? ? ? ? 84 C0 74 ? 48 8B 8E A8 01 00 00 48 8B 01 48 8B 80 40 04 00 00";
        static constexpr const char* clientInstance_getLocalPlayer = //26.52
            "48 83 EC 48 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 40 48 8D 91 ? ? ? ? 48 8D 4C 24 20 E8 ? ? ? ? 80 7C 24 38 01";
        static constexpr const char* clientInstance_getScreenName = //26.52
            "55 56 57 48 83 EC ? 48 8D 6C 24 ? 48 C7 45 ? FE FF FF FF 48 89 D6 48 8B 01 48 8B 80 80 07 00 00 48 8D 55 ? FF 15 ? ? ? ? 48 83 7D ? 00 75 ? 48 8D 05 ? ? ? ? 48 89 44 24 ? 48 8D 0D ? ? ? ? 48 8D 15 ? ? ? ? 4C 8D 0D ? ? ? ? 41 B8 2C 01 00 00 E8 ? ? ? ? 84 C0 74 ? C7 04 25 00 00 00 00 DE C0 AD DE 48 8B 45 ? 80 38 00 75 ? 48 8D 05 ? ? ? ? 48 89 44 24 ? 48 8D 0D ? ? ? ? 48 8D 15 ? ? ? ? 4C 8D 0D ? ? ? ? 41 B8 30 01 00 00 E8 ? ? ? ? 84 C0 74 ? C7 04 25 00 00 00 00 DE C0 AD DE 48 8B 4D ? 48 8B 01 48 8B 80 60 01 00 00";
        static constexpr const char* actor_normalTick = //26.52
            "55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 0F 29 BD ? ? ? ? 0F 29 B5 ? ? ? ? 48 C7 85 ? ? ? ? FE FF FF FF 48 89 CE 80 B9 ? ? ? ? 01 75 ? 83 BE ? ? ? ? 00";
        static constexpr const char* actor_setRotation = //26.52
            "41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC D8 00 00 00 44 0F 29 94 24 C0 00 00 00 44 0F 29 8C 24 B0 00 00 00 44 0F 29 84 24 A0 00 00 00 0F 29 BC 24 90 00 00 00 0F 29 B4 24 80 00 00 00 4C 89 C6 49 8B 40 10 41 8B 78 18";
        static constexpr const char* options_getPerspective = //26.52
            "48 83 EC ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 48 8B 01 48 8B 40 ? 48 8D 54 24 ? 41 B8 03 00 00 00";
        static constexpr const char* render_levelRender = //26.52
            "E8 ? ? ? ? 45 31 E4 48 83 BE";
        static constexpr const char* render_tessVertex = //26.52
            "55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 44 0F 29 ? ? 44 0F 29 ? ? 0F 29 ? ? 0F 29 ? ? 48 C7 ? ? FE FF FF FF 0F 28 F3 0F 28 FA 44 0F 28 C1 48 89 CE 48 8B 0D ? ? ? ? 48 8B 01";
        static constexpr const char* render_tessBegin = //26.52
            "56 57 55 53 48 83 EC ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 80 B9 ? ? ? ? ? 0F 85 ? ? ? ? 80 B9";
        static constexpr const char* render_tessColor = //26.52
            "80 B9 ? ? ? ? ? 0F 85 ? ? ? ? F3 0F 10 05 ? ? ? ? F3 0F 10 0A";
        static constexpr const char* render_meshImmediate = //26.52
            "55 41 57 41 56 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 80 BA ? ? ? ? ? 0F 85 ? ? ? ? 4C 89 CF";
        static constexpr const char* render_uiFillColor = //26.52
            "C6 45 D0 00 4C 8D 05 ? ? ? ? 4C 8D 4D B0 48 89 F1 48 89 FA E8";
    };

}