#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"

#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>

#ifdef _WIN32
int run_command_and_get_status(const std::string& command) {
    return std::system(command.c_str());
}
#else
#include <sys/wait.h>
int run_command_and_get_status(const std::string& command) {
    int status = std::system(command.c_str());
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    else
        return -1;
}
#endif

void process_input_file(const std::string& input, std::string& result_message) {
    std::ofstream out("gui_input.txt");
    out << input;
    out.close();

    std::filesystem::create_directory("output");

#ifdef _WIN32
    int exit_code = run_command_and_get_status("syllogist.exe gui_input.txt");
#else
    int exit_code = run_command_and_get_status("./syllogist gui_input.txt");
#endif

    switch (exit_code) {
        case 0: result_message = "Argument is valid. Wrote Aris proof to output directory."; break;
        case 1: result_message = "Argument is invalid. Wrote Willow proof to output directory"; break;
        case 2: result_message = "Parsing error occurred..."; break;
        default: result_message = "Unknown error occurred..."; break;
    }
}

int main() {
    InitWindow(800, 600, "Syllogist");
    SetTargetFPS(60);

    std::string input_text = "";
    std::string result_message = "";
    bool editMode = true;

    Rectangle textArea = { 20, 20, 760, 480 };
    int fontSize = 24;
    Vector2 textPos = { textArea.x + 5, textArea.y + 5 };

    float blinkTimer = 0.0f;
    bool showCursor = true;

    while (!WindowShouldClose()) {
        // Blinking cursor logic
        blinkTimer += GetFrameTime();
        if (blinkTimer >= 0.5f) {
            showCursor = !showCursor;
            blinkTimer = 0.0f;
        }

        // Input handling
        if (editMode) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= 32) input_text += (char)key;
                key = GetCharPressed();
            }

            int special = GetKeyPressed();
            if (special == KEY_BACKSPACE && !input_text.empty()) input_text.pop_back();
            else if (special == KEY_ENTER) input_text += '\n';

            // Copy/Paste
            if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
                if (IsKeyPressed(KEY_C)) {
                    SetClipboardText(input_text.c_str());
                } else if (IsKeyPressed(KEY_V)) {
                    input_text += GetClipboardText();
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw textbox background
        DrawRectangleRec(textArea, WHITE);
        DrawRectangleLinesEx(textArea, 1, LIGHTGRAY);

        // Draw text
        DrawTextEx(GetFontDefault(), input_text.c_str(), textPos, fontSize, 2, BLACK);

        // Draw cursor
        if (editMode && showCursor) {
            Vector2 textSize = MeasureTextEx(GetFontDefault(), input_text.c_str(), fontSize, 2);
            float cursorX = textPos.x + textSize.x;
            float cursorY = textPos.y + textSize.y - fontSize;
            DrawLine(cursorX, cursorY, cursorX, cursorY + fontSize, BLACK);
        }

        // Run button
        if (GuiButton((Rectangle){ 20, 520, 100, 40 }, "Run")) {
            process_input_file(input_text, result_message);
        }

        // Display result message
        DrawText(result_message.c_str(), 140, 530, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
