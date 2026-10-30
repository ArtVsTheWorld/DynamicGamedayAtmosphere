# Update 3 — working release notes

Working build **2.0.17**, not publicly released. Update 2 remains the latest public release. These notes are being prepared for the final Update 3 release. [Current feature list](../../FEATURES.md).

Changes since Update 2 (1.54.0).

### Tuning candidate 2.0.17 — skill handoffs and dense background

- Trucks use a quick 5–6% impact before crowd/view scaling. Hurdles and stiff arms keep their gentler accents.
- Skill accents can lift a weak run buildup without replacing it or adding shake strengths. Scoring and larger reactions keep priority.
- Short skill swells preserve an ongoing run response’s sound identity and fade.
- Listening-tested native Beds/Chants reach +3.5 dB in dense crowds; very large packed crowds ease toward +4 dB. Sparse and medium mixes stay unchanged, and reaction buses retain their +30% ceiling. No extra loop or play trigger.

### Tuning candidate 2.0.16 — awaiting live testing

- Run/scramble and short catch-and-run shake now builds from 11 to 15 total yards, retaining the 800 ms smoothing and twenty-yard default cheer. Long-catch and kick-return cutoffs stay unchanged.
- Native stadium cannon impact defaults to 18%, independent of crowd size and the live-play strength setting. Former stock 15% updates once; custom and zero strengths are kept.

### Tuning candidate 2.0.15 — awaiting live testing

- Ordinary playcall motion is 2.5–5%, special lineup cheers 4–6%, defensive critical buildup 7.5–10%, and home-offense critical buildup 5.625–7.5%, before occupied-seat and view factors. Punt buildup remains half the ordinary range. Reaction and touchdown peaks stay unchanged.
- Runs and short catch-and-runs still start building at 13 yards and cheer at 20. An 800 ms strength rise smooths late samples, and a tackle fades from the level actually reached. Long air-yard catch impacts and return/showboat timing stay unchanged.
- Qualified dense crowds gain fuller roar/swell layers around 80%/20,000 occupied positions, with an additional tier around 85%/45,000. Smooth transitions preserve the sparse mix; 2.0.17 adds a separate dense-background ceiling. Major moments can use stronger tested cheer variants without adding requests.
- Stock playcall settings migrate once; custom values, zero opt-outs and camera compensation choices are preserved. Attendance formulas and qualification rules are unchanged.

### Settings addition in 2.0.14

**Wide Player Camera Compensation** is now available under Settings → Gameday Atmosphere → Broadcast, directly below Added Camera Shake. It defaults Off and saves to the existing INI option. Broadcast now has 14 controls; the other sections are unchanged. No shake-strength or gameplay behavior is retuned in this build.


### Camera correction in 2.0.13

- Standard views cap shake compensation at 1×. The reported 7.2% critical-down envelope no longer becomes 22.2% just because that camera is wide. Close-ups retain their gentler movement.
- The former RTG/player-camera allowance is a separate opt-in: the **Wide Player Camera Compensation** Broadcast toggle, also available as `WidePlayerCameraCompensation=1` in the INI. It defaults off because a wide field of view does not identify a game mode.
- Native HFA camera shake yields while a fresh DGA crowd-shake output is actually rendered, then restores its latest native tier. Gameplay HFA, composure and the meter stay unchanged. The advanced native-filter zero opt-out remains respected.
- Corrected the stale technical-status reference to a two-voice cheer limit; native cheer overlap remains allowed.
- Occupied-seat scaling, reaction strengths, attendance and audio tuning are unchanged.

## Attendance

**New peak-attendance formula**

- Fresh regular-season Dynasty games now calculate turnout from the programs and matchup instead of simply using EA's default. The overall spread is calibrated against real FBS attendance-to-capacity data: struggling programs can draw sparse crowds, while strong programs regularly approach capacity.
- School support uses Stadium Atmosphere (35%), Team Prestige (25%), Program Tradition (20%), Brand Exposure (10%), Conference Prestige (5%) and AP rank (5%). Season records, opponent appeal and rivalry demand adjust the result. Strong seasons can give smaller programs a real attendance jump.
- Home openers get a variable 10–25% demand boost; night games 5–15%; senior day 10%; in-state opponents 5–10%; service-academy matchups 5–15%. These are relative boosts, capped together at +40%, not automatic sellouts. Weather can reduce turnout. Postseason keeps EA's peak.

**Kickoff crowd is separate from peak**

