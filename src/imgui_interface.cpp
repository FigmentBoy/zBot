#include "imgui_interface.hpp"

void ImGui_Interface::setRenderer(std::function<void()> func) {
    m_renderer = func;
}

void ImGui_Interface::setStyler(std::function<void()> func) {
    m_styler = func;
}

void ImGui_Interface::runRenderer() {
    m_renderer();
}

void ImGui_Interface::runStyler() {
    m_styler();
}

bool ImGui_Interface::isInitialized() {
    return m_initialized;
}

void ImGui_Interface::setInitialized(bool val) {
    m_initialized = val;
}