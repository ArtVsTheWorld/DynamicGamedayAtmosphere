# Update 3 testing and diagnostics

Working candidate **1.74.0**. Update 2 (1.54.0) remains the public release. This is the current checklist; earlier development notes describe older builds and are not release sign-off.

Candidate 1.72 also sharpens forced-fumble onset without changing strength or detection requirements. A live feel check is pending.

Candidate 1.71 adds softer pre-punt buildup (half strength) and home-offense critical-down buildup (15% softer). These changes need an in-game feel check after installation.

Candidate 1.74 halves neutral crowd motion for both teams, gives each its own opening offensive snap, preserves the snap fade across view changes, uses an en dash in the pause label and excludes released passes from sack reactions. Candidate 1.73 moves home blowout traffic earlier and refreshes help while preserving current custom values. Full-screen replay followed by a separate inset repeat is still under investigation and is not included in this build.

## Confirmed so far

- Candidate 1.71 passed the full native/managed checks, including 511 real native-reader configuration cases, plus 7 Python tests and isolated MMC adapter checks. The Wake sparse-peak regression, below-gate sound route, kneel/spike exclusion, broken-tackle motion and away-takeaway cleanup have automated coverage, along with softer punt/home-offense critical buildup and unscaled result handoffs. Installation/rollback is checked in a disposable fixture; the real upgrade and full-game checks below still need user verification.
- The user verified the in-game Gameday Atmosphere tab and pause-menu fullness toggle on 1.61. Later controls and complete navigation still need a final-build check.
- Texas cannon movement was confirmed in game. The 1.67 log records native cannon starts with 15%, 20%, 25% and 30% settings. All nine authored cannon selectors have offline coverage; other stadiums have not been manually verified.
- Airborne home passes were captured; the 1.67 log records new verified airborne passes. An earlier away pass also logged anticipation. Audible timing on both offenses still needs listening on the final build.
- Build 1.67 correctly identified an away defender forcing a home fumble and withheld a positive home impact. A natural home-defense force with its positive sound/motion remains to be checked.
- Truck, hurdle, knockout and deflection signals were observed in game. Capturing a signal or logging a request alone does not establish the final audible/visible effect.
- The native Dynamic Camera Effects setting was verified read-only. The revised default RTG camera projection has automated coverage; its natural eruption strength still needs a visual check.

## Texas/Ole Miss test, October 27

Runtime 1.68 resumed from Midgame Save and finished Ole Miss 45, Texas 37. Attendance restored at 100% without fresh opening arrivals and finished at 89%. Both offenses logged airborne-pass anticipation (19 verified flights in the resumed process). Standard touchdowns, replay restoration, Super Sim/halftime attendance transitions and final sound/camera cleanup were observed in logs. These entries establish requests/state, not perceived output.

The user identified the final logged "defensive sack" as Ole Miss kneeling. This is a false positive, not successful sack coverage. Candidate 1.70 excludes native knee/clock-stop play types from buildup and result impacts, and makes regular broken tackles sound-only. Verified away takeaways also clear previous home-positive motion immediately instead of retaining a short fade. The new sound-only route addresses sparse-game reaction gaps; verify these fixes and the sound-only output live before release. The earlier process crash remains unresolved; no new error was logged during the resumed game. Positive home forced-fumble and pass-breakup output, and other rare-event checks below, remain outstanding.


The Wake/Virginia test exposed a separate native-quieting issue: after an early halftime seat break reduced actual fullness to 48.4%, runtime 1.68 reduced a native 1000 excitement sample to 168.4. Candidate 1.70 uses a fullness/strength blend for the bed ceiling and lets native reaction peaks bypass that ceiling while retaining crowd-presence scaling. The captured 18,829-position fixture is covered in the quieting and borrowed-entity runtime tests. Audible output still needs a live retest.

## Before the two test games

1. Close MMC and the game. Copy the candidate plugin DLL into MMC's Plugins folder, then restart both.
2. Confirm the launch/runtime log identifies 1.74.0 and activation succeeds. A startup/version line alone is not activation proof.
3. On first Update 3 startup, verify the old INI is backed up and the new file has current defaults. Edit one setting, restart once more, and confirm it stays changed without another reset.
4. Use the normal log setting, Debug=0. Cannon entries should now appear without technical logging.

## Full-game test plan

Use one ranked game in a cannon stadium and one unranked home team hosting a Top 25 visitor. Play the full games normally; record game quarter/clock and result when a reaction is missing or surprising. Rare plays are opportunities, not requirements to manufacture during every game.

