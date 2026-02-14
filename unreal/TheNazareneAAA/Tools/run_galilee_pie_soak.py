"""Run a timed Galilee PIE soak in full Unreal Editor.

Usage:
  UnrealEditor.exe TheNazareneAAA.uproject -ExecutePythonScript=Tools/run_galilee_pie_soak.py -unattended -nop4
"""

from __future__ import annotations

import os
import time
import unreal

MAP_PATH = "/Game/Maps/Regions/Galilee/L_GalileeShores"
SOAK_SECONDS = int(os.environ.get("GALILEE_SOAK_SECONDS", str(20 * 60)))
WARMUP_SECONDS = 5.0
PIE_START_TIMEOUT_SECONDS = 20.0


level_editor = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
state = {
    "phase": "load_map",
    "phase_started_at": time.time(),
    "pie_started_at": 0.0,
    "last_report_minute": -1,
    "callback": None,
}


def _request_quit() -> None:
    world = unreal.EditorLevelLibrary.get_editor_world()
    unreal.SystemLibrary.execute_console_command(world, "QUIT_EDITOR")


def _finish(success: bool, message: str) -> None:
    callback = state.get("callback")
    if callback:
        unreal.unregister_slate_post_tick_callback(callback)
        state["callback"] = None
    if success:
        unreal.log(message)
    else:
        unreal.log_error(message)
    _request_quit()


def _tick(_delta_seconds: float) -> None:
    now = time.time()

    try:
        phase = state["phase"]

        if phase == "load_map":
            unreal.log(f"Galilee soak: loading {MAP_PATH}")
            unreal.EditorLevelLibrary.load_level(MAP_PATH)
            state["phase"] = "warmup"
            state["phase_started_at"] = now
            return

        if phase == "warmup":
            if now - state["phase_started_at"] < WARMUP_SECONDS:
                return
            unreal.log("Galilee soak: requesting PIE begin")
            level_editor.editor_request_begin_play()
            state["phase"] = "await_pie"
            state["phase_started_at"] = now
            return

        if phase == "await_pie":
            if level_editor.is_in_play_in_editor():
                state["phase"] = "soak"
                state["pie_started_at"] = now
                unreal.log(f"Galilee soak: PIE started, running for {SOAK_SECONDS} seconds")
                return
            if now - state["phase_started_at"] >= PIE_START_TIMEOUT_SECONDS:
                _finish(False, "Galilee soak failed: PIE did not start within timeout.")
            return

        if phase == "soak":
            elapsed = now - state["pie_started_at"]
            minute = int(elapsed // 60)
            if minute != state["last_report_minute"]:
                state["last_report_minute"] = minute
                unreal.log(f"Galilee soak progress: minute {minute}/20, in_pie={level_editor.is_in_play_in_editor()}")

            if elapsed >= SOAK_SECONDS:
                unreal.log("Galilee soak: ending PIE")
                level_editor.editor_request_end_play()
                state["phase"] = "cooldown"
                state["phase_started_at"] = now
            return

        if phase == "cooldown":
            if now - state["phase_started_at"] >= 3.0:
                _finish(True, "Galilee soak complete: 20-minute PIE run finished.")
            return

    except Exception as exc:
        try:
            level_editor.editor_request_end_play()
        except Exception:
            pass
        _finish(False, f"Galilee soak script exception: {exc}")


state["callback"] = unreal.register_slate_post_tick_callback(_tick)
unreal.log("Galilee soak controller initialized.")
