#include "zBot.hpp"
#include "replay.hpp"

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
using namespace geode::prelude;

class $modify(GJBaseGameLayer) {
    void handleButton(bool down, int button, bool p1) {
        GJBaseGameLayer::handleButton(down, button, p1);

        zBot* mgr = zBot::get();

        if (mgr->state == RECORD) {
            bool p2 = !p1 && m_levelSettings->m_twoPlayerMode && m_gameState.m_isDualMode;
            mgr->currentReplay->addInput(m_gameState.m_currentProgress, button, p2, down);
        }
    }
};

class $modify(PlayLayer) {
    bool init(GJGameLevel* lvl, bool useReplay, bool dontCreateObjects) {        
        zBot* mgr = zBot::get();
        
        if (mgr->state == RECORD) {
            mgr->createNewReplay(lvl);
        }

        return PlayLayer::init(lvl, useReplay, dontCreateObjects);
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        zBot* mgr = zBot::get();

        if (mgr->state == RECORD) {
            mgr->currentReplay->purgeAfter(m_gameState.m_currentProgress);
            mgr->currentReplay->addInput(m_gameState.m_currentProgress + 1, as<int>(PlayerButton::Jump), false, false);
            m_player1->m_isDashing = false;

            if (m_gameState.m_isDualMode && m_levelSettings->m_twoPlayerMode) {
                mgr->currentReplay->addInput(m_gameState.m_currentProgress + 1, as<int>(PlayerButton::Jump), true, false);
                m_player2->m_isDashing = false;
            }
        }
    }

    void levelComplete() {
        zBot* mgr = zBot::get();
        if (mgr->state == RECORD) mgr->currentReplay->save();
        PlayLayer::levelComplete();
    }

    void onExit() {
        zBot* mgr = zBot::get();
        if (mgr->state == RECORD) mgr->currentReplay->save();
        PlayLayer::onExit();
    }
};

