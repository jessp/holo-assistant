/*******************************************************************************************
*
*   raylib [shaders] example - Apply a postprocessing shader and connect a custom uniform variable
*
*   NOTE: This example requires raylib OpenGL 3.3 or ES2 versions for shaders support,
*         OpenGL 1.1 does not support shaders, recompile raylib to OpenGL 3.3 version.
*
*   NOTE: Shaders used in this example are #version 330 (OpenGL 3.3), to test this example
*         on OpenGL ES 2.0 platforms (Android, Raspberry Pi, HTML5), use #version 100 shaders
*         raylib comes with shaders ready for both versions, check raylib/shaders install folder
*
*   This example has been created using raylib 1.3 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
* * cc yourgame.c character.c -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib pocketsphinx` -o YourGame

*
********************************************************************************************/

#include "raylib.h"
#include "rlgl.h"
#include "pocketsphinx.h"
#include <raymath.h>
#include <stdio.h>
#include <math.h>
#include "character.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
const int screenWidth = 1366;
const int screenHeight = 1024;                                   // Set model position
const Vector3 tabletScreenScale = {4.0f, 3.0f, 1.0f};
const float scale = 1.0;
//shader values
bool showingShader = false;
const float power = 1.0;
const float alpha = 1.0;
//classes
Camera camera = { 0 };
Character character = { 0 };

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
RenderTexture2D convertRGBATexture2Map(Image encodedMap, bool flipTexture, RenderTexture2D decodedMapResult);
static void InitProgram(void);

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "raylib [shaders] example - custom uniform variable");
    InitProgram();

    SetConfigFlags(FLAG_MSAA_4X_HINT);      // Enable Multi Sampling Anti Aliasing 4x (if available)


   // Load model texture (diffuse map)
    Image mapTex = LoadImage("resources/maps/IpadProDistortionCalibrationMap-sm.png");   // Load model texture (diffuse map)
    RenderTexture2D decodedTex = LoadRenderTexture(mapTex.width, mapTex.height);
    decodedTex = convertRGBATexture2Map(mapTex, true, decodedTex);
    Texture2D map = decodedTex.texture;

    // Load postprocessing shader    
    Shader shader = LoadShader(TextFormat("resources/shaders/glsl330/warp.vs"), TextFormat("resources/shaders/glsl330/warp.fs", GLSL_VERSION));

    // Create a RenderTexture2D to be used for render to texture
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    
    SetTargetFPS(30);                   // Set our game to run at 30 frames-per-second
    //--------------------------------------------------------------------------------------

    
    int powerLoc = GetShaderLocation(shader, "_power");
    int alphaLoc = GetShaderLocation(shader, "_alpha");
    int texRotationVecLoc = GetShaderLocation(shader, "_TexRotationVec");
    int mapLoc = GetShaderLocation(shader, "texture1");
    int shaderLoc = GetShaderLocation(shader, "texture0");

    Vector3 axis = { 0, 0, 1 };
    Quaternion rot = QuaternionFromAxisAngle(axis, 0.0f);
    Matrix matScale = MatrixScale(tabletScreenScale.x, tabletScreenScale.y, tabletScreenScale.z);
    Matrix matRotation = MatrixRotate(axis,  0.0f);
    Matrix matTranslation = MatrixTranslate(0, 0, 0);
    Matrix matTransform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

    Matrix m = MatrixMultiply(
            MatrixScale(1.0f/tabletScreenScale.x, 1.0f/tabletScreenScale.y, 1), 
            matTransform
            );

    Vector4 texRotationVec = { m.m0, m.m4, m.m1, m.m5 }; 
    SetShaderValue(shader, powerLoc, &power, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, alphaLoc, &alpha, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, texRotationVecLoc, &texRotationVec, SHADER_UNIFORM_VEC4);

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {



        UpdateCamera(&camera);          // Update camera

        UpdateCharacter();

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginTextureMode(target);       // Enable drawing to texture
            ClearBackground(GRAY);  // Clear texture background

            BeginMode3D(camera);        // Begin 3d mode drawing
                rlPushMatrix();
                rlRotatef(180.0f, 0.0f, 1.0f, 0.0f);
                    DrawCharacter();
                rlPopMatrix();
                DrawGrid(10, 1.0f);     // Draw a grid
            EndMode3D();                // End 3d mode drawing, returns to orthographic 2d mode

            DrawText("TEXT DRAWN IN RENDER TEXTURE", 200, 10, 30, RED);
        EndTextureMode();               // End drawing to texture (now we have a texture available for next passes)

        BeginDrawing();
            ClearBackground(RAYWHITE);  // Clear screen background

            if (showingShader){
                // Enable shader using the custom uniform
                BeginShaderMode(shader);
                    SetShaderValueTexture(shader, mapLoc, map);
                    // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
                    DrawTextureRec(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height }, (Vector2){ 0, 0 }, WHITE);
                EndShaderMode();
            } else {
                DrawTextureRec(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height }, (Vector2){ 0, 0 }, WHITE);
            }

            DrawFPS(10, 10);
            // printf("%d\n", animFrameCounter);

        EndDrawing();



        //----------------------------------------------------------------------------------
    }


    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(shader);               // Unload shader
    UnloadTexture(map);             // Unload texture
    UnloadRenderTexture(decodedTex);    // Unload texture
    UnloadRenderTexture(target);        // Unload render texture
    UnloadCharacter();
    CloseWindow();                      // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void InitProgram(void)
{
    // Define the camera to look into our 3d world
    camera.position = (Vector3){ 0.0f, 0.0f, -17.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    InitCharacter();
}

RenderTexture2D convertRGBATexture2Map(Image encodedMap, bool flipTexture, RenderTexture2D decodedMapResult){

        float mapDiv = 4095;
        Color *encodedColor32 = LoadImageColors(encodedMap);
        Color mapColor[encodedMap.width * encodedMap.height];

        Color ec;
        Vector4 tempColor;

        int LOAD_TEX_COLOR_BIT_DEPTH = 8;

        for (int pixel = 0; pixel < (encodedMap.width * encodedMap.height); ++pixel) {
            ec = encodedColor32[pixel];

            tempColor.x = ((ec.r << LOAD_TEX_COLOR_BIT_DEPTH) + ec.g) / mapDiv;
            tempColor.y = ((ec.b << LOAD_TEX_COLOR_BIT_DEPTH) + ec.a) / mapDiv;
         
            if (flipTexture) {
                tempColor.y = 1.0 - tempColor.y;
            }

            tempColor.w = 0.0;
            tempColor.z = 0.0;

            //takes format x y z w
            mapColor[pixel] = ColorFromNormalized(tempColor);

        }
        

        UpdateTexture(decodedMapResult.texture, mapColor);
        UnloadImageColors(encodedColor32);
        return decodedMapResult;
}
