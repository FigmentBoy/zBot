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

        return PlayLayer::update(delta);
    }
};