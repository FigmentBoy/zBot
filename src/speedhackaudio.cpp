#include "zBot.hpp"
#include <Geode/modify/FMODAudioEngine.hpp>
using namespace geode::prelude;

FMOD::Channel* prevChannel = nullptr;
float prevSpeed = 1.f;

class $modify(FMODAudioEngine) {
    void update(float delta) {
        FMODAudioEngine::update(delta);

        zBot* mgr = zBot::get();
        if (prevSpeed == mgr->speed && prevChannel == m_globalChannel) return;
        m_globalChannel->setPitch(mgr->speed);

        prevChannel = m_globalChannel;
        prevSpeed = mgr->speed;
    }
};