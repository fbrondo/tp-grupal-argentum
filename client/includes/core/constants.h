#pragma once

#include <stdint.h>

#define DRIVER_RENDERER -1

// --- QT CONFIG --- //
static constexpr int MIN_ARGS = 2;
static constexpr int BASE_ARG = 3;
static constexpr int GAME_ARG = 5;     // host port user pass
static constexpr int SIGNUP_ARG = 10;  // host port --signup user pass race clase head body
static constexpr int LOGIN_ARG = 6;    // host port --login user pass
static constexpr int MAX_HEAD_ID = 9;
static constexpr int MAX_BODY_ID = 9;

// --- ANIMATION CONFIG --- //
static constexpr int TARGET_FPS = 60;
static constexpr int FRAME_MS = 1000 / TARGET_FPS;
static constexpr uint32_t MOVE_REPEAT_MS = 264;
static constexpr uint16_t DEFAULT_ANIMATION_SPEED_MS = 150;

// --- RENDERER CONSTANTS --- //
static constexpr int TILE_SIZE = 32;
static constexpr int MAX_TILE_TEXTURE_SIZE = 1024;
static constexpr int CULLING_MARGIN_TILES = MAX_TILE_TEXTURE_SIZE / TILE_SIZE + 2;
static constexpr uint32_t PLAYER_ENTITY_OFFSET = 0;
static constexpr uint32_t NPC_ENTITY_OFFSET = 1000000;
static constexpr uint32_t ITEM_ENTITY_OFFSET = 2000000;
static constexpr uint32_t CITIZEN_ENTITY_OFFSET = 3000000;

// --- WINDOW CONFIG --- //
static constexpr int WINDOW_W = 960;
static constexpr int WINDOW_H = 540;

// --- HUD RENDERER --- //
constexpr int PANEL_RIGHT_X = 686;
constexpr int PANEL_RIGHT_Y = 7;
constexpr int PANEL_RIGHT_W = 267;

constexpr int USER_INFO_Y = 7;
constexpr int USER_INFO_H = 114;

constexpr int INVENTORY_Y = 128;
constexpr int INVENTORY_W = 267;
constexpr int INVENTORY_H = 294;

constexpr int STATS_Y = 429;
constexpr int STATS_W = 267;
constexpr int STATS_H = 104;
constexpr int PROGRESS_BAR_X = PANEL_RIGHT_X + 20;
constexpr int PROGRESS_BAR_W = 227;
constexpr int PROGRESS_BAR_H = 20;
constexpr int HP_BAR_Y = STATS_Y + 27;
constexpr int MANA_BAR_Y = STATS_Y + 72;
constexpr int EXP_BAR_Y = USER_INFO_Y + 82;
constexpr int LEVEL_X = PANEL_RIGHT_X + 20;
constexpr int LEVEL_Y = USER_INFO_Y + 12;
constexpr int LEVEL_W = 50;
constexpr int LEVEL_H = 40;

constexpr int NAME_X = PANEL_RIGHT_X + 67;
constexpr int NAME_Y = USER_INFO_Y + 12;
constexpr int NAME_W = 185;
constexpr int NAME_H = 44;

constexpr int CONSOLE_X = 7;
constexpr int CONSOLE_Y = 7;
constexpr int CONSOLE_W = 672;
constexpr int CONSOLE_H = 135;
constexpr int CONSOLE_INPUT_H = 22;
constexpr int CONSOLE_INPUT_Y = CONSOLE_Y + CONSOLE_H - CONSOLE_INPUT_H;
