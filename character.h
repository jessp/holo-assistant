#ifndef CHARACTER_H
#define CHARACTER_H

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum
{
    idle = 0,
    enterListen,
    exitListen
} pose;

typedef struct Character {
    Texture2D texture;
    Model model;
    Vector3 position;
    unsigned int animsCount;
    int animFrameCounter;
    pose currentPose;
    pose nextPose;
    bool justEnter;
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
void SetPose(int);
void IdlePose(void);
void ExitListenPose(void);
void EnterListenPose(void);
void doPose(bool forward, bool loop, int poseIndex, bool backToIdle);


#endif 