/*******************************************************************************************
*
* * cc yourgame.c character.c -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib` -o YourGame
* * /usr/local/bin/envs/pepperscone/bin/python main.py
* * source /usr/local/bin/envs/pepperscone/bin/activate
*
********************************************************************************************/

#include "raylib.h"
#include "rlgl.h"
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "character.h"
#include <string.h>
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <unistd.h>
#include <pthread.h>


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
void *runClientThread(void*);



int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "raylib [shaders] example - custom uniform variable");
    InitProgram();

    SetConfigFlags(FLAG_MSAA_4X_HINT);      // Enable Multi Sampling Anti Aliasing 4x (if available)

    //Socket stuff
    int sock;
    struct sockaddr_in server;
    
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
    
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 65432 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
    
    puts("Connected\n");
    

   // Load model texture (diffuse map)
    Image mapTex = LoadImage("resources/maps/IpadProDistortionCalibrationMap.png");   // Load model texture (diffuse map)
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

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, runClientThread, &sock);

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {   

        //debugging poses
        if (IsKeyDown(KEY_UP)) {
            SetPose(1);
        } 

        if (IsKeyDown(KEY_DOWN)) {
            SetPose(2);
        } 

        //debugging shader
        if (IsKeyReleased(KEY_LEFT)) {
            showingShader = !showingShader;
        } 


        UpdateCamera(&camera);          // Update camera

        UpdateCharacter();

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginTextureMode(target);       // Enable drawing to texture
            ClearBackground(BLACK);  // Clear texture background

            BeginMode3D(camera);        // Begin 3d mode drawing
                rlPushMatrix();
                rlRotatef(180.0f, 0.0f, 1.0f, 0.0f);
                    DrawCharacter();
                rlPopMatrix();
            EndMode3D();                // End 3d mode drawing, returns to orthographic 2d mode

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

        EndDrawing();



        //----------------------------------------------------------------------------------
    }


    // De-Initialization
    //--------------------------------------------------------------------------------------
    pthread_exit(NULL);
    close(sock);
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
        int arraySize = encodedMap.width * encodedMap.height;
        // static Color mapColor[5595136];
        static Color* mapColor;
        mapColor = malloc(arraySize * sizeof(Color));
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

void *runClientThread(void* my_sock)
{
    char server_reply[2000];
    while(1)
    {
        //Receive a reply from the server
        if( recv(* (int*)my_sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
        }
            
        puts("Server reply :");
        puts(server_reply);
        if (TextIsEqual(server_reply, "listen")) {
            SetPose(1);
        } 
        if (TextIsEqual(server_reply, "exit listen")) {
            SetPose(2);
        } 
        memset(server_reply, 0, 2000 * (sizeof server_reply[0]) );
    }
}