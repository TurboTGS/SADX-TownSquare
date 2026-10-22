#include "pch.h"
#include "hud.h"
#include "o_skybox.h"

//	Animated Textures:

void ANIM_Water()
{
	TEXLIST_TownSquare.textures[0].texaddr = TEXLIST_TownSquare_Water.textures[(FrameCounter / 2) % (LengthOfArray(TEX_TownSquare_Water))].texaddr; // Water Animation
	TEXLIST_TownSquare.textures[1].texaddr = TEXLIST_TownSquare_Fountain.textures[(FrameCounter / 3) % (LengthOfArray(TEX_TownSquare_Fountain))].texaddr; // Fountain Animation
}


//	Level Task:

void RD_TownSquare(task* tp)
{
	auto twp = tp->twp;

	if (!twp->mode)
	{		
		ADXTaskInit();
		PlayMusic(MusicIDs_lstwrld1);

		SETVIEWDATA_TownSquare();

		twp->mode++;
	}
	
	ANIM_Water();

	if (CurrentCharacter == Characters_Sonic)
		DrawDragonHUD();
}


//	Level Destructor Funchook:

static FunctionHook<void, int>RunLevelDestructor_t(RunLevelDestructor);

void __cdecl RunLevelDestructor_r(int heap)
{
	if (heap == 0 && IsTownSquare())
		DragonCount = 0;

	return RunLevelDestructor_t.Original(heap);
}

//	Init LevelTask:

void INIT_LevelTask()
{
	RunLevelDestructor_t.Hook(RunLevelDestructor_r); // Init Level Destructor Funchook.

	if (CS_Mode())
		return; // Custom Stages: switched with the stage (customstages.cpp)

	RoundMasterList[LevelIDs_LostWorld] = RD_TownSquare; // Level Task.
	ScrollMasterList[LevelIDs_LostWorld] = BG_TownSquare; // Skybox Task.

	WriteData<10>((void*)0x434A19, 0x90); // Remove "No Free-Cam" boxes in Lost World Act 2.
}