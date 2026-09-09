#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 800
#define PLAYER_HEIGHT 25
#define PLAYER_WIDTH 50
#define PLAYER_MAX_SPEED 300
#define ENEMY_ROWS 4
#define ENEMY_COLS 10
#define ENEMY_WIDTH 40
#define ENEMY_HEIGHT 25
#define MAX_BULLETS 50
#define MAX_BULLET_SPEED 500
#define BULLET_HEIGHT 10
#define BULLET_WIDTH 5
#define ENEMY_SPEED 100
#define ENEMY_MOVINGDOWN 15
#define MAX_ENEMY_BULLETS 20
#define ENEMY_BULLET_SPEED 300

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders");

    SetTargetFPS(60);

    float ground = 95 * SCREEN_HEIGHT / 100;
    Vector2 player_position = {SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2, ground};
    Rectangle groundRect = {0, ground, SCREEN_WIDTH, SCREEN_HEIGHT - ground};

    Vector2 speed = Vector2Zero();

    Rectangle enemies[ENEMY_ROWS][ENEMY_COLS];
    Rectangle bullets[MAX_BULLETS];
    bool bullet_active[MAX_BULLETS] = {false};
    bool enemy_active[ENEMY_ROWS][ENEMY_COLS] = {true};
    Rectangle enemyBullets[MAX_ENEMY_BULLETS];
    bool enemyBulletActive[MAX_ENEMY_BULLETS] = {false};
    int playerLives = 3;
    int score = 0;

    bool gameOver = false;
    bool playerWon = false;
    bool paused = false;

    int enemy_direction = 1;
    int enemyspacing_x = 20;
    int enemyspacing_y = 20;
    int enemyspacing_alignment_x = 240;
    int enemyspacing_alignment_y = 50;

    for (int r = 0; r < ENEMY_ROWS; r++)
    {
        for (int c = 0; c < ENEMY_COLS; c++)
        {
            enemies[r][c] = (Rectangle){
                .x = enemyspacing_alignment_x + c * (ENEMY_WIDTH + enemyspacing_x),
                .y = enemyspacing_alignment_y + r * (ENEMY_HEIGHT + enemyspacing_y),
                .width = ENEMY_WIDTH,
                .height = ENEMY_HEIGHT};

            enemy_active[r][c] = true;
        }
    }

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if (!gameOver && IsKeyPressed(KEY_P))
        {
            paused = true;
        }
        if (!gameOver && !paused)
        {
            if (IsKeyDown(KEY_RIGHT))
            {
                speed.x = PLAYER_MAX_SPEED;
            }
            else if (IsKeyDown(KEY_LEFT))
            {
                speed.x = -PLAYER_MAX_SPEED;
            }
            else
            {
                speed.x = 0;
            }

            player_position = Vector2Add(player_position, Vector2Scale(speed, dt));

            Rectangle player = {
                player_position.x,
                player_position.y - PLAYER_HEIGHT,
                PLAYER_WIDTH,
                PLAYER_HEIGHT};

            if (player_position.x < 0)
            {
                player_position.x = 0;
            }
            else if (player_position.x > SCREEN_WIDTH - PLAYER_WIDTH)
            {
                player_position.x = SCREEN_WIDTH - PLAYER_WIDTH;
            }

            if (IsKeyPressed(KEY_SPACE))
            {
                for (int i = 0; i < MAX_BULLETS; i++)
                {
                    if (!bullet_active[i])
                    {
                        bullets[i] = (Rectangle){
                            player_position.x + (PLAYER_WIDTH / 2) - (BULLET_WIDTH / 2),
                            player_position.y - PLAYER_HEIGHT - BULLET_HEIGHT,
                            BULLET_WIDTH,
                            BULLET_HEIGHT};

                        bullet_active[i] = true;
                        break;
                    }
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (bullet_active[i])
                {
                    bullets[i].y -= MAX_BULLET_SPEED * dt;

                    if (bullets[i].y + BULLET_HEIGHT < 0)
                    {
                        bullet_active[i] = false;
                    }

                    for (int r = 0; r < ENEMY_ROWS; r++)
                    {
                        for (int c = 0; c < ENEMY_COLS; c++)
                        {
                            if (bullet_active[i] &&
                                enemy_active[r][c] &&
                                CheckCollisionRecs(bullets[i], enemies[r][c]))
                            {
                                enemy_active[r][c] = false;
                                bullet_active[i] = false;
                                score += 10;
                            }
                        }

                        if (!bullet_active[i])
                            break;
                    }
                }
            }

            bool allEnemiesDead = true;

            for (int r = 0; r < ENEMY_ROWS; r++)
            {
                for (int c = 0; c < ENEMY_COLS; c++)
                {
                    if (enemy_active[r][c])
                    {
                        allEnemiesDead = false;
                        break;
                    }
                }

                if (!allEnemiesDead)
                    break;
            }

            if (allEnemiesDead)
            {
                gameOver = true;
                playerWon = true;
            }

            int shooters[ENEMY_COLS];
            int shooterCount = 0;

            for (int c = 0; c < ENEMY_COLS; c++)
            {
                for (int r = ENEMY_ROWS - 1; r >= 0; r--)
                {
                    if (enemy_active[r][c])
                    {
                        shooters[shooterCount] = r * ENEMY_COLS + c;
                        shooterCount++;
                        break;
                    }
                }
            }

            if (shooterCount > 0 && GetRandomValue(1, 100) == 1)
            {
                int randomShooter = GetRandomValue(0, shooterCount - 1);
                int index = shooters[randomShooter];

                int r = index / ENEMY_COLS;
                int c = index % ENEMY_COLS;

                for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
                {
                    if (!enemyBulletActive[i])
                    {
                        enemyBullets[i] = (Rectangle){
                            enemies[r][c].x + ENEMY_WIDTH / 2 - BULLET_WIDTH / 2,
                            enemies[r][c].y + ENEMY_HEIGHT,
                            BULLET_WIDTH,
                            BULLET_HEIGHT};

                        enemyBulletActive[i] = true;
                        break;
                    }
                }
            }

            for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
            {
                if (enemyBulletActive[i])
                {
                    enemyBullets[i].y += ENEMY_BULLET_SPEED * dt;

                    if (enemyBullets[i].y > SCREEN_HEIGHT)
                    {
                        enemyBulletActive[i] = false;
                    }

                    if (CheckCollisionRecs(enemyBullets[i], player))
                    {
                        enemyBulletActive[i] = false;
                        playerLives--;

                        if (playerLives <= 0)
                        {
                            playerLives = 0;
                            gameOver = true;
                            playerWon = false;
                        }
                    }
                }
            }

            bool change_direction = false;

            for (int r = 0; r < ENEMY_ROWS; r++)
            {
                for (int c = 0; c < ENEMY_COLS; c++)
                {
                    if (enemy_active[r][c])
                    {
                        float enemyposition_x =
                            enemies[r][c].x +
                            ENEMY_SPEED * enemy_direction * dt;

                        if (enemyposition_x + ENEMY_WIDTH >= SCREEN_WIDTH ||
                            enemyposition_x <= 0)
                        {
                            change_direction = true;
                        }
                    }
                }
            }

            if (change_direction)
            {
                enemy_direction *= -1;

                for (int r = 0; r < ENEMY_ROWS; r++)
                {
                    for (int c = 0; c < ENEMY_COLS; c++)
                    {
                        enemies[r][c].y += ENEMY_MOVINGDOWN;
                    }
                }
            }
            else
            {
                for (int r = 0; r < ENEMY_ROWS; r++)
                {
                    for (int c = 0; c < ENEMY_COLS; c++)
                    {
                        if (enemy_active[r][c])
                        {
                            enemies[r][c].x +=
                                ENEMY_SPEED * enemy_direction * dt;
                        }
                    }
                }
            }
        }

        BeginDrawing();

        ClearBackground((Color){0, 0, 0, 255});

        DrawRectangleRec(groundRect, (Color){0, 0, 255, 255});

        Rectangle player = {
            player_position.x,
            player_position.y - PLAYER_HEIGHT,
            PLAYER_WIDTH,
            PLAYER_HEIGHT};

        DrawRectangleRec(player, (Color){0, 255, 0, 255});

        for (int r = 0; r < ENEMY_ROWS; r++)
        {
            for (int c = 0; c < ENEMY_COLS; c++)
            {
                if (enemy_active[r][c])
                {
                    DrawRectangleRec(
                        enemies[r][c],
                        (Color){255, 0, 0, 255});
                }
            }
        }

        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullet_active[i])
            {
                DrawRectangleRec(
                    bullets[i],
                    (Color){255, 255, 255, 255});
            }
        }

        for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
        {
            if (enemyBulletActive[i])
            {
                DrawRectangleRec(
                    enemyBullets[i],
                    (Color){255, 255, 0, 255});
            }
        }

        DrawText(
            TextFormat("Lives: %d", playerLives),
            20,
            20,
            25,
            WHITE);

        DrawText(
            TextFormat("Score: %d", score),
            20,
            50,
            25,
            WHITE);
        if (paused)
        {
            DrawText(
                "PAUSED!",
                400,
                300,
                60,
                WHITE);
            DrawText(
                "Press P to continue",
                400,
                380,
                25,
                WHITE);
        }
        else if (gameOver)
        {
            if (playerWon)
            {
                DrawText(
                    "YOU WON!",
                    350,
                    300,
                    60,
                    WHITE);
            }
            else
            {
                DrawText(
                    "GAME OVER",
                    330,
                    300,
                    60,
                    WHITE);
            }

            DrawText(
                TextFormat("Your Score: %d", score),
                400,
                380,
                30,
                WHITE);
        }

        EndDrawing();
    }

    CloseWindow();

    ;
}
