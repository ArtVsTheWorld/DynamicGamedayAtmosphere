# Dynamic Gameday Atmosphere — full feature list

By Ace · **Update 3 working build 2.0.19 — not publicly released** · College Football 27 for Windows · MMC

This list describes the active features at the default settings. Most effects have their own switch. A crowd sound request does not guarantee EA will play it, and a replay request does not guarantee a clip exists.

Three numbers matter:

- **Expected peak:** how many fans are expected to attend, compared with the stadium capacity.
- **Kickoff share:** how many of those expected fans are seated at the start.
- **Current fullness:** how many seats are actually filled now, after arrivals, breaks and departures.

For example, a 60% expected peak with a 55% kickoff share means about 33% of stadium seats are filled at kickoff. Shake qualification and the crowd mix follow actual seated fans.

## Attendance: the crowd for this game

### Attendance Profile

Settings → Gameday Atmosphere → Attendance offers two choices, in this order:

| Choice | Expected peak | Kickoff crowd |
| --- | --- | --- |
| Fuller Crowds | Adds 8 capacity percentage points to a fresh regular-season Dynasty target | Adds 7 points to the share of expected fans seated at kickoff |
| Default | The normal mod formula below | The normal arrival balance below |

**Default stays selected unless you change it.** For example, a 60% peak and 55% seated share become 68% and 62% with Fuller Crowds: about 42% of stadium seats filled at kickoff rather than 33%. Existing caps, weather deductions, school arrival groups and arrival timing still apply. A sellout is not guaranteed. The ordinary 70% kickoff ceiling and exceptional 80% ceiling remain.

Choose before launching a fresh game. Existing crowd plans and Midgame Saves keep their recorded kickoff plan; changing the setting does not refill a game in progress. Postseason peak attendance is unchanged. Play Now, RTG and launches without verified revised-attendance inputs keep their existing attendance behavior. Fan Patience is a separate setting for departures.

### Expected turnout

Fresh regular-season Dynasty games use a revised expected peak attendance, calculated during launch before seating is generated. It does not change attendance stored for simulated season games. Already loaded games, Midgame Saves, postseason, practice and unsupported inputs keep the game's existing peak rather than recalculating it. Play Now does not use the revised peak formula. Missing or invalid required inputs leave EA's result in place.

School support uses:

| Input | Weight |
| --- | ---: |
| Stadium Atmosphere | 35% |
| Team Prestige | 25% |
| Program Tradition | 20% |
| Brand Exposure | 10% |
| Conference Prestige | 5% |
| AP/media-poll rank | 5% |

The school is compared with the current save's FBS programs. That support standing maps to capacity-ratio trends from the 2025 FBS attendance data: about 25.76% at the low end, 47.12% at the bottom tenth, 66.15% at the bottom quarter, 87.86% at the median and 97.99% at the upper quarter. Stronger demand can reach capacity. These are calibration points, not a claim that annual reported attendance measures kickoff occupancy.

Team Prestige uses the game's evolving half-star rating. Stadium Atmosphere is counted once; toughest-place rank does not get a second weight. Campus Lifestyle and Championship Contender are not separate inputs. AP/media rank supplies the support component; ordinary game rankings still help classify matchups and upset chances.

| Game circumstance | Effect on expected peak |
| --- | --- |
| Season performance | Up to 6 capacity points either way, with less weight early in the season |
| Opponent and rivalry appeal | Bounded matchup adjustment; ordinary season-plus-appeal adjustment stays between −15 and +12 points |
| Sustained success or AP Top 25 | Extra demand for a convincing breakout season, including lower-support programs |
| Home opener | 10–25% relative demand boost |
| Stadium-local night kickoff, 7 PM or later | 5–15% relative demand boost |
| Senior day / verified last regular-season home game | 10% relative demand boost |
| Same-state schools | 5–10% relative demand boost; half the extra local bonus if already rivals |
| Two service academies | 5–15% relative demand boost |
| Combined special-game boosts | Multiply together, capped at a 40% combined increase |
| Known light / medium / heavy precipitation | −5 / −9 / −14 capacity points outdoors |
| Game-to-game variation | −3 to +3 capacity points |

Boosts and variation are fixed for the matchup, stadium, week and season; reloading does not reroll them. A 10% relative boost takes a 60% peak to 66%, not 70%. Fair-weather peak stays between 25% and 100% of capacity; precipitation can lower that minimum to 20%. Roofed venues avoid the rain deduction. An uncertain dynamic-weather forecast does not get treated as confirmed rain at launch. Actual weather still affects departures.

Night detection uses verified stadium-local kickoff time, rather than assuming every late Eastern broadcast is a night game. Regular neutral games average the two programs' support and form, apply shared matchup demand and omit the home-opener benefit. Swapping designated home and away sides gives the same turnout.

### Kickoff and first-quarter arrivals

Peak turnout and kickoff crowd are separate. If a stadium expects to fill 50% of its seats and 55% of those fans start seated, kickoff fullness is about 27.5%.

| Game | Seated at kickoff, as a share of expected fans | Arrival deadline |
| --- | --- | --- |
| Ordinary regular season | 50–60% baseline, adjusted for school support | Halfway through Q1 |
| Late-arriving home schools | 45% baseline | 75% of the way through Q1 |
| Big regular-season game | Modest opening bonuses; usually no more than 70% | 30% of the way through Q1, unless the late-school rule applies |
| Bowl / non-championship CFP | 70–80% | 20% of the way through Q1 |
| Conference / national championship | 80–90% | 20% of the way through Q1 |

