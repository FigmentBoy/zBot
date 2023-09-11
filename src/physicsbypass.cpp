#include "zBot.hpp"
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>
using namespace geode::prelude;

class $modify(PlayLayer) {
    bool init(GJGameLevel* lvl) {
        zBot* mgr = zBot::get();
        mgr->runningTotal = 0;
        mgr->justLoaded = true;
        mgr->playing = true;

        return PlayLayer::init(lvl);
    }

    void togglePracticeMode(bool practice) {
        zBot* mgr = zBot::get();
        mgr->runningTotal = 0;
        mgr->justLoaded = true;

        return PlayLayer::togglePracticeMode(practice);
    }

    void updateVisibility() {
        if (!zBot::get()->disableRender) {
            PlayLayer::updateVisibility();
        }
    }

    void update(float delta) {
        m_shouldTryToKick = false;
        m_antiCheatPassed = true;

        zBot* mgr = zBot::get();
        if (mgr->smoothFrames > 0) mgr->smoothFrames--;

        PlayLayer::update(delta);
    }

    void resetLevel() {
        if (m_isPracticeMode || m_isTestMode) zBot::get()->smoothFrames = 3;
        PlayLayer::resetLevel();
    }

    void onExit() {
        zBot* mgr = zBot::get();
        mgr->playing = false;

        return PlayLayer::onExit();
    }
};

class $modify(CCScheduler) {
    void update(float delta) {
        zBot* mgr = zBot::get();

        mgr->speed = mgr->speed <= 0 ? 1 : mgr->speed;

        if (mgr->ignoreBypass || !mgr->gameLoaded || !mgr->playing) {
            mgr->runningTotal = 0;
            return CCScheduler::update(delta * mgr->speed);
        }

        mgr->runningTotal += delta;

        int times = 1;
        float newDelta = mgr->runningTotal;

        if (mgr->state == NONE || !mgr->playing || !mgr->currentReplay) {
            if (CCDirector::sharedDirector()->getAnimationInterval() <= 0) {
                mgr->runningTotal = 0;
                return CCScheduler::update(delta * mgr->speed);
            }
            newDelta = delta;
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

        if (mgr->playing && mgr->frameAdvance) {
            mgr->runningTotal = 0;
            if (!mgr->doAdvance) return;
            mgr->doAdvance = false;
            return CCScheduler::update(newDelta);
        }

        times = (int)(mgr->runningTotal * mgr->speed / newDelta);

        if (newDelta / mgr->speed >= delta) {
            if (mgr->runningTotal >= newDelta / mgr->speed) {
                mgr->runningTotal = mgr->runningTotal - (newDelta / mgr->speed);
                mgr->disableRender = false;
                return CCScheduler::update(newDelta);
            }
        } else {
            mgr->runningTotal -= newDelta * times;

            mgr->disableRender = true;
            for (int i = 0; i < times - 1; i++) {
                CCScheduler::update(newDelta);
            }

            mgr->disableRender = false;
            return CCScheduler::update(newDelta);
        }
    }
};