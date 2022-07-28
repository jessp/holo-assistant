#include "raylib.h"
#include "character.h"
#include <stdio.h>

void InitCharacter(void){
	character.model = LoadModel("resources/models/body_empty.iqm");    
    character.texture = LoadTexture("resources/models/Base_texture.png");
    character.position = (Vector3){ 0.0f, -1.0f, 0.0f };
    character.animsCount = 0;
    character.animFrameCounter = 0;
    character.anims = LoadModelAnimations("resources/models/body_empty.iqm", &character.animsCount);
    character.currentPose = idle;
    character.nextPose = idle;
    character.justEnter = true;
    UpdateModelAnimation(character.model, character.anims[0], character.animFrameCounter);
    SetMaterialTexture(&character.model.materials[0], MATERIAL_MAP_DIFFUSE, character.texture);
}

void SetCharacterShader(Shader lightShader){
    character.model.materials[0].shader = lightShader;
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
	DrawModelEx(character.model, character.position, (Vector3){ 1.0f, 0.0f, 0.0f }, 0.0f, (Vector3){ 1.75f, 1.75f, 1.75f }, WHITE);
}

void goToNext(void){
    if (character.currentPose != character.nextPose){
        character.currentPose = character.nextPose;
    }
}

void doPose(bool forward, bool loop, int poseIndex, bool backToIdle){
    if (character.justEnter == true && character.currentPose == character.nextPose){
        if (forward){
            character.animFrameCounter = 0;
        } else {
            character.animFrameCounter = character.anims[poseIndex].frameCount - 1;
        }
        character.justEnter = false;
    }

    if (forward){
        if (loop){
            character.animFrameCounter++;
            if (character.animFrameCounter >= character.anims[poseIndex].frameCount){
                character.animFrameCounter = 0;
                goToNext();
            }
        } else {
            if (character.animFrameCounter < character.anims[poseIndex].frameCount - 1){
                character.animFrameCounter++;
                if (backToIdle && character.animFrameCounter == character.anims[poseIndex].frameCount - 1){
                    character.nextPose = idle;
                }
            } else {
                goToNext();
            }
        }
    } else {
        if (loop){
            character.animFrameCounter--;
            if (character.animFrameCounter <= 0){
                character.animFrameCounter = character.anims[poseIndex].frameCount - 1;
                goToNext();
            }
        } else {
            if (character.animFrameCounter > 0){
                character.animFrameCounter--;
                if (backToIdle && character.animFrameCounter == 0){
                    character.nextPose = idle;
                }
            } else {
                goToNext();
            }  
        }

    }
        
    UpdateModelAnimation(character.model, character.anims[poseIndex], character.animFrameCounter);
}

void EnterListenPose(void){
    doPose(true, false, 1, false);
}

void ExitListenPose(void){
    doPose(false, false, 1, true);
}

void IdlePose(void){
    doPose(true, true, 0, false);
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