Late-arriving homes are **UCLA, Miami (FL), Georgia Tech, Maryland and Utah**. Neutral games use normal arrival timing. Smaller home venues receive up to 8 extra kickoff percentage points for easier access, tapering to zero in the largest venues. Same-state opponents add 2 points, night adds 5, a home opener adds 1, a qualifying big game adds up to 3, and two Top 10 teams add 1. These additions share the normal 70% ceiling.

Exceptional regular-season games can open at 75–80% of expected peak. This requires at least 95% expected capacity, top-fifth school support, a night kickoff or home opener, and either two Top 10 teams or rivals both in the Top 25. Late-arriving home schools do not use this exception. **80% is the hard regular-season kickoff cap.**

Only weather can take the kickoff share below 40%, with a hard 35% minimum. Outdoor light/medium/heavy precipitation takes 3/5/8 kickoff points. Postseason openings stay within their protected bands.

A big regular-season game means **two Top 25 teams, or rivals with either team Top 25 or both Top 50**. A rivalry between two struggling teams can still increase attendance without getting big-game atmosphere shortcuts. The same classification is used for arrivals, regular-season patience, halftime protection and big-game reaction context.

Timings scale with quarter length. For 12-minute quarters, ordinary arrivals finish at 6:00 remaining, big games at 8:24, late-arriving schools at 3:00 and postseason at 9:36. Seat changes wait for a suitable break; a clock threshold does not make fans appear during live action. Turning arrivals off starts with the expected crowd, not an automatic sellout.

### Seating patterns

- Upper and more distant rows generally clear earlier. Midfield sidelines and useful viewing elevation keep fans longer; end zones tend to thin sooner.
- Small row-oriented group gaps and stragglers break up uniform thinning. The full-looking selection at 90% retained crowd and above is preserved.
- The pattern stays fixed for the game. It does not reshuffle at each update, on a camera turn or when watching a replay.
- Arrivals, departures and halftime breaks share the pattern. Replays use their recorded attendance; leaving replay restores the live crowd.
- Only seats populated in the game's generated baseline can fill. Permanent departures do not return. Freshly loaded crowd renderers inherit the correct attendance, including during postgame shots.
- Unverified seating layouts retain the simpler thinning fallback. Small sideline groups are excluded from the stadium fullness measurement.

### Permanent departures

Score, time, rankings, matchup, weather and the way the game developed determine patience. Winning home crowds generally stay longer. Fans leave in smaller steps at safe opportunities, rather than vanishing on a scoring play.

| General halftime situation | Permanent departure target, share of expected crowd |
| --- | ---: |
| Close big game | 1–3% |
| Close ordinary game | 3–5% |
| 17–24-point margin | 10–20% |
| 25–34-point margin | 20–30% |
| 35+ margin | 30–40% |
| Postseason | 0–5% |

Margins between these bands blend gradually. These are targets, not guaranteed cuts. Home wins, weather, protection, floors and available update windows can change the result. Home deficits above 16 can begin thinning before halftime. Moderate deficits stay gentler in Q3; substantial Q4 blowouts can catch up when too many fans remain for the score.

An underdog competing with a Top 25 opponent gets more patience, especially when leading a Top 10 team late. Recovering at least 14 points after trailing by 21 can pause new departures while the comeback is credible. A fading rally or too little time can end that protection. Possession, field position and remaining timeouts help when known. Neither an upset nor a comeback refills departed seats.

After halftime, regular-season night games add up to 20% departure pressure. Low scoring pace can add up to 15%; a projected 72+ combined points can reduce pressure by up to 10%. These changes build through Q3, reach full strength in Q4 and are reduced to a quarter of that effect in one-score games. Postseason keeps its own rules. These are pressure adjustments, not repeated percentages removed from the remaining crowd.

### Halftime breaks and returns

About 20–30% of fans still present can temporarily leave their seats for the concourse. Each game has a fixed, slightly different timetable: departures begin in the final 20–30% of Q2; most return in the first 15–25% of Q3, with a smaller later wave finishing 25–35% into Q3. Close big games and packed close games hold most of that exodus until halftime.

In ordinary home games down 18+, some seat-breakers may not return. This only applies when more than 80% of the original crowd is still at the stadium and excludes big games, neutral venues and postseason. About 20–25% of the seat-breakers can become permanent exits as the deficit grows; floors and quarter limits still apply. It reduces later returns rather than removing another visible group.

Permanent and temporary absences are tracked separately. Only temporary seats can return. Matching Midgame Saves keep the original timetable.

### Late-game exits

- A small permanent group can leave at the Q3/Q4 break: roughly 1 point of the original crowd in a close home loss, 2 at a 14-point deficit, 5 at 21 and 7 at 28+. Winning and postseason crowds get more patience.
- Home deficits of 17+ can accelerate traffic departures in the last third of Q4. Deficits of 28+ can start gently in the last half. With 12-minute quarters, these begin at 4:00 and 6:00 remaining.
- Less time, few timeouts and opponent possession increase urgency. Home possession near a score or a recent home score buys patience. Unknown possession/timeout information uses a patient fallback.
- Severe late home losses can share up to 10 extra departure points beyond the ordinary quarter cap. This is an allowance, not an automatic cut. Neutral venues and postseason do not use this home-loss exception.
- In outdoor rain/snow, a home lead above 21 weakens winning-fan protection during the last half of Q4, reaching full severity at 35+. It shares a limited extra allowance instead of stacking a separate score penalty.
- A late costly home interception or lost fumble can send up to 10 extra points home when recovery looks unlikely. Neutral and postseason games are excluded. A return touchdown uses the larger score/turnover group, not both.
- A final-two-minute opponent score can send 5 extra points home if the home team trails by 9+ with at most one timeout. PATs cannot repeat it. Neutral and postseason games are excluded.
- Q4 has a separate 2:00 checkpoint when quarters are longer than two minutes. Ordinary departures stop in overtime.

