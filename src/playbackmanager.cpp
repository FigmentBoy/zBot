#include "zBot.hpp"
#include "replay.hpp"

#include <Geode/modify/PlayLayer.hpp>
using namespace geode::prelude;

class $modify(PlayLayer) {
    void update(float delta) {
        zBot* mgr = zBot::get();

        if (mgr->state == PLAYBACK) {
            auto p1_inputs = mgr->currentReplay->p1_inputs.find(mgr->frame);
            auto p2_inputs = mgr->currentReplay->p2_inputs.find(mgr->frame);

            if (p1_inputs != mgr->currentReplay->p1_inputs.end()) {
                for (bool click : p1_inputs->second) {
                    if (click) {
                        m_player1->pushButton(1);
                    } else {
                        m_player1->releaseButton(1);
                    }
                }
            }

            if (p2_inputs != mgr->currentReplay->p2_inputs.end()) {
                for (bool click : p2_inputs->second) {
                    if (click) {
                        m_player2->pushButton(1);
                    } else {
                        m_player2->releaseButton(1);
                    }
                }
            }
        }

        return PlayLayer::update(delta);
    }
};