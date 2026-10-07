#include "modules/visual/Trail.h"
#include "minecraft/sdk/client/client_instance.h"
#include "minecraft/sdk/actor/local_player.h"

#include <imgui.h>
#include <algorithm>

Trail::Trail() : Module("Visual", "Trail", "Draws a trailing path behind the player using ImGui.") {
    m_maxAge = add_setting<config::FloatSetting>("Duration", "How long trail points last (seconds).", 5.0f, 1.0f, 30.0f);
    m_spacing = add_setting<config::FloatSetting>("Spacing", "Minimum distance between trail points.", 0.3f, 0.1f, 2.0f);
    m_color = add_setting<config::ColorSetting>("Color", "Trail color.", 0.2f, 0.6f, 1.0f, 1.0f);
    m_thickness = add_setting<config::FloatSetting>("Thickness", "Line thickness.", 2.0f, 0.5f, 5.0f);
}

void Trail::on_enable() {
    m_tickListener = EventBus::instance().subscribe<ActorTickEvent>([this](ActorTickEvent& event) {
        if (!event.localPlayer) return;

        math::Vec3 pos = event.localPlayer->getPosition();
        pos.y -= 1.62f;
        const float maxAge = m_maxAge->value();
        const float minSpacing = m_spacing->value();

        std::lock_guard<std::mutex> lock(m_mutex);

        for (auto& point : m_points) {
            point.age += 0.05f;
        }

        std::erase_if(m_points, [maxAge](const TrailPoint& p) { return p.age >= maxAge; });
        bool shouldAdd = m_points.empty();
        if (!shouldAdd) {
            const float dist = pos.distance(m_points.back().position);
            shouldAdd = dist >= minSpacing;
        }

        if (shouldAdd) {
            m_points.push_back({ pos, 0.0f });
        }
    });
}

void Trail::on_disable() {
    if (m_tickListener != 0) {
        EventBus::instance().unsubscribe(m_tickListener);
        m_tickListener = 0;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_points.clear();
}

void Trail::on_imgui_render() {
    auto* ci = ClientInstance::get();
    if (!ci || !ci->getLocalPlayer()) return;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return;

    std::vector<TrailPoint> points;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_points.size() < 2) return;
        points = m_points;
    }

    const ImVec4 col = m_color->as_imvec4();
    const float thickness = m_thickness->value();
    const float maxAge = m_maxAge->value();

    for (size_t i = 1; i < points.size(); ++i) {
        math::Vec2 screenA, screenB;
        if (!ci->world_to_screen(points[i - 1].position, screenA)) continue;
        if (!ci->world_to_screen(points[i].position, screenB)) continue;

        // Fade older points
        const float alpha = 1.0f - (points[i].age / maxAge);
        const ImU32 lineColor = ImGui::ColorConvertFloat4ToU32(
            ImVec4(col.x, col.y, col.z, col.w * alpha));

        drawList->AddLine(
            ImVec2(screenA.x, screenA.y),
            ImVec2(screenB.x, screenB.y),
            lineColor,
            thickness
        );
    }
}