### Floors, caps and timing

| Minimum retained share of expected crowd | Dry | Rain / snow |
| --- | ---: | ---: |
| Ordinary game | 25% | 15% |
| CFP / conference championship | 60% | 50% |
| National championship | 80% | 70% |

A winning Top 25 home team also has a 35% floor; higher postseason floors win. Permanent exits additionally respect a **15% actual-capacity floor** when the mapped baseline or validated launch peak is available. These rules stop further departures; they do not refill already empty seats, force sparse opening arrivals upward or override original empty seats. Temporary breaks also respect the applicable retained-crowd floors.

Default permanent-departure limits are 45 original-crowd points per ordinary quarter, 20 in CFP/conference championships and 10 in the national championship, plus the narrow late-blowout allowances described above. A blocked allowance is not saved up to hit all at once later.

Attendance decisions follow game time even while the field is visible. Actual seat changes wait for play selection, breaks or verified offscreen simulation. Ordinary steps start at 4 points and can grow to 16 after a longer wait; already-earned permanent exits can catch up within the applicable limits. Pausing does not accumulate departures. Normal/Fast Super Sim follow simulated score and time; Slow Sim uses the visible-field update rules.

## Soundscape: crowd sound and reactions

### The occupied-seat mix

Sound uses measured fullness across the verified seating layout and the number of occupied mapped positions. It does not use just the current camera view, a percentage of original attendance or real-world advertised capacity. Duplicate renderers are counted once; missing measurements keep native sound until ready.

The native crowd mixer handles one synchronized mix for the Crowd bus and its seven children. Chants follow the background; there is no separate chant cut. Commentary, players, referees and field sounds keep their normal direct volume. The old crowd-excitement limiter and graph-volume reductions are removed.

| Example at default mix strength | Background / chants | Collective reactions | Individual detail |
| --- | ---: | ---: | ---: |
| Smallest reference layout, 25% full | 50% of vanilla signal | About 62.5% | About 95.8% |
| Smallest reference layout, 70% full | 100% | 100% | 100% |
| Smallest reference layout, packed | 115% | 115% | 115% |
| Largest reference layout, 30% full | 100% | 100% | 100% |
| Dense medium layout, 94% full / about 27,500 occupied positions | About 150% (+3.5 dB) | About 124% | About 124% |
| Largest reference layout, packed | About 158.5% (+4 dB) | 130% | 130% |

Nonempty crowds never request less than 40% background/chant signal, 55% collective reaction signal or 95% individual-detail signal. The curve is smooth. Exactly zero attendance mutes crowd sound. These are audio-signal ratios, not measured perceived-loudness percentages. Individual-detail audibility depends on native routing and source material.

The vanilla balance point runs from about 70% fullness in the smallest layout to 30% in the largest. Above it, crowds get fuller than vanilla. Background Beds, Chants and StudentSection gain an additional smooth lift above 80% fullness and 20,000 occupied positions, reaching +3.5 dB around 92% / 27,000 occupied positions. Above 45,000 occupied positions, the background ceiling eases toward +4 dB at 61,888. Reaction and individual-detail gains retain the +30% maximum. Sparse crowds keep their native big-play peaks rather than having excitement flattened to the background level. Crowd Mix Strength scales how much of this profile is used; 0 keeps native levels for nonempty stands.

### Which crowds get extra layers?

Simple earned cheers or disappointment can work below the layered-atmosphere requirement. Fuller effects—roars, shake and larger celebrations—need the right situation and enough occupied seats.

| Situation | Small reference layout, 6,060 positions | Large reference layout, 61,888 positions |
| --- | ---: | ---: |
| Ordinary packed crowd | 98% full | 60% full |
| Big game / opening drive / close critical down / goal-line tension | 90% | 55% |
| Overtime / late winning drive | 85% | 50% |

Requirements blend between sizes and stop at those endpoints. Regular-season night games lower admission requirements by 10% relative to daytime, never below 50% measured fullness. This does not change the separate packed-house label. Enough people must actually be seated; qualification never adds fans.

### Before the snap, opening and closing scenes

- Eligible play selection gets a roar and anticipation swell, usually without an extra cheer. Home and visitor offensive possessions can get buildup.
- Special pre-snap cheers are reserved for calls inside the opponent's 10, the first home offensive call, or the first call of a late drive that can win. Neutral sites allow either offense.
- Big games can begin from 50% actual fullness in the intro and early Q1. The usual requirement returns gradually through Q1.
- An ordinary game expecting at least 90% peak capacity can get gentler intro/first-possession energy from 55% fullness, or its lower normal gate. This ends with that possession and does not give the stronger hostile opening surge.
- A qualifying opening kickoff can carry its cheer, roar and swell through the kick, return and whistle. At a home venue, a visitor receiving the first offensive possession can get a hostile roar/swell welcome; neutral games allow each team's first Q1 offensive surge once.
- Sparse fresh openings can receive a modest sound-only welcome. Resuming, attaching late or simulating does not invent an opener.
- Ordinary sequences run about 40 real seconds including their fade. Snapping bridges sound into the play; shake fades instead of cutting off instantly. Ordinary layers weaken across 16–24-point score margins and stop starting at 24+.
- Consecutive useful home gains and conversions build drive excitement. Failures cool it. It halves over one eighth of a quarter of game time; scores, possession changes, halftime and simulation clear it. It strengthens an existing qualified situation rather than bypassing crowd requirements. Neutral games support either offense.
- Qualified home wins, or either winner at a neutral site, can retain closing atmosphere until the stats/highlights menu, with a two-minute safety limit. Earned winning eruptions take priority. Intro atmosphere has a five-minute safety limit.

