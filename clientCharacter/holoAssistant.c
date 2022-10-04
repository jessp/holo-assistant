/*******************************************************************************************
*
* * Holographic Virutal Assistant
* * 
* * Run with:
* * cd clientCharacter && cc holoAssistant.c character.c stopWatch.c -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib` -o HoloAssistant && ./HoloAssistant
*
********************************************************************************************/

#include "raylib.h"
#include "rlgl.h"
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "character.h"
#include "stopWatch.h"
#include <string.h>
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <unistd.h>
#include <pthread.h>
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"


#define GLSL_VERSION            100


//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
const Vector3 tabletScreenScale = {4.0f, 3.0f, 1.0f};
//shader values
bool showingShader = true;
const float power = 1.0;
const float alpha = 1.0;
//classes
Camera camera = { 0 };
Character character = { 0 };
StopWatch stopWatch = { 0 };

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
RenderTexture2D convertRGBATexture2Map(Image encodedMap, bool flipTexture, RenderTexture2D decodedMapResult);
int getAspectRatioScale(int screenX, int screenY, Vector3 resolution);
static void InitProgram(void);
void *runClientThread(void*);



int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(0, 0, "Holographic Virtual Assistant");
    InitProgram();
    ToggleFullscreen();
    DisableCursor();
    int currentMonitor = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(currentMonitor);
    int monitorHeight = GetMonitorHeight(currentMonitor);
    //get the largest 4 x 3 ractangle we can to show our image
    int screenScale = getAspectRatioScale(monitorWidth, monitorHeight, tabletScreenScale);
    int displayRatioWidth = screenScale * tabletScreenScale.x;
    int displayRatioHeight = screenScale * tabletScreenScale.y;
    Rectangle dest = {(int)((monitorWidth - displayRatioWidth)/2), (int)((monitorHeight - displayRatioHeight)/2), (float) displayRatioWidth, (float) displayRatioHeight}; //for final render texture


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
    Shader warpShader = LoadShader(TextFormat("resources/shaders/glsl%i/warp.vs", GLSL_VERSION), TextFormat("resources/shaders/glsl%i/warp.fs", GLSL_VERSION));

    //Load lighting shader
    Shader lightingShader = LoadShader(TextFormat("resources/shaders/glsl%i/base_lighting.vs", GLSL_VERSION),
                               TextFormat("resources/shaders/glsl%i/lighting.fs", GLSL_VERSION));

    // Get some required shader locations
    lightingShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(lightingShader, "viewPos");
    // Ambient light level (some basic lighting)
    int ambientLoc = GetShaderLocation(lightingShader, "ambient");
    SetShaderValue(lightingShader, ambientLoc, (float[4]){ 2.5f, 2.5f, 2.5f, 1.0f }, SHADER_UNIFORM_VEC4);
    // Create lights
    Light lights[MAX_LIGHTS] = { 0 };

    if (GLSL_VERSION == 330) {
        lights[0] = CreateLight(LIGHT_POINT, (Vector3){ 0, 0, -50 }, Vector3Zero(), WHITE, lightingShader);
    }
    else {
        lights[0] = CreateLight(LIGHT_POINT, (Vector3){ 0, 0, 50 }, Vector3Zero(), WHITE, lightingShader);
    }

    //apply the lighting shader to the character
    SetCharacterShader(lightingShader);
    // Create a RenderTexture2D to be used for render to texture
    RenderTexture2D target = LoadRenderTexture(displayRatioWidth, displayRatioHeight);
    
    SetTargetFPS(30);                   // Set our game to run at 30 frames-per-second
    //--------------------------------------------------------------------------------------

    
    int powerLoc = GetShaderLocation(warpShader, "_power");
    int alphaLoc = GetShaderLocation(warpShader, "_alpha");
    int texRotationVecLoc = GetShaderLocation(warpShader, "_TexRotationVec");
    int mapLoc = GetShaderLocation(warpShader, "texture1");
    int shaderLoc = GetShaderLocation(warpShader, "texture0");

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
    SetShaderValue(warpShader, powerLoc, &power, SHADER_UNIFORM_FLOAT);
    SetShaderValue(warpShader, alphaLoc, &alpha, SHADER_UNIFORM_FLOAT);
    SetShaderValue(warpShader, texRotationVecLoc, &texRotationVec, SHADER_UNIFORM_VEC4);

    pthread_t threadId;
    pthread_create(&threadId, NULL, runClientThread, &sock);

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
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(lightingShader, lightingShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        UpdateCharacter();
        UpdateStopWatch();

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginTextureMode(target);       // Enable drawing to texture
            ClearBackground(BLACK);  // Clear texture background

            BeginMode3D(camera);        // Begin 3d mode drawing
                rlPushMatrix();
                rlRotatef(180.0f, 0.0f, 1.0f, 0.0f);
                    DrawCharacter();
                    DrawStopWatch();
                rlPopMatrix();
            EndMode3D();                // End 3d mode drawing, returns to orthographic 2d mode

        EndTextureMode();               // End drawing to texture (now we have a texture available for next passes)

        BeginDrawing();
            ClearBackground(BLACK);  // Clear screen background

            if (showingShader){
                // Enable shader using the custom uniform
                BeginShaderMode(warpShader);
                    SetShaderValueTexture(warpShader, mapLoc, map);
                    // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
                    DrawTexturePro(target.texture, (Rectangle){ (float)target.texture.width/2-(float)displayRatioWidth/4, -(float)displayRatioHeight/2, (float)displayRatioWidth/2, -(float)displayRatioHeight/2 }, dest, (Vector2){0,0}, 0.0f, WHITE);
                EndShaderMode();
            } else {
                DrawTextureRec(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height }, (Vector2){ (int)((monitorWidth - displayRatioWidth)/2), (int)((monitorHeight - displayRatioHeight)/2) }, WHITE);
            }


        EndDrawing();



        //----------------------------------------------------------------------------------
    }


    // De-Initialization
    //--------------------------------------------------------------------------------------
    pthread_exit(NULL);
    close(sock);
    UnloadShader(warpShader);               // Unload shader
    UnloadShader(lightingShader);   // Unload shader
    UnloadTexture(map);             // Unload texture
    UnloadRenderTexture(decodedTex);    // Unload texture
    UnloadRenderTexture(target);        // Unload render texture
    UnloadCharacter();
    UnloadStopWatch();
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
    InitStopWatch();
}

