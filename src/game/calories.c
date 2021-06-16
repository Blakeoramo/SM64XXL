#include "sm64.h"
#include "level_update.h"
#include "interaction.h"
#include "mario.h"
#include "object_helpers.h"
#include "types.h"
#include "hud.h"
#include "game_init.h"
#include "calories.h"
u16 gNumDeaths= 0;
u16 gPoints= 0;
u16 gNextCoin= 0;
u8 gPauseTimer= 0; 
u8 gPauseCoolDown= 0;
u8 cheatLSD = 0;
u8 cheatLSDBuffer = 0;
u8 cheatDKToggle= 0;
u8 cheat1enable= 0;
u8 cheat2enable= 0;
u8 cheat3enable= 0;
u8 fatmariostate= 0;

////MAIN
void cal(void) {
	//CALORIES SYSTEM
	//DEFAULT WEIGHT: gMarioState->squishTimer = 4; gMarioState->scaleY = 2;
	//print_text_fmt_int(220, 100, "%d", cheat1enable);
	//print_text_fmt_int(220, 75, "%d", cheat2enable);
	//print_text_fmt_int(220, 50, "%d", cheat3enable);
	//obj_update_dialog_unk2(s32 arg0, s32 dialogFlags, s32 dialogID, s32 arg3);
	gMarioState->numDeaths = gNumDeaths;
	gMarioState->squishTimer = 4;
	gMarioState->scaleY = 2;
	render_hud();

	if (gPauseTimer > 0 && gPauseCoolDown > 0) {
		gPoints+= 0;
	}
	
	if (gPlayer1Controller->buttonPressed & START_BUTTON) {
		gPauseCoolDown=1;
	} else if (gMarioState->action==ACT_FREEFALL && ACT_IDLE) {
			gPauseTimer=0;
			gPauseCoolDown=0;
	} else if (gPauseCoolDown > 0){
			gPauseTimer=1;
	}
	
	if (gMarioState->numCoins > gNextCoin){
		gPoints-=5;
		gMarioState->forwardVel -= 10.0f;
		gNextCoin = gMarioState->numCoins;
		gNextCoin = gNextCoin;
	}
	
	if (gPoints > 24 && gPoints < 50) {
		gMarioState->squishTimer = 3;
		gMarioState->scaleY = 2;
	}
	
	if (gPoints > 49 && gPoints < 75) {
		gMarioState->squishTimer = 2;
		gMarioState->scaleY = 2;
	}
	
	if (gPoints > 74 && gPoints < 100) {
		gMarioState->squishTimer = 1;
		gMarioState->scaleY = 1;
	}
	
	if (gPoints > 99) {
		gMarioState->squishTimer = 0;
		gMarioState->scaleY = 0;
	}
	
    if (gPoints > 999) {
	  gPoints = 999;
	}
	
	if (gPoints < 1) {
		gPoints= 1;
	}
	
	if (gPoints == 999) {
		gMarioState->health= 0;
	}
	
	if (gMarioState->numLives < gMarioState->numDeaths) {
		gMarioState->numLives = gMarioState->numDeaths;
	}
	
	if (gMarioState->health== 0) {
		gPoints=0;
		gNextCoin=0;
	}
	
	if (gMarioState->numCoins < 1) {
		gNextCoin=0;
	}
	
	if (cheatLSDBuffer > 0) {
		cheatLSDBuffer+=1;
	}
	
	if (cheatLSDBuffer > 19) {
		cheatLSDBuffer=20;
	}
	
}
	