#include "ImGuiComponent.h"
#include "KernelCore/SceneManager.h"
#include "Configuration.h"

static GLuint g_VaoHandle = 0;
static GLuint g_VboHandle = 0;
static GLuint g_IboHandle = 0;

void ImGui_RenderDrawLists(ImDrawData* draw_data)
{
    ImGuiIO& io = ImGui::GetIO();
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);
    glBindVertexArray(g_VaoHandle);
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;
        
        glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IboHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);
        
        for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
        {
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }
    
    // Restore modified state
    glBindVertexArray(0);
    glBindBuffer( GL_ARRAY_BUFFER, 0);
}

veImGuiComponent::veImGuiComponent()
    : _isAnyWindowFocus(false)
{
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = veEvent::VE_KEY_TAB;                     // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = veEvent::VE_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = veEvent::VE_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = veEvent::VE_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = veEvent::VE_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = veEvent::VE_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = veEvent::VE_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = veEvent::VE_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = veEvent::VE_KEY_END;
    io.KeyMap[ImGuiKey_Delete] = veEvent::VE_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = veEvent::VE_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = veEvent::VE_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = veEvent::VE_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = veEvent::VE_KEY_A;
    io.KeyMap[ImGuiKey_C] = veEvent::VE_KEY_C;
    io.KeyMap[ImGuiKey_V] = veEvent::VE_KEY_V;
    io.KeyMap[ImGuiKey_X] = veEvent::VE_KEY_X;
    io.KeyMap[ImGuiKey_Y] = veEvent::VE_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = veEvent::VE_KEY_Z;
    
    io.RenderDrawListsFn = ImGui_RenderDrawLists;
    
    _renderProjMatrix = new veUniform("u_ProjectionMatrix");
}

veImGuiComponent::~veImGuiComponent()
{
    if (g_VaoHandle) glDeleteVertexArrays(1, &g_VaoHandle);
    if (g_VboHandle) glDeleteBuffers(1, &g_VboHandle);
    if (g_IboHandle) glDeleteBuffers(1, &g_IboHandle);
    ImGui::Shutdown();
}

void veImGuiComponent::initPass(veSceneManager *sm)
{
    _renderPass = new vePass;
    _renderPass->cullFace() = false;
    _renderPass->depthTest() = false;
    _renderPass->blendFunc() = veBlendFunc::ALPHA;
    _fontTexture = sm->createTexture("_VE_IMGUI_COMPONENT_FONT_TEXTURE");
    
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    _fontTexture->storage(width, height, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, pixels, 1);
    _fontTexture->setFilterMode(veTexture::LINEAR);
    _fontTexture->bind();
    io.Fonts->TexID = (void *)(intptr_t)_fontTexture->glTex();
    
    _renderPass->addUniform(_renderProjMatrix.get());
    _renderPass->addUniform(new veUniform("u_texture", 0));
    
    const char* vertex_shader = " \
    layout(location = 0) in vec2 position; \n \
    layout(location = 1) in vec2 texcoord; \n \
    layout(location = 2) in vec4 color; \n \
    uniform mat4 u_ProjectionMatrix; \n \
    out vec2 v_texcoord; \n \
    out vec4 v_color; \n \
    void main() \n \
    {                   \n \
        v_texcoord = texcoord;  \n \
        v_color = color; \n \
        gl_Position = u_ProjectionMatrix * vec4(position, 0.0, 1.0); \n \
    }";
    
    const char* fragment_shader = " \
    uniform sampler2D u_texture; \n \
    in vec2 v_texcoord; \n \
    in vec4 v_color; \n \
    layout(location=0) out vec4 fragColor;\n \
    void main() {  \n \
        fragColor = v_color * texture(u_texture, v_texcoord); \n \
    }";
    
    _renderPass->setShader(new veShader(veShader::VERTEX_SHADER, vertex_shader));
     _renderPass->setShader(new veShader(veShader::FRAGMENT_SHADER, fragment_shader));
}

