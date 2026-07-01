#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class GJBaseGameLayer;

// XDBot-style layout mode.
//
// Applied ONLY to the on-screen pass of the play layer. It is a reversible,
// per-frame visual transform: apply() before drawing to screen, restore()
// afterwards, so the level's real object state is never mutated (the Spout
// pass always sees the original decorated level).
namespace trakines {

class LayoutMode {
public:
    static LayoutMode& get();

    // Apply the layout look to the given game layer (hide decorations, flatten
    // colors to silhouettes, set solid background).
    void apply(GJBaseGameLayer* layer);

    // Restore everything apply() changed this frame.
    void restore(GJBaseGameLayer* layer);

private:
    LayoutMode() = default;

    // Snapshot of state we mutate, so restore() is exact.
    struct Saved {
        cocos2d::CCNode* node = nullptr;
        bool visible = true;
        cocos2d::ccColor3B color = {255, 255, 255};
        GLubyte opacity = 255;
        bool hadColor = false;
    };

    std::vector<Saved> m_saved;
    bool m_bgSaved = false;
    cocos2d::ccColor3B m_bgColor = {0, 0, 0};
};

} // namespace trakines
