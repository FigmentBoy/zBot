#include "zBot.hpp"
#include "replay.hpp"

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
using namespace geode::prelude;

bool respawnInput(PlayerObject* player) {
    // if (player->m_isShip || player->m_isBird || player->m_isBall || player->m_isDart || player->m_isRobot) return player->m_hasJustHeld;
    // return player->m_isHolding; // Cube, Spider

    if (player->m_isHolding) player->m_hasJustHeld = true;
    return player->m_hasJustHeld;
}

class $modify(PlayLayer) {
    bool init(GJGameLevel* lvl) {        
        zBot* mgr = zBot::get();
        
        if (mgr->state == RECORD) {
            mgr->currentReplay = new Replay();
            mgr->currentReplay->delta = CCDirector::sharedDirector()->getAnimationInterval();
            mgr->currentReplay->name = lvl->m_levelName;
        }

        return PlayLayer::init(lvl);
    }

    void resetLevel() {
        PlayLayer::resetLevel();


        zBot* mgr = zBot::get();

        if (mgr->state == RECORD) {
            mgr->currentReplay->purgeInputs(mgr->respawnFrame);

            mgr->currentReplay->addInput(mgr->respawnFrame, respawnInput(m_player1), true);
            mgr->currentReplay->addInput(mgr->respawnFrame, respawnInput(m_player2), false);
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

class $modify(PlayerObject) {
    void pushButton(PlayerButton unk) {
        zBot* mgr = zBot::get();
        if (m_isInPlayLayer && mgr->state == RECORD) mgr->currentReplay->addInput(mgr->frame, true, !m_unk684);

        PlayerObject::pushButton(unk);
    }

    void releaseButton(PlayerButton unk) {
        zBot* mgr = zBot::get();
        if (m_isInPlayLayer && mgr->state == RECORD) mgr->currentReplay->addInput(mgr->frame, false, !m_unk684);

        PlayerObject::releaseButton(unk);
    }
};