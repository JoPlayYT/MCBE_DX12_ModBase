#include "modules/visual/Freelook.h"
#include "events/actor_set_rot_event.h"
#include "minecraft/hooks/perspective_hook.h"
#include "minecraft/sdk/client/client_instance.h"
#include "minecraft/sdk/actor/local_player.h"

Freelook::Freelook()
    : Module("Visual", "Freelook", "Locks player facing direction while allowing free camera look.") {
    set_hold_to_enable(true);
}

void Freelook::on_enable() {
    m_hasTarget = false;

    if (auto* localPlayer = ClientInstance::getLocalPlayer()) {
        m_targetRotation = localPlayer->getRotation();
        m_hasTarget = true;
    }

    minecraft::hooks::set_perspective(1);

    m_rotListener = EventBus::instance().subscribe<ActorSetRotEvent>([this](ActorSetRotEvent& event) {
        if (!event.localPlayer) return;

        if (!m_hasTarget) {
            m_targetRotation = event.localPlayer->getRotation();
            m_hasTarget = true;
        }

        event.localPlayer->setRotation(m_targetRotation);
        event.localPlayer->setHeadYaw(m_targetRotation.yaw);
    });
}

void Freelook::on_disable() {
    EventBus::instance().unsubscribe(m_rotListener);
    m_rotListener = 0;
    m_hasTarget = false;

    minecraft::hooks::set_perspective(-1);
}