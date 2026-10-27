# Diagnostics — working Update 3 build

Working build **1.65.0**. Update 2 remains the public release. These changes have automated checks but still need an in-game preview test. The native pause-menu label route has been verified in game; the integrated working build still needs its restart checks.

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
    Runtime-1.65.0/
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

The window shows the same measured stadium-fullness percentage as the audio status. This is a separate desktop tool. The regular in-game pause menu also shows fullness beside Resume when PauseMenuFullness is enabled.

## Game compatibility report

With the game open, click **Save read-only game compatibility report** in the same tool. This button also works when DGA reports that no hooks were installed. It does not require a sound test or an unpaused game. The tool must run on the same computer as the game.

For someone who cannot activate the plugin, the standalone `DgaDiagnostics.exe` can also run outside MMC. In that case, it creates its own `DynamicGamedayAtmosphere/Diagnostics` folder beside the tool for reports. Sound/camera previews still require the matching native runtime and its installed status file; the standalone compatibility button does not.

The JSON file is saved in DGA's `Diagnostics` folder. It contains game build metadata, names of loaded modules, and small code samples at DGA's existing addresses. It does not include the game EXE, saves, a full memory dump, or user-folder paths. Nothing is uploaded automatically. It only requests permission to read the process; it cannot change game memory, clocks or settings. For a support report, share that JSON alongside the DGA log from the same session.

The report is evidence for checking a layout difference, not a compatibility certificate. Protected on-disk EXEs can differ from their running code, and a generic Windows file-version label may not identify the actual game build. An older PE timestamp alone does not prove that a Steam installation is out of date.

## Native crowd continuity

The working build retains a reaction ID after its one-shot cheer window ends or is deduplicated against the game. A native presentation stop can then recover the existing roar/swell promptly instead of waiting up to two seconds. The loop recovery leaves native stops and cleanup in place, does not repeat a cheer, and does not extend the original timer. DGA-authored start/stop messages are excluded from native-interruption detection. A new verified result can start its response even when the previous response is still in its quiet gap and its cheer is already playing natively.

Live timeout observation preserved native excitement. Penalty-specific one-shot cancellation versus a missing native trigger still needs an observed native-voice capture; an intensity value alone does not prove that a particular cheer played. For a repeat case, enable Logging Debug=1 temporarily. The native-reactions entries record changes in engine crowd voice types/handles, alongside phase and pause state. Compare these with reaction-audio to distinguish an observed engine instance from DGA dispatch. This is observation only; a listed instance still does not prove audible output. Disable Debug afterward. Normal logging does not scan these arrays. The game already reacts at the penalty announcement. That timing is intentionally unchanged; no extra acceptance/decline cheer or boo is added.

## Five-yard defensive losses

A fresh completed offensive play losing five or more yards can trigger the existing defensive cheer and 10% impact without a native sack flag. Confirmed shorter sacks still work. The play must already have qualifying atmosphere/fullness; unavailable yardage, stale results, special teams and unproved fourth-down field flips do not create a TFL. The play summary explicitly reports five-yard loss confirmation. Check an exactly five-yard loss, a larger loss and a four-yard loss without sack proof. Include a timeout/selection handoff to verify the existing fade.

A fresh native sack confirmation can also identify a fourth-down sack after possession has already flipped, even if yardage is unavailable. This does not guess the loss or treat a turnover on downs as a sack. Test a fourth-down sack without a takeaway, then an ordinary incomplete fourth-down pass. The sack should receive its eligible impact; the incompletion keeps only its normal defensive-stop rules.

## Opening visitors and earlier camera buildup

Use a fresh home-stadium game with qualifying atmosphere, receive the opening kickoff as the visitors, then select the first offensive play. Expect the defensive cheer and alternative large cheer together, a quick-building anticipation roar and supporting swell, and stronger opening motion. The default strength is 5% at 6,060 or fewer valid mapped positions, scaling smoothly to 15% at 61,888 or more. HostileOpeningShakeMaximumPercent changes the large-venue ceiling; smaller venues use one third. Zero removes the extra strength while ordinary qualified motion remains. The +35% high-leverage sound cap is unchanged.

The opening log reports when this one-time cheer opportunity is requested. Native duplicates or the two-family voice limit can still cover or suppress a cheer. After the first offensive play, the special welcome must not repeat. Reverse who receives: if the home team opens, the visitors' later first possession must not get it. Also check a neutral venue and a resumed scenario. OpeningAtmosphere=0 disables this extra sequence.

