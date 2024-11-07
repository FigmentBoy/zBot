#ifndef _zbot_hpp
#define _zbot_hpp
#include "replay.hpp"
#include <Geode/Bindings.hpp>
using namespace geode::prelude;

enum zState {
    NONE, RECORD, PLAYBACK
};

enum zError {
    ERROR_NONE,
    KEY_NOT_FOUND_ERROR,
    KEY_INVALID_ERROR,
    CURL_FAILED_ERROR,
    INVALID_HWID_ERROR,
    UNKNOWN_ERROR,
    KEY_LINKED_DIFFERENT_COMPUTER_ERROR
};

class zBot {
public:
    zState state = NONE;
    zError error = ERROR_NONE;

    bool fmodified = false;

    float extraTPS = 0.f;

    bool disableRender = false;
    bool ignoreBypass = false;
    bool justLoaded = false;
    bool ignoreInput = false;
    bool frameAdvance = false;
    bool doAdvance = false;
    bool internalRenderer = false;
    bool speedHackAudio = true;
    bool clickbotEnabled = false;

    
    double speed = 1;
    double tps = 240.f;
    zReplay* currentReplay;

    void createNewReplay(GJGameLevel* level) {
        currentReplay = new zReplay();
        currentReplay->levelInfo.id = level->m_levelID;
        currentReplay->levelInfo.name = level->m_levelName;
        currentReplay->name = level->m_levelName;
        currentReplay->framerate = tps;
    }

    static auto* get() {
        static zBot* instance = new zBot();
        return instance;
    }

    void playSound(bool p2, int button, bool down);
};

#endif