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
* * cc yourgame.c -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib` -o YourGame

*
********************************************************************************************/

#include "raylib.h"
#include <stdio.h>

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif



RenderTexture2D convertRGBATexture2Map(Image encodedMap, bool flipTexture, RenderTexture2D decodedMapResult){

        float mapDiv = 4095;
        Color *encodedColor32 = LoadImageColors(encodedMap);
        Color mapColor[encodedMap.width * encodedMap.height];

        Color ec;
        Vector4 tempColor;

        int LOAD_TEX_COLOR_BIT_DEPTH = 8;

        for (int pixel = 0; pixel < (encodedMap.width * encodedMap.height); ++pixel) {
            ec = encodedColor32[pixel];

            tempColor.w = ((ec.r << LOAD_TEX_COLOR_BIT_DEPTH) + ec.g) / mapDiv;
            tempColor.x = ((ec.b << LOAD_TEX_COLOR_BIT_DEPTH) + ec.a) / mapDiv;
         
            if (flipTexture) {
                tempColor.x = 1.0 - tempColor.x;
            }

            tempColor.y = 0.0;
            tempColor.z = 0.0;

            mapColor[pixel] = ColorFromNormalized(tempColor);

        }
        

        UpdateTexture(decodedMapResult.texture, mapColor);
        UnloadImageColors(encodedColor32);
        return decodedMapResult;
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1366;
    const int screenHeight = 1024;
    SetConfigFlags(FLAG_MSAA_4X_HINT);      // Enable Multi Sampling Anti Aliasing 4x (if available)

    InitWindow(screenWidth, screenHeight, "raylib [shaders] example - custom uniform variable");

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ -1.0f, 4.0f, 20.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, -80.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;


    Model model = LoadModel("resources/models/turret.obj");    
    Texture2D texture = LoadTexture("resources/models/turret_diffuse.png");   // Load model texture (diffuse map)
    Image mapTex = LoadImage("resources/maps/IpadProDistortionCalibrationMap-sm.png");   // Load model texture (diffuse map)
    RenderTexture2D decodedTex = LoadRenderTexture(mapTex.width, mapTex.height);
    decodedTex = convertRGBATexture2Map(mapTex, 1, decodedTex);
    Texture2D map = decodedTex.texture;

    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;                     // Set model diffuse texture

    Vector3 position = { 0.0f, 0.0f, 2.0f };                                    // Set model position
    float scale = 1.0;
    // Load postprocessing shader    
    Shader shader = LoadShader(TextFormat("resources/shaders/glsl140/swirl.vs"), TextFormat("resources/shaders/glsl140/swirl.fs", GLSL_VERSION));

    // Create a RenderTexture2D to be used for render to texture
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);


    SetTargetFPS(30);                   // Set our game to run at 30 frames-per-second
    //--------------------------------------------------------------------------------------

    
    int powerLoc = GetShaderLocation(shader, "_power");
    int alphaLoc = GetShaderLocation(shader, "_alpha");
    int texRotationVecLoc = GetShaderLocation(shader, "_TexRotationVec");
    int mapLoc = GetShaderLocation(shader, "texture1");
    
    // Send new value to the shader to be used on drawing
    float power = 1.0;
    float alpha = 1.0;
    float texRotationVec[4] = { 1.0f, 0.0f, 0.0f, 1.0f }; 
    SetShaderValue(shader, powerLoc, &power, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, alphaLoc, &alpha, SHADER_UNIFORM_FLOAT);

    SetShaderValue(shader, texRotationVecLoc, &texRotationVec, SHADER_UNIFORM_VEC4);

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {

        //move texture around to help see effect of arrow keys
        if (IsKeyDown(KEY_RIGHT)) scale += 0.1f;
        if (IsKeyDown(KEY_LEFT)) scale -= 0.1f;
        if (IsKeyDown(KEY_UP)) position.z -= 2.0f;
        if (IsKeyDown(KEY_DOWN)) position.z += 2.0f;


        UpdateCamera(&camera);          // Update camera
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginTextureMode(target);       // Enable drawing to texture
            ClearBackground(GRAY);  // Clear texture background

            BeginMode3D(camera);        // Begin 3d mode drawing
                DrawModel(model, position, scale, WHITE);   // Draw 3d model with texture
                DrawGrid(10, 1.0f);     // Draw a grid
            EndMode3D();                // End 3d mode drawing, returns to orthographic 2d mode

            DrawText("TEXT DRAWN IN RENDER TEXTURE", 200, 10, 30, RED);
        EndTextureMode();               // End drawing to texture (now we have a texture available for next passes)

        BeginDrawing();
            ClearBackground(RAYWHITE);  // Clear screen background

            // Enable shader using the custom uniform
            BeginShaderMode(shader);
                SetShaderValueTexture(shader, mapLoc, map);
                // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
                DrawTextureRec(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height }, (Vector2){ 0, 0 }, WHITE);
            EndShaderMode();

            // Draw some 2d text over drawn texture
            DrawText("(c) Barracks 3D model by Alberto Cano", screenWidth - 220, screenHeight - 20, 10, GRAY);
            DrawFPS(10, 10);
            printf("z: %f y: %f\n", position.z, position.y);

        EndDrawing();



        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(shader);               // Unload shader
    UnloadTexture(texture);             // Unload texture
    UnloadTexture(map);             // Unload texture
    UnloadModel(model);                 // Unload model
    UnloadRenderTexture(decodedTex);    // Unload texture
    UnloadRenderTexture(target);        // Unload render texture

    CloseWindow();                      // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