On ordinary qualified calls, watch for motion easing in before the lineup instead of appearing at full strength. The selection and lineup keep one ramp and one random choice. Buildup motion uses only the native gameplay camera context (1), including the hostile opener and critical downs; separate presentation views stay steady unless an earned play reaction/celebration is still finishing. This filter reads the current camera table without acquiring or retaining camera references or changing camera events. Check a short unsuccessful play followed by a quarterback cutaway, then a big play with a timeout/cutaway while its earned fade is still active. The former must not acquire the next lineup's shake; the latter should retain its original reaction clock. Check a recorded replay too; it must stay steady. These visual/timing changes need a fresh-process live check even after automated tests pass.

## Fast fumble recoveries

Recovery-only shake is removed in 1.63.0: live pickups are excluded from camera event routing, the recovery buildup is empty and the settled recovery fallback cannot create motion without separate forced-fumble proof. Defensive recovery sounds remain. A loose ball retires ordinary gain detection for that snap, preventing a held pickup from reviving pre-fumble catch/run motion. An already-playing forced-fumble impact keeps its original timer. Confirmed decisive eruptions and touchdowns retain priority.

## Reaction consistency checks

Short impacts request a roar/swell for their complete hold and fade, even when the native cheer is already playing and another cheer is skipped. This pairing adds no extra cheer or volume bonus. Camera preparation can request a supporting swell only after movement was actually rendered on the gameplay view; hidden presentation cutaways remain still. Automatic motion requires a healthy native sound route. Camera-only diagnostic previews remain intentionally silent.

The neutral-site first offensive play gets a positive opening counterpart for whichever side has the ball. Existing home-stadium behavior stays visitors-only. Other offensive and defensive reactions keep their side-relative neutral rules. Neutral games avoid treating the whole crowd as disappointed by one side's success.

After a return loses possession, its old celebration stops and its remaining motion settles briefly. A forced-fumble impact can replace it; a later recovery adds no new movement and does not restart the existing impact.

Check close and wide cameras during ordinary buildup, live returns, an impact and a celebration. Status reports the configured envelope, view-compensation multiplier and resulting movement separately. Every added shake reduces close-ups and compensates wider views, bounded at four times for extreme projections. This never changes the camera angle, zoom or audio gain. Recorded replays remain excluded. A successful dispatch or render count is diagnostic evidence, not proof of what was audible or visible; the integrated build still needs listening and camera checks in a fresh game.

## Later live check

Working 1.56.0 also recognizes the captured kind-1 airborne pass to held ball-3 receiver sequence and held ball-2 running states, without treating an airborne provisional record as a completed catch. Fresh reset, possession, coordinate and yardage checks remain. A non-scoring result can receive one bounded score upgrade before the next snap if the game publishes its touchdown late; the same play ID is retained and scoring sound/motion takes over. All added shake types now compensate for zoom, including ordinary/critical buildup and blocked-kick impacts.

The shared live post-whistle fade defaults to five seconds at every play distance. The distance-extension setting is retired; old stock 4.5-second settings migrate to five, while other custom fade values remain. Touchdown/eruption profiles retain their own durations. A qualifying home FG block uses the native blocked-FG flag and the original selected FG play, rather than guessing from a missed kick. A tie or 1–3-point home lead receives the touchdown-style impact; other qualifying margins use the regular impact. Test a real block, a normal miss, both lead bands, a possible return/TD takeover and zoomed cutaways. The optional binding was checked read-only in the running game; the integrated blocked-kick reaction still needs a live block.

For bug reports, check the current session's startup lines first. `Activation: INACTIVE` or `Required native bytes differ; no hooks installed` means DGA did not activate. A version/startup line alone does not mean its features are running. Keep the full log so earlier sessions are distinguishable from the reported one. A required game-code mismatch must be investigated; turning off individual settings will not make that layout compatible.

Play summaries distinguish missing native sack proof, unsupported defensive ownership and contradictory yardage. Unavailable yardage is reported as unavailable, rather than as a negative yardage value. Include the summary around a missed reaction, along with status; fullness alone does not show that every event requirement passed.

