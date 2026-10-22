#pragma once

//	SADX Custom Stages support (custom_stages_api.h):
//	When SADX Custom Stages is enabled, Town Square is listed on its Custom page in Trial select
//	and only takes over Lost World's slot while it's played from there. Lost World stays stock
//	everywhere else (story, Trial). Without Custom Stages, it replaces Lost World as before.

bool CS_Mode();				// SADX Custom Stages is enabled (checked once, at Init)
void CS_Detect();
bool IsTownSquare();		// In Lost World, and it's Town Square right now
void INIT_CustomStages();	// Registers the stages (Custom Stages mode only)
