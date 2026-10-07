#pragma once

#include "modules/module.h"
#include "events/event_bus.h"
#include "events/actor_tick_event.h"
#include "utils/math.h"

#include <vector>
#include <mutex>

class Trail final : public Module {
public:
    Trail();

    void on_enable() override;
    void on_disable() override;
    void on_imgui_render() override;

private:
    struct TrailPoint {
        math::Vec3 position;
        float age = 0.0f;
    };

    EventBus::ListenerId m_tickListener = 0;
    std::mutex m_mutex;
    std::vector<TrailPoint> m_points;

    config::FloatSetting* m_maxAge = nullptr;
    config::FloatSetting* m_spacing = nullptr;
    config::ColorSetting* m_color = nullptr;
    config::FloatSetting* m_thickness = nullptr;
};
