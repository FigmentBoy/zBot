#include <Geode/Geode.hpp>

#include "imgui_interface.hpp"
#include "gui.hpp"

USE_GEODE_NAMESPACE();

$on_mod(Enabled) {
	ImGui_Interface* inter = ImGui_Interface::get();
	GUI* gui = GUI::get();

	inter->setRenderer(std::bind(&GUI::renderer, gui));
	inter->setStyler(std::bind(&GUI::styler, gui));	
}