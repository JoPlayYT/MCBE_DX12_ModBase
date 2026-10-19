#include "modules/visual/ESP.h"
#include "minecraft/sdk/client/client_instance.h"
#include "minecraft/sdk/actor/local_player.h"
#include "minecraft/sdk/render/draw_3d.h"
#include <imgui.h>

ESP::ESP() : Module("Visual", "ESP", "Renders 3D bounding boxes around entities.") {
    m_targetPlayers = add_setting<config::BoolSetting>("Players", "Render ESP on players.", true);
    m_targetMobs = add_setting<config::BoolSetting>("Mobs", "Render ESP on mobs.", false);
    m_color = add_setting<config::ColorSetting>("Color", "Bounding box color.", 1.0f, 0.2f, 0.2f, 1.0f);
    m_thickness = add_setting<config::FloatSetting>("Thickness", "Line thickness.", 1.5f, 0.5f, 5.0f);
    m_fillOpacity = add_setting<config::FloatSetting>("Fill Opacity", "Interior fill opacity.", 0.2f, 0.0f, 1.0f);
}

void ESP::on_enable() {
    m_renderListener = EventBus::instance().subscribe<LevelRenderEvent>([this](LevelRenderEvent& event) {
        render(event);
        });
}

void ESP::on_disable() {
    if (m_renderListener != 0) {
        EventBus::instance().unsubscribe(m_renderListener);
        m_renderListener = 0;
    }
}

void ESP::render(LevelRenderEvent&) {
    LocalPlayer* localPlayer = ClientInstance::getLocalPlayer();
    if (!localPlayer) return;

    Draw3D dc;
    if (!dc.is_valid()) return;

    const ImVec4 col = m_color->as_imvec4();
    const mce::Color boxColor{ col.x, col.y, col.z, col.w };
    const float fillOpacity = m_fillOpacity->value();
    const mce::Color fillColor{ col.x, col.y, col.z, col.w * fillOpacity };
    const bool targetPlayers = m_targetPlayers->value();
    const bool targetMobs = m_targetMobs->value();

    for (Actor* actor : Actor::get_all(localPlayer)) {
        if (!actor || actor == localPlayer) continue;

        const bool isPlayer = actor->isPlayer();
        if (isPlayer && !targetPlayers) continue;
        if (!isPlayer && !targetMobs) continue;

        const math::Vec3 renderPos = actor->getRenderPosition();
        const float yOffset = isPlayer ? -1.62f : 0.0f;
        const math::Vec3 center = { renderPos.x, renderPos.y + yOffset, renderPos.z };

        float halfW = 0.3f;
        float h = 1.8f;
        if (auto* aabb = actor->getAABB()) {
            halfW = aabb->mWidth * 0.5f;
            h = aabb->mHeight;
        }

        const math::Vec3 min = { center.x - halfW, center.y, center.z - halfW };
        const math::Vec3 max = { center.x + halfW, center.y + h, center.z + halfW };

        if (fillOpacity > 0.001f) {
            dc.fill_box(min, max, fillColor);
        }

        dc.draw_box(min, max, boxColor);
    }

    dc.flush();
}