### Upset Watch and late drives

From Q2 onward, a credible home underdog bid against a Top 25 visitor can qualify for extra sound at a lower threshold, never below 50% actual fullness. Rank gap, score and game stage shape the effect; either underdog can qualify at a neutral site. A deficit above 14 ends the route, with stricter late-game treatment. This does not change packed-house status or ordinary pre-snap camera rules.

In the final three minutes of Q4, a home offense tied or down by at most eight can build winning-drive anticipation inside the opponent's 40. The normal late-drive fullness requirement applies. Neutral sites allow either offense. Eligible overtime keeps tension through plays, kicks and two-point tries instead of going quiet between them.

### Live plays, skills and defensive results

- **Runs, scrambles and short catch-and-runs:** buildup starts at 11 total yards, targets its peak at 15, and requests one cheer at 20. Strength rises toward the distance target over up to about 0.8 seconds, so a late first sample does not jump straight to the peak. A tackle fades from the movement actually reached. A short catch needs actual running afterward. Backward cuts do not restart the reaction.
- **Long catches:** 20 yards at the catch can earn an immediate response. An immediate tackle gets a short impact; a running receiver keeps the live-play reaction. A short official-result confirmation window covers delayed completion data.
- **Pass anticipation:** either offense's qualifying airborne pass can get a fast swell. No pre-catch cheer, shake or separate volume bonus is added.
- **Kick and punt returns:** buildup starts at 30 return yards, peaks and cheers at 40.
- **Interceptions:** fresh native proof can react before the possession label catches up. Verified defensive fumble returns use the same continuous return treatment.
- **Forced fumbles:** fresh force proof can react on any down. A delayed-proof fallback requires opposing contact, carrier possession loss and new native fumble confirmation. Defensive recovery can supply one missed impact. Own-team pickups never get a takeaway shake or revive an old gain.
- **Defensive losses:** a confirmed loss of at least five yards, including a sack, can shake without waiting for a sack label. Shorter sacks retain sound and replay classification. Carried flags from prior plays, released passes, kneels and spikes cannot invent sacks.
- **Third/fourth-down stops:** verified knockout, breakup or hit-stick evidence can earn one confirmed stop impact. Contact and official-result callbacks cannot stack the same impact. A routine third-down incompletion alone does not earn shake in regulation. Overtime can use a generic confirmed stop.
- **Goal-line stands:** the dedicated stronger impact needs fourth down inside the five. The special high-leverage stop applies in Q4. Turnover on downs has its own paired cheer/impact.
- **Fourth-down conversions:** can react when the live ball reaches the first-down line, with one official-result fallback if missed. Later long gains or scores can still upgrade the reaction without repeating the same conversion impact.
- **Skills:** successful jukes/spins cheer without shake; ordinary broken tackles use sound/swell only. Confirmed trucks add a quick impact; stiff arms and hurdles add gentler accents. These use the stronger effect alongside an existing run buildup, without adding shake strengths or restarting the run. Hit sticks get immediate sound, but need a confirmed third/fourth-down stop for shake. Attempts alone do not count. Accents have a short spacing limit and a maximum of three per play.
- **Live showboating:** a fresh verified home-carrier running celebration builds slowly; visitor showboating gets boos. Both need layered atmosphere. Cached tags and post-whistle celebrations do not trigger it.
- **Kicks and tries:** close made field goals, opponent misses and blocked field goals get appropriate reactions. A game-saving block can use a touchdown-style profile. Overtime includes made/failed two-point tries and field goals; failed home results use disappointment. Safeties use a shorter touchdown-style response.

Positive shake favors home successes and defensive stops at home venues; either team can qualify at neutral sites. Visitor versions of existing triggers can add sound-only disappointment instead. Away takeaways promptly clear old positive home movement and its short skill-move swell. A skill accent cannot restart an existing run sound or shorten its fade. Neutral crowds avoid treating one team's success as a stadium-wide home disappointment. Clock plays, stale data, pauses and recorded replays do not start new live reactions.

### Reaction variety and overlapping cheers

Positive gains cannot also trigger a deep-incompletion response. A measured loss cannot become a first down because an old flag remained set. Routine results that settle more than 2.5 seconds after the whistle do not start new late reactions; scoring updates remain eligible.

If the native event-specific cheer queue drops a request, DGA makes one short, same-size generic cheer or disappointment attempt. Existing game voices keep playing. Registration is logged separately from audible playback.

The palette contains **80 listening-tested native selectors across 27 usable families**: positive and negative ballcarrier actions, catches, field goals, fumbles, incompletions, interceptions, sacks, receiving touchdowns, rushing touchdowns and yardage gains; positive big hits; generic cheers, disappointment, surprise, boos, applause-style cheers and anticipation. Negative big-hit IDs 13–15 and negative catch ID 23 are excluded after failed listening checks.

Higher IDs within a family are generally stronger. Selection considers the play's importance, occupied seats and fullness; it does not choose the loudest variant for every play. The remaining native variants in the auditioned families are used as variety for existing triggers, not as new play detectors.

DGA cheers may overlap native cheers. Matching native sounds and an already busy crowd do not deliberately suppress an intended DGA cheer. Same-event and ownership checks still prevent repeated callback dispatch. A discarded request can get one brief same-play recovery; a major play can have one delayed second accent. A new snap, pause or replay cancels stale work. EA still decides voice allocation; a sent request is not proof that a sound was heard.

