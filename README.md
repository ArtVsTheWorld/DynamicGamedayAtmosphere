# Dynamic Gameday Atmosphere

By Ace. A College Football 27 plugin for MMC.

This branch contains the Update 3 test candidate (2.0.7). It adds an occupied-seat native crowd-bus mix and removes the old excitement limiter and graph-volume cuts. It adds stable special-game demand, record/AP breakout attendance, easier small-venue arrivals, a 15% departure capacity floor, confirmed recovery/conversion camera fallbacks and deferred log I/O. The support weights and section patterns are retained. Update 2 remains the latest public release; this candidate is staged for testing, not published.

Candidate 2.0.7 adds off-by-default **2020 Mode** in Attendance: empty stands and native crowd/band mute, with the normal attendance history preserved for restoration. Qualifying overtime gains a generic confirmed defensive-stop shake, two-point and field-goal result coverage, a steady sound floor through kicks/tries and a 10% crowd-shake lift. Ownership, fullness, accessibility and replay guards remain. See the [change review and live checks](docs/reviews/Update3-2.0.7.md).

Candidate 2.0.6 removes duplicate intensity-context and camera checks, rejects unrelated cannon events earlier, and adds sound/camera callback timing. It retains the live-verified 2.0.5 whistle-hitch fix. See the [remaining-path performance audit](docs/reviews/Update3-2.0.6-performance.md); the candidate still needs a short audio/camera smoke test.

Candidate 2.0.5 reduces repeated play sampling and same-process expression-read overhead in Additional Replays. After the user's restarted OFF/ON comparison isolated that feature, the candidate passed the focused whistle-hitch test through arrivals completion and a touchdown's full-screen/inset sequence. It retains the prior equal-target crowd-update optimization and adds selector/routing timing. See the [performance investigation](docs/reviews/Update3-2.0.5.md).

Fans arrive late, take halftime breaks and leave as games get out of reach. Crowds react to big plays, full stadiums get more involved sound, and winning moments get bigger celebrations.

**[Download the latest release](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/releases/latest)** Ã‚Â· [Full feature list](FEATURES.md)

## Install or update

1. Close the game and MMC.
2. Download the ZIP from Releases.
3. Copy `DynamicGamedayAtmosphere.dll` from its `Plugins` folder into MMC's `Plugins` folder.
4. Start MMC and launch the game through it.

**Update 3 resets previous DGA settings to the current defaults once.** You do not need to delete your INI: the plugin saves the original in `Plugins/DynamicGamedayAtmosphere/Backups` before replacing it. Reapply any custom settings afterward using the new INI or the in-game Gameday Atmosphere tab. Later launches and routine updates preserve your new settings. Restart both programs after replacing the DLL.

Candidate 1.94 adds occupied-seat scaling for crowd motion and DGA reaction audio, live home showboat buildup/away boos, fourth-down-only goal-line motion, Q4-only high-leverage stops and a 13–20-yard gain ramp. Existing fullness qualification remains.

Candidate 1.93 simplifies defensive shake: sacks and runs use one confirmed five-yard-loss rule, and qualifying third/fourth-down stops share one motion decision. Short sacks retain their sounds and replay classification. Existing atmosphere gates and motion strengths remain.

## Settings

Edit `Plugins/DynamicGamedayAtmosphere/DynamicGamedayAtmosphere.ini`. Each option explains its default, range and what it changes.

- **Attendance:** arrivals, departures, halftime seat breaks and seating patterns.
- **Soundscape:** crowd sound, anticipation, play reactions and celebrations.
- **Broadcast:** camera movement and extra replay opportunities.

