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

void UpdateCharacter(void){
	character.animFrameCounter++;
	if (character.animFrameCounter >= character.anims[0].frameCount){
		character.animFrameCounter = 0;
	}
	UpdateModelAnimation(character.model, character.anims[0], character.animFrameCounter);
	printf("%d\n", character.animFrameCounter);
}