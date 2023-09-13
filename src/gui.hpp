#ifndef _gui_hpp
#define _gui_hpp

#include <imgui-cocos.hpp>

class GUI {
private:
    ImFont* s;
    ImFont* l;

    char location[30];
    char tempReplayName[30];
    
public:
    static auto* get() {
        static GUI* instance = new GUI();
        return instance;
    }

    bool visible = false;
    bool key = false;
    bool callbackInit = false;

    void renderReplayInfo();
    void renderStateSwitcher();
    void renderMainPanel();

    void renderer();
    void setup();
};

#endif