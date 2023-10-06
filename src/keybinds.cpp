#include <geode.custom-keybinds/include/Keybinds.hpp>
#include <imgui-cocos.hpp>
#include "gui.hpp"
#include "zBot.hpp"
#include <Geode/Bindings.hpp>
#include <Geode/modify/LoadingLayer.hpp>
using namespace geode::prelude;

$execute {
    using namespace keybinds;
    zBot* mgr = zBot::get();

    BindManager::get()->registerBindable({
        "gui_toggle"_spr,
        "zBot Toggle",
        "Toggles the zBot GUI",
        { Keybind::create(KEY_B, Modifier::None) },
        "zBot"
    });

    new EventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
        if (event->isDown() && !ImGui::GetIO().WantCaptureKeyboard) {
            GUI* gui = GUI::get();
            gui->visible = !gui->visible;
        }

        return ListenerResult::Propagate;
    }, InvokeBindFilter(nullptr, "gui_toggle"_spr));

    BindManager::get()->registerBindable({
        "frame_advance"_spr,
        "Toggle Frame Advance",
        "Toggles Frame Advance",
        { Keybind::create(KEY_V, Modifier::None) },
        "zBot"
    });

    new EventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
        if (event->isDown() && !ImGui::GetIO().WantCaptureKeyboard) mgr->frameAdvance = !mgr->frameAdvance;

        return ListenerResult::Propagate;
    }, InvokeBindFilter(nullptr, "frame_advance"_spr));

    BindManager::get()->registerBindable({
        "advance_frame"_spr,
        "Frame Advance",
        "Advances one frame while Frame Advance is enabled",
        { Keybind::create(KEY_C, Modifier::None) },
        "zBot"
    });

    new EventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
        mgr->doAdvance = event->isDown() && !ImGui::GetIO().WantCaptureKeyboard;

        return ListenerResult::Propagate;
    }, InvokeBindFilter(nullptr, "advance_frame"_spr));

    BindManager::get()->registerBindable({
        "reset_level"_spr,
        "Reset Level",
        "Resets the Level",
        { Keybind::create(KEY_R, Modifier::None) },
        "zBot"
    });

    new EventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
        if (PlayLayer::get() && event->isDown() && !ImGui::GetIO().WantCaptureKeyboard)
            PlayLayer::get()->resetLevel();

        return ListenerResult::Propagate;
    }, InvokeBindFilter(nullptr, "reset_level"_spr));
}