bool veImGuiComponent::handle(veSceneManager *sm, const veEvent &event)
{
    ImGuiIO& io = ImGui::GetIO();
    switch (event.getEventType()) {
        case veEvent::VE_PRESS:
        case veEvent::VE_TOUCH_START:
        {
            if (event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_LEFT
                || event.getEventType() == veEvent::VE_TOUCH_START){
                io.MouseDown[0] = true;
            }else if (event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_RIGHT){
                io.MouseDown[1] = true;
            }else if (event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_MIDDLE){
                io.MouseDown[2] = true;
            }
            
            if (event.getEventType() == veEvent::VE_TOUCH_START){
                auto touch = event.getTouches()[0];
                io.MousePos.x = (touch.x * 0.5f + 0.5f) * event.getWindowWidth();
                io.MousePos.y = (1.0f - (touch.y * 0.5f + 0.5f)) * event.getWindowHeight();
            }
        }
            break;
        case veEvent::VE_RELEASE:
        case veEvent::VE_TOUCH_END:
        {
            if (event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_LEFT
                || event.getEventType() == veEvent::VE_TOUCH_END){
                io.MouseDown[0] = false;
            }else if (event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_RIGHT){
                io.MouseDown[1] = false;
            }else if (event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_MIDDLE){
                io.MouseDown[2] = false;
            }
        }
            break;
        case veEvent::VE_MOVE:
        case veEvent::VE_DRAG:    
        {
            io.MousePos.x = (event.getMouseX() * 0.5f + 0.5f) * event.getWindowWidth();
            io.MousePos.y = (1.0f - (event.getMouseY() * 0.5f + 0.5f)) * event.getWindowHeight();
        }
            break;
        case veEvent::VE_TOUCH_MOVE:
        {
            auto touch = event.getTouches()[0];
            io.MousePos.x = (touch.x * 0.5f + 0.5f) * event.getWindowWidth();
            io.MousePos.y = (1.0f - (touch.y * 0.5f + 0.5f)) * event.getWindowHeight();
        }
            break;
        case veEvent::VE_SCROLL_UP:
        case veEvent::VE_SCROLL_DOWN:
        {
            io.MouseWheel += event.getMouseScroll();
        }
            break;
        case veEvent::VE_DOWN:
        case veEvent::VE_UP:
        case veEvent::VE_REPEAT:
        {
            if (event.getEventType() == veEvent::VE_DOWN){
                io.KeysDown[event.getKeySymbol()] = true;
                ImGuiIO& io = ImGui::GetIO();
                io.AddInputCharacter((unsigned short)event.getKeyChar());
            } else if (event.getEventType() == veEvent::VE_UP){
                io.KeysDown[event.getKeySymbol()] = false;
            }
            
            io.KeyCtrl = io.KeysDown[veEvent::VE_KEY_LEFT_CONTROL] || io.KeysDown[veEvent::VE_KEY_RIGHT_CONTROL];
            io.KeyShift = io.KeysDown[veEvent::VE_KEY_LEFT_SHIFT] || io.KeysDown[veEvent::veEvent::VE_KEY_RIGHT_SHIFT];
            io.KeyAlt = io.KeysDown[veEvent::VE_KEY_LEFT_ALT] || io.KeysDown[veEvent::VE_KEY_RIGHT_ALT];
            io.KeySuper = io.KeysDown[veEvent::VE_KEY_LEFT_SUPER] || io.KeysDown[veEvent::VE_KEY_RIGHT_SUPER];
        }
            break;
        case veEvent::VE_WIN_INIT:
        case veEvent::VE_WIN_RESIZE:
        {
            io.DisplaySize = ImVec2(event.getWindowWidth(), event.getWindowHeight());
            io.DisplayFramebufferScale = ImVec2(veConfiguration::VE_DEVICE_PIXEL_RATIO, veConfiguration::VE_DEVICE_PIXEL_RATIO);
            
            _renderProjMatrix->setValue(veMat4(2.0f / event.getWindowWidth(),                            0.0f, 0.0f, -1.0f
                                             , 0.0f                         , 2.0f / -event.getWindowHeight(), 0.0f,  1.0f
                                             , 0.0f                         ,                            0.0f, -1.0f, 0.0f
                                             , 0.0f                         ,                            0.0f,  0.0f, 1.0f));
        }
            break;
        case veEvent::VE_WIN_NOFOCUS:
        {
            io.MousePos = ImVec2(-1, -1);
        }
            break;
        default:
            break;
    }
    return _isAnyWindowFocus;
}

void veImGuiComponent::beforeUpdate(veSceneManager *sm)
{
}

void veImGuiComponent::afterUpdate(veSceneManager *sm)
{

}

void veImGuiComponent::beforeRender(veSceneManager *sm)
{
}

void veImGuiComponent::afterRender(veSceneManager *sm)
{
    if (!_renderPass.valid())
        initPass(sm);
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = sm->getDeltaTime();
    ImGui::NewFrame();
    _isAnyWindowFocus = ImGui::IsMouseHoveringAnyWindow();
    
    if (_renderFunc)
        _renderFunc();
   
    veRenderCommand rc;
    rc.mask = 0xffffffff;
    rc.worldMatrix = new veMat4Ptr(veMat4::IDENTITY);
    rc.camera = sm->getCamera();
    rc.sceneManager = sm;
    rc.pass = _renderPass.get();
    _renderPass->apply(rc);
    
    if (!g_VaoHandle){
        glGenVertexArrays(1, &g_VaoHandle);
        glGenBuffers(1, &g_VboHandle);
        glGenBuffers(1, &g_IboHandle);
        glBindVertexArray(g_VaoHandle);
        glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF
    }
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);
    ImGui::Render();
    glDisable(GL_SCISSOR_TEST);
}