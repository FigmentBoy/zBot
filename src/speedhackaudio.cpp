#include "zBot.hpp"
#include <Geode/modify/FMODAudioEngine.hpp>
using namespace geode::prelude;


class $modify(FMODAudioEngine) {
    void update(float delta) {
        FMODAudioEngine::update(delta);

        zBot* mgr = zBot::get();

        m_globalChannel->setPitch(mgr->speed);
        m_currentSoundChannel->setPitch(mgr->speed);
    }
};