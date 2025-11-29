#pragma once
struct Ship {

    Rectangle body;

    float baseSpeed;

    float currentSpeed;

    int lives;

    bool immune;

    float immuneTimer;

};

struct Alien {

    Rectangle body;

    bool active;

    int health;

    float fireTimer;

    float dropTimer;

    float fireIntervalMin;

    float fireIntervalMax;

    float dropIntervalMin;

    float dropIntervalMax;

};

struct Bullet {

    Rectangle body;

    float speed;

    bool active;

    bool fromPlayer;

};

struct PowerOrb {

    Rectangle body;

    bool active;

    char typeChar;

    float lifeTimer;

};

struct LevelConfig {

    int rows;

    int cols;

    float alienSpeed;

    float shotSpeed;

    float dropMin;

    float dropMax;

    float fireMin;

    float fireMax;

    int alienHealth;

};

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 640;
const int MAX_ALIENS = 32;
const int MAX_BULLETS = 64;
const int PLAYER_MAX_BULLETS = 12;
const int MAX_LEVELS = 3;
const float POWER_DURATION = 5.0f;


// files 
const char HIGH_SCORE_FILE[] = "highest_score.txt";
const char WIN_LOG_FILE[] = "victory_log.txt";

// bullet speeds 
const float BASE_PLAYER_BULLET_SPEED = -350.0f; // Y up is negative and Y down is positive 
const float BOOSTED_PLAYER_BULLET_SPEED = -520.0f;