| Check | Expected result |
| --- | --- |
| Sparse native peaks | Around 48-52% fullness, native big-play excitement can rise above the calmer background ceiling. No abrupt peak collapse at 50%; empty-crowd scaling, six-second background blend and volume caps remain. |
| Sound-only route | Below the layered gate, confirmed gains/defensive plays/scores get one native reaction; skills and airborne passes can get softer swells. No shake, passive roar, follow-up cheer, eruption or extra volume. Native disappointment applies to home mistakes and away scores. |
| Cannon | One numbered [cannon] entry per admitted native blast, reporting variant and configured strength. Native sound is not duplicated. A request is not proof of visible shake. |
| Pass anticipation | Both offenses' eligible deep throws swell while airborne, then hand off to the catch, breakup or incompletion. No pre-catch added cheer/shake or extra volume bonus. |
| Run/return to score | Live excitement flows into a touchdown/eruption without a quiet dip, duplicate initial hit or restarted lower reaction. |
| Kickoff/punt returns | Swell/movement builds from 30 yards to the existing peak at 40. A pickup after a fumble does not restart gain shake. |
| Skills | Regular broken tackles use sound only, without starting or restarting shake. Successful hurdles use a cheer; trucks use the big-hit crowd family. The small skill buildup is gentle and does not repeat on one collision. |
| Home defense | Confirmed third/fourth-down knockouts and pass breakups get the small paired reaction. A routine incompletion alone does not qualify. Hit-stick motion requires a completed third/fourth-down stop; other-down hits keep sound only. |
| Forced fumble | A verified home-defense force can react once on any down. Recovery alone adds no shake. An away-defense force at a home venue gets no positive home impact. |
| Touchdowns/eruption | Routine touchdowns stay routine. The opening TD exception covers only the first two tracked plays, including kickoff. A 60+ yard close-game TD gets a modest TD accent, not an eruption from distance alone. A genuinely sealing/winning event can still take priority. |
| Upset Watch | A credible home bid against a Top 25 visitor can gain sound from Q2 at its separate fullness gate, never below 50%. It does not grant packed-house status or raise volume caps. |
| Beat the traffic | A 17+ home deficit accelerates exits in the last third of Q4; 28+ can begin gently in the last half. Hopeless 28+ losses may use up to 10 extra original-crowd points above the normal quarter allowance. Floors, safe update windows and postseason limits remain. |
| Settings | Edit/reopen a toggle, slider, decimal value and Fan Patience; check Reset, Back and keyboard/controller tab navigation. Stock game settings must still work. The basic tab/fullness toggle is already verified. |
| Camera/sound gates | Dynamic Camera Effects Off and DGA Camera Shake Off stop added movement. Cannon strength 0 keeps native cannon audio. Pause and recorded replay stay steady; normal effects resume without stale celebrations. |
| Neutral camera | Both teams receive half-strength crowd movement, including their own first offensive snap. Verify long gain/TD handoffs and the 0.9-second snap fade. Native cannons retain their independent strength. |
| Sack freshness | A sack followed by an incomplete pass cannot carry the sack shake onto the incompletion. Genuine sacks still react; test with pass-anticipation sound disabled too. |
| Clock management | Kneels and clock-stopping spikes produce no added pre-snap or sack/stop shake. Genuine sacks and fake-spike plays remain eligible. |
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
  Runtime-1.74.0/
```

The one-time Update 3 reset saves the exact original INI in Backups before replacing it. SettingsResetVersion=3 records the release-family reset and must stay unchanged in routine builds. Later custom settings persist. The launcher waits for a running game to close before moving/resetting older settings. Locked settings or unavailable backups stop preparation without discarding the original. Game saves, native settings and unrelated plugins are untouched.

Normal logs include play summaries, live reactions, pass-flight, cannon, departures, configuration and activation errors. Cannon starts have a process-local sequence number so separate blasts are not swallowed by identical-message suppression. Native callback duplicates remain bounded by the existing observer, and no per-frame cannon log is added. Entries say an impact was requested; native accessibility, camera context and higher-priority motion can still prevent or cover visible output.

Use Debug=1 temporarily for native voice diagnostics, then restore 0. A native instance or dispatch request does not guarantee audible output. For a missed reaction, save the complete session logs and note the play's quarter/clock, side, down, result and expected effect. Status shows fullness and admission gates; 100% of original attendance can still mean a partly full stadium.

## Controlled previews and compatibility reports

After a verified offline match loads, open Tools/DgaDiagnostics.exe. Choose a preset, Sound and/or Camera movement and a duration, return to the unpaused lineup without snapping, and use F8 to start or F9 to stop. Disable the play clock manually if needed. Presets end automatically; a snap, pause, replay or world change cancels them. Camera-only previews are intentionally silent. Previews compare output routes; they do not prove automatic play detection or eligibility.

The same tool can save a read-only game compatibility report even when DGA cannot activate. Reports contain build metadata, module names and bounded samples at existing bindings, not game binaries, saves or full memory dumps. Nothing is uploaded automatically. A report is diagnostic evidence, not a compatibility certificate.

Launcher failures name the operation, PID/path and Windows error where available. Readiness failures back off at 5, 15, 30 and 60 seconds; unchanged errors are summarized. Each game process gets one actual runtime-load attempt. A required native mismatch stays inactive rather than installing speculative hooks.

See [building and verification](BUILDING.md), [candidate release notes](releases/Update3.md) and the [README troubleshooting steps](../README.md#problems).
