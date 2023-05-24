#include "zBot.hpp"
#include "utils.hpp"
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameObject.hpp>
using namespace geode::prelude;

typedef std::tuple<int, std::vector<std::vector<std::byte>>, int> checkpoint; // frame, saved pairs, activated objects index
std::vector<checkpoint> checkpoints;

const std::vector<std::pair<size_t, size_t>> memberPairs = {
    makeMemberPair(&PlayerObject::m_xVelocity),
    makeMemberPair(&PlayerObject::m_yVelocity),
    makeMemberPair(&PlayerObject::m_hasHitPortal),
    makeMemberPair(&PlayerObject::m_jumpAccel),
    makeMemberPair(&PlayerObject::m_decelerationRate),
    makeMemberPair(&PlayerObject::m_position),
    makeMemberPair(&PlayerObject::m_rotation),
    makeMemberPair(&PlayerObject::m_objectSnappedTo),
    makeMemberPair(&PlayerObject::m_unk4D4), // isGoingDown
    makeMemberPair(&PlayerObject::m_unk674), // hasJumped
    makeMemberPair(&PlayerObject::m_unk675), // hasRingJumped
    makeMemberPair(&PlayerObject::m_unk69C) // lastYVelocity
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
        checkpoint cp;
        
        std::get<0>(cp) = mgr->frame + 1;
        for (auto pair : memberPairs) {
            std::get<1>(cp).push_back(getValue(m_player1, pair));
            std::get<1>(cp).push_back(getValue(m_player2, pair));
        }
        std::get<2>(cp) = mgr->activatedObjects.size();

        checkpoints.push_back(cp);

        mgr->respawnFrame = mgr->frame + 1;

        return PlayLayer::createCheckpoint();
    }

    void removeLastCheckpoint() {
        if (!checkpoints.empty()) {
            checkpoints.pop_back();
            zBot::get()->respawnFrame = checkpoints.empty() ? 0 : std::get<0>(checkpoints.back());
        }
        
        return PlayLayer::removeLastCheckpoint();
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        if (checkpoints.empty()) return;

        zBot* mgr = zBot::get();
        checkpoint cp = checkpoints.back();

        int i = 0;
        for (auto pair : memberPairs) {
            restoreValue(m_player1, pair, std::get<1>(cp)[i++]);
            restoreValue(m_player2, pair, std::get<1>(cp)[i++]);
        }

        mgr->activatedObjects.erase(mgr->activatedObjects.begin() + std::get<2>(cp), mgr->activatedObjects.end());
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