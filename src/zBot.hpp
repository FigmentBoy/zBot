#ifndef _zbot_hpp
#define _zbot_hpp

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
private:
    zState m_state = NONE;
    zError m_error = ERROR_NONE;
    
    bool m_gui_visible = false;
    bool m_render_disable = false;
    bool m_game_loaded = false;
    bool m_ignore_fps = false;
    bool m_playing = false;
    bool m_practice = false;

    double m_speed = 1;

public:
    static auto* get() {
        static zBot* instance = new zBot();
        return instance;
    }

    void setState(zState val) {
        m_state = val;
    }

    zState getState() {
        return m_state;
    }

    void setError(zError val) {
        m_error = val;
    }

    zError getError() {
        return m_error;
    }

    bool hasError() {
        return m_error != ERROR_NONE;
    }

    void setGuiVisible(bool val) {
        m_gui_visible = val;
    }

    bool isGuiVisible() {
        return m_gui_visible;
    }

    void setRenderDisable(bool val) {
        m_render_disable = val;
    }

    bool isRenderDisable() {
        return m_render_disable;
    }

    void setGameLoaded(bool val) {
        m_game_loaded = val;
    }

    bool isGameLoaded() {
        return m_game_loaded;
    }

    void setIgnoreFPS(bool val) {
        m_ignore_fps = val;
    }

    bool isIgnoreFPS() {
        return m_ignore_fps;
    }

    void setPlaying(bool val) {
        m_playing = val;
    }

    bool isPlaying() {
        return m_playing;
    }

    void setPractice(bool val) {
        m_practice = val;
    }

    bool isPractice() {
        return m_practice;
    }

    void setSpeed(double val) {
        m_speed = val;
    }

    double getSpeed() {
        return m_speed;
    }
};

#endif