#include "gui.hpp"
#include "zBot.hpp"
#include <Geode/Bindings.hpp>
#include <Geode/modify/LoadingLayer.hpp>

using namespace geode::prelude;

void GUI::renderReplayInfo() {
    zBot* mgr = zBot::get();
    if (mgr->currentReplay) {
        ImGui::Text("Current Replay Name: ");
        ImGui::SameLine();

        ImGui::TextColored({ 0,255,255,255 }, "%s", mgr->currentReplay->name.c_str());

        ImGui::Text("Replay TPS: ");
        ImGui::SameLine();
        ImGui::TextColored({ 0,255,255,255 }, "%.0f", mgr->currentReplay->framerate);
    }
}

void GUI::renderStateSwitcher() {
    zBot* mgr = zBot::get();

    ImGui::RadioButton("Disable", reinterpret_cast<int *>(&mgr->state), NONE);
    ImGui::SameLine();	
    if (ImGui::RadioButton("Record", reinterpret_cast<int *>(&mgr->state), RECORD)) {
        if (PlayLayer::get() && !mgr->currentReplay) {
            mgr->createNewReplay(GameManager::sharedState()->getPlayLayer()->m_level);
        }

        if (PlayLayer::get()) {
            mgr->currentReplay->purgeAfter(PlayLayer::get()->m_gameState.m_currentProgress);
        }
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Playback", reinterpret_cast<int *>(&mgr->state), PLAYBACK)) {
        if (!mgr->currentReplay) {
            mgr->state = NONE;
        }
    }
}

