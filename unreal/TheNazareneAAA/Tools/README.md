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

