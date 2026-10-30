# Dynamic Gameday Atmosphere — Update 3
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


## Update 3 — Crowd audio

**A new native-mixer system**

- Crowd volume now follows both how full the stadium is and how many mapped seats are occupied. A small full crowd and a large full crowd no longer get the same treatment.
- Sparse crowds lose the wall of continuous roar while keeping more collective reactions and individual fan detail. As people arrive, the sound grows smoothly; as they leave, it thins again.
- The vanilla-volume crossover varies with venue size: roughly 70% fullness in the smallest venue and 30% in the largest. Above that point, the crowd grows beyond vanilla. A packed largest venue reaches +30% audio signal; the smallest packed venue reaches +15%.
- At 25% fullness in the smallest venue, background sound is 50% of vanilla. Nonempty crowds have minimums of 40% background, 55% collective reactions and 95% individual detail. Zero attendance mutes the crowd. These are signal levels, not literal perceived-loudness percentages.
- Chants follow the background mix. The old separate chant cut and excitement limiter are gone: a sparse crowd can still deliver a strong native big-play reaction. Commentary and field sound keep their normal direct volume.

**Much more reaction variety**

- 80 listening-tested sounds across 27 usable families now feed existing triggers: catches, gains, hits, takeaways, sacks, touchdowns, kicks, cheers, boos, disappointment, surprise and anticipation.
- Stronger variants are chosen for bigger moments and larger occupied crowds instead of using the loudest sound every time.
- More visitor equivalents get sound-only disappointment at home venues; neutral crowds can support either side. DGA cheers can overlap vanilla cheers, while repeated callbacks cannot replay the same event.
- Both offenses get pass-flight anticipation. Spins/jukes and ordinary broken tackles remain sound-only; qualified big plays can add fuller layers and shake.


## Update 3 — Shake, broadcast and fixes

**Rebalanced shake**

- Strength follows occupied seats; neutral sites use 75%. Wide normal cameras no longer amplify it; RTG/player-camera amplification has its own Broadcast toggle, off by default. Native HFA camera motion yields during DGA shake.
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
Download: [add the official link when the final build is released]
