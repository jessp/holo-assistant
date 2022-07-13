#include "raylib.h"
#include "character.h"
#include <stdio.h>

void InitCharacter(void){
	character.model = LoadModel("resources/models/body_empty.iqm");    
    character.texture = LoadTexture("resources/models/Base_texture.png");
    character.position = (Vector3){ 0.0f, 0.0f, 0.0f };
    character.animsCount = 0;
    character.animFrameCounter = 0;
    character.anims = LoadModelAnimations("resources/models/body_empty.iqm", &character.animsCount);
    character.currentPose = idle;
    character.nextPose = idle;
    character.justEnter = true;
    UpdateModelAnimation(character.model, character.anims[0], character.animFrameCounter);
    SetMaterialTexture(&character.model.materials[0], MATERIAL_MAP_DIFFUSE, character.texture);
}

void UnloadCharacter(void){
    UnloadTexture(character.texture);
    UnloadModel(character.model);

    // Unload model animations data
    for (unsigned int i = 0; i < character.animsCount; i++) {
    	UnloadModelAnimation(character.anims[i]);
    }
    //RL_FREE(anims); //TODO - debug error
}

void DrawCharacter(void){
	DrawModelEx(character.model, character.position, (Vector3){ 1.0f, 0.0f, 0.0f }, 0.0f, (Vector3){ 1.0f, 1.0f, 1.0f }, WHITE);
}

void EnterListenPose(void){
    if (character.justEnter == true && character.currentPose == character.nextPose){
        character.animFrameCounter = 0;
        character.justEnter = false;
    }

    if (character.animFrameCounter < character.anims[1].frameCount - 1){
        character.animFrameCounter++;
    } else {
        if (character.currentPose != character.nextPose){
            character.currentPose = character.nextPose;
        }
    }
    UpdateModelAnimation(character.model, character.anims[1], character.animFrameCounter);
}

void ExitListenPose(void){
    if (character.justEnter == true && character.currentPose == character.nextPose){
        character.animFrameCounter = character.anims[1].frameCount - 1;
        character.justEnter = false;
    }

    if (character.animFrameCounter > 0){
        character.animFrameCounter--;
        if (character.animFrameCounter == 0){
            character.nextPose = idle;
        }
        if (character.currentPose != character.nextPose){
            character.currentPose = character.nextPose;
        }
    }  
    UpdateModelAnimation(character.model, character.anims[1], character.animFrameCounter);
}

void IdlePose(void){
    if (character.justEnter == true && character.currentPose == character.nextPose){
        character.animFrameCounter = 0;
        character.justEnter = false;
    }

    character.animFrameCounter++;
    if (character.animFrameCounter >= character.anims[0].frameCount){
        character.animFrameCounter = 0;
        if (character.currentPose != character.nextPose){
            character.currentPose = character.nextPose;
        }
    }
    UpdateModelAnimation(character.model, character.anims[0], character.animFrameCounter);
}

void SetPose(int pose){
    character.nextPose = pose;
    character.justEnter = true;
}

void UpdateCharacter(void){
    if (character.currentPose == enterListen){
        EnterListenPose();
    } else if (character.currentPose == exitListen){
        ExitListenPose();
    } else {
        IdlePose();
    }
}