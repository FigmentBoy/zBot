#ifndef _gui_hpp
#define _gui_hpp

#include <imgui.h>

class GUI {
private:
    ImFont* s;
    ImFont* l;

public:
    static auto* get() {
        static GUI* instance = new GUI();
        return instance;
    }

    void renderer();
    void styler();
};

#endif