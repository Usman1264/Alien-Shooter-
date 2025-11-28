#include "raylib.h"
#include <iostream>
#include <ctime>
#include <fstream>
#include"our header.h"



Color LerpColor(Color a, Color b, float t) // blends colors 
{

    if (t < 0.0f) t = 0.0f; // if t = 0 u get a , if t = 1 u get b , if 0.5 u get middle. 

    if (t > 1.0f) t = 1.0f;

    Color result;

    result.r = (unsigned char)(a.r + (b.r - a.r) * t); // ( r = 255 + (0 - 255) * 0.5 = 127 ) 

    result.g = (unsigned char)(a.g + (b.g - a.g) * t);

    result.b = (unsigned char)(a.b + (b.b - a.b) * t);

    result.a = (unsigned char)(a.a + (b.a - a.a) * t);

    return result;

}



// power up section 
void ApplyOrbPower(char orbType, Ship& player, char& currentPower, float& powerTimer)
{

    if (orbType == 'L') { // extra life orb , timer 0 

        player.lives += 1;

        currentPower = 'N';

        powerTimer = 0.0f;

        return;

    }

    currentPower = orbType;

    powerTimer = POWER_DURATION;

    if (orbType == 'H') { // orb that gives shield around the ship 

        player.immune = true;

        player.immuneTimer = POWER_DURATION; // timer starts for certain duration

    }

}

// alien bullet amount controller 
int CountActiveAlienBullets(const Bullet(&bullets)[MAX_BULLETS])
{

    int count = 0;

    for (int i = 0; i < MAX_BULLETS; i++) { // bullets fired cannot be more than the max bullets passed to it 

        if (bullets[i].active && !bullets[i].fromPlayer) // means that the bullet is fired from the alien
        {

            count++; // counting how many bullets are being fired by aliens 

        }

    }

    return count;

}

// power removing which will make the ship normal again 
void ResetShip(Ship& ship) {

    ship.body = { (float)SCREEN_WIDTH / 2.0f - 25.0f, (float)SCREEN_HEIGHT - 70.0f, 50.0f, 32.0f };

    ship.baseSpeed = 240.0f; // our normal speed 

    ship.currentSpeed = ship.baseSpeed; // if the speed had inc due to orb we lowered if down to normal now

    ship.lives = 3;  // sets the lives back to 3 

    ship.immune = false; // removing immunity 

    ship.immuneTimer = 0.0f; // timer of immunity is off

}


// generating the random value between a min and max 
float RandomFloat(float minValue, float maxValue) {

    int minInt = (int)(minValue * 1000.0f); // we first convert to whole numbers so that we can get decimals 

    int maxInt = (int)(maxValue * 1000.0f);

    if (maxInt <= minInt) { // if the min and max r same or less we then inc the max by min + 1 otherwise the func fails 

        maxInt = minInt + 1;

    }

    int value = minInt + rand() % (maxInt - minInt + 1); // generating random number and taking mod to set a range 

    return (float)value / 1000.0f;

}



