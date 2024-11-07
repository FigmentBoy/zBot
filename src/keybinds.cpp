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

            auto pl = PlayLayer::get();

            if (!gui->visible && pl && !pl->m_isPaused) {
                PlatformToolbox::hideCursor();
            }
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
}