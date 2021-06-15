#include <PR/ultratypes.h>

#include "audio/external.h"
#include "engine/math_util.h"
#include "game/area.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/main.h"
#include "game/memory.h"
#include "game/print.h"
#include "game/calories.h"
#include "game/save_file.h"
#include "game/sound_init.h"
#include "game/rumble_init.h"
#include "level_table.h"
#include "seq_ids.h"
#include "sm64.h"

#define PRESS_START_DEMO_TIMER 800

#define STUB_LEVEL(textname, _1, _2, _3, _4, _5, _6, _7, _8) textname,
#define DEFINE_LEVEL(textname, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) textname,

static char gLevelSelect_StageNamesText[64][16] = {
    #include "levels/level_defines.h"
};
#undef STUB_LEVEL
#undef DEFINE_LEVEL

static u16 gDemoCountdown = 0;
#ifndef VERSION_JP
static s16 D_U_801A7C34 = 1;
static s16 gameOverNotPlayed = 1;
#endif
u8 pauseblink = 0;
u8 optionsmenu = 0;
u8 optionselector= 0;

#define OPTIONXY 10
#define OPTIONY 30
#define OPTIONX2 170


// run the demo timer on the PRESS START screen.
// this function will return a non-0 timer once
// the demo starts, signaling to the subsystem that
// the demo needs to be ran.

// don't shift this function from being the first function in the segment.
// the level scripts assume this function is the first, so it cant be moved.
s32 run_press_start_demo_timer(s32 timer) {
}

// input loop for the level select menu. updates the selected stage
// count if an input was received. signals the stage to be started
// or the level select to be exited if start or the quit combo is
// pressed.

s16 level_select_input_loop(void) {
	switch (optionsmenu) {
		case 0: //default
				print_text_centered(160, 5, "PRESS A FOR OPTIONS");
				if (pauseblink < 30) {
					print_text_centered(160, 30, "PAUSED");
				}
			break;
		case 1: //options
				if (pauseblink < 30) {
					print_text_centered(160, 80, "OPTIONS");
				}
				print_text(OPTIONXY,OPTIONY, "LSD MODE");
				print_text(OPTIONXY,OPTIONXY, "DK MODE");
				print_text(OPTIONX2,OPTIONY, "E*IT COURSE");
				print_text(OPTIONX2,OPTIONXY, "WIDESCREEN");
			break;
	}
	
	switch (optionselector) {
		case 1: //LSD
				if (optionsmenu == 1) {
					print_text(OPTIONXY + 105,OPTIONY, "*");
					if (gPlayer1Controller->buttonPressed & START_BUTTON && cheat1enable < 1 && optionselector == 1) {
						play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
						cheat1enable=1;
					} else if (gPlayer1Controller->buttonPressed & START_BUTTON && cheat1enable > 0 && optionselector == 1) {
						play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
						cheat1enable=0;
					}
				}
			break;
		case 2: //DK
				if (optionsmenu == 1) {
					print_text(OPTIONXY + 95,OPTIONXY, "*");
					if (gPlayer1Controller->buttonPressed & START_BUTTON && cheat2enable < 1 && optionselector == 2) {
						play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
						cheat2enable=1;
					} else if (gPlayer1Controller->buttonPressed & START_BUTTON && cheat2enable > 0 && optionselector == 2) {
						play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
						cheat2enable=0;
					}
				}
			break;
		case 3: //EXIT COURSE
				if (optionsmenu == 1)
				print_text(OPTIONX2 - 25,OPTIONY, "*");
				if (gPlayer1Controller->buttonPressed & START_BUTTON) {
					play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
					gCurrLevelNum = LEVEL_CASTLE;
					return gCurrLevelNum;
				}
			break;
		case 4: //WIDESCREEN MODE
				if (optionsmenu == 1) {
					print_text(OPTIONX2 - 25,OPTIONXY, "*");
				}
			break;
	}
	
	if (pauseblink < 60) {
		pauseblink++;
	}
	if (pauseblink > 59) {
		pauseblink=0;
	}
	
	if (optionselector > 4) {
		optionselector=1;
	} else if (optionselector < 1) {
		optionselector=4;
	}
	gNumDeaths= gMarioState->numLives;
	if (optionsmenu == 0 && gPlayer1Controller->buttonPressed & A_BUTTON) {
		play_sound(SOUND_GENERAL_LEVEL_SELECT_CHANGE, gGlobalSoundSource);
		optionsmenu=1;
	} else if (optionsmenu == 1 && gPlayer1Controller->buttonPressed & B_BUTTON) {
		play_sound(SOUND_GENERAL_LEVEL_SELECT_CHANGE, gGlobalSoundSource);
		optionsmenu=0;
	}
	
	if (optionsmenu == 1 && (gPlayer1Controller->buttonPressed & D_JPAD || gPlayer1Controller->buttonPressed & A_BUTTON)) {
		optionselector++;
	} else if (optionsmenu == 1 && gPlayer1Controller->buttonPressed & U_JPAD) {
		optionselector--;
	}
	
	if (cheat1enable == 1) {
		cheatLSDBuffer++;
	} else if (cheat1enable == 0) {
		cheatLSDBuffer=0;
	}
	if (cheat2enable == 1) {
		cheatDKToggle=1;
	} else if (cheat2enable == 0) {
		cheatDKToggle=0;
	}

    // start being pressed signals the stage to be started. that is, unless...
    if (optionsmenu == 0 && gPlayer1Controller->buttonPressed & START_BUTTON) {
        // ... the level select quit combo is being pressed, which uses START. If this
        // is the case, quit the menu instead.
        play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
		gNumDeaths = gNumDeaths;
		//gCurrLevelNum= LEVEL_BOWSER_1;
        return gCurrLevelNum;
    }
    return 0;
}

