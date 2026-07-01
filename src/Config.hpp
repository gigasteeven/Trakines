#pragma once

#include <Geode/Geode.hpp>
#include <string>

using namespace geode::prelude;

// Central place to read mod settings so hooks/managers stay in sync.
namespace trakines {

struct Config {
    static bool enabled() {
        return Mod::get()->getSettingValue<bool>("enabled");
    }
    static bool spoutEnabled() {
        return Mod::get()->getSettingValue<bool>("spout-enabled");
    }
    static std::string spoutSenderName() {
        auto name = Mod::get()->getSettingValue<std::string>("spout-sender-name");
        return name.empty() ? std::string("Trakines") : name;
    }
    static ccColor3B layoutBgColor() {
        return Mod::get()->getSettingValue<ccColor3B>("layout-bg-color");
    }
    static ccColor3B layoutObjectColor() {
        return Mod::get()->getSettingValue<ccColor3B>("layout-object-color");
    }
    static bool layoutInEditor() {
        return Mod::get()->getSettingValue<bool>("layout-in-editor");
    }
};

} // namespace trakines
