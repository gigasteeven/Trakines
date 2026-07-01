#include "LayoutMode.hpp"
#include "Config.hpp"

#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/binding/GameObject.hpp>

using namespace geode::prelude;

namespace trakines {

LayoutMode& LayoutMode::get() {
    static LayoutMode inst;
    return inst;
}

// Decide whether an object is "decoration" (hidden in layout) or gameplay
// (kept as a silhouette). Mirrors XDBot's layout logic: solids/hazards/slopes
// and interactive triggers stay; pure deco is hidden.
static bool isDecoration(GameObject* obj) {
    if (!obj) return true;
    switch (obj->m_objectType) {
        case GameObjectType::Solid:
        case GameObjectType::Hazard:
        case GameObjectType::AnimatedHazard:
        case GameObjectType::Slope:
        case GameObjectType::Player:
        case GameObjectType::GravityPad:
        case GameObjectType::GravityPortal:
        case GameObjectType::YellowJumpPad:
        case GameObjectType::PinkJumpPad:
        case GameObjectType::RedJumpPad:
        case GameObjectType::YellowJumpRing:
        case GameObjectType::PinkJumpRing:
        case GameObjectType::RedJumpRing:
        case GameObjectType::YellowPortal:
        case GameObjectType::PurplePortal:
        case GameObjectType::BluePortal:
        case GameObjectType::GreenPortal:
        case GameObjectType::PinkPortal:
        case GameObjectType::OrangePortal:
        case GameObjectType::TeleportPortal:
        case GameObjectType::YellowGravityPortal:
        case GameObjectType::BlueGravityPortal:
        case GameObjectType::SpeedPortal:
        case GameObjectType::Collectible:
        case GameObjectType::UserCoin:
        case GameObjectType::SecretCoin:
            return false;
        default:
            return true;
    }
}

void LayoutMode::apply(GJBaseGameLayer* layer) {
    if (!layer) return;

    m_saved.clear();

    auto objectColor = Config::layoutObjectColor();

    auto objects = layer->m_objects;
    if (objects) {
        for (int i = 0; i < objects->count(); i++) {
            auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
            if (!obj) continue;

            Saved s;
            s.node = obj;
            s.visible = obj->isVisible();

            if (isDecoration(obj)) {
                obj->setVisible(false);
            } else {
                s.hadColor = true;
                s.color = obj->getColor();
                s.opacity = obj->getOpacity();
                obj->setColor(objectColor);
                obj->setOpacity(255);
            }
            m_saved.push_back(s);
        }
    }

    if (auto bg = layer->m_background) {
        m_bgSaved = true;
        m_bgColor = bg->getColor();
        bg->setColor(Config::layoutBgColor());
    }
}

void LayoutMode::restore(GJBaseGameLayer* layer) {
    for (auto& s : m_saved) {
        if (!s.node) continue;
        s.node->setVisible(s.visible);
        if (s.hadColor) {
            if (auto obj = typeinfo_cast<GameObject*>(s.node)) {
                obj->setColor(s.color);
                obj->setOpacity(s.opacity);
            }
        }
    }
    m_saved.clear();

    if (m_bgSaved && layer && layer->m_background) {
        layer->m_background->setColor(m_bgColor);
        m_bgSaved = false;
    }
}

} // namespace trakines
