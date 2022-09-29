#include "raylib.h"
#include "character.h"
#include <stdio.h>

void InitCharacter(void){
	character.model = LoadModel("resources/models/body_empty.iqm");    
    character.texture = LoadTexture("resources/models/Base_texture.png");
    character.talkTextures[0] = LoadTexture("resources/models/Base_texture-talk-0.png");
    character.talkTextures[1] = LoadTexture("resources/models/Base_texture-talk-1.png");
    character.position = (Vector3){ 0.0f, -1.0f, 0.0f };
    character.animsCount = 0;
    character.animFrameCounter = 0;
    character.anims = LoadModelAnimations("resources/models/body_empty.iqm", &character.animsCount);
    character.currentPose = idle;
    character.nextPose = idle;
    character.justEnter = true;
    character.isTalking = false;
    UpdateModelAnimation(character.model, character.anims[0], character.animFrameCounter);
    SetMaterialTexture(&character.model.materials[0], MATERIAL_MAP_DIFFUSE, character.texture);
}

void SetCharacterShader(Shader lightShader){
    character.model.materials[0].shader = lightShader;
}

void UnloadCharacter(void){
    UnloadTexture(character.texture);
    for (unsigned int tex = 0; tex < sizeof(character.talkTextures) / sizeof(character.talkTextures[0]); tex++) {
        UnloadTexture(character.talkTextures[tex]);
    }
    UnloadModel(character.model);

    // Unload model animations data
    for (unsigned int i = 0; i < character.animsCount; i++) {
    	UnloadModelAnimation(character.anims[i]);
    }
    //RL_FREE(anims); //TODO - debug error
}

void Talk(void){
    int talkIndex = character.animFrameCounter%9/3;
    if (talkIndex == 2){
        SetMaterialTexture(&character.model.materials[0], MATERIAL_MAP_DIFFUSE, character.texture);
    } else {
        SetMaterialTexture(&character.model.materials[0], MATERIAL_MAP_DIFFUSE, character.talkTextures[talkIndex]);
    }
}

void SetTalk(bool isTalking){
    //close the character's mouth if she's stopped talking
    if (isTalking == false){
        SetMaterialTexture(&character.model.materials[0], MATERIAL_MAP_DIFFUSE, character.texture);
    }
    character.isTalking = isTalking;
}

void DrawCharacter(void){
	DrawModelEx(character.model, character.position, (Vector3){ 1.0f, 0.0f, 0.0f }, 0.0f, (Vector3){ 1.75f, 1.75f, 1.75f }, WHITE);
}

void GoToNext(void){
    if (character.currentPose != character.nextPose){
        character.currentPose = character.nextPose;
    }
}

void DoPose(bool forward, bool loop, int poseIndex, bool backToIdle){
    if (character.justEnter == true && character.currentPose == character.nextPose){
        if (forward){
            character.animFrameCounter = 0;
        } else {
            character.animFrameCounter = character.anims[poseIndex].frameCount - 1;
        }
        character.justEnter = false;
    }

    //the idle loop is long which creates the impression of a long time between
    //hearing wakeword and responding. so, jump forward a bit where it makes
    //sense in the animation
    if (character.currentPose == idle && character.nextPose != character.currentPose){
        if (character.animFrameCounter < 5 || character.animFrameCounter > 45 || (character.animFrameCounter > 21 && character.animFrameCounter < 29)){
            GoToNext();
        }
    }


    if (forward){
        if (loop){
            character.animFrameCounter++;
            if (character.animFrameCounter >= character.anims[poseIndex].frameCount){
                character.animFrameCounter = 0;
                GoToNext();
            }
        } else {
            if (character.animFrameCounter < character.anims[poseIndex].frameCount - 1){
                character.animFrameCounter++;
                if (backToIdle && character.animFrameCounter == character.anims[poseIndex].frameCount - 1){
                    character.nextPose = idle;
                }
            } else {
                GoToNext();
            }
        }
    } else {
        if (loop){
            character.animFrameCounter--;
            if (character.animFrameCounter <= 0){
                character.animFrameCounter = character.anims[poseIndex].frameCount - 1;
                GoToNext();
            }
        } else {
            if (character.animFrameCounter > 0){
                character.animFrameCounter--;
                if (backToIdle && character.animFrameCounter == 0){
                    character.nextPose = idle;
                }
            } else {
                GoToNext();
            }  
        }

    }
        
    UpdateModelAnimation(character.model, character.anims[poseIndex], character.animFrameCounter);
}

void EnterListenPose(void){
    DoPose(true, false, 1, false);
}

void ExitListenPose(void){
    DoPose(false, false, 1, true);
}

void IdlePose(void){
    DoPose(true, true, 0, false);
}

void DabPose(void){
    DoPose(true, false, 2, true);
}

void WavePose(void){
    DoPose(true, false, 3, true);
}

void SetPose(int pose){
    character.nextPose = pose;
    character.justEnter = true;
}

void UpdateCharacter(void){
    if (character.isTalking){
        Talk();
    }

    if (character.currentPose == enterListen){
        EnterListenPose();
    } else if (character.currentPose == exitListen){
        ExitListenPose();
    } else if (character.currentPose == dab){
        DabPose();
    } else if (character.currentPose == wave){
        WavePose();
    } else {
        IdlePose();
    }
}