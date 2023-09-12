// #include "imgui_interface.hpp"
// #include <imgui.h>

// #include <Geode/modify/CCEGLView.hpp>
// using namespace geode::prelude;

// class $modify(CCEGLView) {
//     void swapBuffers() {
//         if (ImGui_Interface::get()->isInitialized())
//             ImGui_Interface::get()->drawFrame();

//         return CCEGLView::swapBuffers();
//     }
// };

// void ImGui_Interface::setRenderer(std::function<void()> func) {
//     // m_renderer = func;
// }

// void ImGui_Interface::setSetup(std::function<void()> func) {
//     // m_setup = func;
// }

// bool ImGui_Interface::isInitialized() {
//     return m_initialized;
// }

// void ImGui_Interface::init() {
//     if (m_initialized) return;

//     ImGui::CreateContext();

//     ImGuiIO& io = ImGui::GetIO();
//     io.BackendRendererName = "zbot renderer";

//     static const auto fileName = (Mod::get()->getSaveDir() / "imgui.ini").string();
//     io.IniFilename = fileName.c_str();

//     m_setup();

//     m_fontTexture = new CCTexture2D();

//     unsigned char* tex_pixels = nullptr;
//     int tex_width, tex_height;
//     io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);

//     m_fontTexture->initWithData(tex_pixels, kCCTexture2DPixelFormat_Default, tex_width, tex_height, CCSize(tex_width, tex_height));

//     io.Fonts->SetTexID(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(m_fontTexture->getName())));

//     m_initialized = true;
// }

// CCPoint cocosToGL(const CCPoint& in) {
//     CCSize cocos = CCDirector::sharedDirector()->getWinSize();
//     CCSize gl = CCDirector::sharedDirector()->getOpenGLView()->getFrameSize();
    
//     return CCPoint(
//         in.x / cocos.width * gl.width,
//         (1.f - in.y / cocos.height) * gl.height
//     );
// }

// CCPoint glToCocos(const ImVec2& in) {
//     CCSize cocos = CCDirector::sharedDirector()->getWinSize();
//     CCSize gl = CCDirector::sharedDirector()->getOpenGLView()->getFrameSize();
    
//     return CCPoint(
//         in.x / gl.width * cocos.width,
//         (1.f - in.y / gl.height) * cocos.height
//     );
// }

// void ImGui_Interface::drawFrame() {
//     if (!m_initialized) return;

//     ImGuiIO& io = ImGui::GetIO();
//     CCDirector* director = CCDirector::sharedDirector();

//     CCSize cocos = director->getWinSize();
//     CCSize gl = director->getOpenGLView()->getFrameSize();

//     io.DisplaySize = ImVec2(gl.width, gl.height);
//     io.DisplayFramebufferScale = ImVec2(cocos.width / gl.width, cocos.height / gl.height);
//     io.DeltaTime = director->getDeltaTime();

//     CCKeyboardDispatcher* dp = CCKeyboardDispatcher::get();
//     io.KeyAlt = dp->getAltKeyPressed();
//     io.KeyShift = dp->getShiftKeyPressed();
//     io.KeyCtrl = dp->getCommandKeyPressed() || dp->getControlKeyPressed();

//     CCPoint mouse = cocosToGL(cocos::getMousePos());
//     io.AddMousePosEvent(mouse.x, mouse.y);

//     ImGui::NewFrame();
//     m_renderer();    
//     ImGui::Render();

//     ImDrawData* data = ImGui::GetDrawData();

//     ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//     glEnable(GL_SCISSOR_TEST);

//     GLuint vao;
//     GLuint vbos[2] = {0, 0};

//     glGenVertexArrays(1, &vao);
//     glBindVertexArray(vao);

//     glGenBuffers(2, &vbos[0]);
    
//     glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);

//     glEnableVertexAttribArray(kCCVertexAttrib_Position);
//     glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<GLvoid*>(offsetof(ImDrawVert, pos)));

//     glEnableVertexAttribArray(kCCVertexAttrib_TexCoords);
//     glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<GLvoid*>(offsetof(ImDrawVert, uv)));
    
//     glEnableVertexAttribArray(kCCVertexAttrib_Color);
//     glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<GLvoid*>(offsetof(ImDrawVert, col)));

//     CCGLProgram* shader = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor);
//     shader->use();
//     shader->setUniformsForBuiltins();

//     for (int i = 0; i < data->CmdListsCount; i++) {
//         ImDrawList* list = data->CmdLists[i];

// 		for (int j = 0; j < list->VtxBuffer.size(); j++) {
// 			const auto point = glToCocos(list->VtxBuffer[j].pos);
// 			list->VtxBuffer[j].pos = ImVec2(point.x, point.y);
// 		}

//         glBufferData(GL_ARRAY_BUFFER, list->VtxBuffer.size() * sizeof(ImDrawVert), list->VtxBuffer.Data, GL_STREAM_DRAW);
//         glBufferData(GL_ELEMENT_ARRAY_BUFFER, list->IdxBuffer.size() * sizeof(ImDrawIdx), list->IdxBuffer.Data, GL_STREAM_DRAW);

//         for (ImDrawCmd& cmd : list->CmdBuffer) {
//             ccGLBindTexture2D(static_cast<GLuint>(reinterpret_cast<uintptr_t>(cmd.GetTexID())));

//             CCPoint start = glToCocos(ImVec2(cmd.ClipRect.x, cmd.ClipRect.y));
//             CCPoint end = glToCocos(ImVec2(cmd.ClipRect.z, cmd.ClipRect.w));

//             if (end.x <= start.x || end.y >= start.y) continue;

//             CCDirector::sharedDirector()->getOpenGLView()->setScissorInPoints(start.x, start.y, end.x - start.x, start.y - end.y);

//             glDrawElements(GL_TRIANGLES, cmd.ElemCount, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>(cmd.IdxOffset * sizeof(ImDrawIdx)));
//         }
//     }

//     glBindVertexArray(0);
// 	glBindBuffer(GL_ARRAY_BUFFER, 0);
// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

// 	glDeleteBuffers(2, &vbos[0]);
// 	glDeleteVertexArrays(1, &vao);

// 	glDisable(GL_SCISSOR_TEST);
// }

// $on_mod(Loaded) {
//     ImGui_Interface::get()->init();
// }