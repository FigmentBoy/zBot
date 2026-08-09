#include <imgui-cocos.hpp>
#include "gui.hpp"
#include "zBot.hpp"
#include <Geode/Bindings.hpp>
#include <Geode/modify/LoadingLayer.hpp>
using namespace geode::prelude;

$on_game(Loaded){
    zBot* mgr = zBot::get();

    listenForKeybindSettingPresses("gui_toggle", [](Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down && !ImGui::GetIO().WantCaptureKeyboard) {
            GUI* gui = GUI::get();
            gui->visible = !gui->visible;

            auto pl = PlayLayer::get();

            if (!gui->visible && pl && !pl->m_isPaused) {
                PlatformToolbox::hideCursor();
            }
        }
    });

    listenForKeybindSettingPresses("frame_advance", [=](Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down && !ImGui::GetIO().WantCaptureKeyboard) {
            mgr->frameAdvance = !mgr->frameAdvance;
        }
    });

    listenForKeybindSettingPresses("advance_frame", [=](Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down && !ImGui::GetIO().WantCaptureKeyboard) {
            mgr->doAdvance = !mgr->doAdvance;
        }
    });
}