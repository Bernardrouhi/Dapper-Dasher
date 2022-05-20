#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int totalFrames, int maxFrame)
{
    // update running time
    data.runningTime += deltaTime;
    
    // update animation frame
    if(data.runningTime >= data.updateTime)
    {
        data.rec.x = ((data.frame % totalFrames)%maxFrame) * data.rec.width;
        data.rec.y = ((data.frame % totalFrames)/maxFrame) * data.rec.height;
        data.runningTime = 0.0;
        data.frame++;
        data.frame = data.frame % totalFrames;
    }

    return data;
}

int main()
{
    // window width and height
    int windowDimension[2];
    windowDimension[0] = 512;
    windowDimension[1] = 380;

    // initiate window
    InitWindow(windowDimension[0],windowDimension[1],"Platform Movement");

    // World Property
    const int gravity{1500};

    // nebula 
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    const int sizeOfNebulae{10};
    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0 ; i < sizeOfNebulae ; i++)
    {
        nebulae[i].rec.x = 0.0f;
        nebulae[i].rec.y = 0.0f;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.x = windowDimension[0] + (i*300);
        nebulae[i].pos.y = windowDimension[1] - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].updateTime = 0.05;
        nebulae[i].runningTime = 0.0;
    }

    float finishLine{nebulae[sizeOfNebulae-1].pos.x};

    int nebulaVelocity{-200};

    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX{};
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};

    // character
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData{
        {0,0,scarfy.width/6,scarfy.height},
        { windowDimension[0]/2 - scarfyData.rec.width/2, windowDimension[1] -  scarfyData.rec.height},
        0,
        (1.0/12.0),
        0
    };

    int Char_Velocity{0};
    const int Char_JumpVelocity{-800};
    const int Char_Speed{10};

    bool isInAir{false};

    bool collision{};

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        // delta time (time since last frame)
        const float dt{GetFrameTime()};
        
        BeginDrawing();
        ClearBackground(WHITE);

        // scroll background
        bgX -= 20 * dt;
        if (bgX <= -background.width*2)
        {
            bgX = 0;
        }
        // scroll midground
        mgX -= 40 * dt;
        if (mgX <= -midground.width*2)
        {
            mgX = 0;
        }
        // scroll foreground
        fgX -= 80 * dt;
        if (fgX <= -foreground.width*2)
        {
            fgX = 0;
        }

        // draw the background
        Vector2 bg1Pos{bgX,0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        Vector2 bg2Pos{(bgX + background.width * 2), 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);
        // draw the midground
        Vector2 mg1Pos{mgX,0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos{(mgX + midground.width * 2), 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);
        // draw the foreground
        Vector2 fg1Pos{fgX,0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos{(fgX + foreground.width * 2), 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

        // update Velocity
        if (isOnGround(scarfyData, windowDimension[1]))
        {
            Char_Velocity = 0;
            isInAir = false;
        }
        else
        {
            // Apply gravity
            Char_Velocity += gravity * dt;
            isInAir = true;
        }

        // Jump
        if(IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            // add Velocity
            Char_Velocity += Char_JumpVelocity;
        }

        // Movement
        if(IsKeyDown(KEY_A))
        {
            if(scarfyData.pos.x > 0)
            {
                scarfyData.pos.x -= Char_Speed;
            }
            else
            {
                scarfyData.pos.x = 0;
            }
        }
        if(IsKeyDown(KEY_D))
        {
            if (scarfyData.pos.x < windowDimension[0]-scarfyData.rec.width)
            {
                scarfyData.pos.x += Char_Speed;
            }
            else
            {
                scarfyData.pos.x = windowDimension[0]-scarfyData.rec.width;
            }
        }

        for (int i = 0 ; i < sizeOfNebulae ; i++)
        {
            // update each nebula position
            nebulae[i].pos.x += nebulaVelocity * dt;
        }

        // Update finish line
        finishLine += nebulaVelocity * dt; 

        // update character position
        scarfyData.pos.y += Char_Velocity * dt;

        if (!isInAir)
        {
            scarfyData = updateAnimData(scarfyData, dt, 6, 6);
        }

        // Update nebula position
        for (int i = 0 ; i < sizeOfNebulae ; i++)
        {
            nebulae[i] = updateAnimData(nebulae[i], dt, 60, 8);
        }

        for (AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2*pad,
                nebula.rec.height - 2*pad
            };
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height,
            };
            if (CheckCollisionRecs(nebRec,scarfyRec))
            {
                collision = true;
            }
        }

        if (collision)
        {
            // lose the game
            DrawText("Game Over!", windowDimension[0]/4, windowDimension[1]/2, 40, RED);
        }
        else if (finishLine <= 0)
        {
            // win the game
            DrawText("You Win!", windowDimension[0]/4, windowDimension[1]/2, 40, GREEN);
        }
        else
        {
            for (int i = 0 ; i< sizeOfNebulae ; i++)
            {
                // Draw Nebula
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }
            // Draw Character
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }
        EndDrawing();
    }

    // clean up
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    UnloadTexture(nebula);
    UnloadTexture(scarfy);
    CloseWindow();
}