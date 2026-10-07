#pragma once

#include "modules/module.h"
#include "events/event_bus.h"
#include "events/level_render_event.h"

class ESP final : public Module {
public:
    ESP();

    void on_enable() override;
    void on_disable() override;

private:
    void render(LevelRenderEvent& event);

    EventBus::ListenerId m_renderListener = 0;

    config::BoolSetting* m_targetPlayers = nullptr;
    config::BoolSetting* m_targetMobs = nullptr;
    config::ColorSetting* m_color = nullptr;
    config::FloatSetting* m_thickness = nullptr;
    config::FloatSetting* m_fillOpacity = nullptr;
};