s32 intro_default(void) {
    s32 sp1C = 0;

#ifndef VERSION_JP
    if (D_U_801A7C34 == 1) {
        if (gGlobalTimer < 0x81) {
            play_sound(SOUND_MARIO_HELLO, gGlobalSoundSource);
        } else {
            play_sound(SOUND_MARIO_PRESS_START_TO_PLAY, gGlobalSoundSource);
        }
        D_U_801A7C34 = 0;
    }
#endif
    print_intro_text();

    if (gPlayer1Controller->buttonPressed & START_BUTTON) {
        play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
#ifdef VERSION_SH
        queue_rumble_data(60, 70);
        func_sh_8024C89C(1);
#endif
        sp1C = 100 + gDebugLevelSelect;
#ifndef VERSION_JP
        D_U_801A7C34 = 1;
#endif
    }
    return run_press_start_demo_timer(sp1C);
}

s32 intro_game_over(void) {
    s32 sp1C = 0;

#ifndef VERSION_JP
    if (gameOverNotPlayed == 1) {
        play_sound(SOUND_MARIO_GAME_OVER, gGlobalSoundSource);
        gameOverNotPlayed = 0;
    }
#endif

    print_intro_text();

    if (gPlayer1Controller->buttonPressed & START_BUTTON) {
        play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
#ifdef VERSION_SH
        queue_rumble_data(60, 70);
        func_sh_8024C89C(1);
#endif
        sp1C = 100 + gDebugLevelSelect;
#ifndef VERSION_JP
        gameOverNotPlayed = 1;
#endif
    }
    return run_press_start_demo_timer(sp1C);
}

s32 intro_play_its_a_me_mario(void) {
    set_background_music(0, SEQ_SOUND_PLAYER, 0);
    play_sound(SOUND_MENU_COIN_ITS_A_ME_MARIO, gGlobalSoundSource);
    return 1;
}

s32 lvl_intro_update(s16 arg1, UNUSED s32 arg2) {
    s32 retVar;

    switch (arg1) {
        case 0:
            retVar = intro_play_its_a_me_mario();
            break;
        case 1:
            retVar = intro_default();
            break;
        case 2:
            retVar = intro_game_over();
            break;
        case 3:
            retVar = level_select_input_loop();
            break;
    }
    return retVar;
}
