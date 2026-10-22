#include "pch.h"

//	Mod Version: v2.2.0

std::string ModPath;
HelperFunctions HelperFunctionsGlobal;

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		ModPath = path;
		HelperFunctionsGlobal = helperFunctions;

		CheckActiveMods();
		CS_Detect(); // SADX Custom Stages enabled? (customstages.h)
		CheckBASS();
		ExpandDrawQueueMemoryPool();

		
		//	Init Mod:

		INIT_Level();
		INIT_LevelTask();
		INIT_LevelRanks();
		INIT_Objects();
		INIT_Graphics();
		INIT_CustomStages();
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}