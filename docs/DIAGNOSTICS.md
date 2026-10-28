# Update 3 testing and diagnostics

Working candidate **1.69.0**. Update 2 (1.54.0) remains the public release. This is the current checklist; earlier development notes describe older builds and are not release sign-off.

## Confirmed so far

- Candidate 1.69 passed 105 automated test groups, including 511 real native-reader configuration cases, plus 7 Python tests and isolated MMC adapter checks. Installation/rollback is checked in a disposable fixture; the real upgrade and full-game checks below still need user verification.
- The user verified the in-game Gameday Atmosphere tab and pause-menu fullness toggle on 1.61. Later controls and complete navigation still need a final-build check.
- Texas cannon movement was confirmed in game. The 1.67 log records native cannon starts with 15%, 20%, 25% and 30% settings. All nine authored cannon selectors have offline coverage; other stadiums have not been manually verified.
- Airborne home passes were captured; the 1.67 log records new verified airborne passes. An earlier away pass also logged anticipation. Audible timing on both offenses still needs listening on the final build.
- Build 1.67 correctly identified an away defender forcing a home fumble and withheld a positive home impact. A natural home-defense force with its positive sound/motion remains to be checked.
- Truck, hurdle, knockout and deflection signals were observed in game. Capturing a signal or logging a request alone does not establish the final audible/visible effect.
- The native Dynamic Camera Effects setting was verified read-only. The revised default RTG camera projection has automated coverage; its natural eruption strength still needs a visual check.

## Before the two test games

1. Close MMC and the game. Copy the candidate plugin DLL into MMC's Plugins folder, then restart both.
2. Confirm the launch/runtime log identifies 1.69.0 and activation succeeds. A startup/version line alone is not activation proof.
3. On first Update 3 startup, verify the old INI is backed up and the new file has current defaults. Edit one setting, restart once more, and confirm it stays changed without another reset.
4. Use the normal log setting, Debug=0. Cannon entries should now appear without technical logging.

## Full-game test plan

Use one ranked game in a cannon stadium and one unranked home team hosting a Top 25 visitor. Play the full games normally; record game quarter/clock and result when a reaction is missing or surprising. Rare plays are opportunities, not requirements to manufacture during every game.

| Check | Expected result |
| --- | --- |
| Cannon | One numbered [cannon] entry per admitted native blast, reporting variant and configured strength. Native sound is not duplicated. A request is not proof of visible shake. |
| Pass anticipation | Both offenses' eligible deep throws swell while airborne, then hand off to the catch, breakup or incompletion. No pre-catch added cheer/shake or extra volume bonus. |
| Run/return to score | Live excitement flows into a touchdown/eruption without a quiet dip, duplicate initial hit or restarted lower reaction. |
| Kickoff/punt returns | Swell/movement builds from 30 yards to the existing peak at 40. A pickup after a fumble does not restart gain shake. |
| Skills | Successful hurdles use a cheer; trucks use the big-hit crowd family. The small skill buildup is gentle and does not repeat on one collision. |
| Home defense | Confirmed third/fourth-down knockouts and pass breakups get the small paired reaction. A routine incompletion alone does not qualify. Hit-stick motion requires a completed third/fourth-down stop; other-down hits keep sound only. |
| Forced fumble | A verified home-defense force can react once on any down. Recovery alone adds no shake. An away-defense force at a home venue gets no positive home impact. |
| Touchdowns/eruption | Routine touchdowns stay routine. The opening TD exception covers only the first two tracked plays, including kickoff. A 60+ yard close-game TD gets a modest TD accent, not an eruption from distance alone. A genuinely sealing/winning event can still take priority. |
| Upset Watch | A credible home bid against a Top 25 visitor can gain sound from Q2 at its separate fullness gate, never below 50%. It does not grant packed-house status or raise volume caps. |
| Beat the traffic | A 17+ home deficit accelerates exits in the last quarter of Q4. Hopeless 28+ losses may use up to 10 extra original-crowd points above the normal quarter allowance. Floors, safe update windows and postseason limits remain. |
| Settings | Edit/reopen a toggle, slider, decimal value and Fan Patience; check Reset, Back and keyboard/controller tab navigation. Stock game settings must still work. The basic tab/fullness toggle is already verified. |
| Camera/sound gates | Dynamic Camera Effects Off and DGA Camera Shake Off stop added movement. Cannon strength 0 keeps native cannon audio. Pause and recorded replay stay steady; normal effects resume without stale celebrations. |
| Session cleanup | Try a timeout, replay, Super Sim and a second game. No stuck loops, old reaction IDs, crowd refills or repeated errors should carry over. |

