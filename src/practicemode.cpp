#include "zBot.hpp"
#include "utils.hpp"
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameObject.hpp>
using namespace geode::prelude;

struct Checkpoint {
    int frame;
    std::vector<std::vector<std::byte>> savedPairs;
    int activatedObjectsCount;
};

std::vector<Checkpoint> checkpoints;

const std::array memberPairs = {
    makeMemberPair(&PlayerObject::m_xVelocity),
    makeMemberPair(&PlayerObject::m_yVelocity),
    makeMemberPair(&PlayerObject::m_position),
    makeMemberPair(&PlayerObject::m_rotation),
    makeMemberPair(&PlayerObject::m_unk4D4), // isGoingDown
};

class $modify(PlayLayer) {
    bool init(GJGameLevel* level) {
        zBot::get()->activatedObjects.clear();
        checkpoints.clear();
        return PlayLayer::init(level);
    }

    void togglePracticeMode(bool practice) {
        checkpoints.clear();
        zBot::get()->respawnFrame = 0;
        PlayLayer::togglePracticeMode(practice);
    }

    CheckpointObject* createCheckpoint() {
        zBot* mgr = zBot::get();
        Checkpoint cp;
        
        cp.frame = mgr->frame;
        for (auto pair : memberPairs) {
            cp.savedPairs.push_back(getValue(m_player1, pair));
            cp.savedPairs.push_back(getValue(m_player2, pair));
        }
        cp.activatedObjectsCount = mgr->activatedObjects.size();

        checkpoints.push_back(cp);

        mgr->respawnFrame = mgr->frame;

        return PlayLayer::createCheckpoint();
    }

    void removeLastCheckpoint() {
        if (!checkpoints.empty()) {
            checkpoints.pop_back();
            zBot::get()->respawnFrame = checkpoints.empty() ? 0 : checkpoints.back().frame;
        }
        
        return PlayLayer::removeLastCheckpoint();
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        zBot* mgr = zBot::get();
        if (checkpoints.empty()) {
            mgr->activatedObjects.clear();
            return;
        }

        Checkpoint cp = checkpoints.back();

        int i = 0;
        for (auto pair : memberPairs) {
            restoreValue(m_player1, pair, cp.savedPairs[i++]);
            restoreValue(m_player2, pair, cp.savedPairs[i++]);
        }

        mgr->activatedObjects.erase(mgr->activatedObjects.begin() + cp.activatedObjectsCount, mgr->activatedObjects.end());
        for (auto object : mgr->activatedObjects) {
            object->m_hasBeenActivated = true;
        }

    }

    void onExit() {
        PlayLayer::onExit();
        checkpoints.clear();
        zBot* mgr = zBot::get();
        mgr->activatedObjects.clear();
        mgr->respawnFrame = 0;
    }
};

class $modify(GameObject) {
    void activateObject() {
        zBot* mgr = zBot::get();
        GameObject::activateObject();
        if (m_hasBeenActivated) mgr->activatedObjects.push_back(this);
    }
};