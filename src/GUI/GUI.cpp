#include "GUI.h"
#include "Functions\Misc\functionality.h"
#include "Sub Menus\subMenus.h"
#include "User Settings/Lightbar/Lightbar.h"
#include "ControllerIO/Device Hiding/deviceHiding.h"
#include "Startup/startup.h"
#include <conio.h>

bool rumbleWindow = false;
extern bool debugOpen = false;
extern bool macroOpen = false;
extern bool profileOpen = false;
extern bool lightbarOpen = false;
bool minimized = false;
float lightbar = 0.0f;

void inline app(controller& x360Controller,const GLuint* Images, std::vector<Macros>& Macro, std::vector<gameProfile>& gameProfiles);

void inline drawController(const float& displaySizeX, const float& displaySizeY, float xMultiplier, float yMultiplier, const controller& x360Controller, const GLuint* Images);

void inline notificationBar(ImVec2 cursorPosition,const bool& isConnected,const int& batteryLevel);

void inline topBar(const GLuint* Images, const float& displaySizeX, const float* RGB);

int GUI(controller& x360Controller,std::vector<Macros>& Macro, std::vector<gameProfile>& gameProfiles,bool minimized){
    GLuint Images[21];

    glfwInit();

    GLFWwindow* window = glfwCreateWindow(defaultWindowWidth, defaultWindowHeigth, "PCXSense", nullptr, nullptr);
    if (window == nullptr) return 1;
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    loadTexture(Images,window);
    if (minimized) glfwIconifyWindow(window);
    // GUI BoilerPlate

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    while (!glfwWindowShouldClose(window)) { // Render
        glfwWaitEventsTimeout(0.01666666666f); //60 fps

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        app(x360Controller,Images,Macro,gameProfiles);
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);   
    }
   
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate(); 
    exitFunction(2);
    return 0;
}

void inline app(controller& x360Controller,const GLuint* Images, std::vector<Macros>& Macro, std::vector<gameProfile>& gameProfiles) {
    extern bool triggerMaker, profileMacroOpen, lightEditor, buttonRemapper , profileEdit;

    //Boilerplate Window Code
    static ImGuiIO& io = ImGui::GetIO();
    if (lightbar >= 1) lightbar = 1;
    if (lightbar < 0) lightbar = 0;
    float xMultiplier = io.DisplaySize.x / defaultWindowWidth;
    float yMultiplier = io.DisplaySize.y / defaultWindowHeigth;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::Begin("PCXSense", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
    //Setting colors for child window
    setColors();
    topBar(Images,io.DisplaySize.x,x360Controller.RGB[0].colors);
    notificationBar({ io.DisplaySize.x , io.DisplaySize.y - 35 }, x360Controller.isConnected, x360Controller.batteryLevel);

    drawController(io.DisplaySize.x, io.DisplaySize.y, xMultiplier, yMultiplier, x360Controller, Images);

    if (rumbleWindow) rumleTestWindow(rumbleWindow);
    if (debugOpen) debugMenu(x360Controller);
    if (macroOpen) macroMenu(Macro,x360Controller);
    if (profileOpen) profileMenu(profileOpen, gameProfiles, x360Controller);

    //Prevents the app from stalling the profile changer
    profileEdit = (profileOpen == 1) * profileEdit;
    triggerMaker = (profileOpen == 1) * triggerMaker;
    profileMacroOpen = (profileOpen == 1) * profileMacroOpen;
    buttonRemapper = (profileOpen == 1) * buttonRemapper;

    if (lightbarOpen) lightbarEditor(lightbarOpen, x360Controller.RGB);

    ImGui::PopStyleColor(3);

    ImGui::End();

}

void inline notificationBar(ImVec2 cursorPosition,const bool& isConnected,const int& batteryLevel) {

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

    ImGui::SetCursorPos({ cursorPosition.x - 130 ,17});
    ImGui::Text(Version.c_str());

   // ImGui::SetCursorPos({500, 5});
   // ImGui::ColorButton("Connected", { 0,255,0,1 },ImGuiColorEditFlags_NoTooltip,{20,20});

    ImGui::EndChild();
}

void inline topBar(const GLuint* Images, const float& displaySizeX,const float* RGB) {

    ImGui::SetCursorPos({ displaySizeX-60,0 });
    ImVec2 combo_pos = ImGui::GetCursorScreenPos();
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::SetCursorScreenPos(ImVec2(combo_pos.x + style.FramePadding.x, combo_pos.y));
    float iconSize = ImGui::GetTextLineHeightWithSpacing() - style.FramePadding.y;

    if (ImGui::BeginCombo("##Misc","Misc", ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_PopupAlignLeft)) {
        if(ImGui::Selectable("##Github")) ShellExecute(0, 0, L"https://github.com/Denellyne", 0, 0, SW_SHOW);
        ImGui::SameLine(7);
        ImGui::Image((void*)Images[17], { iconSize,iconSize });
        ImGui::SameLine();
        ImGui::Text("Github");

        if (ImGui::Selectable("##PCXSense")) ShellExecute(0, 0, L"https://github.com/Denellyne/PCXSense", 0, 0, SW_SHOW);
        ImGui::SameLine(7);
        ImGui::Image((void*)Images[18], { iconSize,iconSize });
        ImGui::SameLine();
        ImGui::Text("PCXSense");

        if (ImGui::Selectable("##Debug Menu")) debugOpen = true;
        ImGui::SameLine(30);
        ImGui::Text("Debug Menu");


        if (ImGui::RadioButton("##Minimized", minimized))
            minimized = !minimized;
        ImGui::SameLine();
        if (ImGui::Selectable("##Startup")) setStartup(minimized);
        ImGui::SameLine(30);
        if (ImGui::BeginItemTooltip()) {
            ImGui::Text("Toggle if you want to start minimized");
            ImGui::EndTooltip();
        }
        ImGui::Text("Open at Startup");
       
        if (ImGui::Selectable("##Update")) autoUpdater();
        ImGui::SameLine(30);
        ImGui::Text("Update");

        ImGui::EndCombo();
    }

    ImGui::SetCursorPos({ displaySizeX - 216,0 });


    if (ImGui::BeginCombo("##Controller Settings", "Controller Settings", ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_PopupAlignLeft)) {
        if (ImGui::Selectable("##Rumble Test")) rumbleWindow = true;
        ImGui::SameLine(30);
        ImGui::Text("Rumble Settings");

        if (ImGui::Selectable("##Lightbar Settings")) lightbarOpen = true; //Ligthbar Menu
        ImGui::SameLine(7);
        ImGui::ColorButton("Lightbar", { (RGB[0]),(RGB[1]),(RGB[2]),1 },0, {iconSize,iconSize});
        ImGui::SameLine();
        ImGui::Text("Lightbar Settings");

        if (ImGui::Selectable("##Game Profiles")) profileOpen = true;
        ImGui::SameLine(30);
        ImGui::Text("Game Profiles");

     //   if (ImGui::Selectable("##Device Hiding")) hideDevice();
     //   ImGui::SameLine(30);
     //   ImGui::Text("Hide current device");

        if (ImGui::Selectable("##Macros"))  macroOpen = true; //Macro Editor
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
    ImGui::Image((void*)Images[3], { (801 / 3) * displaySizeX / 1280,(388 / 2.7f) * displaySizeY / 720 }, {}, { 1,1 }, { (x360Controller.RGB[0].colors[0] ),(x360Controller.RGB[0].colors[1]),(x360Controller.RGB[0].colors[2]),lightbar }); //RGB

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



