#pragma once

#include "modules/module.h"
#include "events/event_bus.h"
#include "utils/math.h"

class Freelook final : public Module {
public:
    Freelook();

    void on_enable() override;
    void on_disable() override;

private:
    EventBus::ListenerId m_rotListener = 0;
    math::ViewAngles m_targetRotation{ 0.0f, 0.0f };
    bool m_hasTarget = false;
};