- In an ordinary matchup, test the opening offensive possession between its critical-down and packed-house fullness thresholds. Buildup and long-play reactions should qualify there, then lose that opening allowance for the next possession. Ordinary downs stay gentle; big games retain their existing lower opening allowance. Check OpeningAtmosphere=0 too.
- Verify migration preserves your custom settings and leaves no loose DGA INI/log files.
- Compare a roar, swell, cheer, combined eruption and camera-only preview. Check F8/F9, natural fading, pause/replay/snap cancellation and normal gameplay afterward.
- For the 1.56.0 reaction shapes, compare an immediately tackled 20+ yard catch with a 40+ yard completion followed by a short run. The latter should use the larger cheer, keep the roar through the catch and hold briefly after the whistle. Also finish a long run/return, call a timeout and proceed to play selection. Check that the earned response settles smoothly, then ends without restarting over the next snap. Include an immediately tackled catch and a touchdown takeover. Automated tests cover these handoffs; the integrated behavior still needs a live check.
- Check the regular pause menu: Resume should show Stadium Fullness matching the audio fullness in status, rounded to a whole percent. Resume/reopen after arrivals or departures to check it changes. Check replay, a second game without restarting, and PauseMenuFullness=0. Reopen the menu after editing the setting.

Build/host helpers are described in [BUILDING.md](BUILDING.md). Do not publish this as Update 3 before the remaining integrated live checks are resolved.

## 1.56.0 reaction checks

These additions pass automated checks but still need a fresh-process in-game check:

- In an eligible packed game, beat a defender with a truck or stiff arm, and land a hit stick. Confirm a short cheer/motion accent, without repeated effects on the same collision. Check a successful juke, spin and hurdle too; those success fields were traced but not yet positively demonstrated live.
- At a neutral site, repeat a skill or defensive play for the designated away side. At a home stadium, away-offense play calls should use roars/swells without DGA's lineup cheer; home-offense special calls retain theirs.
- Convert fourth-and-short on a short pass, then continue for a qualifying long gain. Look for the immediate conversion accent followed by the long-gain response, subject to the two-reaction voice limit.
- Stop third down inside the five, then a fourth-down attempt there. Fourth down should feel slightly stronger. A routine stop must not erupt merely because the game is close.
- In a packed ranked game or rivalry, clinch a one-score win with a takeaway/goal-line stop or offensive first down. Check the eruption/scoring-style upgrade. Unknown or sufficient opponent timeouts should prevent speculative upgrades.
- Let a sack or other earned shake carry onto a player close-up. Its original timer should continue with softer movement on the close-up. Recorded replay footage must stay steady. Ordinary lineup movement should still stay off presentation cutaways.
- For a major play or eruption, listen for at most one additional cheer after the initial hit. Logs label it Staggered second cheer; a voice-limit skip is intentional and must never backfill later.

Optional skill signatures fail closed without preventing other features from loading. New SkillMoveReactions, StaggeredMajorCheers and PackedBigGameReactions switches are in Dynamic Gameday Soundscape. Existing settings and volume limits are preserved. No release package or public release was created for this working build.

## 1.57.0 launcher and late-traffic checks

The launch watcher now reports the exact process/file operation, PID, Windows error and HRESULT instead of a generic Access is denied. It reports MMC/game administrator levels where readable. Metadata checks retry after 5, 15, 30 and then 60 seconds, with coalesced unchanged-error summaries and a recovery notice. The native loader names failed file, process, module, memory and thread operations and captures error codes before cleanup can change them. The same game process receives only one actual runtime-load attempt; a timeout is treated as possibly pending, not permission to inject again. These changes report restrictions; they do not change Windows permissions or game security settings.

The late home-blowout traffic rule now builds urgency faster in the final 25% of Q4. Deficits of 17+ use the stronger ramp, while hopeless 28+ deficits can use the approved extra quarter allowance, up to ten original-crowd points. Default 45 can therefore reach at most 55; the actual allowance is smaller in less decisive situations. Crowd floors, comeback protection, safe visibility windows and postseason limits stay in force. New LateTrafficExtraQuarterAllowance=0 restores the absolute ordinary cap, without turning traffic target acceleration off. LateTrafficEnabled=0 disables both. Status shows the live bound and why further departures may be limited.

For a live check after restarting, test a close game becoming a 28�35-point home loss inside the final three minutes of a 12-minute Q4. Check that exits settle at breaks/inside Super Sim, never visibly during live action, and that a late rally cannot refill earlier permanent departures. Check a different quarter length too. The integrated build is staged; the running 1.56.0 game is unchanged.

