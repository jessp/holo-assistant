#include "raylib.h"
#include "rlgl.h"
#include "stopWatch.h"
#include <stdio.h>

void InitStopWatch(void){
	stopWatch.model = LoadModel("resources/models/clock.iqm");
    stopWatch.texture = LoadTexture("resources/models/clock_texture.png");
    stopWatch.animsCount = 0;
    stopWatch.animFrameCounter = 0;
    stopWatch.anims = LoadModelAnimations("resources/models/clock.iqm", &stopWatch.animsCount);
    stopWatch.position = (Vector3){ 0.0f, -0.75f, -2.75f };
    SetMaterialTexture(&stopWatch.model.materials[0], MATERIAL_MAP_DIFFUSE, stopWatch.texture);
    
    /*
    There's an issue I found with IQM animations that I need to split the hands on clock animation
    into two segments when exporting, otherwise it alternates between just the first and last frame.
    FirstHalf measures whether we're playing the first or second clock hand animation.
    */
    stopWatch.firstHalf = false; 
    UpdateModelAnimation(stopWatch.model, stopWatch.anims[stopWatch.firstHalf], stopWatch.animFrameCounter);

    stopWatch.rotation = 0.0f;
    stopWatch.rotationSpeed = 6.0f;

}

void SetClockShader(Shader lightShader){
    stopWatch.model.materials[0].shader = lightShader;
}

void DrawStopWatch(void){
    stopWatch.rotation += (stopWatch.rotationSpeed);
    rlPushMatrix();
        rlRotatef(stopWatch.rotation, 0.0f, 1.0f, 0.0f);
        DrawModelEx(stopWatch.model, stopWatch.position, (Vector3){ 0.0f, 0.0f, 1.0f }, -90.0f, (Vector3){ 0.6f, 0.6f, 0.6f }, WHITE);
    rlPopMatrix();
}


void UpdateStopWatch(void){
	stopWatch.animFrameCounter++;

    if (stopWatch.animFrameCounter >= stopWatch.anims[stopWatch.firstHalf].frameCount){
    	stopWatch.animFrameCounter = 0;
        stopWatch.firstHalf = !stopWatch.firstHalf;
    }

    UpdateModelAnimation(stopWatch.model, stopWatch.anims[stopWatch.firstHalf], stopWatch.animFrameCounter);
}


void UnloadStopWatch(void){
    UnloadTexture(stopWatch.texture);
    UnloadModel(stopWatch.model);

    // Unload model animations data
    for (unsigned int i = 0; i < stopWatch.animsCount; i++) {
        UnloadModelAnimation(stopWatch.anims[i]);
    }
    //RL_FREE(anims); //TODO - debug error
}