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

## create_opening_chapter_layout.py
Builds a repeatable blockout layer for opening chapter pacing in `/Game/Maps/NazareneCampaign`.

The script:
- Clears prior `OpeningChapterBlockout` actors.
- Places traversal lane, encounter landmarks, and boss arena ring.
- Ensures `PlayerStart` and `NavMeshBoundsVolume` are present.

### Run headless
```bat
UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_opening_chapter_layout.py -unattended -nop4
```

If invoking from outside the project directory, pass an absolute `-script=` path.