// spawning the aliens 
void SpawnAliens(Alien(&aliens)[MAX_ALIENS], int& alienCount, int levelIndex, LevelConfig configs[MAX_LEVELS]) {

    for (int i = 0; i < MAX_ALIENS; i++) {

        aliens[i].active = false; // clearing the aliens that were in the previous level 

    }

    alienCount = 0;

    LevelConfig config = configs[levelIndex]; // checking which level we are on 

    int total = config.rows * config.cols; // bec every level has different set of rows and cols for aliens so we get what is it . 

    if (total > MAX_ALIENS) {

        total = MAX_ALIENS;

    }

    float startX = 80.0f; // starting position 

    float startY = 80.0f;

    float spacingX = 70.0f;

    float spacingY = 60.0f;

    //spawning the boss
    if (levelIndex == MAX_LEVELS - 1) {

        aliens[0].body = { (float)SCREEN_WIDTH / 2.0f - 40.0f, 80.0f, 80.0f, 48.0f };

        aliens[0].active = true;

        aliens[0].health = 20;

        aliens[0].fireTimer = RandomFloat(config.fireMin, config.fireMax);

        aliens[0].dropTimer = RandomFloat(config.dropMin, config.dropMax);

        aliens[0].fireIntervalMin = config.fireMin;

        aliens[0].fireIntervalMax = config.fireMax;

        aliens[0].dropIntervalMin = config.dropMin;

        aliens[0].dropIntervalMax = config.dropMax;

        alienCount = 1;

        return;

    }

    // drawing the grid of aliens 
    for (int row = 0; row < config.rows; row++) {

        for (int col = 0; col < config.cols; col++) {

            int idx = row * config.cols + col;

            if (idx >= total) {

                break;

            }

            aliens[idx].body = { startX + col * spacingX, startY + row * spacingY, 48.0f, 32.0f }; // spacing and drawing of the aliens 

            aliens[idx].active = true;

            aliens[idx].health = config.alienHealth;

            aliens[idx].fireTimer = RandomFloat(config.fireMin, config.fireMax);

            aliens[idx].dropTimer = RandomFloat(config.dropMin, config.dropMax);

            aliens[idx].fireIntervalMin = config.fireMin;

            aliens[idx].fireIntervalMax = config.fireMax;

            aliens[idx].dropIntervalMin = config.dropMin;

            aliens[idx].dropIntervalMax = config.dropMax;

            alienCount = alienCount + 1;

        }

    }

}

// resting the bullet array 
void ResetBullets(Bullet(&bullets)[MAX_BULLETS]) {

    for (int i = 0; i < MAX_BULLETS; i++) {

        bullets[i].active = false;

        bullets[i].speed = 0.0f;

        bullets[i].fromPlayer = false;

        bullets[i].body = { 0.0f, 0.0f, 6.0f, 16.0f };

    }

}

// shooting and drawing bullet 
void FireBullet(Bullet(&bullets)[MAX_BULLETS], bool fromPlayer, float startX, float startY, float speed) {

    int count = 0;

    for (int i = 0; i < MAX_BULLETS; i++) {

        if (bullets[i].active) {

            if (bullets[i].fromPlayer) {

                count++;

            }

        }

    }

    if (fromPlayer && count >= PLAYER_MAX_BULLETS) {

        return;

    }

    for (int i = 0; i < MAX_BULLETS; i++) {

        if (!bullets[i].active) {

            bullets[i].active = true;

            bullets[i].fromPlayer = fromPlayer;

            bullets[i].speed = speed;

            bullets[i].body.x = startX;

            bullets[i].body.y = startY;

            bullets[i].body.width = 6.0f;

            bullets[i].body.height = 16.0f;

            break;

        }

    }

}

// high score is taken from the file 
int LoadHighScore() {

    std::ifstream file(HIGH_SCORE_FILE);

    if (!file.is_open()) {

        return 0;

    }

    int score = 0;

    file >> score;

    return score;

}

// score put in file 
void SaveHighScore(int score) {

    std::ofstream file(HIGH_SCORE_FILE, std::ios::trunc);

    if (file.is_open()) {

        file << score;

    }

}


// log is put in the victory file 
void LogVictory(int score, int lives, int totalTime) {

    std::ofstream file(WIN_LOG_FILE, std::ios::app);

    if (file.is_open()) {

        file << "Score:" << score << " Lives:" << lives << " Time:" << totalTime << "s\n";

    }

}


