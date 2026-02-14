Status: Active

# Packaging And Profiling Pipeline (Win64)

## Objective
Define a repeatable Win64 packaging pipeline with perf and crash diagnostics gates.

## Build/cook/package command

Use Unreal Automation Tool (UAT):
```bat
RunUAT.bat BuildCookRun ^
 -project="unreal\\TheNazareneAAA\\TheNazareneAAA.uproject" ^
 -platform=Win64 ^
 -clientconfig=Shipping ^
 -serverconfig=Shipping ^
 -build -cook -allmaps -stage -pak -archive ^
 -archivedirectory="artifacts\\win64"
```

## Recommended profiles
1. `Development` (daily validation)
   - Include logs and console commands.
2. `Test` (balance/perf rehearsal)
   - Shipping-like optimization, validation-friendly symbols.
3. `Shipping` (release candidate)
   - Final packaging, full pak, crash reporting enabled.

## Performance budgets
| Metric | Budget |
|---|---|
| Frame time (60 FPS target) | <= 16.6 ms |
| Game thread | <= 6.0 ms |
| Render thread | <= 6.5 ms |
| GPU frame | <= 11.0 ms |
| Draw calls | <= 2,000 |
| VRAM usage | <= 3.5 GB |
| System memory (runtime) | <= 8 GB |

## Profiling pass
1. Enable traces:
   - `-trace=cpu,gpu,frame,bookmark,loadtime`
2. Capture with Unreal Insights:
   - `stat unit`
   - `stat gpu`
   - `stat game`
   - `stat memory`
3. Keep one trace per region and one full-campaign trace.

## Crash logging and triage
- Crash artifacts: `%LOCALAPPDATA%\\TheNazareneAAA\\Saved\\Crashes`
- Runtime logs: `%LOCALAPPDATA%\\TheNazareneAAA\\Saved\\Logs`
- Symbol store:
  - Archive PDBs per build ID for post-mortem symbolication.

## Release gates
1. No crash repro in 60-minute soak run.
2. All four regions under frame-time budget on target test hardware.
3. Save/load regression pass clean.
4. Input/UI flow validated for keyboard/mouse and gamepad.

