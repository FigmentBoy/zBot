#include "zBot.hpp"
#include <Geode/modify/CCScheduler.hpp>
USE_GEODE_NAMESPACE();

class $modify(CCScheduler) {
    float m_running_total = 0;

    void update(float delta) {
        zBot* mgr = zBot::get();

        mgr->setSpeed(mgr->getSpeed() <= 0 ? 1 : mgr->getSpeed());

        if (mgr->isIgnoreFPS() || !mgr->isGameLoaded()) {
            m_fields->m_running_total = 0;
            return CCScheduler::update(delta);
        }

        m_fields->m_running_total += delta;

        int times = 1;
        float newDelta = m_fields->m_running_total;

        if (mgr->getState() == NONE || !mgr->isPlaying() || !rpm->replay ) {
            if (CCDirector::sharedDirector()->getAnimationInterval() <= 0) {
                m_fields->m_running_total = 0;
                return CCScheduler::update(delta);
            }
            newDelta = mgr->FPS;
        } else {
            if ((rpm->replay->delta * rpm->replay->scale) <= 0) {
                m_fields->m_running_total = 0;
                return CCScheduler::update(delta);
            }
            newDelta = rpm->replay->delta * rpm->replay->scale;
        }
        times = (int)(m_fields->m_running_total * mgr->getSpeed() / newDelta);

        if (mgr->frameAdvance && mgr->isPlaying()) {
            m_fields->m_running_total = 0;
            if (mgr->advanceNextFrame) {
                CCScheduler::update(newDelta);
                mgr->advanceNextFrame = false;
            }
            return;
        }

        if (newDelta / mgr->speed >= delta) {
            if (m_fields->m_running_total >= newDelta / mgr->speed) {
                m_fields->m_running_total = m_fields->m_running_total - (newDelta / mgr->speed);
                mgr->disableRender = false;
                return CCScheduler::update(newDelta);
            }
        } else {
            m_fields->m_running_total -= newDelta * times;

            mgr->disableRender = true;
            for (int i = 0; i < times - 1; i++) {
                CCScheduler::update(newDelta);
            }

            mgr->disableRender = false;
            return CCScheduler::update(newDelta);
        }
    }
};