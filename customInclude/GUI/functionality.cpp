#include "functionality.h"
#define GL_CLAMP_TO_EDGE 0x812F
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

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
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

     // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;

    return true;
}

bool loadTexture(GLuint* Images, GLFWwindow* window) {

    int width, height, channels;
    unsigned char* icon = stbi_load("./images/PCXSense.png", &width, &height, &channels, 4);
    GLFWimage images[1]{};
    images[0].width = width;
    images[0].height = height;
    images[0].pixels = icon;
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(icon);

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
    //16
    LoadTextureFromFile("./images/github.png", &Images[17], 512, 512);
    LoadTextureFromFile("./images/PCXSense.png", &Images[18], 218, 219);

    return true;
}

void setColors() {
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
