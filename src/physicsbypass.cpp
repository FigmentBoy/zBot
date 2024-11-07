#include "zBot.hpp"
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
using namespace geode::prelude;

class $modify(PlayLayer) {
    void updateVisibility(float dt) {
        if (zBot::get()->disableRender) {
            return;
        }
        PlayLayer::updateVisibility(dt);
    }
};

class $modify(CCScheduler) {
    void update(float dt) {
        zBot* mgr = zBot::get();
        return CCScheduler::update(dt * mgr->speed);
    }
};

class $modify(GJBaseGameLayer) {
    void update(float dt) {
        zBot* mgr = zBot::get();

        mgr->extraTPS += dt;
        float newDelta = 1.f / (mgr->tps);

        if (mgr->frameAdvance) {
            mgr->extraTPS = 0;
            if (!mgr->doAdvance) return;
            mgr->doAdvance = false;
            return GJBaseGameLayer::update(newDelta);
        }

        if (mgr->extraTPS >= newDelta) {
            int times = std::floor(mgr->extraTPS / newDelta);       
            mgr->extraTPS -= newDelta * times;

            mgr->disableRender = true;
            for (int i = 0; i < times - 1; i++) {
                GJBaseGameLayer::update(newDelta);
            }

            mgr->disableRender = false;
            return GJBaseGameLayer::update(newDelta);
        } 
    }

    float getModifiedDelta(float dt) {
        GJBaseGameLayer::getModifiedDelta(dt);

        zBot* mgr = zBot::get();
        double newDelta = 1.f / mgr->tps;

        return newDelta;
    }
};