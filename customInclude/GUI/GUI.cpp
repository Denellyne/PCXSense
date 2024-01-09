#include "GUI.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include <GLFW/glfw3.h>
#include <format>

#define GL_CLAMP_TO_EDGE 0x812F
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void app(controller& x360Controller,const GLuint* Images);
bool inline LoadTextureFromFile(const char* filename, GLuint* out_texture, int image_width, int image_height);
bool LoadTextureFromFile(GLuint* Images);

void setColors();
void inline drawController(float displaySizeX, float displaySizeY, float xMultiplier, float yMultiplier, const controller& x360Controller, const GLuint* Images);
void inline notificationBar(ImVec2 cursorPosition,const bool& isConnected,const int& batteryLevel,float& lightbar,const ImTextureID& updateButton);
static void testRumble(bool& rumbleWindow);


int GUI(controller& x360Controller){
    GLuint Images[17];
    
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "PSXSense", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    LoadTextureFromFile(Images);

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

    bool static rumbleWindow = false;
    static ImGuiIO& io = ImGui::GetIO();
    static float lightbar = 0.0f;
    if (lightbar >= 1) lightbar = 1;
    if (lightbar < 0) lightbar = 0;
    float xMultiplier = io.DisplaySize.x / 1280;
    float yMultiplier = io.DisplaySize.y / 720;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::Begin("PSXSense", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
    if (ImGui::Button("RAINBOW", { 20,20 })) x360Controller.rainbow = !x360Controller.rainbow;

    ImGui::SetCursorPos({ io.DisplaySize.x / 2.49f, io.DisplaySize.y / 3.935f });
    ImGui::Image((void*)Images[3], { (801 / 3) * io.DisplaySize.x / 1280,(388 / 2.7f) * io.DisplaySize.y / 720 }, {}, { 1,1 }, { x360Controller.RGB.red/255,x360Controller.RGB.green/255,x360Controller.RGB.blue/255,lightbar }); //RGB

    drawController(io.DisplaySize.x,io.DisplaySize.y,xMultiplier,yMultiplier,x360Controller,Images);

    //Setting colors for child window
    setColors();


    notificationBar({ io.DisplaySize.x - 210 , io.DisplaySize.y - 35 },x360Controller.isConnected,x360Controller.batteryLevel,lightbar,(void*)Images[16]);

    ImGui::SetCursorPos({ io.DisplaySize.x - 210, 50 });
    if (ImGui::Button("Rumble Test", { 20,20 })) rumbleWindow = true;

    if (rumbleWindow) testRumble(rumbleWindow);

    
    ImGui::PopStyleColor(3);

    ImGui::End();

}

void inline setColors() {
    ImGuiStyle& style = ImGui::GetStyle();

    ImGui::PopStyleVar();
    style.Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.082, 0.086, 0.09, 1);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.082, 0.086, 0.10, 1);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.082, 0.086, 0.10, 1);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.082, 0.086, 0.10, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.185, 0.186, 0.302, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.185, 0.186, 0.302, 1));

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

void testRumble(bool& rumbleWindow){
    static int motor1{}, motor2{};

    if (ImGui::Begin("Rumble Test##2", &rumbleWindow)) {

        ImGui::SliderInt("Right Motor", &motor1, 0, 255);
        ImGui::SliderInt("Left Motor", &motor2, 0, 255);
        
        rumble[0] = motor1;
        rumble[1] = motor2;

        ImGui::End();
    }
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

    //Controller
    ImGui::SetCursorPos(controllerPosition);
    ImGui::Image((void*)Images[1 - (bool)x360Controller.isConnected], controllerSize);


    //Left  Analogic
    ImGui::SetCursorPos({ ((displaySizeX / 2.935f) + (((int)(x360Controller.ControllerState.Gamepad.sThumbLX + 32768) / 470)) * xMultiplier) ,
                        (displaySizeY / 2.62f) + (((32767 - (int)x360Controller.ControllerState.Gamepad.sThumbLY) / 470)) * yMultiplier });

    if (x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
        ImGui::Image((void*)Images[2], { (226 / 3) * displaySizeX / 1280,(226 / 2.7f) * displaySizeY / 720 }, {}, { 1,1 }, { 0.8f, 0.8f, 0.8f, 1 });
    else ImGui::Image((void*)Images[2], { (226 / 3) * displaySizeX / 1280,(226 / 2.7f) * displaySizeY / 720 }, {}, { 1,1 }, { 1,1,1, (float)x360Controller.isConnected});

    //Right Analogic
    ImGui::SetCursorPos({ ((displaySizeX / 1.99f) + ((int)(x360Controller.ControllerState.Gamepad.sThumbRX + 32768) / 470) * xMultiplier) ,
                        (displaySizeY / 2.61f) + ((32767 - (int)x360Controller.ControllerState.Gamepad.sThumbRY) / 470) * yMultiplier });

    if (x360Controller.ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
        ImGui::Image((void*)Images[2], { (226 / 3) * displaySizeX / 1280,(226 / 2.7f) * displaySizeY / 720 }, {}, { 1,1 }, { 0.8f, 0.8f, 0.8f, 1 });

    else ImGui::Image((void*)Images[2], { (226 / 3) * displaySizeX / 1280,(226 / 2.7f) * displaySizeY / 720 }, {}, { 1,1 }, { 1,1,1, (float)x360Controller.isConnected });

    for (int i = 0; i < 12; i++) {
        if (buttonPressed[i]) {
            ImGui::SetCursorPos(controllerPosition);
            ImGui::Image((void*)Images[i+4], controllerSize);
        }
    }

}





bool LoadTextureFromFile(GLuint* Images) {

    LoadTextureFromFile("./images/dualsense.png", &Images[0], 2524, 1419);
    LoadTextureFromFile("./images/dualsenseD.png", &Images[1], 2524, 1419);
    LoadTextureFromFile("./images/stick.png", &Images[2], 226, 226);
    LoadTextureFromFile("./images/lightbar.png", &Images[3], 801, 388);
    LoadTextureFromFile("./images/Cross.png", &Images[4], 2524, 1419);
    LoadTextureFromFile("./images/Circle.png", &Images[5], 2524, 1419);
    LoadTextureFromFile("./images/Triangle.png", &Images[6], 2524, 1419);
    LoadTextureFromFile("./images/Square.png", &Images[7], 2524, 1419);
    LoadTextureFromFile("./images/Start.png", &Images[8], 2524, 1419);
    LoadTextureFromFile("./images/Select.png", &Images[9], 2524, 1419);
    LoadTextureFromFile("./images/DpadUp.png", &Images[10], 2524, 1419);
    LoadTextureFromFile("./images/DpadDown.png", &Images[11], 2524, 1419);
    LoadTextureFromFile("./images/DpadLeft.png", &Images[12], 2524, 1419);
    LoadTextureFromFile("./images/DpadRight.png", &Images[13], 2524, 1419);
    LoadTextureFromFile("./images/ShoulderLeft.png", &Images[14], 2524, 1419);
    LoadTextureFromFile("./images/ShoulderRight.png", &Images[15], 2524, 1419);
    LoadTextureFromFile("./images/updateButton.png", &Images[16], 200, 30);

    return true;
}

bool inline LoadTextureFromFile(const char* filename, GLuint* out_texture, int image_width, int image_height)
{
    // Load from file
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;

    return true;
}