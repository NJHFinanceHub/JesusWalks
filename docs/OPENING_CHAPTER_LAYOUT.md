# Opening Chapter Layout

## Goal
Define the first playable chapter as Bethlehem Nativity: a story-first traversal with prayer, dialogue, and pilgrimage beats (not shooter-style combat framing).

## Map target
- Map: `/Game/Maps/NazareneCampaign`
- Region: Chapter 1 (`Bethlehem Nativity`)

## Encounter pacing
1. **Manger Arrival**
   - Safe spawn with Mary and Joseph dialogue.
2. **Shepherd Path**
   - Guided movement/tutorial beats with non-combat interaction prompts.
3. **Nativity Courtyard**
   - Prayer, scripture narration, and worship ambience.
4. **Temple Foreshadowing**
   - Story beat introducing the mission arc toward baptism and ministry.
5. **Chapter Exit**
   - Clearly framed travel gate silhouette toward the Jordan chapter.

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
