#ifndef CHARACTER_H
#define CHARACTER_H

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Character {
    Texture2D texture;
    Model model;
    Vector3 position;
    unsigned int animsCount;
    int animFrameCounter;
    // Load animation data
    ModelAnimation *anims;
} Character;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
extern Character character;


//----------------------------------------------------------------------------------
// Functions Declaration
//----------------------------------------------------------------------------------
void InitCharacter(void);
void UnloadCharacter(void);
void DrawCharacter(void);
void UpdateCharacter(void);


#endif 