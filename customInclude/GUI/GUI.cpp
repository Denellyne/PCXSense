#include "GUI.h"
#include "functionality.h"
#include <conio.h>
#define defaultWindowWidth 1280
#define defaultWindowHeigth 720

void app(controller& x360Controller,const GLuint* Images);
void inline drawController(const float& displaySizeX, const float& displaySizeY, float xMultiplier, float yMultiplier, const controller& x360Controller, const GLuint* Images);
void inline notificationBar(ImVec2 cursorPosition,const bool& isConnected,const int& batteryLevel,float& lightbar,const ImTextureID& updateButton);
void inline topBar(const GLuint* Images, const float& displaySizeX, const float& displaySizeY,const float* RGB);

bool rumbleWindow = false;
float lightbar = 0.0f;

//const HWND thisProcess = FindWindow(NULL, L"PCXSense");

/*
bool inline isFocus() {
    
    HWND activeWindow = GetActiveWindow();

    if (thisProcess != activeWindow) return true;
    return false;

}
*/ //Needs testing

int GUI(controller& x360Controller){
    GLuint Images[18];
    
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(defaultWindowWidth, defaultWindowHeigth, "PCXSense", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    loadTexture(Images,window);

    // GUI BoilerPlate

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    while (!glfwWindowShouldClose(window)) { // Render
        glfwPollEvents();
   
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        app(x360Controller, Images);
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
/*
        if (isFocus()) {
            
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            app(x360Controller, Images);
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
            

        }
 */      

    }

    glfwTerminate();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    exitFunction(2);
    return 0;
}

void app(controller& x360Controller,const GLuint* Images) {
    //Boilerplate Window Code

    static ImGuiIO& io = ImGui::GetIO();
    if (lightbar >= 1) lightbar = 1;
    if (lightbar < 0) lightbar = 0;
    float xMultiplier = io.DisplaySize.x / defaultWindowWidth;
    float yMultiplier = io.DisplaySize.y / defaultWindowHeigth;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::Begin("PSXSense", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
    if (ImGui::Button("RAINBOW", { 90,20 })) x360Controller.rainbow = !x360Controller.rainbow;
    float RGB[3] = { x360Controller.RGB.red / 255,x360Controller.RGB.green / 255,x360Controller.RGB.blue / 255 };


    //Setting colors for child window
    setColors();
    topBar(Images,io.DisplaySize.x,io.DisplaySize.y,RGB);
    notificationBar({ io.DisplaySize.x , io.DisplaySize.y - 35 }, x360Controller.isConnected, x360Controller.batteryLevel, lightbar, (void*)Images[16]);

    drawController(io.DisplaySize.x, io.DisplaySize.y, xMultiplier, yMultiplier, x360Controller, Images);

    if (rumbleWindow) rumleTestWindow(rumbleWindow);

    ImGui::PopStyleColor(3);

    ImGui::End();

}


void inline notificationBar(ImVec2 cursorPosition,const bool& isConnected, const int& batteryLevel, float& lightbar, const ImTextureID& updateButton) {

    ImGui::SetCursorPosY(cursorPosition.y);

    ImGui::BeginChild("##Notifications");

    ImGui::SetCursorPosX(5);

    if (isConnected) {
        lightbar += 0.05;
        ImGui::Text("Device Status: Connected");
        ImGui::SetCursorPosX(5);
        ImGui::Text(std::format("Device Battery: {}%%", batteryLevel).c_str());
    }
    else {
        lightbar -= 0.005;
        ImGui::Text("Device Status: Disconnected");
    }
    
#ifdef _DEBUG
    static ImGuiIO& io = ImGui::GetIO();
    ImGui::SetCursorPos({ 200,17 });
    ImGui::Text(std::format("Framerate: {}", (int)io.Framerate).c_str());
#endif // _DEBUG

    ImGui::SetCursorPos({ cursorPosition.x - 90 ,17});
    ImGui::Text("ver Beta0.3");

    ImGui::EndChild();
}

inline void topBar(const GLuint* Images, const float& displaySizeX, const float& displaySizeY, const float* RGB) {

    ImGui::SetCursorPos({ displaySizeX-60,0 });
    ImVec2 combo_pos = ImGui::GetCursorScreenPos();
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::SetCursorScreenPos(ImVec2(combo_pos.x + style.FramePadding.x, combo_pos.y));
    float h = ImGui::GetTextLineHeightWithSpacing() - style.FramePadding.y;

    if (ImGui::BeginCombo("##Misc","Misc", ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_PopupAlignLeft)) {
        if(ImGui::Selectable("##Github")) ShellExecute(0, 0, L"https://github.com/Denellyne", 0, 0, SW_SHOW);
        ImGui::SameLine(7);
        ImGui::Image((void*)Images[17], { h,h });
        ImGui::SameLine();
        ImGui::Text("Github");

        if (ImGui::Selectable("##PCXSense")) ShellExecute(0, 0, L"https://github.com/Denellyne/PCXSense", 0, 0, SW_SHOW);
        ImGui::SameLine(7);
        ImGui::Image((void*)Images[18], { h,h });
        ImGui::SameLine();
        ImGui::Text("PCXSense");

        if (ImGui::Selectable("##Debug Menu")) ;
        ImGui::SameLine(30);
        ImGui::Text("Debug Menu");

        if (ImGui::Selectable("##Update")) autoUpdater();
        ImGui::SameLine(30);
        ImGui::Text("Update");



        ImGui::EndCombo();
    }

    ImGui::SetCursorPos({ displaySizeX - 216,0 });


    if (ImGui::BeginCombo("##Controller Settings", "Controller Settings", ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_PopupAlignLeft)) {
        if (ImGui::Selectable("##Rumble Test")) rumbleWindow = true;
        ImGui::SameLine(30);
        ImGui::Text("Rumble Test");

        if (ImGui::Selectable("##Lightbar Settings")); //Ligthbar Menu
        ImGui::SameLine(7);
        ImGui::ColorButton("Lightbar", { RGB[0],RGB[1],RGB[2],1 },0, {h,h});
        ImGui::SameLine();
        ImGui::Text("Lightbar Settings");

        if (ImGui::Selectable("##Adaptive Triggers")); //Ligthbar Menu
        ImGui::SameLine(30);
        ImGui::Text("Adaptive Triggers");

        if (ImGui::Selectable("##Macros")); //Ligthbar Menu
        ImGui::SameLine(30);
        ImGui::Text("Macros");



        ImGui::EndCombo();
    }




}

void inline drawController(const float& displaySizeX, const float& displaySizeY, float xMultiplier, float yMultiplier, const controller& x360Controller,const GLuint* Images){
    ImVec2 controllerPosition = { displaySizeX / 5.8f, displaySizeY / 5.8f };
    ImVec2 controllerSize = { (2524 / 3) * displaySizeX / 1280,(1419 / 2.7f) * displaySizeY / 720 };

    bool buttonPressed[12]{
        x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A ,x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B,x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y,
        x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X,x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK,x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START,
        x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP,x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN,x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT,
        x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT,x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER,x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER
    };
    
    //Lightbar
    ImGui::SetCursorPos({ displaySizeX / 2.49f, displaySizeY / 3.935f });
    ImGui::Image((void*)Images[3], { (801 / 3) * displaySizeX / 1280,(388 / 2.7f) * displaySizeY / 720 }, {}, { 1,1 }, { x360Controller.RGB.red / 255,x360Controller.RGB.green / 255,x360Controller.RGB.blue / 255,lightbar }); //RGB


    //Controller
    ImGui::SetCursorPos(controllerPosition);
    ImGui::Image((void*)Images[1 - (bool)x360Controller.isConnected], controllerSize);


    //Left  Analogic
    ImGui::SetCursorPos({ ((displaySizeX / 2.935f) + (((int)(x360Controller.ControllerState.Gamepad.sThumbLX + 32768) / 470)) * xMultiplier) ,
                        (displaySizeY / 2.62f) + (((32767 - (int)x360Controller.ControllerState.Gamepad.sThumbLY) / 470)) * yMultiplier });

    if (x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
        ImGui::Image((void*)Images[2], { (226 / 3) * displaySizeX / 1280,(226 / 2.7f) * displaySizeY / 720 }, {}, { 1,1 }, { 0.8f, 0.8f, 0.8f, 1 });
    else ImGui::Image((void*)Images[2], { (226 / 3) * displaySizeX / 1280,(226 / 2.7f) * displaySizeY / 720 }, {}, { 1,1 }, { 1,1,1, (float)x360Controller.isConnected });

    //Right Analogic
    ImGui::SetCursorPos({ ((displaySizeX / 1.99f) + ((int)(x360Controller.ControllerState.Gamepad.sThumbRX + 32768) / 470) * xMultiplier) ,
                        (displaySizeY / 2.61f) + ((32767 - (int)x360Controller.ControllerState.Gamepad.sThumbRY) / 470) * yMultiplier });

    if (x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
        ImGui::Image((void*)Images[2], { (226 / 3) * displaySizeX / 1280,(226 / 2.7f) * displaySizeY / 720 }, {}, { 1,1 }, { 0.8f, 0.8f, 0.8f, 1 });

    else ImGui::Image((void*)Images[2], { (226 / 3) * displaySizeX / 1280,(226 / 2.7f) * displaySizeY / 720 }, {}, { 1,1 }, { 1,1,1, (float)x360Controller.isConnected });

    if (x360Controller.isConnected) {
        
        for (int i = 0; i < 12; i++) {
            if (buttonPressed[i]) {
                ImGui::SetCursorPos(controllerPosition);
                ImGui::Image((void*)Images[i + 4], controllerSize);
            }
        }
    }

}


