#include "zBot.hpp"
#include "replay.hpp"

#include <Geode/modify/PlayLayer.hpp>
using namespace geode::prelude;

class $modify(PlayLayer) {
    bool init(GJGameLevel* lvl) {
        zBot* mgr = zBot::get();
        bool res = PlayLayer::init(lvl);

        mgr->frame = 0;
        mgr->justLoaded = true;
        mgr->previousPosition = m_player1->m_position;

        switch (mgr->state) {
            case RECORD: {
                mgr->currentReplay = new Replay();
                mgr->currentReplay->delta = CCDirector::sharedDirector()->getAnimationInterval();
                mgr->currentReplay->name = lvl->m_levelName;
            }
        }

        return res;
    }

    void update(float delta) {
        zBot* mgr = zBot::get();

        if (m_player1->m_position != mgr->previousPosition) mgr->frame++;
        mgr->previousPosition = m_player1->m_position;

        mgr->justLoaded = false;

        switch (mgr->state) {
            case RECORD: {
            }
            case PLAYBACK: {
                auto p1_inputs = mgr->currentReplay->p1_inputs.find(mgr->frame);
                auto p2_inputs = mgr->currentReplay->p2_inputs.find(mgr->frame);

                if (p1_inputs != mgr->currentReplay->p1_inputs.end()) {
                    for (bool click : p1_inputs->second) {
                        if (click) {
                            pushButton(1, true);
                        } else {
                            releaseButton(1, true);
                        }
                    }
                }

                if (p2_inputs != mgr->currentReplay->p2_inputs.end()) {
                    for (bool click : p2_inputs->second) {
                        if (click) {
                            pushButton(1, false);
                        } else {
                            releaseButton(1, false);
                        }
                    }
                }

            }
        }

        m_antiCheatPassed = true;
        return PlayLayer::update(delta);
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        zBot* mgr = zBot::get();
        mgr->frame = 0;
        mgr->previousPosition = m_player1->m_position;
    }
};