RenderTexture2D convertRGBATexture2Map(Image encodedMap, bool flipTexture, RenderTexture2D decodedMapResult){

        float mapDiv = 4095;
        Color *encodedColor32 = LoadImageColors(encodedMap);
        int arraySize = encodedMap.width * encodedMap.height;
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

void *runClientThread(void* mySock)
{
    char serverReply[2000];
    while(1)
    {
        //Receive a reply from the server
        if( recv(* (int*)mySock , serverReply , 2000 , 0) < 0)
        {
            puts("recv failed");
        }
            
        puts("Server reply :");
        puts(serverReply);
        if (TextIsEqual(serverReply, "listen\n")) {
            SetPose(1);
        } 
        if (TextIsEqual(serverReply, "exit listen\n")) {
            SetPose(2);
        } 
        if (TextIsEqual(serverReply, "dab\n")) {
            SetPose(3);
        } 
        if (TextIsEqual(serverReply, "wave\n")) {
            SetPose(4);
        } 
        if (TextIsEqual(serverReply, "talk\n")) {
            SetTalk(true);
        } 
        if (TextIsEqual(serverReply, "exit talk\n")) {
            SetTalk(false);
        } 
        memset(serverReply, 0, 2000 * (sizeof serverReply[0]) );
    }
}

int getAspectRatioScale(int screenX, int screenY, Vector3 resolution){
    int scale = floor(fminf(screenX/resolution.x, screenY/resolution.y));
    return scale;
}