Fresh regular-season Dynasty games use grade/rank demand calibrated to real capacity-ratio statistics, including local-time night detection. Ordinary openings use a 50-60% baseline of expected peak by day, with up to8 small-venue access points,2 same-state points and5 night points, within the usual70% ceiling, with a hard 80% cap reserved for exceptional regular-season games; postseason starts 70-80% and championships 80-90%. Utah joins the severe-late arrival group. Postseason/non-regular games retain EA turnout. The new value is applied during launch; saved/simulated attendance stays native. See the [formula and examples](FEATURES.md#starting-turnout---first-pass-candidate). Sound uses how full the mapped stands actually look. Gameplay home-field advantage stays unchanged. Offline Dynasty is the verified mode; other modes are not guaranteed.

Deep throws by either offense use a fast native swell while the pass is airborne. It works alongside lineup buildup, settles after the pass ends and adds no pre-catch cheer, shake or volume bonus. `PassFlightAnticipation=0` disables it.

Stadium cannon blasts have their own **Stadium Cannon Shake (%)** setting, default **15%** (`CannonShakeStrengthPercent` under Broadcast). Higher values make the blast stronger; lower values make it gentler; 0 disables its shake while keeping the native sound. It covers all nine native cannon variants through the shared stadium sound route, with no team or fullness restriction. The impact lasts 0.9 seconds, respects Dynamic Camera Effects and does not stack on top of a stronger celebration. Detected blasts appear once in the normal log as numbered `[cannon]` entries, including variant and configured strength; Debug logging is not required.

Forced fumbles also have a fallback for delayed game signals: a recent opposing-player contact, the carrier losing the ball and fresh native fumble confirmation must all agree. A verified live defensive fumble return now uses the interception-return shake, subject to the same atmosphere gates. A result-only recovery adds no impact, and own-team pickups add no new motion. The reaction follows the original carrier, so an away defender forcing a home fumble does not produce a positive home-crowd impact.

Candidate 2.0.3 rebalances qualified crowd motion: a smooth occupied-seat curve gives 0.80x at 6,060 spectators and 1.10x at 61,888, with separate strengths for preplay tension, gains, stops, takeaways and scoring plays. Short impacts settle in 2-3 seconds. Earned reactions build smoothly into scoring peaks. See the [strength table and first tests](docs/reviews/Update3-2.0.3.md).

The working build adds a **Gameday Atmosphere** tab at the far right of the game's Settings menu. It contains 36 essential controls across Attendance (11), Soundscape (12) and Broadcast (13), with On/Off controls first and numeric settings below. There is no troubleshooting section. Advanced options remain in the INI; changing the menu does not reset them. The tab and fullness toggle were verified in game; complete navigation, numeric editing and Reset remain on the final-build checklist. A long-run touchdown also carries its earned crowd level into the celebration instead of restarting a full-size native swell's attack.

An existing live shake now rises into the TD/eruption level over 300 ms, without a new impact or an amplitude dip. Neutral crowd movement uses 75% of normal strength for either team. Successful spins/jukes get a cheer without added shake. Confirmed turnover-on-downs stops get a small paired shake and cheer.

In verified manual play, the candidate major-play replay route waits for the full native clip sequence before requesting a separate inset repeat. Slow Sim keeps native full-screen playback and does not receive added inset requests; unavailable UI queries retain original inset handling. It requires live verification, and cannot create missing recordings. DGA no longer promotes knees/spikes into routine replays; the native postgame repeated-highlight issue remains under investigation.

Quieter games now have a **sound-only reaction route**: verified big plays and results can get a single native cheer or disappointment sound, plus softer skill/pass swells. These do not unlock shake, sustained roar layers, follow-up cheers, eruptions or extra volume. Ordinary broken tackles are sound-only; knees/spikes do not get false sack reactions, and away takeaways clear previous home-positive motion.

The working build also adds **Upset Watch sound** for credible home underdog bids against Top 25 visitors from Q2 onward. Ranking gap, score and game stage can qualify roars/swells and earned reactions below the normal sound gate, never below 50% actual fullness. Packed-house requirements and volume caps stay unchanged. `UpsetWatchAtmosphere=0` disables this route.

The working build shows **Stadium Fullness** beside Resume in the regular pause menu. This is how full the mapped stands are, not attendance remaining. Set `PauseMenuFullness=0` under Dynamic Gameday Broadcast to hide it, then reopen the menu.

## Problems?

If the mod is not working, try these steps in order:

1. **Run MMC as administrator.** Right-click your mod manager and select **Run as administrator**, then launch the game through it.
2. **Update your mod manager** to the latest available version, then try again.
3. **Restore the game's executable.** Close the game and MMC, delete `CollegeFB27.exe` from the game's installation folder, then use your game launcher's **Repair** or **Verify/Validate game files** option to restore it. Once repair finishes, launch through MMC again.
4. **If it still does not work, send Ace all the logs.** Reproduce the issue, then collect every file in `Plugins\DynamicGamedayAtmosphere\Logs` inside your MMC folder. You can ZIP the Logs folder and send it together.

Include your plugin, game and MMC versions, matchup and relevant settings. You can also [open a bug report](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/issues/new/choose). Activation errors in `launcher.log` name the failed step and Windows error; reported MMC/game administrator levels help diagnose access-denied errors.

For missing audio, check that 2020 Mode is off, then compare Soundscape's `Enabled` option off/on. The native callback support stays installed for live toggles; disabled Soundscape retires its additions. Replacing the DLL still requires closing MMC and the game. There is no EXE hash requirement, but an EA update may still require new game bindings.

## Source and updates

[Build instructions](docs/BUILDING.md) Ã‚Â· [Updating after an EA patch](docs/UPDATING-THE-GAME.md) Ã‚Â· [Release history](RELEASES.md)

[Working diagnostics and remaining live checks](docs/DIAGNOSTICS.md)
