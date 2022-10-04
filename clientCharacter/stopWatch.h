#ifndef STOPWATCH_H
#define STOPWATCH_H

typedef struct StopWatch {
    Model model;
    Vector3 position;
    int animFrameCounter;
    unsigned int animsCount;
    Texture2D texture;
    // Load animation data
    ModelAnimation *anims;
    int firstHalf;
} StopWatch;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
extern StopWatch stopWatch;

//----------------------------------------------------------------------------------
// Functions Declaration
//----------------------------------------------------------------------------------
void InitStopWatch(void);
void DrawStopWatch(void);
void UpdateStopWatch(void);
void UnloadStopWatch(void);

#endif 