One event can move from a live gain to a confirmed result to a score without restarting the same effect. A stronger score replaces the prior reaction. Ordinary successful-play responses can hold about 5–8 seconds and fade over about 7–9; continuous runs/returns use their whistle fade instead. Native scene interruptions may resume the remaining roar/swell on its original timer, without replaying the cheer.

### Major celebrations and postgame carry

Qualifying home moments, or either team's at a neutral site, can erupt for:

- The game's first touchdown within its first two plays, including an opening kickoff return, when tracked from the start.
- A last-gasp touchdown pass snapped in the last 15 seconds of Q2/Q4, with at least 30 air yards and a catch within five yards of the goal line.
- A go-ahead score in the final 30 seconds of Q4, or a verified winning score, including kicks and two-point tries.
- An overtime interception, defensive fumble recovery or winner.
- A game-sealing takeaway protecting a one-score lead in Q4's final minute, when remaining timeouts support that conclusion. More opponent timeouts require less time remaining; unknown timeouts are not guessed.

A 60+ yard touchdown in a one-score game receives the normal touchdown profile with added excitement; long distance alone does not cause an eruption. All celebrations retain their crowd and ownership requirements. There is no extra bus-volume multiplier stacked on the occupied-seat mix.

Ordinary eruptions last about 45 seconds; overtime winners/takeaways can last about 60. Winning eruptions can continue through postgame scenes until the stats/highlights menu. Other earned final-play reactions can finish their original fade into postgame too: ordinary effects stay bounded by their remaining fade and score effects by 11 seconds after final. Nothing restarts its cheer because the game ended. Pauses, interactive replay, ownership changes and final menus clean up the effects.

### 2020 Mode

Disabled by default. This option empties the stands and mutes collective crowd beds, chants, reactions, student-section sound, swells and scripted crowd audio. Both bands, drum cadence and the PA announcer are muted. Quiet individual Sweeteners remain at −3 dB as sideline flavor; they are existing fan samples, not newly identified player dialogue.

Outdoor ambience is lifted by 6 dB and the existing reverb return by 3 dB during unpaused field scenes for a more open sound. This uses the game's existing reverb, not a new room effect. Commentary, field, player and referee direct gains stay unchanged. Cannons and other stadium/music groups remain separate. If required crowd subgroups cannot be verified, a complete crowd mute is the fallback.

The normal attendance plan and history continue underneath. Switching off restores the current normal target and retires the owned audio changes. It does not rewrite the game's scheduled attendance or save. Fullness displays zero while active, and crowd-driven shake is suppressed. The mode works independently of the normal Attendance/Soundscape switches in supported stadium contexts. Its quiet individual-detail balance still needs a final live check on this release build.

### Fuller dense-crowd layers

Already-qualified atmosphere gets stronger roar support around 80% fullness and 20,000 occupied mapped positions. A second tier around 85% and 45,000 strengthens very large crowds further. Both blend in gradually. The first tier adds up to 20% to DGA layer intensity; the second adds another 12 percentage points, within the native intensity limit. These are layer-intensity changes. The background mixer uses the separate +3.5 to +4 dB dense-crowd ceiling described above; reaction buses retain their +30% ceiling. Dense crowds can sustain a supporting swell at lower excitement levels, while ordinary plays keep restrained cheer sizes. Major moments can use the strongest tested variants once at least 24,000 positions are occupied at 80% fullness. No new play triggers or extra cheer requests are added.

## Broadcast: shake and replays

### Strength follows the fans in the seats

Fullness and situation decide whether shake qualifies. Once admitted, occupied mapped positions determine strength on a smooth curve: about 0.80× at 6,060 occupied positions, 0.90× at 15,000, 0.99× at 30,000, 1.07× at 50,000 and 1.10× at 61,888. The curve continues below 6,060 and caps at 1.10×. Unknown layouts and zero occupancy add no crowd motion.

The same stadium strengthens as fans arrive and softens as they leave. Neutral-site crowd shake is **75% of normal for either team**. Qualifying overtime adds one 10% lift within the existing limits. Cannons keep their independent physical-blast setting.

| Effect | Default profile before crowd/view scaling |
| --- | --- |
| Ordinary play selection | 2.5–5% |
| Before a punt | 1.25–2.5% |
| Special lineup cheer | 4–6% |
| Close defensive third/fourth down | 7.5–10%; home offense 5.625–7.5% |
| Successful truck | 5–6%; quick 0.1-second rise, about 2 seconds total |
| Hurdle / stiff arm | 3–4%; gentler rise, about 2 seconds total |
| Long gain / return | 10–12% |
| Five-yard loss / fourth-down conversion | 8–10% |
| Qualified breakup / knockout / hit-stick stop | 6–8% |
| Turnover on downs | 10–12% |
| Interception / defensive fumble return | 12–14% |
| Forced fumble / fourth-down goal-line stand / Q4 high-leverage stop | 13–15% |
| Touchdown | 20% opening impact, 16% hold; ends by about 11 seconds |
| Major celebration | 30% opening impact, 22.5% hold; camera ends by about 16 seconds |
| Safety / clinching first down | 16% impact, 12% hold; shorter touchdown timing |
| Ordinary blocked field goal | 14% impact, 10% hold; game-saving Q4/OT blocks use 20/16% |

Pre-snap shake stays on gameplay views. Third down needs ten or fewer yards to go; fourth down needs an offensive go-for-it call. Field goals can get ordinary buildup; routine PATs and kickoffs do not. A qualifying fresh big-game opening kickoff has its own motion.

