#include "pch.h"
#include "deathzones.h"
#include "paths.h"

//	Custom FogData, DrawDistance and SkyboxDrawDistance values:

___stcFog FOGDATA_TownSquare[] = {
	{ 100.0f, 1000.0f, 0xFFFFFF, 0 },
	{ 100.0f, 1000.0f, 0xFFFFFF, 0 },
	{ 100.0f, 1000.0f, 0xFFFFFF, 0 }
};

___stcClip DRAWDISTANCE_TownSquare[] = {
	{ -1.0f, -8000.0f },
	{ -1.0f, -8000.0f },
	{ -1.0f, -8000.0f }
};

___stcClip SKYBOXDRAWDISTANCE_TownSquare[] = {
	{ -1.0f, -10000.0f },
	{ -1.0f, -10000.0f },
	{ -1.0f, -10000.0f }
};

void SETVIEWDATA_TownSquare()
{
	gFog = FOGDATA_TownSquare[ssActNumber];
	gClipMap = DRAWDISTANCE_TownSquare[ClipLevel];
	gClipSky = SKYBOXDRAWDISTANCE_TownSquare[ClipLevel];
}


//	Load Landtable:

LandTableInfo* LAND_TownSquare00 = nullptr;

void LANDTABLE_TownSquare()
{
	LoadLandTable(&LAND_TownSquare00, "TownSquare_Landtable", &TEXLIST_TownSquare);

	GeoLists[LevelIDs_LostWorld * 8] = LAND_TownSquare00->getlandtable();
	GeoLists[LevelIDs_LostWorld * 8 + 1] = LAND_TownSquare00->getlandtable();
	GeoLists[LevelIDs_LostWorld * 8 + 2] = LAND_TownSquare00->getlandtable();
}


//	Load Deathzones:

void DEATHZONES_TownSquare()
{
	KillingCollisionModelsListList[LevelIDs_LostWorld][0] = DEATHZONES_TownSquare00;
	KillingCollisionModelsListList[LevelIDs_LostWorld][1] = DEATHZONES_TownSquare01;
	KillingCollisionModelsListList[LevelIDs_LostWorld][2] = DEATHZONES_TownSquare00;
}


//	Start Positions:

StartPosition STARTPOS_TownSquare00 = { LevelIDs_LostWorld, 0, { 173.22f, -25.0f, 300.9f }, 0xC000 };
StartPosition STARTPOS_TownSquare01 = { LevelIDs_LostWorld, 1, { 173.22f, -25.0f, 300.9f }, 0xC000 };
StartPosition STARTPOS_TownSquare02 = { LevelIDs_LostWorld, 2, { 173.22f, -25.0f, 300.9f }, 0xC000 };

void STARTPOSITIONS_TownSquare()
{
	for (unsigned char i = 0; i < Characters_MetalSonic; ++i)
	{
		HelperFunctionsGlobal.RegisterStartPosition(i, STARTPOS_TownSquare00);
		HelperFunctionsGlobal.RegisterStartPosition(i, STARTPOS_TownSquare01);
		HelperFunctionsGlobal.RegisterStartPosition(i, STARTPOS_TownSquare02);
	}
}


//	Paths:

void PATHS_TownSquare()
{
	HelperFunctionsGlobal.RegisterPathList(PATHDATA_TownSquare00);
	HelperFunctionsGlobal.RegisterPathList(PATHDATA_TownSquare01);
	HelperFunctionsGlobal.RegisterPathList(PATHDATA_TownSquare02);
}


//	File Replacements:

void FILES_TownSquare()
{
	// SET Replacement:

	ReplaceBIN("SET0700S", "SET_TownSquare-S"); // Target
	ReplaceBIN("SET0701S", "SET_TownSquare-S");
	ReplaceBIN("SET0702S", "SET_TownSquare-S");
	
	ReplaceBIN("SET0700K", "SET_TownSquare-K");
	ReplaceBIN("SET0701K", "SET_TownSquare-K"); // Target
	ReplaceBIN("SET0702K", "SET_TownSquare-K");
	

	// CAM Replacement:

	ReplaceBIN("CAM0700S", "CAM_TownSquare-S"); // Target
	ReplaceBIN("CAM0701S", "CAM_TownSquare-S");
	ReplaceBIN("CAM0702S", "CAM_TownSquare-S");

	ReplaceBIN("CAM0700K", "CAM_TownSquare-K");
	ReplaceBIN("CAM0701K", "CAM_TownSquare-K"); // Target
	ReplaceBIN("CAM0702K", "CAM_TownSquare-K");

	
	// Lantern PL & SL Replacement:

	ReplaceBIN("PL_70B", "LANTERN_TownSquare-PL");
	ReplaceBIN("PL_71B", "LANTERN_TownSquare-PL");

	ReplaceBIN("SL_70B", "LANTERN_TownSquare-SL");
	ReplaceBIN("SL_71B", "LANTERN_TownSquare-SL");


	// BGM Replacement:

	ReplaceBGM("lstwrld1", "BGM_TownSquare");
}


//	Level Init:

void INIT_Level()
{
	if (CS_Mode())
	{
		// Custom Stages: Lost World's slot is only taken while Town Square is played (customstages.cpp).
		LoadLandTable(&LAND_TownSquare00, "TownSquare_Landtable", &TEXLIST_TownSquare);
		return;
	}

	WriteData<5>((void*)0x422DEF, 0x90); // Disable DC Conversion Lost World
	
	LANDTABLE_TownSquare();
	DEATHZONES_TownSquare();
	STARTPOSITIONS_TownSquare();
	PATHS_TownSquare();
	FILES_TownSquare();
}