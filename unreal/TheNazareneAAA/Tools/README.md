# Unreal Tools

## create_campaign_level.py
Bootstraps the first playable campaign map (`/Game/Maps/NazareneCampaign`), applies
`NazareneCampaignGameMode` defaults, seeds baseline world actors, and writes project
default map settings.

### Run from Unreal Editor Python console
```python
exec(open("Tools/create_campaign_level.py").read())
```

### Run headless (recommended for CI/bootstrap)
```bat
UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_campaign_level.py -unattended -nop4
```

## validate_asset_pipeline.py
Runs naming and baseline quality checks for imported assets across `/Game`.

Current checks:
- Class-to-prefix naming enforcement (`SM_`, `SK_`, `M_`, `MI_`, etc.).
- Static mesh LOD count recommendations.
- Static mesh simple collision presence checks.

### Run headless
```bat
UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/validate_asset_pipeline.py -unattended -nop4
```



## validate_vertical_slice_content.py
Validates Opening Chapter vertical-slice readiness for map presence, character-model naming coverage, and biblical music cue/wave conventions.

Current checks:
- Required campaign map exists (`/Game/Maps/NazareneCampaign`).
- Skeletal-mesh prefix coverage for player/enemy production assets.
- Recommended dynamic music cue and SoundWave naming coverage.

### Run headless
```bat
UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/validate_vertical_slice_content.py -unattended -nop4
```

If invoking from outside the project directory, pass an absolute `-script=` path.

## create_biblical_art_pack.py
Creates a project-local art pack under `/Game/Art` using available engine/tutorial content.

The script:
- Duplicates a humanoid skeletal mesh + animation blueprint baseline into `/Game/Art`.
- Creates project-owned environment mesh assets for map dressing.
- Creates material instances for biblical-themed character and environment palettes.

### Run headless
```bat
UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_biblical_art_pack.py -unattended -nop4
```

## create_region_sublevels.py
Creates campaign streaming region levels under `/Game/Maps/Regions/*` with base lighting/nav setup.

```bat
UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_region_sublevels.py -unattended -nop4
```

## create_audio_pack.py
Creates project-local combat/music audio placeholders under `/Game/Audio/*` so gameplay slot wiring resolves.

```bat
UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_audio_pack.py -unattended -nop4
```

## create_behavior_tree_assets.py
Creates behavior tree placeholders under `/Game/AI/BehaviorTrees/*` for archetype routing.

```bat
UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_behavior_tree_assets.py -unattended -nop4
```