## 1.58.0 Upset Watch sound checks

This build implements a separate sound admission, leaving the 1.57.0 launcher and traffic behavior intact. The UNC-Notre Dame sample (native ranks 120/12, 7-3 in early Q2, 15,371 of 27,663 positions occupied) qualifies at a 54.9% requirement. Packed house still requires 83.3%. The new `Upset Watch sound` status field shows the current story stage, whether it qualifies and its own actual-fullness gate; `upset-watch` log entries report stage/admission changes rather than ticking every frame. Existing upset retention is a separate field and behavior.

Automated checks cover the sample, unknown ranks/layouts, smaller discrepancies, quarter-length scaling, neutral-side symmetry, failed bids, off switches, reset ownership, reaction and final-whistle handoffs, both native swell channels and existing volume/camera gates. Manual listening remains required after installing the staged build and restarting MMC/game:

- In the sample matchup, listen for a modest Q2 lineup roar/swell. Confirm that packed-house status and ordinary pre-snap camera motion stay inactive at 55.6%.
- Try a home big play, then an away offensive call. The earned play should react normally; the away call should add roars/swells without a lineup cheer.
- Compare an early close bid with the final 25% of Q4. A major late upset bid should sustain tension without stacking boosts or restarting cheers on every tick.
- Let the favorite lead by more than 14, or by more than eight in that final window. This route should release. Pause/replay/simulation must not generate fresh reactions.
- Confirm an upset victory's existing celebration still takes priority and follows postgame carry rules.
- Set `UpsetWatchAtmosphere=0`; other routes, packed requirements and configured volume caps must retain their values.

## 1.59.0 airborne-pass swell checks

A read-only live capture recorded three qualifying home throws at 100% mapped fullness, with verified airborne spans of about 0.95, 1.95 and 3.12 seconds. The final throw was confirmed incomplete by the user. Both old and corrected detectors recognize those passes. The old mixer could leave a low-intensity lineup without an actual supporting-swell request, and the callback forced the supporting channel to use a four-second attack.

The corrected callback uses the existing gameplay swell with a 500ms attack, one pass identity and the existing 1.2-second pass-end fade. It can start despite the prior lineup's quiet gap and refresh an already-playing supporting channel once for the new flight. Native stop recovery is bounded; primary roar ownership, higher-priority results/eruptions and configured caps remain intact. The pure detector also accepts reset proof already verified at the line for quick releases. Actual settings, fullscreen/pause ownership, native ball proof and fullness admission are still required.

`status.txt` now distinguishes the pass gate, verified-flight count and an actually playing airborne-pass supporting swell. One `pass-flight` log entry is written when a new airborne pass is verified, without logging every tick.

Automated tests include replay of the captured ball sequence and native-event fixtures for cold/warm supporting voices, fast attack, pass-end release, repeated ticks, quiet gaps, interruptions and result priority. The code is staged for later installation. Listening to the corrected build after restarting MMC/game remains necessary; the capture used the installed build and made no game writes.

## 1.60.0 scoring handoff and in-game settings

A full-size, uninterrupted live-gain roar/swell carries into a confirmed touchdown or eruption without restarting its native attack. Smaller or natively stopped voices still refresh promptly. An earned crowd-intensity floor bridges the opening 250 ms, then releases into the scoring profile by 500 ms. Existing IDs, timing, fullness admission, volume caps and priorities are preserved. Cold, stale, paused and new-snap inputs cannot manufacture a score buildup.

The Gameday Atmosphere tab uses the offline native settings provider and appears after the standard tabs. It includes all 73 user controls, grouped into Attendance, Soundscape, Broadcast and Logging, with native descriptions, Off/On choices, sliders, fractional-value choices and the departure profile. It reads and writes only the existing DGA INI; upgrade markers and native profile options are excluded. Existing custom fractional values are included as choices rather than rounded away.

The tab is added only after the named active frontend script and its category fallback match the inspected layout. The category extension changes only the unknown-button fallback for DGA's new ID. Existing native cases and bytecode branch targets retain their offsets. The provider retains all 42 virtual methods and its RTTI entry; unrelated operations forward their full arguments. The implementation uses the existing loaded-file cache on the menu thread, without heap scans or game-asset edits.

