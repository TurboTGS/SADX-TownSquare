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

constexpr int ApiVersion = 1;

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
