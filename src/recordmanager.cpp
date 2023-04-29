#include "zBot.hpp"
#include "replay.hpp"

#include <Geode/modify/PlayLayer.hpp>
using namespace geode::prelude;

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
};
