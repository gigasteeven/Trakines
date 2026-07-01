#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/CCEGLView.hpp>

#include "Config.hpp"
#include "RenderManager.hpp"
#include "SpoutSender.hpp"

using namespace geode::prelude;
using namespace trakines;

// Tracks whether the current GJBaseGameLayer is a real play session (PlayLayer)
// vs the editor, so layout mode only affects the play layer by default.
namespace {
    bool g_isEditor = false;
    bool g_inGame = false;
}

// Whether Trakines should act on this frame.
static bool shouldRun() {
    if (!Config::enabled()) return false;
    if (!g_inGame) return false;
    if (g_isEditor && !Config::layoutInEditor()) return false;
    return true;
}

class $modify(TrakinesGameLayer, GJBaseGameLayer) {
    // visit() is the per-frame draw entry for the game layer subtree.
    void visit() {
        if (!shouldRun()) {
            GJBaseGameLayer::visit();
            return;
        }

        auto drawOriginal = [this]() { GJBaseGameLayer::visit(); };

        bool handled = RenderManager::get().renderDual(this, drawOriginal);
        if (!handled) {
            GJBaseGameLayer::visit();
        }
    }
};

class $modify(TrakinesPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        g_isEditor = false;
        g_inGame = true;
        return true;
    }
    void onQuit() {
        g_inGame = false;
        SpoutSender::get().release();
        RenderManager::get().invalidate();
        PlayLayer::onQuit();
    }
};

class $modify(TrakinesEditorLayer, LevelEditorLayer) {
    bool init(GJGameLevel* level, bool p1) {
        if (!LevelEditorLayer::init(level, p1)) return false;
        g_isEditor = true;
        g_inGame = true;
        return true;
    }
};

// Drop render targets when the window/resolution changes.
class $modify(TrakinesEGLView, CCEGLView) {
    void setFrameSize(float width, float height) {
        CCEGLView::setFrameSize(width, height);
        RenderManager::get().invalidate();
    }
};

$on_mod(Loaded) {
    log::info("Trakines loaded: layout on screen, decorated frame to Spout2.");
}
