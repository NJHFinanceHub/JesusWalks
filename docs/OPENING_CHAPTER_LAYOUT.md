# Opening Chapter Layout

## Goal
Define the first playable chapter as a paced traversal from spawn to boss arena, with clear encounter beats and chapter exit readability.

## Map target
- Map: `/Game/Maps/NazareneCampaign`
- Region: Chapter 1 (`Galilee Shores`)

## Encounter pacing
1. **Sanctuary Start**
   - Safe spawn and first prayer site orientation.
2. **Shore Skirmish**
   - Intro melee/ranged pressure and player control onboarding.
3. **Ridge Choke**
   - Mid-combat lane control, dodge/parry pressure, miracle usage.
4. **Legion Sovereign Arena**
   - Dedicated circular boss arena with readable bounds.
5. **Chapter Exit**
   - Clearly framed travel gate silhouette and post-boss flow.

## Layout automation
- Script: `unreal/TheNazareneAAA/Tools/create_opening_chapter_layout.py`
- Function:
  - Loads `NazareneCampaign`.
  - Rebuilds a tagged blockout layer (`OpeningChapterBlockout`).
  - Places traversal ribbon, encounter landmarks, and arena ring.
  - Ensures `PlayerStart` and `NavMeshBoundsVolume` exist.

## Run command
```bat
UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_opening_chapter_layout.py -unattended -nop4
```

If running from outside `unreal/TheNazareneAAA`, use an absolute `-script=` path.

## Next art pass steps
- Swap blockout meshes with region kit modules by zone.
- Replace text markers with set-dressed landmarks and lighting cues.
- Bake navigation and validate enemy pathing in all beats.