void RenderInfoPanel() {
    zBot* mgr = zBot::get();

    ImGui::SetNextWindowSize(ImVec2(200, 320), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(385, 10), ImGuiCond_Once);
    ImGui::Begin("utilities", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    
    ImGui::Text("TPS: ");
    ImGui::SameLine();
    ImGui::TextColored({ 0,255,255,255 }, "%.0f", mgr->tps);

    ImGui::Text("Speed: ");
    ImGui::SameLine();
    ImGui::TextColored({ 0,255,255,255 }, "%.2f", mgr->speed);
    
    ImGui::Text("Frame: ");
    ImGui::SameLine();
    ImGui::TextColored({ 0,255,255,255 }, "%i", PlayLayer::get() ? PlayLayer::get()->m_gameState.m_currentProgress : 0);
    

    static float tempTPS = mgr->tps;
    ImGui::Text("Set TPS: ");
    ImGui::InputFloat("##tps", &tempTPS);
    if (ImGui::Button("Apply TPS")) {
        if (mgr->state == NONE || !PlayLayer::get()) mgr->tps = tempTPS;
    }

    ImGui::NewLine();

    static float tempSpeed = 1;
    ImGui::Text("Set Speed: ");
    ImGui::InputFloat("##speed", &tempSpeed);
    if (ImGui::Button("Apply Speedhack")) {
        mgr->speed = tempSpeed; 
    }
    ImGui::End();
}

void RenderHackPanel() {
    zBot* mgr = zBot::get();
    
    ImGui::SetNextWindowSize(ImVec2(200, 320), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(610, 10), ImGuiCond_Once);
    ImGui::Begin("hacks", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    
    ImGui::Checkbox("Frame Advance", &mgr->frameAdvance);
    ImGui::Checkbox("Speedhack Audio", &mgr->speedHackAudio);
    if (ImGui::Checkbox("Clickbot", &mgr->clickbotEnabled) && !GUI::get()->key) {
        mgr->clickbotEnabled = false;
        ImGui::OpenPopup("Upgrade to Pro!");
    }

    ImGui::SetNextWindowSize(ImVec2(400, 125), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 200, ImGui::GetIO().DisplaySize.y / 2 - 62.5), ImGuiCond_Always);
    if (ImGui::BeginPopupModal("Upgrade to Pro!", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::Text("You've discovered a pro feature!"); 

        if (ImGui::Button("Upgrade")) {
            system("start https://zbot.figmentcoding.me/");
        }

        ImGui::SameLine();

        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // ImGui::Checkbox("Internal Renderer", &mgr->internalRenderer);

    ImGui::NewLine();
    ImGui::End();
}

void GUI::renderMainPanel() {
    ImGui::SetNextWindowSize(ImVec2(350, 525), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::Begin("info", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    ImGui::PushFont(l);
    ImGui::TextColored(ImVec4(1.f, 0.78f, 0.17f, 1.f), "zBot");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.3f, 0.3f, 0.3f, 1.f), "v" MOD_VERSION);
    ImGui::PopFont();

    ImGui::PushFont(s);
    renderReplayInfo();
    renderStateSwitcher();

    ImGui::NewLine();

    zBot* mgr = zBot::get();

    if (key) {
        ImGui::Text("Import Replay by name\n(must be in replays folder)");
        ImGui::InputText("##replaylocation", location, sizeof(location));
        
        if (ImGui::Button("Import")) {
            if (!key) {
                ImGui::OpenPopup("Upgrade to Pro!");
            } else {
                zReplay* rec = zReplay::fromFile(location);
                if (rec) {
                    mgr->currentReplay = rec;
                    mgr->state = PLAYBACK;
                }
            }
        }
    } else {
        ImGui::Text("Upgrade to import replays!");
        if (ImGui::Button("Upgrade to Pro!")) {
            system("start https://zbot.figmentcoding.me/");
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Open Replays Folder")) {
        auto dir = Mod::get()->getSaveDir() / "replays";
        if (std::filesystem::exists(dir) || std::filesystem::create_directory(dir)) {
            utils::file::openFolder(dir);
        }
    }

    if (!key) {
        if (ImGui::Button("Enter Product Key")) {
            ImGui::OpenPopup("Enter your product key:");
        }
    }

    ImGui::SetNextWindowSize(ImVec2(400, 125), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 200, ImGui::GetIO().DisplaySize.y / 2 - 62.5), ImGuiCond_Always);
    if (ImGui::BeginPopupModal("Enter your product key:", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        static char key[37];

        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 20);
        ImGui::InputText("##key", key, 36);

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Paste")) {
            if (const char* clipboard = ImGui::GetClipboardText()) {
                strncpy(key, clipboard, 36);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Submit")) {
            std::string keyStr = key;
            memset(key, 0, sizeof(key));

            if (keyStr.length() < 36) {
                keyCheckFailed = true;
            } else {
                Mod::get()->setSettingValue<std::string>("product_key", keyStr);
            }

            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }

    if (mgr->currentReplay) {
        ImGui::NewLine();
        ImGui::Text("Override Recording Name");
        ImGui::InputText("##replayname", tempReplayName, sizeof(tempReplayName));
        if (ImGui::Button("Apply")) {
            mgr->currentReplay->name = tempReplayName;
            memset(tempReplayName, 0, sizeof(tempReplayName));
        }

        ImGui::SameLine();
        if (ImGui::Button("Manually Save to File")) {
            mgr->currentReplay->save();
        }
    }
    
    ImGui::PopFont();
    ImGui::End();
}

void GUI::renderer() {
    if (!key && zBot::get()->state != NONE && PlayLayer::get()) {
        ImGui::PushFont(vl);
        
        char key1[16] = "6g6tT67BOYXNQDd";
        char key2[16] = "f\bA\021&SSb- x4\023+\020";
        char text[16];
        
        for (int i = 0; i < 16; i++) {
            text[i] = key1[i] ^ key2[i];
        }


        ImVec2 displaySize = ImGui::GetIO().DisplaySize;
        ImGui::GetForegroundDrawList()->AddText(ImVec2((displaySize.x - ImGui::CalcTextSize(text).x) / 2.f, displaySize.y - 130), ImColor(1.f, 1.f, 1.f, 0.5), text);
        ImGui::PopFont();
    }

    if (!visible) return;
    
    PlatformToolbox::showCursor();

    renderMainPanel();
    RenderInfoPanel();
    RenderHackPanel();

    if (keyCheckFailed) {
        keyCheckFailed = false;
        ImGui::OpenPopup("Key Check Failed");
    }

    ImGui::SetNextWindowSize(ImVec2(500, 140), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 250, ImGui::GetIO().DisplaySize.y / 2 - 70), ImGuiCond_Always);
    if (ImGui::BeginPopupModal("Key Check Failed", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::Text("Your key is invalid or has been linked to a different computer.");
        ImGui::Text("Please enter a valid key or contact support.");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (showCBFMessage && !shownCBFMessage) {
        shownCBFMessage = true;
        ImGui::OpenPopup("CBF Detected!");
    }

    ImGui::SetNextWindowSize(ImVec2(500, 140), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 250, ImGui::GetIO().DisplaySize.y / 2 - 70), ImGuiCond_Always);
    if (ImGui::BeginPopupModal("CBF Detected!", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::Text("Click between frames has been detected!");
        ImGui::Text("Even when disabled in options, playback may be affected.");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void GUI::setup() {
    ImGuiStyle* style = &ImGui::GetStyle();

    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 5.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 4.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;

    style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_Border] = ImVec4(0,0,0,0);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0,0,0,0);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);


    ImGuiIO& io = ImGui::GetIO();

    auto path = (Mod::get()->getResourcesDir() / "micross.ttf").string();

    s = io.Fonts->AddFontFromFileTTF(path.c_str(), 18.0f);
    l = io.Fonts->AddFontFromFileTTF(path.c_str(), 28.0f);
    vl = io.Fonts->AddFontFromFileTTF(path.c_str(), 100.0f);
    io.Fonts->Build();
}

class $modify(LoadingLayer) {
    bool init(bool fromReload) {
        ImGuiCocos::get().setup([] {
            GUI::get()->setup();
        }).draw([] {
            GUI::get()->renderer();
        });

        return LoadingLayer::init(fromReload);
    }
};