Status: Active

# Unreal Campaign Level Bootstrap

## Status (February 6, 2026)
- Initial campaign level path defined: `/Game/Maps/NazareneCampaign`.
- Automated map bootstrap script added: `unreal/TheNazareneAAA/Tools/create_campaign_level.py`.
- Level default game mode set to `NazareneCampaignGameMode` by script and project map settings.

## What was delivered
1. Map bootstrap automation that:
   - Creates or opens `/Game/Maps/NazareneCampaign`.
   - Applies level default game mode.
   - Seeds baseline playable actors (PlayerStart, DirectionalLight, SkyLight, NavMeshBoundsVolume).
   - Updates project startup/game default map settings to campaign map.
2. Runtime campaign environment pass:
   - Region-specific environment generation for Galilee, Decapolis, Wilderness, and Jerusalem.
   - Distinct lighting/tint presets and handcrafted geometry layout per region in `ANazareneCampaignGameMode::SpawnRegionEnvironment`.

## Runbook
From project root inside Unreal editor:
```python
exec(open("Tools/create_campaign_level.py").read())
```

Headless:
```bat
UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_campaign_level.py -unattended -nop4
```

## Validation checklist
- Open map `/Game/Maps/NazareneCampaign`.
- Confirm world settings default game mode is `NazareneCampaignGameMode`.
- Press PIE and verify player spawn, prayer site, enemies, and travel gate cycle.

