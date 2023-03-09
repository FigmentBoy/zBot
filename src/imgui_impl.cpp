#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>
#include "imgui_interface.hpp"

#include <Geode/modify/CCEGLView.hpp>
USE_GEODE_NAMESPACE();

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

WPARAM MapLeftRightKeys( WPARAM vk, LPARAM lParam)
{
    WPARAM new_vk = vk;
    UINT scancode = (lParam & 0x00ff0000) >> 16;
    int extended  = (lParam & 0x01000000) != 0;

    switch (vk) {
    case VK_SHIFT:
        new_vk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
        break;
    case VK_CONTROL:
        new_vk = extended ? VK_RCONTROL : VK_LCONTROL;
        break;
    case VK_MENU:
        new_vk = extended ? VK_RMENU : VK_LMENU;
        break;
    default:
        // not a key we map from generic to left/right specialized
        //  just return it.
        new_vk = vk;
        break;    
    }

    return new_vk;
}

class $modify(CCEGLView) {
	void swapBuffers() {
        ImGui_Interface* inter = ImGui_Interface::get();
		if (!inter->isInitialized()) {
			inter->setInitialized(true);
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGui::GetIO();

			auto hwnd = WindowFromDC(*reinterpret_cast<HDC*>(reinterpret_cast<uintptr_t>(this->getWindow()) + 0x244));
            ImGui_ImplWin32_Init(hwnd);
			ImGui_ImplOpenGL3_Init();

			inter->runStyler();
		}

		    ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			inter->runRenderer();

			ImGui::EndFrame();
			ImGui::Render();

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			CCEGLView::swapBuffers();
	}

	void pollEvents() {
		auto& io = ImGui::GetIO();

		bool blockInput = false;
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);

			if (io.WantCaptureMouse) {
				switch (msg.message) {
					case WM_LBUTTONDBLCLK:
					case WM_LBUTTONDOWN:
					case WM_LBUTTONUP:
					case WM_MBUTTONDBLCLK:
					case WM_MBUTTONDOWN:
					case WM_MBUTTONUP:
					case WM_MOUSEACTIVATE:
					case WM_MOUSEHOVER:
					case WM_MOUSEHWHEEL:
					case WM_MOUSELEAVE:
					case WM_MOUSEMOVE:
					case WM_MOUSEWHEEL:
					case WM_NCLBUTTONDBLCLK:
					case WM_NCLBUTTONDOWN:
					case WM_NCLBUTTONUP:
					case WM_NCMBUTTONDBLCLK:
					case WM_NCMBUTTONDOWN:
					case WM_NCMBUTTONUP:
					case WM_NCMOUSEHOVER:
					case WM_NCMOUSELEAVE:
					case WM_NCMOUSEMOVE:
					case WM_NCRBUTTONDBLCLK:
					case WM_NCRBUTTONDOWN:
					case WM_NCRBUTTONUP:
					case WM_NCXBUTTONDBLCLK:
					case WM_NCXBUTTONDOWN:
					case WM_NCXBUTTONUP:
					case WM_RBUTTONDBLCLK:
					case WM_RBUTTONDOWN:
					case WM_RBUTTONUP:
					case WM_XBUTTONDBLCLK:
					case WM_XBUTTONDOWN:
					case WM_XBUTTONUP:
						blockInput = true;
				}
			}

			if (io.WantCaptureKeyboard) {
				switch (msg.message) {
					case WM_HOTKEY:
					case WM_KEYDOWN:
					case WM_KEYUP:
					case WM_KILLFOCUS:
					case WM_SETFOCUS:
					case WM_SYSKEYDOWN:
					case WM_SYSKEYUP:
						blockInput = true;
				}
			}

			if (!blockInput)
				DispatchMessage(&msg);

			ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam);
		}

		CCEGLView::pollEvents();
	}

	void toggleFullScreen(bool fullscreen) {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

        ImGui_Interface* inter = ImGui_Interface::get();

		CCEGLView::toggleFullScreen(fullscreen);

        inter->setInitialized(true);

		ImGui::CreateContext();
		ImGui::GetIO();
		auto hwnd = WindowFromDC(*reinterpret_cast<HDC*>(reinterpret_cast<uintptr_t>(this->getWindow()) + 0x244));
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplOpenGL3_Init();

		inter->runStyler();
	}
};
