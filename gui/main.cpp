#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"

#include <fstream>
#include <string>

void process_input_file(const std::string& input) {
    std::ofstream out("gui_input.txt");
    out << input;
    out.close();

    system("./syllogist temp_input.txt"); // Windows: "logic_tool.exe temp_input.txt"
}

int main() {
    InitWindow(800, 600, "Logic Proof GUI");
    SetTargetFPS(60);

    char input[4096] = "Paste your logic expressions here.";
    bool editMode = true;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        GuiTextBox((Rectangle){ 20, 20, 760, 480 }, input, 4096, editMode);

        if (GuiButton((Rectangle){ 20, 520, 100, 40 }, "Run")) {
            process_input_file(input);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
