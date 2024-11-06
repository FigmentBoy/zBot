#include "zBot.hpp"
#include "replay.hpp"

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
using namespace geode::prelude;

int currIndex = 0;
class $modify(zGJBaseGameLayer, GJBaseGameLayer) {
    void processCommands(float delta) {
        if (!zBot::get()->ignoreInput) {
            GJBaseGameLayer::processCommands(delta);
        }

        zBot* mgr = zBot::get();

        if (mgr->state == PLAYBACK && mgr->currentReplay) {
            while (currIndex < mgr->currentReplay->inputs.size() && 
                   mgr->currentReplay->inputs[currIndex].frame <= m_gameState.m_currentProgress) {
                
                auto input = mgr->currentReplay->inputs[currIndex++];
                mgr->playSound(input.player2, input.button, input.down);
                GJBaseGameLayer::handleButton(input.down, input.button, !input.player2);
            }
        }
    }
};

class $modify(PlayLayer) {
    void resetLevel() {
        PlayLayer::resetLevel();

        zBot* mgr = zBot::get();
        if (mgr->state == PLAYBACK) {
            currIndex = 0;
            while (currIndex < mgr->currentReplay->inputs.size() && 
                   mgr->currentReplay->inputs[currIndex].frame <= m_gameState.m_currentProgress) {
                currIndex++;
            }
        }
    }
};