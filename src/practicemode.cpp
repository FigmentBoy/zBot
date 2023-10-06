#include "zBot.hpp"
#include "utils.hpp"
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameObject.hpp>
using namespace geode::prelude;

struct Checkpoint {
    int frame;
    std::vector<std::vector<std::byte>> savedPairs;
    int activatedObjectsCount;
    float player1RotationRate = 0, player2RotationRate = 0;
    float player1InitialBallRotation = 0, player2InitialBallRotation = 0;
    float ball1RotationElapsed = 0, ball2RotationElapsed = 0;
    int rotationTag = 0;
};

std::vector<Checkpoint> checkpoints;

const std::array memberPairs = {
    makeMemberPair(&PlayerObject::m_xVelocity),
    makeMemberPair(&PlayerObject::m_yVelocity),
    makeMemberPair(&PlayerObject::m_position),
    makeMemberPair(&PlayerObject::m_rotation),
    makeMemberPair(&PlayerObject::m_unk4D4), // isGoingDown
};

void runBallRotation2(PlayerObject* pl, bool player2, Checkpoint c)
{
    pl->stopActionByTag(1);
        float v2 = 1.0f;
        float v3 = 0;
        if (pl->m_vehicleSize == 1.0f)
            v3 = 1.0f;
        else
            v3 = 0.8f;
        if (pl->m_playerSpeed == 0.7f)
            v2 = 1.2405638f;
        else if (pl->m_playerSpeed == 1.1f)
        {
            v2 = 0.80424345f;
        }
        else if (pl->m_playerSpeed == 1.3f)
        {
            v2 = 0.66576928f;
        }
        else if (pl->m_playerSpeed == 1.6f)
        {
            v2 = 0.54093748f;
        }
        float v5 = v3 * 0.8f * v2;
        auto rotate = CCRotateBy::create(v5, (float)(-340 * (2 * !pl->m_isUpsideDown - 1)));
        auto easeout = CCEaseOut::create(rotate, 1.2f);
        easeout->setTag(1);
        pl->setRotation(player2 ? c.player2InitialBallRotation : c.player1InitialBallRotation);
        pl->runAction(easeout);
        easeout->step(0);
        easeout->step(player2 ? c.ball2RotationElapsed : c.ball1RotationElapsed);
}

void runBallRotation(PlayerObject* pl, bool player2, Checkpoint c)
{
    pl->stopActionByTag(0);
        float v4 = 1.0f;
        float v5 = 0.f;
        if (pl->m_vehicleSize == 1.0f)
            v5 = 1.0f;
        else
            v5 = 0.8f;
        if (pl->m_playerSpeed == 0.7f)
            v4 = 1.2405638f;
        else if (pl->m_playerSpeed == 1.1f)
        {
            v4 = 0.80424345f;
        }
        else if (pl->m_playerSpeed == 1.3f)
        {
            v4 = 0.66576928f;
        }
        else if (pl->m_playerSpeed == 1.6f)
        {
            v4 = 0.54093748f;
        }
        auto rotate = CCRotateBy::create((v5 * 0.2f) * v4, (float)(120 * (2 * !pl->m_isUpsideDown - 1)));
        auto repeatForever = CCRepeatForever::create(rotate);
        repeatForever->setTag(0);
        pl->setRotation(player2 ? c.player2InitialBallRotation : c.player1InitialBallRotation);
        pl->runAction(repeatForever);
        repeatForever->step(0);
        repeatForever->step(player2 ? c.ball2RotationElapsed : c.ball1RotationElapsed);
}

CCRotateBy* runNormalRotation(PlayerObject* player, float rate)
{
    float rotateBy;
    cocos2d::CCRotateBy* action;
    if (player->m_vehicleSize == 1.0)
        rotateBy = 0.421;
    else
        rotateBy = 0.41966;
    int flipMod = rate >= 0 ? 1 : -1;
    action = cocos2d::CCRotateBy::create(0.421, (float)(180 * flipMod));
    action->setTag(0);
    return action;
}

uint8_t GetGamemode(PlayerObject *p)
{
    if (p->m_isShip)
        return 1;
    else if (p->m_isBird)
        return 3;
    else if (p->m_isBall)
        return 2;
    else if (p->m_isDart)
        return 4;
    else if (p->m_isRobot)
        return 5;
    else if (p->m_isSpider)
        return 6;
    else
        return 0;
}

void restoreActions(PlayerObject* player, bool player2, Checkpoint cp)
{
    float ballRotation = player2 ? cp.ball2RotationElapsed : cp.ball1RotationElapsed;
    float rotationRate = player2 ? cp.player2RotationRate : cp.player1RotationRate;
    if(GetGamemode(player) == 0 && !player->m_isOnGround && !player->m_isDashing)
    {
        player->runAction(runNormalRotation(player, rotationRate));
    }
    else if(player->m_isBall && !player->m_isOnGround && !player->m_isDashing && ballRotation != 0)
    {
        if(cp.rotationTag == 1)
            runBallRotation2(player, player2, cp);
        else
            runBallRotation(player, player2, cp);
    }
}

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

    void getBallRotations(PlayerObject* player, bool player2, Checkpoint& cp)
    {
        cp.rotationTag = 1;
		cocos2d::CCActionInterval* ac = dynamic_cast<cocos2d::CCActionInterval*>(player->getActionByTag(1));

		if (!ac && player->getPositionX() != m_playerStartPosition.x)
		{
			cocos2d::CCRepeatForever* forever = dynamic_cast<cocos2d::CCRepeatForever*>(player->getActionByTag(0));
			if (forever)
				ac = forever->getInnerAction();
			cp.rotationTag = 0;
		}

		if (ac)
		{
			float el = ac->getElapsed();
            if(player2)
			    cp.ball2RotationElapsed = el;
            else
                cp.ball1RotationElapsed = el;
            
			if (!player->m_isOnGround)
			{
				ac->step(-el);
				if(player2)
                    cp.player2InitialBallRotation = el;
                else
                    cp.player1InitialBallRotation = el;
				ac->step(el);
			}
		}
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

        if(m_player1->m_isBall && !m_player1->m_isOnGround)
            getBallRotations(m_player1, false, cp);
        if(m_player2->m_isBall && !m_player2->m_isOnGround)
            getBallRotations(m_player2, true, cp);

        cp.player1RotationRate = mgr->player1RotationRate;
        cp.player2RotationRate = mgr->player2RotationRate;

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

    void update(float delta) {

        static float prevRot1;
        static float prevRot2;

        zBot* mgr = zBot::get();

        if (prevRot1 != m_player1->getRotation())
        {
            mgr->player1RotationRate = m_player1->getRotation() - prevRot1;
            prevRot1 = m_player1->getRotation();
        }

        if (prevRot2 != m_player2->getRotation())
        {
            mgr->player2RotationRate = m_player2->getRotation() - prevRot2;
            prevRot2 = m_player2->getRotation();
        }

        PlayLayer::update(delta);
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

        restoreActions(m_player1, false, cp);
        restoreActions(m_player2, true, cp);
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