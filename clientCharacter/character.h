#ifndef CHARACTER_H
#define CHARACTER_H

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum
{
    idle = 0,
    enterListen,
    exitListen,
    dab
} pose;

typedef struct Character {
    Texture2D texture;
    Texture2D talkTextures[2];
    Model model;
    Vector3 position;
    unsigned int animsCount;
    int animFrameCounter;
    pose currentPose;
    pose nextPose;
    bool justEnter;
    bool isTalking;
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
void SetCharacterShader(Shader lightShader);
void UnloadCharacter(void);
void DrawCharacter(void);
void UpdateCharacter(void);
void Talk(void);
void SetTalk(bool isTalking);
void SetPose(int);
void IdlePose(void);
void ExitListenPose(void);
void EnterListenPose(void);
void DabPose(void);
void DoPose(bool forward, bool loop, int poseIndex, bool backToIdle);
void GoToNext(void);


#endif 