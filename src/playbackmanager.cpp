#include "zBot.hpp"
#include "replay.hpp"

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
using namespace geode::prelude;

int currIndex = 0;
int clickBotIndex = 0;

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
                GJBaseGameLayer::handleButton(input.down, input.button, !input.player2);
            }

            int offset = mgr->currentReplay->framerate * 0.1;

            while (clickBotIndex < mgr->currentReplay->inputs.size() && 
                   mgr->currentReplay->inputs[clickBotIndex].frame <= m_gameState.m_currentProgress + offset) {
                
                auto click = mgr->currentReplay->inputs[clickBotIndex++];
                mgr->playSound(click.player2, click.button, click.down);
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