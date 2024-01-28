#include "raylib.h"
#include <time.h>
#include <string.h>
#include "Tetris.h"
#include "CheckCollision.c"
#include "TetrisPiece.c"

/*
mettere lo score a schermo
mettere la musuca con la libreria di raylib
aumento velocità con lo score
*/

const Color colorTypes[8] =
{
    {255,255,85,255},
    {85,43,158,255},
    {56,255,85,255},
    {255,255,63,255},
    {255,42,85,255},
    {255,100,85,255},
    {97,5,85,255},
    {85,45,63,255},
};

float currScore = 0;
float scoreMultipier = 1.f;
const int rowValue = 1;

void drawTetromino(const Color currentColor, const int startOffsetX, const int startOffsetY, const int tetrominoStartX, const int tetrominoStartY, const int *tetromino);

void ResetLines(int startLineY);

void DeleteLines();

int RotateTetromino(int currentRotation, int currentTetrominoX, int currentTetrominoY, int currentTetrominoType);

int MoveTetromino(int currentTetrominoX, int currentTetrominoY, int currentTetrominoType, int currentRotation);

void MovetetrominoDown();

void DrawStage(int startOffsetX, int startOffsetY);

void UpdateScore();

void CloseWindow();

int main(int argc, char** argv, char** environ)
{
    const int windowWidth = 600; 
    const int windowHeight = 700; 

    const int startOffsetX = (windowWidth / 2) - ((STAGE_WIDTH * TILE_SIZE) / 2);
    const int startOffsetY = (windowHeight / 2) - ((STAGE_HEIGHT * TILE_SIZE) / 2);

    const int tetrominoStartX = STAGE_WIDTH / 2;
    const int tetrominoStartY = 0;

    int currentTetrominoX = tetrominoStartX;
    int currentTetrominoY = tetrominoStartY;

    InitAudioDevice();
    Music bgMusic = LoadMusicStream("TetrisBGMusic.mp3");
    //PlaySound(bgMusic);
    PlayMusicStream(bgMusic);


    time_t unixTime;

    time(&unixTime);

    SetRandomSeed(unixTime);

    int currentTetrominoType = GetRandomValue(0, 6);
    int currentRotation = 0;

    const float moveTetrominoDownTimer = 1.f;
    float timeToMoveTetrominoDown = moveTetrominoDownTimer;
    int currentColor = GetRandomValue(0, 7);

    TraceLog(LOG_INFO, "Number of arguments : %d", argc);

    for(int i = 0; i < argc; i++)
    {
        TraceLog(LOG_INFO, "Argument : %d = %s", i, argv[i]);
    }

    while(*environ != 0)
    {
        TraceLog(LOG_INFO, "Argument : %s", *environ);
        environ++;
    }

    InitWindow(windowWidth, windowHeight, "Title");

    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        timeToMoveTetrominoDown -= GetFrameTime() * (scoreMultipier);
        
        currentRotation = RotateTetromino(currentRotation, currentTetrominoX, currentTetrominoY, currentTetrominoType);
        
        currentTetrominoX = MoveTetromino(currentTetrominoX, currentTetrominoY, currentTetrominoType, currentRotation);
        UpdateMusicStream(bgMusic);

        if(timeToMoveTetrominoDown <= 0 || IsKeyPressed(KEY_DOWN))
        {            
            if(!CheckCollision(currentTetrominoX,currentTetrominoY+1,tetrominoTypes[currentTetrominoType][currentRotation]))
            {
                currentTetrominoY++;
                timeToMoveTetrominoDown = moveTetrominoDownTimer;
            }

            else
            {
                // const int currentTetrominoOffset = currentTetrominoY * STAGE_WIDTH + currentTetrominoX;
                
                // stage[currentTetrominoOffset] = 1;

                for(int y = 0; y < TETROMINO_SIZE; y++)
                {
                    for(int x = 0; x < TETROMINO_SIZE; x++)
                    {
                        const int offset = y * TETROMINO_SIZE + x;

                        const int *tetromino = tetrominoTypes[currentTetrominoType][currentRotation];

                        if(tetromino[offset] == 1)
                        {
                            const int offset_stage = (y + currentTetrominoY) * STAGE_WIDTH + (x + currentTetrominoX);

                            stage[offset_stage] = currentColor+1;
                        }
                    }
                }

                DeleteLines();

                currentTetrominoX = tetrominoStartX;
                currentTetrominoY = tetrominoStartY;

                currentTetrominoType = GetRandomValue(0, 6);
                currentRotation = 0;
                currentColor = GetRandomValue(0, 7);
            }
        }

        BeginDrawing();
        ClearBackground(RED);

        DrawStage(startOffsetX, startOffsetY);

        DrawText(TextFormat("Score: %08i", (int)currScore), 50, 20, 20, BLACK);
        
        drawTetromino(colorTypes[currentColor],startOffsetX, startOffsetY, currentTetrominoX, currentTetrominoY, tetrominoTypes[currentTetrominoType][currentRotation]);


        EndDrawing();
    }

    return 0;
}

void DrawStage(int startOffsetX, int startOffsetY)
{
    for(int y = 0; y < STAGE_HEIGHT; y++)
        {
            for(int x = 0; x < STAGE_WIDTH; x++)
            {
                const int offset = y * STAGE_WIDTH + x;
                const int color = stage[offset];

                if(stage[offset] != 0)
                {
                    DrawRectangle(x * TILE_SIZE + startOffsetX, y * TILE_SIZE + startOffsetY, TILE_SIZE, TILE_SIZE, colorTypes[color-1]);
                }

                DrawRectangleLines(x * TILE_SIZE + startOffsetX, y * TILE_SIZE + startOffsetY, TILE_SIZE, TILE_SIZE, BLACK);
            }
        }
}

