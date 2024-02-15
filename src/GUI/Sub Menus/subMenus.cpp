#include "subMenus.h"

extern bool rumbleWindow;
extern bool debugOpen;

void rumleTestWindow(bool& rumbleWindow) {
    static int motor1{}, motor2{};

    if (ImGui::Begin("Rumble Test##2", &rumbleWindow)) {

        ImGui::SliderInt("Right Motor", &motor1, 0, 255);
        ImGui::SliderInt("Left Motor", &motor2, 0, 255);

        rumble[0] = motor1;
        rumble[1] = motor2;

        ImGui::End();
    }
}

void debugMenu(controller& x360Controller) {
    static ImGuiIO& io = ImGui::GetIO();

    if (ImGui::Begin("Debug Menu", &debugOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings)) {
        ImGui::Text(std::format("Framerate: {}", (int)io.Framerate << 1).c_str()); //glfwWaitEventsTimeout makes so the reading is halved
        ImGui::Text("Controller Readings:");
        ImGui::Text(std::format("Left Joystick Horizontal  Value: {}", (int)(x360Controller.ControllerState.Gamepad.sThumbLX)).c_str());
        ImGui::Text(std::format("Left Joystick Vertical    Value: {}", (int)(x360Controller.ControllerState.Gamepad.sThumbLY)).c_str());
        ImGui::Text(std::format("Right Joystick Horizontal Value: {}", (int)(x360Controller.ControllerState.Gamepad.sThumbRX)).c_str());
        ImGui::Text(std::format("RightJoystick Vertical    Value: {}", (int)(x360Controller.ControllerState.Gamepad.sThumbRY)).c_str());
        ImGui::Text(std::format("Left Trigger              Value: {}", (int)x360Controller.ControllerState.Gamepad.bLeftTrigger).c_str());
        ImGui::Text(std::format("Right Trigger             Value: {}", (int)x360Controller.ControllerState.Gamepad.bRightTrigger).c_str());
    }
    ImGui::End();
}