Shake eases in and softens on close-ups. By default, wide cameras never amplify the configured shake: view compensation is capped at 1×. The captured wide RTG/player-camera allowance remains available through the **Wide Player Camera Compensation** toggle in **Broadcast** (or `WidePlayerCameraCompensation=1` in the INI), which allows up to 4×. It defaults to 0. Field of view cannot reliably distinguish RTG from a wide normal camera, so this is an explicit opt-in, not automatic mode detection. Turn it off when returning to ordinary broadcast views. The game keeps its own camera angle and zoom. At the snap, lineup shake fades over 0.9 seconds while following the new live camera. Run/catch effects build into touchdown/eruption impact over about 300 ms without restarting the motion. The stronger effect wins rather than stacking amplitudes.

Runs, catch-and-runs and qualifying returns use a shared five-second whistle fade by default, adjustable from one to eight seconds. Play distance adds no extra time. Short impacts have their own roughly 2–3-second envelopes; a confirmed force has a sharper 35 ms onset. Camera cuts and timeout scenes keep the original timer. Recorded replays stay steady. Pause, interactive replay, new games and the game's Dynamic Camera Effects switch stop added shake.

Native Stadium Pulse camera motion follows DGA's crowd/context admission, with an additional advanced minimum-fullness safeguard. While a fresh DGA crowd-shake output is actually being rendered, native HFA camera motion yields instead of adding another effect. Its latest requested tier is remembered and restored when DGA stops. Mere eligibility or a prepared request does not take ownership. This changes only its camera channel: composure, the HFA meter, play art and gameplay advantages stay native. Setting the advanced filter minimum to zero preserves its opt-out. A cannon blast does not depend on the crowd gate.

### Stadium cannons

All nine native cannon variants use a separate adjustable impact, default 18%, range 0–30%, lasting 0.9 seconds. It works at any crowd fullness, respects camera settings and uses the stronger of cannon/crowd movement rather than adding them. Setting it to zero removes only the added cannon shake. A detected blast gets one numbered log entry with its variant and requested strength; this is not proof of visible output.

### Extra replays

The mod asks the game's own recording and presentation system for more opportunities:

- Big gains can get full-screen replays; the default yardage cutoff is 15.
- Routine runs and completions can get a small replay during play selection.
- Sacks and tackles for loss can get defensive highlights. Takeaways and touchdowns use major-play routing.
- In manual play, confirmed major routes wait for full-screen playback to finish before requesting a separate inset repeat. If the game passes straight to play calling without starting that full-screen sequence, DGA returns a native inset request it held instead of waiting until the next snap loses it. It cannot create a missing recording. The normal skip controls remain available.
- Slow Sim keeps native full-screen presentation and does not ask for an inset, because it has no play-call screen. Mode switches cancel pending inset work.
- Kneels and spikes do not receive routine replay promotion. A running native replay is not interrupted by an inset timeout fallback.

The game still controls recording availability and scene selection. Injuries, penalties, safeties, skips and special presentation can change or interrupt a sequence. Neither full-screen nor inset replay is guaranteed after every qualifying play. The reported base-game repeated kneel-down postgame montage is not fixed.

### Recorded crowds, saves and simulation

Instant Replay, automatic clips and the postgame highlights viewer use attendance recorded near the chosen play. Earlier highlights can show a fuller crowd. Automatic clips also use that recorded crowd for the volume mix; they do not invent old cheers or run new departure decisions. Exiting restores the live crowd and only seats controlled by the mod can return.

Midgame Saves restore recent matching crowd history using the teams, score, time and quarter length. Loading-only records cannot replace actual played history. Pausing records the visible crowd; arrivals hold that level until game time advances. Matching Q1 saves retain peak, kickoff share, arrival deadline and local night status. Otherwise, the mod estimates from the resumed game and says so in the logs. It cannot recreate missing earlier play history exactly.

History keeps the last eight games in the DGA folder. New games and real scenario loads reset ownership correctly; a delayed save notice after Super Sim does not start a second crowd. Normal/Fast Super Sim update attendance from their score/clock; Slow Sim keeps field-visible timing. Returning to manual play clears obsolete presentation work.

## Settings, files and support

The **Gameday Atmosphere** tab appears after Audio Settings. It has 38 controls: **Attendance 12, Soundscape 12, Broadcast 14**. On/Off switches come first; numeric controls follow. Each has plain-language help. The pause menu can show Stadium Fullness beside Resume; a dash means the measurement is not ready.

The full option list follows below. Advanced settings remain in `Plugins/DynamicGamedayAtmosphere/DynamicGamedayAtmosphere.ini`, with defaults, accepted ranges and explanations. They cover arrival/traffic limits, comeback protection, halftime timing, atmosphere admission, reaction thresholds/durations, replay routing, camera profiles and logging. Retired excitement-limiter and independent chant-cut values do not affect the new mix. Gameplay HFA is not an adjustable DGA feature.

**Update 3 resets previous DGA settings once, including test-build settings.** The exact old INI is backed up before replacement. No manual deletion is needed. Later edits are kept. Game saves and native game settings are unaffected. Close both the game and MMC before replacing the DLL.

Important combinations:

- Attendance off leaves native attendance and disables attendance-dependent soundscape; extra replays stay separate. 2020 Mode has its own override.
- Crowd Sound Effects off at launch skips DGA audio hooks. Restart the game/MMC after changing this master switch.
- Extra Crowd Energy off disables the added layer system; the independently configured occupied-seat volume mix can remain.
- Fans Leaving Early at zero disables formula pressure, not independently configured late-score exits or temporary halftime breaks.
- Added Camera Shake off stops DGA motion. The game's Dynamic Camera Effects must also be on for shake to appear.
- The advanced MaximumBoostPercent zero also disables added layers/motion; its legacy higher values do not stack a second gain on the native-bus mix.

