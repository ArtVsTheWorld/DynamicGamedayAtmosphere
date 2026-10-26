# Diagnostics — working Update 3 build

Working build **1.55.1**. Update 2 remains the public release. These changes have automated checks but still need an in-game preview test. The native pause-menu fullness label is not implemented yet; its live UI trace was deferred.

## Files

Only the plugin DLL stays directly in MMC's Plugins folder. Its other files live here:

```text
Plugins/
  DynamicGamedayAtmosphere.dll
  DynamicGamedayAtmosphere/
    DynamicGamedayAtmosphere.ini
    Logs/
    History/
    Backups/
    Diagnostics/
    Tools/DgaDiagnostics.exe
    Runtime-1.55.1/
```

The launcher copies/upgrades existing settings before retiring the old loose files into Backups. Custom values are preserved. Existing nested settings take precedence if both locations exist; the other copy is backed up. Former INI `.previous`/`.tmp` files and DGA-owned loose logs/control files are kept in Backups too. Other plugins' files are untouched.

Close both MMC and the game when updating. The launcher waits for a running game to close before moving loose settings that an older DLL might still be reading. Existing history and nested logs stay in place.

## Repeatable previews

1. Launch the new working DLL through MMC and load an offline game.
2. Open `Plugins/DynamicGamedayAtmosphere/Tools/DgaDiagnostics.exe`.
3. Choose a preset, Sound and/or Camera movement, and 5–60 seconds.
4. Return to the game, line up without snapping, disable the play clock in the game's settings, then press **F8**. **F9** stops the preview. Leave the control window open for the shortcuts to work.

The buttons also work if your game remains unpaused when another window has focus. If a shortcut is already registered by another app, the window reports that conflict. No effect starts merely because the window is open.

Presets include roar, supporting swell, large cheer, conversion cheer, defensive cheer, disappointment, lineup atmosphere, critical-down atmosphere, touchdown and eruption. Camera samples use the configured strength/envelope; ordinary/critical previews use the configured ceiling rather than a newly random strength. Fullness and current volume limits still apply. Disappointment does not add an excitement lift that could hide its tone.

These are controlled previews of the native routes, not a simulated play or a test of automatic eligibility. They do not change scores, attendance, clocks or gameplay HFA. A quiet stadium can still make a preview quieter. Mute/accessibility controls, native voice limits and unavailable bindings can prevent part of a requested effect; inspect status/logs rather than treating a request as proof of playback.

Tests are one at a time, include their fading period and end automatically. Repeated Start does not queue another test. A snap, pause, replay, game change, expired request or disabled control cancels it. Stop ends added loops/motion; already-started native one-shot cheers finish naturally. The ordinary atmosphere then resumes. Requests are tied to one game process and expire, so an old file cannot start a test in a later game.

The window shows the same measured stadium-fullness percentage as the audio status. This is a separate desktop tool; it is not the planned in-game pause-menu widget.

## Game compatibility report

With the game open, click **Save read-only game compatibility report** in the same tool. This button also works when DGA reports that no hooks were installed. It does not require a sound test or an unpaused game. The tool must run on the same computer as the game.

For someone who cannot activate the plugin, the standalone `DgaDiagnostics.exe` can also run outside MMC. In that case, it creates its own `DynamicGamedayAtmosphere/Diagnostics` folder beside the tool for reports. Sound/camera previews still require the matching native runtime and its installed status file; the standalone compatibility button does not.

The JSON file is saved in DGA's `Diagnostics` folder. It contains game build metadata, names of loaded modules, and small code samples at DGA's existing addresses. It does not include the game EXE, saves, a full memory dump, or user-folder paths. Nothing is uploaded automatically. It only requests permission to read the process; it cannot change game memory, clocks or settings. For a support report, share that JSON alongside the DGA log from the same session.

The report is evidence for checking a layout difference, not a compatibility certificate. Protected on-disk EXEs can differ from their running code, and a generic Windows file-version label may not identify the actual game build. An older PE timestamp alone does not prove that a Steam installation is out of date.

## Later live check

For bug reports, check the current session's startup lines first. `Activation: INACTIVE` or `Required native bytes differ; no hooks installed` means DGA did not activate. A version/startup line alone does not mean its features are running. Keep the full log so earlier sessions are distinguishable from the reported one. A required game-code mismatch must be investigated; turning off individual settings will not make that layout compatible.

Play summaries distinguish missing native sack proof, unsupported defensive ownership and contradictory yardage. Unavailable yardage is reported as unavailable, rather than as a negative yardage value. Include the summary around a missed reaction, along with status; fullness alone does not show that every event requirement passed.

- Verify migration preserves your custom settings and leaves no loose DGA INI/log files.
- Compare a roar, swell, cheer, combined eruption and camera-only preview. Check F8/F9, natural fading, pause/replay/snap cancellation and normal gameplay afterward.
- For the 1.55.1 hold fixes, finish a long run/return, call a timeout and proceed to play selection. Check that the earned response settles smoothly, then ends without restarting over the next snap. Include an immediately tackled catch and a touchdown takeover. Automated tests cover these handoffs; the integrated behavior still needs a live check.
- For the separate pause-menu work, leave the regular pause menu open. The next step is to identify its live text/data owner and lifetime. Reflected pause/UI class names alone are not a verified text injection route.

Build/host helpers are described in [BUILDING.md](BUILDING.md). Do not publish this as Update 3 before the remaining live checks and pause-menu work are resolved.
