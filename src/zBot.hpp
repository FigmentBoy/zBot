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

    int frame;
    int respawnFrame;
    float runningTotal;

    float player1RotationRate;
    float player2RotationRate;

    int smoothFrames = 0;

    bool disableRender = false;
    bool ignoreBypass = false;
    bool justLoaded = false;
    bool ignoreInput = false;
    bool frameAdvance = false;
    bool doAdvance = false;
    bool internalRenderer = false;
    
    CCPoint previousPosition;
    std::vector<GameObject*> activatedObjects;

    double speed = 1;

    Replay* currentReplay;

    static auto* get() {
        static zBot* instance = new zBot();
        return instance;
    }
};

#endif