- Ordinary games begin around 50–60% of their expected crowd, with bonuses for smaller venues and special matchups. Exceptional regular-season games can reach an 80% kickoff share. Bowls/CFP start at 70–80%; championships 80–90%.
- UCLA, Miami, Georgia Tech, Maryland and now Utah keep later arrivals. Ranked rivalries qualify as big games when either team is Top 25 or both are Top 50.
- Stadium-local time identifies night games, so a late Eastern listing does not automatically make a West Coast game a night game.

**More natural crowds throughout the game**

- Desirable sideline seats stay fuller, with smaller group gaps and less uniform thinning.
- Varied halftime breaks/returns, better close-game patience, stronger late-blowout exits and night/scoring-pace adjustments. Permanent departures respect a 15% capacity floor without refilling empty seats.


## Crowd audio

**A new native-mixer system**

- Crowd volume now follows both how full the stadium is and how many mapped seats are occupied. A small full crowd and a large full crowd no longer get the same treatment.
- Sparse crowds lose the wall of continuous roar while keeping more collective reactions and individual fan detail. As people arrive, the sound grows smoothly; as they leave, it thins again.
- The vanilla-volume crossover varies with venue size: roughly 70% fullness in the smallest venue and 30% in the largest. Above that point, the crowd grows beyond vanilla. The smallest packed venue reaches +15% audio signal. Dense crowds reach +3.5 dB background, with the largest packed venue reaching +4 dB; collective reactions remain capped at +30% signal.
- At 25% fullness in the smallest venue, background sound is 50% of vanilla. Nonempty crowds have minimums of 40% background, 55% collective reactions and 95% individual detail. Zero attendance mutes the crowd. These are signal levels, not literal perceived-loudness percentages.
- Chants follow the background mix. The old separate chant cut and excitement limiter are gone: a sparse crowd can still deliver a strong native big-play reaction. Commentary and field sound keep their normal direct volume.

**Much more reaction variety**

- 80 listening-tested sounds across 27 usable families now feed existing triggers: catches, gains, hits, takeaways, sacks, touchdowns, kicks, cheers, boos, disappointment, surprise and anticipation.
- Stronger variants are chosen for bigger moments and larger occupied crowds instead of using the loudest sound every time.
- More visitor equivalents get sound-only disappointment at home venues; neutral crowds can support either side. DGA cheers can overlap vanilla cheers, while repeated callbacks cannot replay the same event.
- Both offenses get pass-flight anticipation. Spins/jukes and ordinary broken tackles remain sound-only; qualified big plays can add fuller layers and shake.


## Shake, broadcast and fixes

**Rebalanced shake**

- Strength follows occupied seats after the crowd qualifies. Smaller venues get more noticeable effects, and the same stadium builds strength as it fills. Neutral-site crowd shake uses 75% strength for either team.
- Runs, scrambles and catch-and-runs build from 13 to 20 yards; kick/punt returns build from 30 to 40. Shake fades at the snap and blends into touchdowns instead of restarting abruptly.
- Confirmed five-yard losses no longer need a sack label to react. Improved fourth-down conversions, defensive fumble returns and turnover-on-downs reactions; own fumble pickups do not get takeaway shake.
- Overtime adds confirmed stop, field-goal and two-point coverage, steadier tension and a small shake boost. Earned final-play effects can finish into postgame.

**Broadcast and usability**

- Manual major-play replay handling waits for full-screen playback before requesting a separate inset. Slow Sim uses full-screen presentation without an inset request. Injuries, penalties and native recordings can still affect playback.
- Better saved-crowd restoration, Q1 arrival continuity and recorded attendance in replays.
- New Gameday Atmosphere tab: 37 plain-English options across Attendance, Soundscape and Broadcast. Fullness appears beside Resume.
- Optional 2020 Mode: empty stands, collective crowd/bands/drums/PA muted, quiet individual detail and a more open stadium sound.
- Dynasty and RTG have been tested, including adjustments for RTG's wider player camera. The revised peak formula is Dynasty-only.
- Post-whistle hitch fix, fewer repeated reads, carried-over sack fixes, cleaner reaction handoffs and readable logs alongside full technical logs. Updated for the October 30 Windows patch.

**Close the game and MMC before updating. Old settings reset once with a backup; no INI deletion needed.**
Download: to be added after the final build is approved for release.

## Remaining limits

The revised peak-attendance formula applies to fresh regular-season Dynasty launches; RTG retains the native peak. Mode testing does not establish every rare event or presentation route in every mode. No replay is guaranteed after every qualifying play, and the base game's repeated final-kneel postgame montage is not fixed. The final 2020 Mode individual-detail balance still needs a live check on this build. Custom stadium songs and new crowd animations are not included.
