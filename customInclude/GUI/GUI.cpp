#include "GUI.h"
#include "functionality.h"

void app(controller& x360Controller,const GLuint* Images);
void inline drawController(float displaySizeX, float displaySizeY, float xMultiplier, float yMultiplier, const controller& x360Controller, const GLuint* Images);
void inline notificationBar(ImVec2 cursorPosition,const bool& isConnected,const int& batteryLevel,float& lightbar,const ImTextureID& updateButton);
bool rumbleWindow = false;
float lightbar = 0.0f;


int GUI(controller& x360Controller){
    GLuint Images[17];
    
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "PSXSense", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    loadTexture(Images);

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

        app(x360Controller,Images);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
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
    float xMultiplier = io.DisplaySize.x / 1280;
    float yMultiplier = io.DisplaySize.y / 720;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::Begin("PSXSense", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
    if (ImGui::Button("RAINBOW", { 20,20 })) x360Controller.rainbow = !x360Controller.rainbow;

    //Setting colors for child window
    setColors();

    notificationBar({ io.DisplaySize.x - 210 , io.DisplaySize.y - 35 }, x360Controller.isConnected, x360Controller.batteryLevel, lightbar, (void*)Images[16]);

    drawController(io.DisplaySize.x, io.DisplaySize.y, xMultiplier, yMultiplier, x360Controller, Images);

    ImGui::SetCursorPos({ io.DisplaySize.x - 210, 50 });
    if (ImGui::Button("Rumble Test", { 20,20 })) rumbleWindow = true;

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

    ImGui::SetCursorPos({ cursorPosition.x ,0});
    if (ImGui::ImageButton(updateButton, { 200,29 })) autoUpdater();

    ImGui::EndChild();
}

void inline drawController(float displaySizeX, float displaySizeY, float xMultiplier, float yMultiplier, const controller& x360Controller,const GLuint* Images){
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


