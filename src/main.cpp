#include <Geode/Geode.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include "imgui_interface.hpp"
#include "zBot.hpp"
#include "gui.hpp"

using namespace geode::prelude;

class $modify(LoadingLayer) {
	bool init(bool fromReload) {
		zBot::get()->gameLoaded = false;
		return LoadingLayer::init(fromReload);
	}

	void loadingFinished() {
		zBot::get()->gameLoaded = true;
		return LoadingLayer::loadingFinished();
	}
};