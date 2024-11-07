#include "zBot.hpp"
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
using namespace geode::prelude;

class $modify(PlayLayer) {
    bool init(GJGameLevel* lvl, bool useReplay, bool dontCreateObjects) {
        zBot* mgr = zBot::get();
        mgr->runningTotal = 0;
        return PlayLayer::init(lvl, useReplay, dontCreateObjects);
    }

    void togglePracticeMode(bool practice) {
        zBot* mgr = zBot::get();
        mgr->runningTotal = 0;

        PlayLayer::togglePracticeMode(practice);
    }

    void updateVisibility(float p0) {
        if (zBot::get()->internalRenderer || !zBot::get()->disableRender) {
            PlayLayer::updateVisibility(p0);
        }
    }
};

class $modify(GJBaseGameLayer) {
    float getModifiedDelta(float dt) {
        zBot* mgr = zBot::get();

        if (mgr->frameAdvance && m_resumeTimer <= 0) {
            if (mgr->doAdvance) {
                mgr->doAdvance = false;
                return static_cast<float>(1.f / mgr->tps);
            } else {
                return 0.f;
            }
        }

        double delta = 1.f / (mgr->tps * mgr->speed);

        if (m_gameState.m_timeWarp > 1.f) {
            delta *= m_gameState.m_timeWarp;
        }

        if (m_resumeTimer > 0) {
            m_resumeTimer--;
            dt = 0;
        }

        double total = dt + m_extraDelta;
        int steps = std::round(total / delta);
        m_extraDelta = total - delta * steps;
        return static_cast<float>(delta * steps * mgr->speed);
    }
};