Automated fixtures cover row/value serialization, defaults, limits, INI saving/reopening/comments, future-config refusal, unique IDs, category branch alignment and six-argument stock forwarding. The catalog is checked against every user INI key and the actual configuration audit ranges. Read-only inspection verified the menu script and provider in PID 25260. Its captured prologue also caught and corrected an overly strict compatibility guard. The agent made no game-memory writes. The user subsequently restarted into an early 1.60.0 candidate; the corrected staged DLL needs a later restart to replace that candidate.

The integrated tab still needs an installed-build visual and navigation test: confirm placement after Audio Settings, edit/reopen a toggle, a slider, a fractional control and Profile, check keyboard/controller switching and Back, verify Reset on the DGA page, and confirm every stock tab still edits and saves its own settings. Listen to a long run ending in a normal TD and an eruption to check the crowd handoff. The corrected candidate is staged for later installation. The agent has not replaced the installed early candidate.

## 1.61.0 default Road to Glory camera compensation

A 150-second read-only capture of the default RTG player camera verified the gameplay view and shared camera guards across live and post-play phases. The widest observed vertical FOV was 88.507 degrees, requiring 3.090x compensation against the existing 35-degree reference. The former 2x ceiling under-compensated that view. The ceiling is now 4x; the captured view receives its calculated 3.090x, while close-up attenuation, configured strengths, envelopes, eligibility, replay exclusion and sound caps are unchanged.

An explicitly approved ten-second camera-only eruption preview on installed 1.60.0 confirmed real output at the old 2x ceiling. The user described it as suitable for an ordinary play and slightly light. The preview ended normally. Regression tests cover the captured projection for every reaction kind, projected-motion normalization, the extreme-view bound, unchanged source camera data and eruption impact-to-hold progression. Revised 1.61.0 compensation still needs a visual check after installation; this session did not test a naturally triggered RTG eruption or preview a post-play cutaway.

## 1.62.0 settings, touchdown rules and skill reactions

The user verified the native tab and pause-menu fullness toggle on installed 1.61.0. All 73 controls now have plain-language labels and help. Every numeric description explains higher and lower values; Profile labels are Automatic, Normal, Patient Fans and Bad Weather. INI keys, choices, defaults and bounds are preserved. Numeric editing, controller navigation and Reset still need installed-build checks.

The Penn State-USC log identifies the first two touchdowns as Q1 10:26 (opening-five-play rule in 1.61.0) and Q1 5:11 (60+ yard close-game rule). The new opening cutoff is two tracked plays, including kickoff. A long close-game touchdown now keeps the standard TD duration and sound-volume caps, with a 15% camera accent and bounded extra crowd excitement. Other decisive-score eruptions retain priority. Summaries now print the pre-snap line-to-goal TD distance separately from ordinary result yardage.

A 120-second read-only trace on PID 34916 caught the credited hurdle count settling about 0.27 seconds after the whistle while the hurdle fake-out timestamp stayed zero. The optional native counter binding now accepts only fresh increases from an observed snap and up to 0.6 seconds after the whistle, with the existing side, spacing, per-play and suspension limits. Skill accents use a 0.45-second rise, 50% of configured live strength and a two-second lifetime. A separate sound envelope supplies a short swell even with DGA camera shake off; each fresh accent may refresh the supporting voice once with the native 0.5-second minimum attack. Trucks request the authored area-reaction category BIGHIT_POS, large size (11); existing big-hit voices count toward the two-voice budget. Exact sample/listening checks remain pending.

The native catch-drop observer copies its knockout payload into +0x175 and uses it for the Pass Dropped - KNOCKOUT! event. Its reset clears the flag. The optional read-only binding checks those code signatures, owner vtable and stable pointer; the model requires a new clear-to-knockout transition plus a dropped-pass result on an observed third/fourth defensive snap. Ordinary drops, special teams, stale worlds, simulation and late flags are rejected. The small impact lasts 1.8 seconds at half live strength, with defensive cheer 59 and normal priority. It still needs a naturally observed in-game knockout. Existing within-five-yard third/fourth stops are tested for both their impact event and defensive cheer.

Turning Dynamic Camera Effects Off changed native camera option +0x3e6 from 1 to 0, while the other profile stayed 1. All three added camera routes already require both native options. Regression tests cover each motion category with either profile Off/On; status now distinguishes the native switch from the mod setting. The agent left the user-selected switch untouched.