## Additional checks before claiming full coverage

- A real home blocked field goal, including its sound/impact and possible return/score handoff; a normal missed kick must not count as a block.
- A game-sealing takeaway in the default RTG player camera, if RTG is included in the advertised support. Offline Dynasty remains the verified mode; do not imply all modes are guaranteed.
- Another stadium's native cannon, ideally using a different variant from Texas. All nine paths are covered offline, but that is not nine manually played venues.
- A late blowout with another quarter length, plus a close game that should retain fans.
- Exactly five yards lost versus four without sack proof, and a fourth-down sack after possession flips.
- Confirm any initially failing case after a fix on the same final release candidate. Keep unresolved limitations explicit.

## Files and logs

Only DynamicGamedayAtmosphere.dll stays directly in Plugins. DGA owns:

```text
Plugins/DynamicGamedayAtmosphere/
  DynamicGamedayAtmosphere.ini
  Logs/launcher.log
  Logs/DynamicGamedayAtmosphere.log
  Logs/status.txt
  Backups/
  History/
  Diagnostics/
  Tools/DgaDiagnostics.exe
  Runtime-1.69.0/
```

The one-time Update 3 reset saves the exact original INI in Backups before replacing it. SettingsResetVersion=3 records the release-family reset and must stay unchanged in routine builds. Later custom settings persist. The launcher waits for a running game to close before moving/resetting older settings. Locked settings or unavailable backups stop preparation without discarding the original. Game saves, native settings and unrelated plugins are untouched.

Normal logs include play summaries, live reactions, pass-flight, cannon, departures, configuration and activation errors. Cannon starts have a process-local sequence number so separate blasts are not swallowed by identical-message suppression. Native callback duplicates remain bounded by the existing observer, and no per-frame cannon log is added. Entries say an impact was requested; native accessibility, camera context and higher-priority motion can still prevent or cover visible output.

Use Debug=1 temporarily for native voice diagnostics, then restore 0. A native instance or dispatch request does not guarantee audible output. For a missed reaction, save the complete session logs and note the play's quarter/clock, side, down, result and expected effect. Status shows fullness and admission gates; 100% of original attendance can still mean a partly full stadium.

## Controlled previews and compatibility reports

After a verified offline match loads, open Tools/DgaDiagnostics.exe. Choose a preset, Sound and/or Camera movement and a duration, return to the unpaused lineup without snapping, and use F8 to start or F9 to stop. Disable the play clock manually if needed. Presets end automatically; a snap, pause, replay or world change cancels them. Camera-only previews are intentionally silent. Previews compare output routes; they do not prove automatic play detection or eligibility.

The same tool can save a read-only game compatibility report even when DGA cannot activate. Reports contain build metadata, module names and bounded samples at existing bindings, not game binaries, saves or full memory dumps. Nothing is uploaded automatically. A report is diagnostic evidence, not a compatibility certificate.

Launcher failures name the operation, PID/path and Windows error where available. Readiness failures back off at 5, 15, 30 and 60 seconds; unchanged errors are summarized. Each game process gets one actual runtime-load attempt. A required native mismatch stays inactive rather than installing speculative hooks.

See [building and verification](BUILDING.md), [candidate release notes](releases/Update3.md) and the [README troubleshooting steps](../README.md#problems).