//MAIN
int main() {

    // setting up the game 
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple Space Defender");

    SetTargetFPS(60);

    SetExitKey(0);

    SetRandomSeed((unsigned int)time(0));



    // background making 
    Color topColor = { 14, 17, 44, 255 };
    Color bottomColor = { 5, 5, 15, 255 };
    Image bgImage = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, topColor);
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        float t = (SCREEN_HEIGHT <= 1) ? 0.0f : (float)y / (float)(SCREEN_HEIGHT - 1);
        Color blend = LerpColor(topColor, bottomColor, t);
        ImageDrawLine(&bgImage, 0, y, SCREEN_WIDTH, y, blend);
    }
    Texture2D background = LoadTextureFromImage(bgImage);
    UnloadImage(bgImage);

    //ship image 
    Image shipImg = GenImageColor(52, 34, BLANK);
    Vector2 tip = { 26.0f, 2.0f };
    Vector2 rightWing = { 46.0f, 32.0f };
    Vector2 leftWing = { 6.0f, 32.0f };
    ImageDrawTriangle(&shipImg, tip, rightWing, leftWing, SKYBLUE);
    Texture2D shipTexture = LoadTextureFromImage(shipImg);
    UnloadImage(shipImg);

    //aliens images
    Image alienImg = GenImageColor(48, 32, DARKGREEN);
    ImageDrawRectangle(&alienImg, 10, 8, 28, 12, LIME);
    Texture2D alienTexture = LoadTextureFromImage(alienImg);
    UnloadImage(alienImg);
    Image bossImg = GenImageColor(80, 48, MAROON); // boss
    ImageDrawRectangle(&bossImg, 10, 10, 60, 28, RED);
    Texture2D bossTexture = LoadTextureFromImage(bossImg);
    UnloadImage(bossImg);

    // reseting the entities like ship , bullets , aliens 
    Ship player;
    ResetShip(player);
    Alien aliens[MAX_ALIENS];
    int aliveAliens = 0;
    Bullet bullets[MAX_BULLETS];
    ResetBullets(bullets);

    // handling the orbs structure 
    PowerOrb orb;
    orb.active = false;
    orb.typeChar = 'N';
    orb.body = { 0.0f, 0.0f, 30.0f, 30.0f };
    orb.lifeTimer = 0.0f;

    // info about levels 
    LevelConfig configs[MAX_LEVELS] = {

        {3, 6, 40.0f, 160.0f, 3.5f, 6.0f, 1.6f, 3.2f, 1},

        {4, 7, 60.0f, 210.0f, 2.5f, 4.5f, 1.1f, 2.5f, 2},

        {1, 1, 80.0f, 250.0f, 1.0f, 2.0f, 0.8f, 1.6f, 12}

    };

    // setting data 
    float alienMoveDir = 1.0f;

    int currentLevel = 0;

    int score = 0;

    int hiscore = LoadHighScore();

    float playerShotCooldown = 0.35f;

    float shotTimer = 0.0f;

    float activePlayerBulletSpeed = BASE_PLAYER_BULLET_SPEED;

    float levelTimer = 0.0f;

    int totalTime = 0;

    float orbSpawnTimer = 4.0f;

    char currentPower = 'N';

    float powerTimer = 0.0f;

    bool gameWon = false;

    bool gameOver = false;

    float restartDelay = 0.0f;

    // spawning the aleins 
    SpawnAliens(aliens, aliveAliens, currentLevel, configs);

    while (!WindowShouldClose()) {

        float delta = GetFrameTime();

        levelTimer += delta;

        if (levelTimer >= 1.0f) {

            totalTime += 1;

            levelTimer = 0.0f;

        }

        if (!gameOver && !gameWon) {

            if (IsKeyDown(KEY_LEFT)) {

                player.body.x -= player.currentSpeed * delta;  // moving left 

            }

            if (IsKeyDown(KEY_RIGHT)) {

                player.body.x += player.currentSpeed * delta;  // moving right 

            }

            if (player.body.x < 10.0f) {

                player.body.x = 10.0f;

            }

            if (player.body.x + player.body.width > SCREEN_WIDTH - 10.0f) {

                player.body.x = SCREEN_WIDTH - 10.0f - player.body.width;

            }

            shotTimer -= delta;

            if (shotTimer < 0.0f) {

                shotTimer = 0.0f;

            }

            if (IsKeyDown(KEY_SPACE) && shotTimer <= 0.0f) { // firing bullets 

                FireBullet(bullets, true, player.body.x + player.body.width / 2.0f - 3.0f, player.body.y, activePlayerBulletSpeed);

                shotTimer = playerShotCooldown;

            }

            float baseSpeed = player.baseSpeed;

            activePlayerBulletSpeed = BASE_PLAYER_BULLET_SPEED;

            if (powerTimer > 0.0f) {

                powerTimer -= delta;

                if (currentPower == 'S') {

                    player.currentSpeed = baseSpeed * 1.45f;

                }

                else if (currentPower == 'B') {

                    activePlayerBulletSpeed = BOOSTED_PLAYER_BULLET_SPEED;

                }

                else if (currentPower == 'H') {

                    player.immune = true;

                    player.immuneTimer = powerTimer;

                }

                if (powerTimer <= 0.0f) {

                    currentPower = 'N';

                    player.currentSpeed = baseSpeed;

                    activePlayerBulletSpeed = BASE_PLAYER_BULLET_SPEED;

                    if (!(player.immuneTimer > 0.0f)) {

                        player.immune = false;

                    }

                }

            }

            else {

                player.currentSpeed = baseSpeed;

                activePlayerBulletSpeed = BASE_PLAYER_BULLET_SPEED;

                if (player.immuneTimer <= 0.0f) {

                    player.immune = false;

                }

            }

            if (player.immuneTimer > 0.0f) {

                player.immuneTimer -= delta;

                if (player.immuneTimer <= 0.0f) {

                    player.immune = false;

                }

            }

            int activeAliensNow = 0;

            for (int i = 0; i < MAX_ALIENS; i++) {

                if (aliens[i].active) {

                    activeAliensNow++;

                }

            }

            if (activeAliensNow == 0) {

                currentLevel++;

                if (currentLevel >= MAX_LEVELS) {

                    gameWon = true;

                    if (score > hiscore) {

                        hiscore = score;

                        SaveHighScore(hiscore);

                    }

                    LogVictory(score, player.lives, totalTime);

                }

                else {

                    SpawnAliens(aliens, aliveAliens, currentLevel, configs);

                    ResetBullets(bullets);

                    orb.active = false;

                    currentPower = 'N';

                    powerTimer = 0.0f;

                    player.currentSpeed = player.baseSpeed;

                    alienMoveDir = 1.0f;

                }

            }

            else {

                for (int i = 0; i < MAX_ALIENS; i++) {

                    if (!aliens[i].active) {

                        continue;

                    }

                    aliens[i].body.x += configs[currentLevel].alienSpeed * delta * alienMoveDir;

                    if (aliens[i].body.x < 20.0f || aliens[i].body.x + aliens[i].body.width > SCREEN_WIDTH - 20.0f) {

                        alienMoveDir *= -1.0f;

                        for (int j = 0; j < MAX_ALIENS; j++) {

                            if (aliens[j].active) {

                                aliens[j].body.y += 20.0f;

                            }

                        }

                        break;

                    }

                    aliens[i].fireTimer -= delta;

                    if (aliens[i].fireTimer <= 0.0f) {

                        if (CountActiveAlienBullets(bullets) < 4) {

                            FireBullet(bullets, false, aliens[i].body.x + aliens[i].body.width / 2.0f - 3.0f, aliens[i].body.y + aliens[i].body.height, configs[currentLevel].shotSpeed);

                            aliens[i].fireTimer = RandomFloat(aliens[i].fireIntervalMin, aliens[i].fireIntervalMax);

                        }

                        else {

                            aliens[i].fireTimer = 0.2f;

                        }

                    }

                    aliens[i].dropTimer -= delta;

                    if (aliens[i].dropTimer <= 0.0f) {

                        aliens[i].body.y += 24.0f;

                        aliens[i].dropTimer = RandomFloat(aliens[i].dropIntervalMin, aliens[i].dropIntervalMax);

                    }

                    if (aliens[i].body.y + aliens[i].body.height >= player.body.y && !gameWon) {

                        gameOver = true;

                        restartDelay = 2.0f;

                    }

                }

            }

            for (int i = 0; i < MAX_BULLETS; i++) {

                if (!bullets[i].active) {

                    continue;

                }

                bullets[i].body.y += bullets[i].speed * delta;

                if (bullets[i].body.y < -20.0f || bullets[i].body.y > SCREEN_HEIGHT + 20.0f) {

                    bullets[i].active = false;

                    continue;

                }

                if (bullets[i].fromPlayer) {

                    bool hitTarget = false;

                    for (int a = 0; a < MAX_ALIENS; a++) {

                        if (!aliens[a].active) {

                            continue;

                        }

                        if (CheckCollisionRecs(bullets[i].body, aliens[a].body)) {

                            bullets[i].active = false;

                            hitTarget = true;

                            aliens[a].health -= 1;

                            if (aliens[a].health <= 0) {

                                aliens[a].active = false;

                                score += (currentLevel == MAX_LEVELS - 1) ? 200 : 25;

                            }

                            else {

                                score += 5;

                            }

                            break;

                        }

                    }

                    if (!hitTarget && orb.active && CheckCollisionRecs(bullets[i].body, orb.body)) {

                        bullets[i].active = false;

                        hitTarget = true;

                        ApplyOrbPower(orb.typeChar, player, currentPower, powerTimer);

                        orb.active = false;

                    }

                }

                else {

                    if (!player.immune && CheckCollisionRecs(bullets[i].body, player.body)) {

                        bullets[i].active = false;

                        player.lives -= 1;

                        player.immune = true;

                        player.immuneTimer = 1.5f;

                        if (player.lives <= 0) {

                            gameOver = true;

                            restartDelay = 2.0f;

                        }

                    }

                }

            }

            if (orb.active) {

                orb.lifeTimer -= delta;

                if (orb.lifeTimer <= 0.0f) {

                    orb.active = false;

                }

                if (CheckCollisionRecs(orb.body, player.body)) {

                    orb.active = false;

                    ApplyOrbPower(orb.typeChar, player, currentPower, powerTimer);

                }

            }

            else {

                orbSpawnTimer -= delta;

                if (orbSpawnTimer <= 0.0f) {

                    orbSpawnTimer = RandomFloat(6.0f, 10.0f);

                    orb.active = true;

                    orb.lifeTimer = 6.0f;

                    const char types[4] = { 'S', 'B', 'L', 'H' };

                    int pick = GetRandomValue(0, 3);

                    orb.typeChar = types[pick];

                    orb.body.x = (float)GetRandomValue(60, SCREEN_WIDTH - 60);

                    orb.body.y = (float)GetRandomValue(120, SCREEN_HEIGHT - 200);

                }

            }

        }

        else {

            restartDelay -= delta;

            if (restartDelay <= 0.0f && IsKeyPressed(KEY_ENTER)) {

                gameOver = false;

                gameWon = false;

                ResetShip(player);

                ResetBullets(bullets);

                currentLevel = 0;

                SpawnAliens(aliens, aliveAliens, currentLevel, configs);

                score = 0;

                totalTime = 0;

                orb.active = false;

                currentPower = 'N';

                powerTimer = 0.0f;

            }

            if (gameOver) {

                if (score > hiscore) {

                    hiscore = score;

                    SaveHighScore(hiscore);

                }

            }

        }
        

        BeginDrawing();

        ClearBackground(BLACK);

        DrawTexture(background, 0, 0, WHITE);

        DrawText(TextFormat("Score: %d", score), 10, 10, 20, RAYWHITE);

        DrawText(TextFormat("High Score: %d", hiscore), SCREEN_WIDTH / 2 - 100, 10, 22, ORANGE);

        DrawText(TextFormat("Level %d / %d", currentLevel + 1, MAX_LEVELS), SCREEN_WIDTH - 180, 10, 20, SKYBLUE);

        DrawText(TextFormat("Lives: %d", player.lives), 10, 40, 20, GREEN);

        if (!gameOver && !gameWon) {

            DrawTexture(shipTexture, (int)player.body.x, (int)player.body.y, WHITE);

            if (player.immune) {

                DrawRectangleLines((int)player.body.x - 4, (int)player.body.y - 4, (int)player.body.width + 8, (int)player.body.height + 8, YELLOW);

            }

            for (int i = 0; i < MAX_ALIENS; i++) {

                if (!aliens[i].active) {

                    continue;

                }

                if (currentLevel == MAX_LEVELS - 1) {

                    DrawTexture(bossTexture, (int)aliens[i].body.x, (int)aliens[i].body.y, WHITE);

                    DrawRectangle((int)aliens[i].body.x, (int)aliens[i].body.y - 10, 80, 6, DARKGRAY);

                    int lifeWidth = (int)(80.0f * (aliens[i].health / 12.0f));

                    DrawRectangle((int)aliens[i].body.x, (int)aliens[i].body.y - 10, lifeWidth, 6, RED);

                }
                

                else {

                    DrawTexture(alienTexture, (int)aliens[i].body.x, (int)aliens[i].body.y, WHITE);

                }

            }

            for (int i = 0; i < MAX_BULLETS; i++) {

                if (!bullets[i].active) {

                    continue;

                }

                Color color = bullets[i].fromPlayer ? YELLOW : RED;

                DrawRectangleRec(bullets[i].body, color);

            }

            if (orb.active) {

                Color color = SKYBLUE;

                if (orb.typeChar == 'S') color = BLUE;

                if (orb.typeChar == 'B') color = ORANGE;

                if (orb.typeChar == 'L') color = GREEN;

                if (orb.typeChar == 'H') color = PURPLE;

                DrawCircle((int)(orb.body.x + orb.body.width / 2), (int)(orb.body.y + orb.body.height / 2), 16.0f, color);

                DrawText(TextFormat("%c", orb.typeChar), (int)orb.body.x + 8, (int)orb.body.y + 4, 20, WHITE);

            }

            if (currentPower != 'N') {

                DrawText(TextFormat("Power %c %.1fs", currentPower, powerTimer), 10, SCREEN_HEIGHT - 30, 18, LIGHTGRAY);

            }

        }

        else if (gameOver) {

            DrawText("Game Over!", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 40, 40, RED);

            DrawText("Press ENTER to try again", SCREEN_WIDTH / 2 - 170, SCREEN_HEIGHT / 2 + 10, 24, WHITE);

        }

        else if (gameWon) {

            DrawText("You saved the planet!", SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 - 40, 36, SKYBLUE);

            DrawText(TextFormat("Final Score %d", score), SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 10, 24, WHITE);

            DrawText("Press ENTER to play again", SCREEN_WIDTH / 2 - 190, SCREEN_HEIGHT / 2 + 50, 24, WHITE);

            if (IsKeyPressed(KEY_ENTER)) {

                gameWon = false;

                ResetShip(player);

                ResetBullets(bullets);

                currentLevel = 0;

                SpawnAliens(aliens, aliveAliens, currentLevel, configs);

                score = 0;

                totalTime = 0;

                orb.active = false;

                currentPower = 'N';

                powerTimer = 0.0f;

            }

        }

        EndDrawing();

    }

    UnloadTexture(shipTexture);

    UnloadTexture(alienTexture);

    UnloadTexture(bossTexture);

    UnloadTexture(background);

    CloseWindow();

    return 0;

}