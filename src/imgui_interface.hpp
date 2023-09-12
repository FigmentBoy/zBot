// #ifndef _imgui_interface_hpp
// #define _imgui_interface_hpp

// #include <functional>
// #include <cocos2d.h>
// #include <imgui.h>

// class ImGui_Interface {
// public:
//     bool m_initialized;
//     std::function<void()> m_renderer = [](){
//         ImGui::ShowDemoWindow();
//     };
//     std::function<void()> m_setup = [](){};

//     cocos2d::CCTexture2D* m_fontTexture;

//     static auto* get() {
//         static ImGui_Interface* instance = new ImGui_Interface();
//         return instance;
//     }

//     void setRenderer(std::function<void()> func);
//     void setSetup(std::function<void()> func);

//     void runRenderer();
//     void runSetup();

//     bool isInitialized();
//     void setInitialized(bool val);

//     void init();
//     void drawFrame();
// };

// #endif