### Logging and performance

All owned data stays under `Plugins/DynamicGamedayAtmosphere/`: settings, Logs, Backups, History, runtime and tools. Only `DynamicGamedayAtmosphere.dll` belongs directly in Plugins. Recognized loose old DGA helpers are moved into its Backups; unrelated plugin files are left alone.

| File in Logs | Contents |
| --- | --- |
| `status.txt` | Readable activation, matchup, crowd, sound, shake and replay summary |
| `DynamicGamedayAtmosphere.log` | Readable events and warnings |
| `technical-status.txt` | Full settings, bindings, proofs and timing counters |
| `technical.log` | Detailed native events, IDs, result proof, audio receipts and replay stages |
| `launcher.log` | MMC startup/loading errors and recovery |

Technical details remain available under normal logging; Debug adds extra traces. Event logs have separate size limits, about 2 MB each by default. Status files are replaced rather than appended. Disk writes and readable formatting run on the background worker. Timings report callback wall time, not FPS or whole-plugin CPU usage.

Repeated replay sampling and expression reads at the whistle were reduced after live OFF/ON testing identified the hitch. Disabled replay additions forward directly to native behavior. Repeated crowd part lookups are reused within a single crowd pass, seat distances are prepared before sorting, and unchanged targets avoid redundant work. Actual seating changes can still take time; there is no universal zero-hitch or FPS guarantee.

The plugin checks native layouts before activating. Launcher failures identify the failed operation and available Windows error details; repeated unchanged failures back off instead of retrying every second. Runtime loading is bounded to the game process/start identity. Logs distinguish a sound request from native voice registration; neither alone proves audible output or rendered shake. No logs upload automatically.

The included local diagnostics tool offers bounded sound/camera previews and a read-only compatibility report. Previews help check output but do not prove automatic play detection, change attendance or alter gameplay HFA. Reports remain local until you choose to share them.

### Compatibility and limits

Live testing includes offline Dynasty and Road to Glory, including the default RTG player-camera capture and shake adjustments. The revised peak-attendance formula remains Dynasty-only. The Windows title update inspected October 30, 2026 is supported. Testing does not establish every rare play or presentation route in both modes; other modes and future title updates need their own compatibility checks. The plugin does not patch the game EXE on disk or ship game files.

Custom stadium songs, new crowd animation behavior, spectacular-catch detection and a replacement postgame highlight picker are not included. Seat desirability is inferred from geometry, not ticket prices. Quiet individual-detail gain is limited by native routing. Replay requests cannot guarantee the game's final presentation.

For a report, send the whole Logs folder and note the teams, quarter/clock, play result and what sounded or looked wrong. Do not send only a request line as evidence that the effect played.

## In-game option reference


### Attendance options

| Option | What it does |
| --- | --- |
| Dynamic Attendance | Let fans arrive after kickoff, take halftime breaks and leave early. Turn this off to keep the game's normal crowd. Extra replays have their own switch. |
| 2020 Mode | Play with empty stands. Mute the crowd, bands, drums and PA announcer, but keep quiet individual voices and a more open stadium sound. Turn off to bring back normal attendance and sound. |
| Fans Arriving After Kickoff | Start with some fans still outside, then fill their seats during the first quarter. Turn off to start with the full expected crowd. This does not guarantee a sellout. |
| Beat the Traffic | Let fans leave sooner when the home team is far behind late in the game. With 12-minute quarters, this starts at 4:00 in Q4 when down 17+, or gently at 6:00 when down 28+. |
| Attendance Profile | Default keeps the normal mod balance. Fuller Crowds raises the expected peak by 8 percentage points in fresh regular-season Dynasty games and seats 7 more of every 100 expected fans at kickoff in those games. Existing caps and weather rules still apply. Choose before loading a fresh game. Postseason crowds keep their own rules. |
| Fan Patience | Automatic follows the matchup and weather. Normal uses standard departures. Patient Fans makes people stay longer. Bad Weather makes people leave sooner. This does not change the starting crowd. |
| Ordinary Kickoff Crowd (%) | How much of the expected crowd starts in its seats. At 55, about 55 of every 100 expected fans are seated at kickoff. Smaller stadiums and special games can start fuller. Higher means more fans at kickoff. Applies to fresh games. |
| Late-Arriving Schools: Kickoff (%) | Kickoff crowd for UCLA, Miami (FL), Georgia Tech, Maryland and Utah. At 45, about 45 of every 100 expected fans start seated. Higher means more fans at kickoff. Night games and weather can change it. Applies to fresh games. |
| Big Game Kickoff Bonus (points) | Add up to 3 points to the seated share at kickoff. Big games mean two Top 25 teams, or rivals with either team Top 25 or both Top 50. Most regular games stay at 70% of expected fans or less; exceptional games can reach 80%. |
| When Arrivals Finish (% of Q1) | How far into Q1 ordinary schools finish filling up. At 50, fans finish halfway through the quarter. Higher means later arrivals. Big games, late-arriving schools and postseason games keep their own timing. Applies to fresh games. |
| Fans Leaving Early (%) | How strongly the score, time and matchup make fans leave. 100 is normal, 50 is gentler and 200 is stronger. At 0, these departures stop. Halftime breaks and special late-game exits have their own settings. |
| Halftime Seat Breaks (%) | How many fans temporarily leave their seats around halftime. 100 is normal; higher leaves more empty seats and lower leaves fewer. At 0, there are no temporary breaks. Fans who stay at the stadium return early in Q3. |

