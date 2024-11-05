// A simple practice mode implementation for the free version
#include "zBot.hpp"
#include "utils.hpp"
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameObject.hpp>
using namespace geode::prelude;

struct Checkpoint {
    std::vector<std::vector<std::byte>> savedPairs;
    int activatedObjectsCount;
};

std::vector<Checkpoint> checkpoints;

const std::array memberPairs = {
    makeMemberPair(&PlayerObject::m_yVelocity),
};

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        checkpoints.clear();
        return PlayLayer::init(level, useReplay, dontCreateObjects);
    }

    void togglePracticeMode(bool practice) {
        checkpoints.clear();
        PlayLayer::togglePracticeMode(practice);
    }

    CheckpointObject* createCheckpoint() {
        zBot* mgr = zBot::get();
        Checkpoint cp;
        
        for (auto pair : memberPairs) {
            cp.savedPairs.push_back(getValue(m_player1, pair));
            cp.savedPairs.push_back(getValue(m_player2, pair));
        }
        checkpoints.push_back(cp);

        return PlayLayer::createCheckpoint();
    }

    void removeCheckpoint(bool p0) {
        if (!checkpoints.empty()) {
            checkpoints.pop_back();
        }
        
        return PlayLayer::removeCheckpoint(p0);
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        zBot* mgr = zBot::get();
        if (checkpoints.empty()) {
            return;
        }

        Checkpoint cp = checkpoints.back();

        int i = 0;
        for (auto pair : memberPairs) {
            restoreValue(m_player1, pair, cp.savedPairs[i++]);
            restoreValue(m_player2, pair, cp.savedPairs[i++]);
        }
    }

    void onExit() {
        PlayLayer::onExit();
        checkpoints.clear();
        zBot* mgr = zBot::get();
    }
};