## 1.63.0 returns, recovery motion and reaction handoffs

Kickoff/punt returns expose a coherent buildup from 30 to 40 yards, with one cheer and the existing full peak at 40. Both world directions, all three native kick types, home/neutral ownership, partial stops, high-water progress, brief holder gaps, stale samples and teleports have regression coverage. Partial returns do not own a full confirmed-return hold.

The camera interaction matrix checks 121 ordered pairs across ordinary/critical lineups, runs, catches, soft skills, defensive hits, TDs, eruptions, safeties, blocks and clinchers. It checks amplitude/oscillation continuity, strength bounds and priority. Separate sequences check conversion -> gain -> skill -> forced fumble -> sound-only pickup -> TD -> eruption, shorter custom celebration durations, no hidden lower-effect resurrection, and fresh-snap cleanup.

The sound handoff now covers equal/higher-priority result changes as well as scoring upgrades. It carries only a recently earned normalized level for 250 ms, releasing the bridge by 500 ms without changing effect timers or gain limits. Negative reactions, lost possession, stale/world changes and a new snap cannot inherit the old play. Full-size continuous voices are retained across new result IDs; stopped or undersized voices still refresh.

Offensive/defensive positive cheer IDs 83 and 59 share stable per-event choices. Runtime duplicate matching treats them as equivalent, so variety cannot create a duplicate layered cheer. Native big-hit, disappointment and scoring IDs retain their original families. Existing full-volume and two-voice limits remain. Tests cover both-family coverage, repeated callback stability, cross-family native deduplication and no retries. The agent has not installed the build or changed game settings; actual transition feel still needs in-game listening/visual checks.

## 1.64 sound-linked camera verification

The Cannon sound observer status lists availability and confirmed native starts. A [cannon] log requires the exact stadium FX patch, native Play event, an enabled playing instance, positive authored amplitude and a Cannon_01..09 selector (10..18). It does not infer a blast from a score or team ID. The existing native blast is the sound companion; no cannon sound is duplicated. Nine selector values and native receiver/variant-accessor bytes were verified read-only; in-game event timing is pending on the staged observer.

Crowd motion requires a fresh 150ms audible lease from an owned/native-adopted roar or swell in the same match/epoch. Actual native release deadlines remain audible until their existing tail ends. Rendering a valid gameplay view can request a missing lineup sound without first moving the camera, avoiding a sound/motion startup loop. Lost scenes, failures, mute and stale leases suppress added motion. Native Dynamic Camera Effects still gates every category, including cannon impacts.

A read-only Texas capture at 09:21:29 showed the knockout flag rise while the pass result changed from kind 3 to kind 5, remaining positive through the whistle. The detector now admits kind 5 as well as 4, with the same fresh flag, third/fourth-down home-defense, possession and once-per-play checks. Successful hurdles request a positive cheer (the shared offensive/defensive cheer pool); their camera remains the gentle skill buildup.

## 1.65 cannon trigger correction

The installed 1.64.0 observer was ready but counted zero blasts after the Texas opening-kickoff cannon. Read-only inspection found the enabled stadium FX entity with selector 10 (Cannon_01), authored amplitude 1 and a native sound instance in playing state 2; master/crowd volume were both 100. The hook was installed, the selector/patch/instance checks matched, and master mute was ruled out.

The authored graph identifies 0x01ade220 as the start: AudioGraphEvent 73 feeds ReceiveEntry 60, output port 21, which drives the ADSR Trigger. Event 0x82b90c1c feeds port 22, the ADSR Release. Version 1.64 had those event meanings reversed. Version 1.65 observes the start and rejects release even while the native graph remains playing. The fixture uses these literal authored inputs independently of the detector constants, including a start/release/start sequence. No extra cheer, volume, timing or camera strength change is introduced. The running game remains unchanged; corrected installed-event/shake verification is pending.

A subsequent 1.64.0 Texas third-down pass-breakup capture at 10:20:47 showed the detector armed before the snap and throughout the play, with no native knockout flag in the sampled live/whistle window. The pass remained kind 3. Defensive/deep-incompletion cheer requests were logged; both native reaction slots were full, so additional cheers were suppressed. The user described a two-player DB/receiver breakup animation rather than a definite catch knocked loose. This is not treated as a confirmed knockout or used to broaden the impact to ordinary incompletions.