### Soundscape options

| Option | What it does |
| --- | --- |
| Crowd Sound Effects | Adjust crowd sound to the number of fans present and add reactions to plays. Turn off to keep the game's normal sound. Restart the game and MMC after changing this switch. 2020 Mode controls its own audio. |
| Extra Crowd Energy | Add roars, buildup and stronger reactions when the game and crowd qualify. Sparse crowds can still get simple cheers or disappointment. Bigger effects and shake need enough seats filled. Turn off to stop these extra layers. |
| Opening Atmosphere | Add energy during the stadium intro and early Q1. Big games can qualify from 50% of seats filled. A normal game expecting a near-sellout can get gentler first-drive energy from 55%. Other crowd requirements return as the game progresses. |
| Play Selection Atmosphere | Build crowd noise and anticipation while you choose a play, and add responses to successful plays. Special pre-snap cheers favor the home offense. At neutral sites, either team can get them. |
| Overtime Atmosphere | Keep a qualifying overtime crowd lively between plays, kicks and two-point tries. Important results get a reaction, and crowd shake gets a small 10% lift. The crowd still needs enough seats filled. |
| Upset Watch Sounds | Build excitement from Q2 onward when a home underdog threatens a Top 25 visitor. At least half the seats must be filled. At neutral sites, either underdog can qualify. |
| Live Play Reactions | Let the crowd react during catches, runs, sacks, takeaways and returns. Bigger plays and larger crowds can use stronger sounds. Sparse crowds can still get sounds, but shake needs the normal crowd requirements. |
| Skill Move and Big Hit Reactions | React to successful moves and big hits. Trucks add a quick shake; hurdles and stiff arms add a gentler one. Spins, jukes and ordinary broken tackles add sound only. Hit sticks need a confirmed third- or fourth-down stop for shake. |
| Buildup During Passes | Let the crowd build anticipation while a qualifying pass is in the air, then settle after the catch or incompletion. Does not add shake before the catch. Live Play Reactions must be on. |
| Cheers and Disappointment | Cheer defensive plays and add disappointment for home mistakes or visitor successes. Visitor versions add sound without positive home-team shake. At neutral sites, either team can get cheers without treating the whole crowd as home fans. |
| Major Celebrations | Use the biggest celebration for special moments, such as a very early touchdown, a late winning score, an overtime winner or a game-sealing takeaway. A long touchdown alone gets the normal touchdown celebration. |
| Crowd Volume Adjustment (%) | How much the mod changes crowd volume for the fans present. 100 uses the full mix: quieter sparse crowds and louder full crowds. Lower stays closer to the game's normal volume; 0 keeps normal volume unless the stands are empty. Does not reduce the game's own excitement. |

### Broadcast options

| Option | What it does |
| --- | --- |
| Show Fullness in Pause Menu | Show the percentage of seats filled beside Resume in the pause menu. It updates when you open the menu. A dash means the crowd measurement is not ready yet. |
| Added Camera Shake | Allow camera shake added by this mod. Turn off to stop all of the mod's added shake. The game's Dynamic Camera Effects setting must also be on. |
| Wide Player Camera Compensation | Make shake more noticeable on very wide RTG or player-lock cameras. Leave Off for normal gameplay cameras: On can make shake several times stronger. Turn it off again when returning to a normal camera. Added Camera Shake must be on. |
| Additional Replays | Ask the game for more replays after plays. Turn off to stop these extra requests. The game can still show its own replays. |
| Full-Screen Big Play Replays | Ask for full-screen replays after big plays. The game still chooses what it can show, and you can still skip replays normally. Additional Replays must be on. |
| Small Replays During Play Selection | Allow small replay windows while you choose a play after ordinary runs and completions. Slow Sim has no play-selection screen, so it gets no added small window. Additional Replays must be on. |
| Defensive Play Replays | Allow extra replays after sacks and tackles for loss, along with interception and lost-fumble highlights. Additional Replays must be on. |
| Full-Screen Replay: Minimum Yards | How many yards a gain needs for an extra full-screen replay chance. Higher saves it for longer plays; lower includes shorter plays. The game still decides whether a replay appears. This does not set the touchdown or takeaway rules. |
| Normal Play Selection Shake (%) | Maximum shake during ordinary play selection. Higher is stronger; lower is gentler. At 0, this type is off. Buildup before a punt uses half this strength. A qualifying crowd is still needed. |
| Important Down Shake (%) | Maximum pre-snap shake on qualifying third and fourth downs in close games. Higher is stronger; 0 turns this type off. Home-offense buildup is 25% gentler than when the home crowd is defending. A qualifying crowd is still needed. |
| Live Play Shake (%) | Strength of shake for gains, takeaways and important stops. Higher is stronger; 0 turns it off. Big events get more shake than routine ones. Kick and punt returns build from 30 to 40 yards. Picking up your own fumble adds no shake. |
| Touchdown Shake (%) | Shake level held after the first touchdown impact. Higher is stronger; lower is gentler. At 0, touchdown shake is off. An active run or catch reaction builds smoothly into the celebration. |
| Major Celebration Shake (%) | Shake level held after the first impact of a major celebration, such as a winning play. Higher is stronger; 0 turns this type off. An active play reaction builds smoothly into it. |
| Stadium Cannon Shake (%) | Shake when a stadium cannon fires. Higher is stronger; 0 turns off its shake but keeps the cannon sound. Works even in sparse crowds. Added Camera Shake and the game's Dynamic Camera Effects must be on. |
