#pragma once

#include <Geode/Geode.hpp>
#include <functional>

using namespace geode::prelude;

class GJBaseGameLayer;

// Owns the dual-pass render:
//   Pass 1 (decorated) -> render the game layer normally into an offscreen
//                         CCRenderTexture, then push that texture to Spout (OBS).
//   Pass 2 (layout)    -> apply layout transform and draw to the real screen.
//
// This is where the RTX 3090 does its work: the level geometry is rendered
// twice per frame into GPU render targets.
namespace trakines {

class RenderManager {
public:
    static RenderManager& get();

    // Called from the GJBaseGameLayer visit hook. Returns true if it fully
    // handled drawing (caller should NOT call the original visit again).
    bool renderDual(GJBaseGameLayer* layer, const std::function<void()>& drawOriginal);

    void invalidate(); // drop render textures (e.g. on resolution change)

private:
    RenderManager() = default;

    void ensureTarget(unsigned int width, unsigned int height);

    CCRenderTexture* m_decoratedRT = nullptr;
    unsigned int m_width = 0;
    unsigned int m_height = 0;
};

} // namespace trakines
