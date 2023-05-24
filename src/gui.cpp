#include "gui.hpp"
#include "zBot.hpp"
#include <Geode/Bindings.hpp>
#include <hjfod.custom-keybinds/include/Keybinds.hpp>
using namespace geode::prelude;

$execute {
    using namespace keybinds;

    BindManager::get()->registerBindable({
        "gui_toggle"_spr,
        "zBot Toggle",
        "Toggles the zBot GUI",
        { Keybind::create(KEY_B, Modifier::None) },
        "Global",
        false
    });

    new EventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
        if (event->isDown()) {
            GUI* gui = GUI::get();
            gui->visible = !gui->visible;
        }

        return ListenerResult::Propagate;
    }, InvokeBindFilter(nullptr, "gui_toggle"_spr));
}

void GUI::renderReplayInfo() {
    zBot* mgr = zBot::get();
    if (mgr->currentReplay) {
        ImGui::Text("Current Replay Name: ");
        ImGui::SameLine();

        ImGui::TextColored({ 0,255,255,255 }, mgr->currentReplay->name.c_str());

        ImGui::Text("Replay FPS: ");
        ImGui::SameLine();
        ImGui::TextColored({ 0,255,255,255 }, "%.0f", 1 / mgr->currentReplay->delta);
    }
}

void GUI::renderStateSwitcher() {
    zBot* mgr = zBot::get();

    ImGui::RadioButton("Disable", reinterpret_cast<int *>(&mgr->state), 0);
    ImGui::SameLine();	
    if (ImGui::RadioButton("Record", reinterpret_cast<int *>(&mgr->state), 1)) {
        if (mgr->playing && !mgr->currentReplay) {
            mgr->currentReplay = new Replay();
            mgr->currentReplay->delta = CCDirector::sharedDirector()->getAnimationInterval();
            mgr->currentReplay->name = GameManager::sharedState()->getPlayLayer()->m_level->m_levelName;
        }

        if (mgr->playing) {
            mgr->currentReplay->purgeInputs(mgr->frame);
        }
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Playback", reinterpret_cast<int *>(&mgr->state), 2)) {
        if (!mgr->currentReplay) {
            mgr->currentReplay = new Replay();
            mgr->currentReplay->delta = CCDirector::sharedDirector()->getAnimationInterval();
            mgr->currentReplay->name = "No Replay Loaded";
        }
    }
}

void RenderInfoPanel() {
    zBot* mgr = zBot::get();

    ImGui::SetNextWindowSize(ImVec2(200, 320), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(385, 10), ImGuiCond_Once);
    ImGui::Begin("utilities", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    
    ImGui::Text("FPS: ");
    ImGui::SameLine();
    ImGui::TextColored({ 0,255,255,255 }, "%.0f", 1 / (CCDirector::sharedDirector()->getAnimationInterval()));

    ImGui::Text("Speed: ");
    ImGui::SameLine();
    ImGui::TextColored({ 0,255,255,255 }, "%.2f", mgr->speed);
    
    ImGui::Text("Frame: ");
    ImGui::SameLine();
    ImGui::TextColored({ 0,255,255,255 }, "%i", mgr->playing ? mgr->frame : 0);
    

    static float tempFPS = 1 / ((float) CCDirector::sharedDirector()->getAnimationInterval());
    ImGui::Text("Set FPS: ");
    ImGui::InputFloat("  ", &tempFPS);
    if (ImGui::Button("Apply FPS")) {
        CCDirector::sharedDirector()->setAnimationInterval(1 / tempFPS);
    }

    ImGui::NewLine();

    static float tempSpeed = 1;
    ImGui::Text("Set Speed: ");
    ImGui::InputFloat("   ", &tempSpeed);
    if (ImGui::Button("Apply Speedhack")) {
        mgr->speed = tempSpeed; 
    }
    ImGui::End();
}

void RenderHackPanel() {
    zBot* mgr = zBot::get();
    size_t base = (size_t)GetModuleHandle(0);
    
    ImGui::SetNextWindowSize(ImVec2(200, 320), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(610, 10), ImGuiCond_Once);
    ImGui::Begin("hacks", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    
    ImGui::Text("Respawn Time: ");
    ImGui::SameLine();
    ImGui::TextColored({ 0,255,255,255 }, "%.2f", **(float**)(base + 0x20A677));
    
    static float tempRT = 1;
    ImGui::InputFloat("  ", &tempRT);
    if (ImGui::Button("Apply")) {
        static float* respawnTime = new float;
        *respawnTime = tempRT;

        DWORD old_prot;
		VirtualProtect((void*)((float**)(base + 0x20A677)), sizeof(size_t), PAGE_EXECUTE_READWRITE, &old_prot);
		*(float**)(base + 0x20A677) = respawnTime;
		VirtualProtect((void*)((float**)(base + 0x20A677)), sizeof(size_t), old_prot, &old_prot);
    }

    ImGui::NewLine();
}

void GUI::renderMainPanel() {
    ImGui::SetNextWindowSize(ImVec2(350, 525), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::Begin("info", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    ImGui::PushFont(l);
    ImGui::TextColored(ImVec4(1.f, 0.78f, 0.17f, 1.f), "zBot");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.3f, 0.3f, 0.3f, 1.f), MOD_VERSION);
    ImGui::PopFont();

    ImGui::PushFont(s);
    renderReplayInfo();
    renderStateSwitcher();

    ImGui::NewLine();
    ImGui::Text("Import Replay by name\n(must be in replays folder)");
    ImGui::InputText("", location, sizeof(location));
    
    zBot* mgr = zBot::get();

    if (ImGui::Button("Import")) {
        Replay* rec = Replay::fromFile(location);
        if (rec) mgr->currentReplay = rec;
    }

    ImGui::SameLine();
    if (ImGui::Button("Open Replays Folder"))
    {
        if (CreateDirectory("replays", NULL) ||
            ERROR_ALREADY_EXISTS == GetLastError())
        {
            ShellExecuteA(NULL, "open", "replays", NULL, NULL, SW_SHOWDEFAULT);
        }
    }

    if (mgr->currentReplay) {
        ImGui::NewLine();
        ImGui::Text("Override Recording Name");
        ImGui::InputText(" ", tempReplayName, sizeof(tempReplayName));
        if (ImGui::Button("Apply")) {
            mgr->currentReplay->name = tempReplayName;
            memset(tempReplayName, 0, sizeof(tempReplayName));
        }

        ImGui::SameLine();
        if (ImGui::Button("Manually Save to File")) {
            mgr->currentReplay->save();
        }
        ImGui::NewLine();
        ImGui::Checkbox("Ignore user input on playback", &mgr->ignoreInput);
    }
    
    ImGui::PopFont();
    ImGui::End();
}


void GUI::renderer() {
    if (!visible || !key) return;
    renderMainPanel();
    RenderInfoPanel();
    RenderHackPanel();
}

void GUI::styler() {
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

    char windows[MAX_PATH];

    ImGuiIO& io = ImGui::GetIO();

    if (GetWindowsDirectoryA(windows, MAX_PATH)) {
        std::string wd(windows);
        std::string path = wd + "\\Fonts\\Micross.ttf";
        io.Fonts->ClearFonts();
        io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;

        s = io.Fonts->AddFontFromFileTTF(path.c_str(), 18.0f);
        l = io.Fonts->AddFontFromFileTTF(path.c_str(), 28.0f);
        io.Fonts->Build();
    }
}