void drawTetromino(const Color currentColor, const int startOffsetX, const int startOffsetY, const int tetrominoStartX, const int tetrominoStartY, const int *tetromino)
{
    for(int y = 0; y < TETROMINO_SIZE; y++)
    {
        for(int x = 0; x < TETROMINO_SIZE; x++)
        {
            const int offset = y * TETROMINO_SIZE + x;

            if(tetromino[offset] == 1)
            {
                DrawRectangle((x + tetrominoStartX) * TILE_SIZE + startOffsetX, (y + tetrominoStartY) * TILE_SIZE + startOffsetY, TILE_SIZE, TILE_SIZE, currentColor);
                if(CheckCollision(tetrominoStartX, tetrominoStartY, tetromino ))
                {
                    CloseWindow();
                }
            }
        }
    }
}

int RotateTetromino(int currentRotation, int currentTetrominoX, int currentTetrominoY, int currentTetrominoType)
{
    if (IsKeyPressed(KEY_SPACE))
    {
        const int lastRotation = currentRotation;

        currentRotation++;

        if (currentRotation > 3)
        {
            currentRotation = 0;
        }

        if (CheckCollision(currentTetrominoX,currentTetrominoY,tetrominoTypes[currentTetrominoType][currentRotation]))
        {
            currentRotation = lastRotation;
        }
    }
    return currentRotation;
}

int MoveTetromino(int currentTetrominoX, int currentTetrominoY, int currentTetrominoType, int currentRotation)
{
    if (IsKeyPressed(KEY_RIGHT))
    {
        // No need to check overflow, wall is your protector
        if (!CheckCollision(currentTetrominoX+1,currentTetrominoY,tetrominoTypes[currentTetrominoType][currentRotation]))
        {
            currentTetrominoX++;
        }
    }

    if (IsKeyPressed(KEY_LEFT))
    {
        // No need to check overflow, wall is your protector
        if (!CheckCollision(currentTetrominoX-1,currentTetrominoY,tetrominoTypes[currentTetrominoType][currentRotation]))
        {
            currentTetrominoX--;
        }
    }
    return currentTetrominoX;
}

//void MovetetrominoDown(int timeToMoveTetrominoDown, int currentTetrominoX, int currentTetrominoY, int currentTetrominoType, int currentRotation)
// {
//     if(timeToMoveTetrominoDown <= 0 || IsKeyPressed(KEY_DOWN))
//         {            
//             if(!CheckCollision(currentTetrominoX,currentTetrominoY+1,tetrominoTypes[currentTetrominoType][currentRotation]))
//             {
//                 currentTetrominoY++;
//                 timeToMoveTetrominoDown = moveTetrominoDownTimer;
//             }

//             else
//             {
//                 // const int currentTetrominoOffset = currentTetrominoY * STAGE_WIDTH + currentTetrominoX;
                
//                 // stage[currentTetrominoOffset] = 1;

//                 for(int y = 0; y < TETROMINO_SIZE; y++)
//                 {
//                     for(int x = 0; x < TETROMINO_SIZE; x++)
//                     {
//                         const int offset = y * TETROMINO_SIZE + x;

//                         const int *tetromino = tetrominoTypes[currentTetrominoType][currentRotation];

//                         if(tetromino[offset] == 1)
//                         {
//                             const int offset_stage = (y + currentTetrominoY) * STAGE_WIDTH + (x + currentTetrominoX);

//                             stage[offset_stage] = currentColor+1;
//                         }
//                     }
//                 }

//                 DeleteLines();

//                 currentTetrominoX = tetrominoStartX;
//                 currentTetrominoY = tetrominoStartY;

//                 currentTetrominoType = GetRandomValue(0, 6);
//                 currentRotation = 0;
//                 currentColor = GetRandomValue(0, 7);
//             }
//         }
// }

void ResetLines(int startLineY)
{
    for (int y = startLineY; y >= 0; y--)
    {
        for (int x = 1; x < STAGE_WIDTH - 1; x++)
        {
            const int offset = y * STAGE_WIDTH + x;
            const int offset_below = (y+1) * STAGE_WIDTH + x;

            if (stage[offset_below] == 0 && stage[offset] > 0)
            {
                stage[offset_below] = stage[offset];
                stage[offset] = 0;
            }
        }
    }   
}

void DeleteLines()
{
    for (int y = 0; y < STAGE_HEIGHT - 1; y++)
    {
        int checkLine = 1;

        for (int x = 1; x < STAGE_WIDTH - 1; x++)
        {
            const int offset = y * STAGE_WIDTH + x;

            if (stage[offset] == 0)
            {
                checkLine = 0;
                break;
            }
        }

        if(checkLine)
        {
            const int offset = y * STAGE_WIDTH + 1;
            memset(stage+offset,0,(STAGE_WIDTH-2)* sizeof(int));

            ResetLines(y);
            UpdateScore();
        }
    }   
}

void UpdateScore()
{
    scoreMultipier += (currScore) * .01f;
    currScore = currScore + scoreMultipier * rowValue;
    TraceLog(LOG_INFO, "score %d %f", (int)currScore, scoreMultipier);
}

void CloseWindow()
{
    CloseWindow();
}