#include "zBot.hpp"
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>
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
            if (1.f / (mgr->currentReplay->framerate) <= 0) {
                mgr->runningTotal = 0;
                return CCScheduler::update(delta * mgr->speed);
            }
            newDelta = 1.f / mgr->currentReplay->framerate;
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