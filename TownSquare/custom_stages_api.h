#pragma once
// SADX Custom Stages API (header-only; copy this file into your level mod).
//
// SADX Custom Stages (custom_stages.dll) lists modded stages on a Custom page
// in Trial select. A custom stage BORROWS a stock level slot (level + act)
// while it's played from that page; the rest of the time the slot is stock,
// so your level doesn't replace a stock level for story or Trial.
//
// How a level mod uses it, in its Init:
//
//   #include "custom_stages_api.h"
//   static int g_stage = -1;
//   static void __cdecl OnEnable(int, void *) { /* write your table entries */ }
//   static void __cdecl OnDisable(int, void *) { /* put the stock ones back */ }
//
//   if (CustomStages::Available()) {
//     CustomStages::StageInfo info = {};
//     info.key = "TownSquareSonic";          // unique, stable (saves / ranks use it)
//     info.name = "Town Square";            // shown in the list
//     info.level = LevelIDs_LostWorld;      // the slot it borrows
//     info.act = 0;
//     info.characters = CustomStages::Character(Characters_Sonic);
//     info.onEnable = OnEnable;
//     info.onDisable = OnDisable;
//     g_stage = CustomStages::RegisterStage(info);
//     CustomStages::ReplaceFile(g_stage, "system\\SET0700S.BIN", "system\\SET_MyLevel-S.bin");
//     CustomStages::AddStartPosition(g_stage, Characters_Sonic, myStart);
//     CustomStages::AddPathList(g_stage, myPaths);
//   } else {
//     // No Custom Stages: replace the level as before (or don't load).
//   }
//
// With API 2 a stage can also have missions (C, B, A) with its own emblems,
// records and (with SADX Ranks) ranks:
//
//     info.missions = 3;                 // C, B and A
//     info.ringsB = 100;                 // B: 100 rings
//     info.timeA = 150;                  // A: within 2:30
//     info.rankBasis[CustomStages::MissionA] = CustomStages::RankByTime;
//     // A's rank by time: D 5:00, C 4:00, B 3:00, A 2:30
//     int a[4] = {300, 240, 180, 150};
//     memcpy(info.rankRequirements[CustomStages::MissionA], a, sizeof a);
//
// and wherever your code checks the slot (CurrentLevel == LevelIDs_LostWorld),
// also check CustomStages::IsActive(g_stage).
//
// While the stage is on: your file replacements, start positions and path
// lists are switched in for you, then onEnable runs. When it ends: onDisable,
// then they're switched out. Code hooks (WriteJump etc.) can stay installed
// as long as they check IsActive.
//
// The functions are found at run time (GetModuleHandle / GetProcAddress), so
// your mod has no link-time dependency on custom_stages.dll. All mod DLLs are
// loaded before any mod's Init runs, so this works in any load order.
#include <windows.h>

struct StartPosition;
struct PathDataPtr;

namespace CustomStages {

constexpr int ApiVersion = 2;

// Missions, in the order they unlock (as in SADX: C, then B, then A).
enum Mission { MissionC = 0, MissionB = 1, MissionA = 2 };
// What a mission's rank is worked out from (SADX Ranks, if it's installed).
enum RankBasis { RankByScore = 0, RankByTime = 1 };

struct StageInfo {
  int size = sizeof(StageInfo); // leave as is (lets the API grow)
  const char *key = nullptr;    // unique id, e.g. "TownSquareSonic" (copied)
  const char *name = nullptr;   // list name (copied)
  int level = -1;               // host level (LevelIDs)
  int act = 0;                  // host act, from 0
  unsigned characters = 0;      // Character(Characters_Sonic) | ...
  void(__cdecl *onEnable)(int stage, void *user) = nullptr;
  void(__cdecl *onDisable)(int stage, void *user) = nullptr;
  void *user = nullptr;

  // ---- API 2: missions, records, ranks --------------------------------------
  // How many missions the stage has: 0 none, 1 C, 2 C and B, 3 C, B and A.
  // C is clearing the stage, B collecting `ringsB` rings, A clearing within
  // `timeA` seconds (Gamma: with at least `timeA` seconds left). The player
  // picks an unlocked one on the Custom page (B after C, A after B); clearing
  // it earns the stage's emblem for it. Each stage keeps its own emblems,
  // high score, best time and most rings per character and save file; the
  // host level's are never touched. Older (API 1) stages: 0.
  int missions = 0;
  int ringsB = 0; // 0 = the host level's own B requirement (Sonic: 50 rings)
  int timeA = 0;  // 0 = the host level's own A requirement (its target time)
  // Rank requirements per mission (index MissionC/B/A) for D, C, B and A, in
  // that order; used by SADX Ranks if it's installed. RankByScore: the least
  // final score, rising. RankByTime: the slowest clear time in seconds,
  // falling (Gamma: the least time left, rising). All four 0 = SADX Ranks'
  // requirements for the host level (sadx_ranks.ini).
  int rankBasis[3] = {RankByScore, RankByScore, RankByScore};
  int rankRequirements[3][4] = {};
};

inline unsigned Character(int character) { return character >= 0 && character < 32 ? 1u << character : 0u; }

namespace detail {
inline FARPROC Get(const char *name) {
  const HMODULE m = GetModuleHandleA("custom_stages.dll");
  return m ? GetProcAddress(m, name) : nullptr;
}
} // namespace detail

// custom_stages.dll is loaded (enabled in the mod manager) and speaks this API.
inline bool Available() {
  using Fn = int(__cdecl *)();
  const auto f = reinterpret_cast<Fn>(detail::Get("CustomStages_ApiVersion"));
  return f && f() >= ApiVersion;
}

// Returns the stage id, or -1 (Custom Stages missing, or the key is taken).
inline int RegisterStage(const StageInfo &info) {
  using Fn = int(__cdecl *)(const StageInfo *);
  const auto f = reinterpret_cast<Fn>(detail::Get("CustomStages_RegisterStage"));
  return f ? f(&info) : -1;
}

// The stage is being played (its slot is borrowed right now).
inline bool IsActive(int stage) {
  using Fn = int(__cdecl *)(int);
  const auto f = reinterpret_cast<Fn>(detail::Get("CustomStages_IsStageActive"));
  return f && f(stage) != 0;
}

// Maps a game file to yours while the stage is on (like HelperFunctions::
// ReplaceFile; `replacement` is resolved through the loader's file map, so
// "system\\MyFile.bin" finds the copy in your mod's system folder).
inline bool ReplaceFile(int stage, const char *original, const char *replacement) {
  using Fn = int(__cdecl *)(int, const char *, const char *);
  const auto f = reinterpret_cast<Fn>(detail::Get("CustomStages_ReplaceFile"));
  return f && f(stage, original, replacement) != 0;
}

// A start position used while the stage is on (like RegisterStartPosition).
inline bool AddStartPosition(int stage, int character, const StartPosition &position) {
  using Fn = int(__cdecl *)(int, int, const StartPosition *);
  const auto f = reinterpret_cast<Fn>(detail::Get("CustomStages_AddStartPosition"));
  return f && f(stage, character, &position) != 0;
}

// A level path list used while the stage is on (like RegisterPathList).
inline bool AddPathList(int stage, const PathDataPtr &paths) {
  using Fn = int(__cdecl *)(int, const PathDataPtr *);
  const auto f = reinterpret_cast<Fn>(detail::Get("CustomStages_AddPathList"));
  return f && f(stage, &paths) != 0;
}

} // namespace CustomStages
