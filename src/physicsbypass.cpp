#include "zBot.hpp"
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>
using namespace geode::prelude;

class $modify(PlayLayer) {
    bool init(GJGameLevel* lvl) {
        zBot* mgr = zBot::get();
        mgr->runningTotal = 0;
        mgr->justLoaded = true;

        return PlayLayer::init(lvl);
    }

    void togglePracticeMode(bool practice) {
        zBot* mgr = zBot::get();
        mgr->runningTotal = 0;

        PlayLayer::togglePracticeMode(practice);
    }

    void updateVisibility() {
        if (zBot::get()->internalRenderer || !zBot::get()->disableRender) {
            PlayLayer::updateVisibility();
        }
    }

    void update(float delta) {
        m_shouldTryToKick = false;
        m_antiCheatPassed = true;

        zBot* mgr = zBot::get();
        mgr->justLoaded = false;
        if (mgr->smoothFrames > 0) mgr->smoothFrames--;

        PlayLayer::update(delta);
    }

    void resetLevel() {
        if (m_isPracticeMode || m_isTestMode) zBot::get()->smoothFrames = 3;
        PlayLayer::resetLevel();
    }
};

class $modify(CCScheduler) {
    void update(float delta) {
        zBot* mgr = zBot::get();

        mgr->speed = mgr->speed <= 0 ? 1 : mgr->speed;

        if (mgr->ignoreBypass || !PlayLayer::get()) {
            mgr->runningTotal = 0;
            return CCScheduler::update(delta * mgr->speed);
        }

        mgr->runningTotal += delta;

        float newDelta = mgr->runningTotal;

        if (!PlayLayer::get() || !mgr->currentReplay) {
            if (CCDirector::sharedDirector()->getAnimationInterval() <= 0) {
                mgr->runningTotal = 0;
                return CCScheduler::update(delta * mgr->speed);
            }
            newDelta = CCDirector::sharedDirector()->getAnimationInterval();
        } else {
            if ((mgr->currentReplay->delta) <= 0) {
                mgr->runningTotal = 0;
                return CCScheduler::update(delta * mgr->speed);
            }
            newDelta = mgr->currentReplay->delta;
        }

        if (mgr->justLoaded) {
            mgr->runningTotal = 0;
            return CCScheduler::update(newDelta);
        }

        if (PlayLayer::get() && mgr->frameAdvance) {
            mgr->runningTotal = 0;
            if (!mgr->doAdvance) return;
            mgr->doAdvance = false;
            return CCScheduler::update(newDelta);
        }

        if (mgr->runningTotal >= newDelta / mgr->speed) {
            int times = (int)(mgr->runningTotal * mgr->speed / newDelta);
            mgr->runningTotal -= newDelta / mgr->speed * times;

            mgr->disableRender = true;
            for (int i = 0; i < times - 1; i++) {
                CCScheduler::update(newDelta);
            }

            mgr->disableRender = false;
            return CCScheduler::update(newDelta);
        } 
    }
};