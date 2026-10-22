#include "pch.h"
#include "customstages.h"
#include "custom_stages_api.h"
#include "deathzones.h"
#include "paths.h"
#include "o_skybox.h"

//	SADX Custom Stages support: see customstages.h.

extern LandTableInfo* LAND_TownSquare00;
extern _OBJ_ITEMTABLE OBJECTLIST_TownSquare;
extern TEX_PVMTABLE TEXTURELIST_TownSquare[];
extern StartPosition STARTPOS_TownSquare00;
extern StartPosition STARTPOS_TownSquare01;
extern StartPosition STARTPOS_TownSquare02;
void RD_TownSquare(task* tp);
void FILES_TownSquare();
void TITLECARDS_TownSquare();

static bool CustomStagesEnabled = false;
static int Stage_Sonic = -1;
static int Stage_Knuckles = -1;

bool CS_Mode()
{
	return CustomStagesEnabled;
}

void CS_Detect()
{
	CustomStagesEnabled = CustomStages::Available();
}

bool IsTownSquare()
{
	if (CurrentLevel != LevelIDs_LostWorld)
		return false;

	return !CustomStagesEnabled || CustomStages::IsActive(Stage_Sonic) || CustomStages::IsActive(Stage_Knuckles);
}


//	Lost World's stock table entries, saved while Town Square is in its slot:

static bool TablesOn = false;

static LandTable* Stock_GeoLists[3];
static PL_KILLCOLLI* Stock_DeathZones[3];
static TaskFuncPtr Stock_LevelTask;
static TaskFuncPtr Stock_SkyboxTask;
static TEX_PVMTABLE* Stock_PvmList;
static _OBJ_ITEMTABLE* Stock_ObjectLists[3];
static uint8_t Stock_DCLostWorld[5];	// 0x422DEF: Dreamcast Conversion's Lost World (NOPed)
static uint8_t Stock_FreeCamBoxes[10];	// 0x434A19: "No Free-Cam" boxes in Lost World Act 2 (NOPed)

static void __cdecl OnEnable(int, void*)
{
	if (TablesOn)
		return;

	const int lw = LevelIDs_LostWorld;

	for (int i = 0; i < 3; i++)
	{
		Stock_GeoLists[i] = GeoLists[lw * 8 + i];
		Stock_DeathZones[i] = KillingCollisionModelsListList[lw][i];
		Stock_ObjectLists[i] = objItemTable[lw * 8 + i];

		GeoLists[lw * 8 + i] = LAND_TownSquare00->getlandtable();
		objItemTable[lw * 8 + i] = &OBJECTLIST_TownSquare;
	}

	KillingCollisionModelsListList[lw][0] = DEATHZONES_TownSquare00;
	KillingCollisionModelsListList[lw][1] = DEATHZONES_TownSquare01;
	KillingCollisionModelsListList[lw][2] = DEATHZONES_TownSquare00;

	Stock_LevelTask = RoundMasterList[lw];
	Stock_SkyboxTask = ScrollMasterList[lw];
	Stock_PvmList = ListofPvmList[lw];

	RoundMasterList[lw] = RD_TownSquare;
	ScrollMasterList[lw] = BG_TownSquare;
	ListofPvmList[lw] = TEXTURELIST_TownSquare;

	memcpy(Stock_DCLostWorld, (void*)0x422DEF, sizeof(Stock_DCLostWorld));
	memcpy(Stock_FreeCamBoxes, (void*)0x434A19, sizeof(Stock_FreeCamBoxes));
	WriteData<5>((void*)0x422DEF, 0x90);
	WriteData<10>((void*)0x434A19, 0x90);

	TablesOn = true;
}

static void __cdecl OnDisable(int, void*)
{
	if (!TablesOn)
		return;

	const int lw = LevelIDs_LostWorld;

	for (int i = 0; i < 3; i++)
	{
		GeoLists[lw * 8 + i] = Stock_GeoLists[i];
		KillingCollisionModelsListList[lw][i] = Stock_DeathZones[i];
		objItemTable[lw * 8 + i] = Stock_ObjectLists[i];
	}

	RoundMasterList[lw] = Stock_LevelTask;
	ScrollMasterList[lw] = Stock_SkyboxTask;
	ListofPvmList[lw] = Stock_PvmList;

	WriteData((void*)0x422DEF, Stock_DCLostWorld, sizeof(Stock_DCLostWorld));
	WriteData((void*)0x434A19, Stock_FreeCamBoxes, sizeof(Stock_FreeCamBoxes));

	DragonCount = 0;
	TablesOn = false;
}


//	Files, start positions and paths go through Custom Stages, switched with the stage:

static void HandOver(int stage)
{
	if (stage < 0)
		return;

	TS_FileStage = stage;	// ReplaceBIN / ReplacePVR / ReplaceBGM -> CustomStages::ReplaceFile
	FILES_TownSquare();
	TITLECARDS_TownSquare();
	TS_FileStage = -1;

	for (unsigned char i = 0; i < Characters_MetalSonic; ++i)
	{
		CustomStages::AddStartPosition(stage, i, STARTPOS_TownSquare00);
		CustomStages::AddStartPosition(stage, i, STARTPOS_TownSquare01);
		CustomStages::AddStartPosition(stage, i, STARTPOS_TownSquare02);
	}

	CustomStages::AddPathList(stage, PATHDATA_TownSquare00);
	CustomStages::AddPathList(stage, PATHDATA_TownSquare01);
	CustomStages::AddPathList(stage, PATHDATA_TownSquare02);
}

static int Register(const char* key, int act, int character)
{
	CustomStages::StageInfo info;
	info.key = key;
	info.name = "Town Square";
	info.level = LevelIDs_LostWorld;
	info.act = act;
	info.characters = CustomStages::Character(character);
	info.onEnable = OnEnable;
	info.onDisable = OnDisable;

	return CustomStages::RegisterStage(info);
}

void INIT_CustomStages()
{
	if (!CustomStagesEnabled)
		return;

	// The keys match SADX Custom Stages' fallback entries for this mod.
	Stage_Sonic = Register("TownSquareSonic", 0, Characters_Sonic);
	Stage_Knuckles = Register("TownSquareKnuckles", 1, Characters_Knuckles);

	HandOver(Stage_Sonic);
	HandOver(Stage_Knuckles);
}
