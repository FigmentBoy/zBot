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

        if (mgr->speed <= 0) {
            mgr->speed = 1;
        }

        CCScheduler::update(dt * mgr->speed);
    }
};

class $modify(GJBaseGameLayer) {
    void update(float delta) {
        if (!m_started) {
            return GJBaseGameLayer::update(delta);
        }

        if (m_resumeTimer > 0) {
            GJBaseGameLayer::update(delta);
            m_resumeTimer--;
            return;
        }

        zBot* mgr = zBot::get();

        m_extraDelta += delta;
        float newDelta = 1.f / mgr->tps;

        if (mgr->frameAdvance) {
            m_extraDelta = 0;
            if (!mgr->doAdvance) return;
            mgr->doAdvance = false;
            return GJBaseGameLayer::update(newDelta);
        }

        if (m_extraDelta >= newDelta / mgr->speed) {
            int times = (int)(m_extraDelta * mgr->speed / newDelta);
            m_extraDelta -= newDelta / mgr->speed * times;

            mgr->disableRender = true;
            for (int i = 0; i < times - 1; i++) {
                GJBaseGameLayer::update(newDelta);
            }

            mgr->disableRender = false;
            return GJBaseGameLayer::update(newDelta);
        } 
    }

    float getModifiedDelta(float dt) {
        if (m_resumeTimer > 0 || !m_started) {
            return dt;
        }

        zBot* mgr = zBot::get();
        double newDelta = 1.f / (mgr->tps * mgr->speed);

        return newDelta;
    }
};