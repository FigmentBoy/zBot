#ifndef _imgui_interface_hpp
#define _imgui_interface_hpp

#include <functional>

class ImGui_Interface {
private:
    bool m_initialized;
    std::function<void()> m_renderer = [](){};
    std::function<void()> m_styler = [](){};

public:
    static auto* get() {
        static ImGui_Interface* instance = new ImGui_Interface();
        return instance;
    }

    void setRenderer(std::function<void()> func);
    void setStyler(std::function<void()> func);

    void runRenderer();
    void runStyler();

    bool isInitialized();
    void setInitialized(bool val);
};

#endif