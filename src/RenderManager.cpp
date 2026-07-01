#include "RenderManager.hpp"
#include "SpoutSender.hpp"
#include "LayoutMode.hpp"
#include "Config.hpp"

#include <Geode/binding/GJBaseGameLayer.hpp>

using namespace geode::prelude;

namespace trakines {

RenderManager& RenderManager::get() {
    static RenderManager inst;
    return inst;
}

void RenderManager::invalidate() {
    if (m_decoratedRT) {
        m_decoratedRT->release();
        m_decoratedRT = nullptr;
    }
    m_width = 0;
    m_height = 0;
}

void RenderManager::ensureTarget(unsigned int width, unsigned int height) {
    if (m_decoratedRT && m_width == width && m_height == height) return;

    invalidate();

    m_decoratedRT = CCRenderTexture::create(
        static_cast<int>(width), static_cast<int>(height),
        kCCTexture2DPixelFormat_RGBA8888
    );
    if (m_decoratedRT) {
        m_decoratedRT->retain();
        m_width = width;
        m_height = height;
    }
}

bool RenderManager::renderDual(GJBaseGameLayer* layer, const std::function<void()>& drawOriginal) {
    if (!layer) return false;

    auto winSize = CCDirector::sharedDirector()->getOpenGLView()->getFrameSize();
    unsigned int w = static_cast<unsigned int>(winSize.width);
    unsigned int h = static_cast<unsigned int>(winSize.height);
    if (w == 0 || h == 0) return false;

    // ---- Pass 1: decorated frame into offscreen RT, then to Spout ----
    if (Config::spoutEnabled()) {
        ensureTarget(w, h);
        if (m_decoratedRT) {
            m_decoratedRT->beginWithClear(0, 0, 0, 0);
            drawOriginal(); // original visit = full decorated level
            m_decoratedRT->end();

            auto& sender = SpoutSender::get();
            sender.ensure(Config::spoutSenderName(), w, h);

            if (auto tex = m_decoratedRT->getSprite()->getTexture()) {
                sender.sendTexture(tex->getName(), w, h, true);
            }
        }
    }

    // ---- Pass 2: layout look to the real screen ----
    LayoutMode::get().apply(layer);
    drawOriginal();
    LayoutMode::get().restore(layer);

    return true;
}

} // namespace trakines
