# Dynamic Gameday Atmosphere

By Ace | Update 3 candidate | Working build 1.93.0 | Offline CFB 27

One plugin, organized into Attendance, Soundscape and Broadcast settings.

The crowd fills in, reacts to the matchup and thins as the game gets out of reach. Play calls bring a buildup of crowd noise and pre-snap camera shake. Stadium fullness controls volume. Big games, packed ordinary stadiums, close critical downs, scoring threats, overtime and late scoring drives can get layered buildup. Winning moments receive longer celebrations. More plays also get the game's own replay presentation.

## Dynamic Gameday Attendance

Candidate 1.91 scales added crowd-camera targets by the complete authored seating layout: 0.35x at 6,060 positions, rising linearly to 1.10x at 61,888, clamped at either end. Fullness still decides eligibility; venue size decides the strength multiplier once admitted. Neutral crowds retain their further 0.75x factor for either team. Native cannon impacts and EA HFA motion keep their separate policies. Fresh earned final-play reactions can finish into postgame without restarting their cheer or camera phase: ordinary tails are bounded by the configured fade, scoring tails by 11 seconds after final, with earlier original expiries preserved. Eruptions retain priority and their existing longer carry. Menu entry, ownership changes and interruptions clean up the carry. See the [review](docs/reviews/Update3-1.91.md) for fixes, simulation coverage and remaining live checks.

### What the crowd percentages mean

Attendance targets and permanent-departure percentages are based on the chosen starting turnout: DGA's revised count on a qualifying regular-season launch, or EA's count otherwise. If the game chose 40,000 fans, a 50% target means about 20,000 of those fans, even if the stadium holds 80,000. Temporary halftime break percentages refer to the fans still present.

**Sound follows the occupied share of the mapped stands.** The mod records how many crowd positions the game originally filled, then applies live arrivals, departures and temporary seat breaks to that baseline. If the game filled 90% of its mapped seating and 80% of that crowd remains, audio uses about 72% fullness.

This uses the whole verified seating layout, not just the fans visible to the current camera. Rotating the camera, changing the view or watching an earlier highlight does not change the live audio baseline. Replacement crowd renderers keep the original empty seats. A delayed Midgame Save announcement after Super Sim does not start a new crowd; actual scenario loads and new games still reset normally. Tiny groups beside the field are excluded, and duplicate renderers of the same seating layout are counted once. The game uses fewer crowd models than real seats, so this is a measure of visual density rather than a literal headcount.

Soundscape additions wait until the stadium's seating layout is verified. During that wait, the game keeps its own sound. Original-attendance percentages and the stadium's listed real-world capacity do not substitute for measured seating fullness.

The launch chooses expected turnout once. Arrivals, halftime breaks and departures then control how much of that chosen crowd is present. Attendance rules apply to the stadium crowd as a whole.

Fans who permanently leave stay gone. First-quarter arrivals, temporary halftime seat breaks and viewing an earlier replay are separate from permanent departures.

### Starting turnout - first-pass candidate

Fresh regular-season **offline Dynasty** launches choose expected peak turnout before native match conditions and seating are generated. The original calculation still returns EA's result to saved and simulated season-game records; only the playable launch count is replaced. Play Now is not enabled. Postseason, conference championships, practice/preseason, resumed games and unknown inputs keep EA peak turnout.

School demand uses actual My School grades: **40% stadium atmosphere, 20% program tradition, 15% brand exposure, 15% current team rank and 10% conference prestige**. Campus lifestyle and championship contender are not inputs. Grades are normalized from A+=0 through F=12; Incomplete is rejected. Team rank is normalized within the valid FBS cohort. Upgrade program points are not used as grades. Invalid/missing grade or cohort data preserves EA turnout.

The playable-launch observation reads the cohort once, excluding stock FCS and placeholders. Schools receive a demand percentile relative to that cohort; equal scores get equal demand, independently of team IDs or array order. The curve follows aggregate capacity-ratio quantiles from the user-supplied **2025 FBS Attendance Trends** table: low end 25.76%, bottom tenth 47.12%, bottom quarter 66.15%, median 87.86%, upper quarter 97.99%, upper fifth 100% or greater. Reported annual attendance is a calibration proxy, not a measurement of kickoff crowd or timed peak occupancy. Excess demand above 100% supplies headroom; rendered attendance never exceeds capacity.

| Input | Peak-turnout adjustment (capacity percentage points) |
| --- | --- |
| Season performance | -6 to +6, reduced through the first six games |
| Top25 visitor | About +2 to +6 |
| Visitor ranked below 100 / stock FCS | -6 / -10 |
| Rivalry / home opener | +6 / +3 |
| Night kickoff | +4 at or after **19:00 stadium-local time** |
| Combined season/matchup/night adjustment | -15 to +12 maximum |
| Light / medium / heavy precipitation | -5 / -9 / -14; fully roofed venues exempt |
| Fixture variation | -3 to +3, fixed by teams, stadium, week and season |

Expected peak is bounded to **25-100% of capacity in fair weather**, with a **20% minimum when outdoor precipitation reduces turnout**. Regular neutral games average both programs' support standing, form and opponent appeal, with no home-opener bonus. Swapping designated sides gives the same result.

The turnout callback runs at the shared pre-finalization launch point for ordinary games and rivalries. It no longer depends on optional rivalry publication, which could bypass the override on 1.83. Missing final options produce an explicit fallback entry naming the failed check. A recently verified Dynasty setup can bridge the game's brief unknown-mode loading transition. The input capture and count write must still match the actual teams, stadium and native count; expired or different-mode inputs are rejected. Capacity comes from the already captured stadium record for that exact launch; startup does not query the gameplay broadcast capacity service. Missing or invalid captured capacity leaves EA turnout unchanged. MMC handles online availability, so starting turnout has no duplicate online/configuration checks. Attendance tuning is unchanged from 1.83.

The normal `[starting-attendance]` log gives EA/DGA counts, FBS cohort size, actual grades, support percentile, adjustments, local kickoff, the fraction seated at kickoff and the Q1 fill deadline. `InitialAttendanceEnabled=0` restores EA peak turnout for future fresh launches; approved arrival behavior remains available. The 35-control menu is unchanged. An already loaded stadium is not rebuilt.

`Tools/CompareStartingAttendance.cpp` emits `StartingAttendanceExamples.csv`, comparing EA, previous 1.81 and current policy. Its optional numeric cohort input compares every FBS pairing using the real compiled policy. Examples are hypothetical, not forecasts for the current save. Rendered seating, streaming, loading and replay/resume still require live acceptance.

Illustrative fixtures with a 138-school field; percentages below are of stadium capacity. The support percentiles are explicit hypothetical inputs, not inferred grades for named real schools. Fixture variation remains active.

| Example | EA peak | 1.81 peak | 1.83 peak | 1.83 kickoff |
| --- | ---: | ---: | ---: | ---: |
| Middle-support ordinary | 66.8% | 74.4% | 85.9% | 47.2% |
| Same matchup - night | 66.8% | 81.4% | 89.9% | 53.9% |
| Same matchup - heavy rain | 66.8% | 60.4% | 71.9% | 33.8% |
| Middle-support ranked rivalry | 100.0% | 89.4% | 96.4% | 55.9% |
| Struggling bottom-five-percent support | 30.0% | 47.5% | 30.3% | 15.4% |
| Weakest support | 30.0% | 35.0% | 25.0% | 12.5% |
| Elite support ordinary | 100.0% | 100.0% | 100.0% | 60.0% |
| Utah severe-late arrival | 100.0% | 100.0% | 100.0% | 45.0% |
| Exceptional Top10 opener rivalry night | 100.0% | 100.0% | 100.0% | 80.0% |

A private regression comparison used all 18,906 distinct pairings of the provided save's 138 FBS programs, with current records, clear daytime weather, no rivalry or opener bonus and a fixed test venue. Peak turnout had a median of 86.16%, 21.89% at full capacity and a 25% minimum; 1.81 yielded 74.99%, 3.45% and 33.42% respectively. This checks the aggregate distribution; it is not a forecast for the actual schedule or rendered-seat proof.

### Where fans stay and leave

Thinning follows the seating layout. Seats farther from the field generally empty sooner, while closer seats keep more fans. Density tapers gradually across the rows. Areas around the stadium fill and empty at different rates, with stronger differences between fuller and thinner areas, including the closer seating. Small groups and individual stragglers remain among the emptier seats. The variation stays fixed for the game, so it does not reshuffle on each update or when viewing a replay.

This changes where the remaining fans sit, not how many the attendance rules allow. After native seating is generated from the chosen turnout, live thinning never fills seats that were empty in that baseline.

Arrivals, permanent departures and temporary halftime absences share the same seating pattern. Earlier replays use that pattern with the attendance recorded for the play; leaving the replay restores the live crowd. Resumed games rebuild the pattern from the stadium's seats and use the restored or estimated attendance.

The pattern is prepared during pregame and stays in place for postgame stadium shots. If a cutscene loads a fresh crowd renderer, it receives the same final attendance. Automatic highlights can still show a fuller crowd when replaying an earlier part of the game.

Using Midgame Save to record a normal game preserves the smaller crowd prepared during pregame. Loading a saved scenario still restores or estimates the crowd for that scenario.

If the mod cannot verify a crowd's seating layout, that crowd keeps the previous scattered thinning. Small groups of people beside the field also keep their usual treatment.

### First-quarter arrivals

Opening fractions are percentages of **expected peak crowd**, not stadium capacity. A 50%-capacity peak with 55% of its fans seated starts at 27.5% capacity. Arrivals reveal only seats the native launch populated for that peak, and never revive permanent departures.

| Opening route | Initial fraction of peak | Finish point, 12-minute Q1 |
| --- | --- | --- |
| Ordinary regular season | 50-60% by support; night adds 5, giving 55-65% | 6:00 remaining |
| Severe-late home | 45% day / 50% night before matchup/weather adjustments | 3:00 remaining |
| Marquee regular season | +3 points; opener +1, night +5, both Top10 +1; usual ceiling 70%, exceptional 75-80% | 8:24 remaining, or severe-late deadline |
| Bowl / non-championship CFP | 70-80% | 9:36 remaining |
| Conference / national championship | 80-90% | 9:36 remaining |

Severe-late homes are **UCLA, Miami (FL), Georgia Tech, Maryland and Utah**. Neutral games use standard arrivals. Timings scale with quarter length: normal 50% of Q1, marquee 30%, severe 75%, postseason 20%. Seat changes still wait for hidden breaks, play selection or verified offscreen simulation.

Revised regular-season precipitation delays are 3/5/8 points. **Only weather can take kickoff below 40%; the hard minimum is 35%.** Indoor venues are exempt. Postseason weather moves openings within protected 70-80% or 80-90% bands. The exceptional 75-80% band requires expected peak of at least 95% of capacity, support in the top fifth, a night kickoff or non-neutral home opener, and either two Top10 teams or a rivalry between two Top25 teams. Severe-late home schools keep their later arrival pattern and cannot use this band. The 80% cap is a share of peak, not a target for all big games. Advanced baseline/bonus settings feed the same fresh-launch policy; when grade-based turnout is unavailable, the conservative fallback cannot unlock the exceptional band. Retired BigGameStartPercent and FcsOpponentPenalty keys no longer affect arrivals.

A regular atmosphere big game is two Top25 teams, or a rivalry with **either team Top25 or both Top50**. A #135-#137 rivalry gets no shortcut. Close marquee rivalries now receive the same modest halftime patience as close two-Top25 games; blowouts retain their existing pressure. Sounds follow actual measured seating, and opening/layered shakes still require existing density and camera-effects gates. A qualifying game can begin too sparsely for shake; no delayed opener is manufactured when seats later fill.

Normal/Fast Super Sim follow simulated score and clock while their play-by-play screen is open. Slow Sim keeps hidden-window timing while the field is visible. Earlier replays use their recorded crowd; exiting restores the live crowd. The mod-owned history now preserves peak/capacity, kickoff fraction, Q1 deadline, local night flag and fixture identity. Matched Q1 saves continue their original arrival plan; later saves do not restart arrivals. Older history files remain readable and use existing estimation when the new metadata is absent. This does not rewrite native saved or simulated attendance.

### How fans decide to leave

Departures use a formula that follows the score, time remaining, rankings, matchup, weather and home team's fortunes. It also remembers how the game unfolded. A lead that suddenly grows late in the game produces different attendance from a blowout that has lasted all afternoon.

**Close games keep more people interested.** Routine departures remain modest when the result is still in doubt. Games between two Top 25 teams get extra patience. Both teams must be ranked for the ranked-matchup benefit.

**Home wins get more patience.** Fans are more willing to stay for a comfortable home victory than an equally large home deficit. This benefit becomes smaller as the opponent gets weaker, but some basic home-win patience remains even against FCS opponents. Comfortable wins against similarly ranked teams get more patience than wins against vastly weaker opponents.

**Ranking gaps matter throughout the rankings.** No. 1 against No. 50 differs from No. 1 against No. 127. A gap near the top of the rankings also has more weight than the same numerical gap farther down. Larger mismatches increase departure pressure when the favorite is winning. The game supplies the rankings; missing FBS rankings are not invented. An FCS opponent without a ranking is treated as below the supported FBS ranks.

**Weather encourages earlier exits.** Rain and snow increase ordinary departure pressure. Ranked-matchup patience generally takes priority over the usual weather multiplier, although that patience fades against a vastly weaker opponent. Weather can still lower the minimum crowd targets listed below.

### Halftime is the main departure window

Departures build across Q2 toward situation-based halftime targets, instead of forcing every ordinary game to lose at least 10%. A close defensive game receives the same basic consideration as a close high-scoring game. Smaller groups can continue leaving in Q3 and Q4.

| Halftime situation | Permanent departures: share of original crowd | Temporary seat breaks: share of fans still present |
| --- | --- | --- |
| Both Top25, or a marquee rivalry, and margin at most 8 | 1-3% | 20-30%, mostly at the halftime break |
| Close ordinary game | 3-5% | 20-30% |
| Margin of 17-24 points | 10-20% | 20-30% |
| Margin of 25-34 points | 20-30% | 20-30% |
| Margin of 35 or more | 30-40% | 20-30% |
| Playoff or championship | 0-5% | 20-30% |

These are general targets; comparable-opponent home wins can lose fewer people. They are not a guaranteed cut at one moment. A margin of 9-16 points bridges the close-game and blowout targets. Home wins, weather, protection, the way the score developed, crowd floors and safe update opportunities affect the result. A small amount of game-to-game variation keeps identical scorelines from always producing identical crowds; it stays fixed throughout the game and survives a matching saved-game resume.

Moderate home deficits are treated more gently during Q3, with departures building faster in Q4. The change from a 21-point game to a 24-point game is gentler, with stronger exits building across a wider range of margins. Large blowouts still generate stronger pressure.

**Home routs start thinning earlier.** A home deficit of more than 16 points can start earning additional departures before halftime. Pressure rises smoothly with the deficit and time played, with a response of roughly 90 game seconds for 12-minute quarters. It counts fans already gone and never charges the earlier competitive part of the game. Home victories, neutral venues and postseason games keep their own treatment. Existing floors, comeback protection, quarter caps and safe update windows still apply. These earlier departures can leave a slightly smaller crowd at halftime than the general target bands above.

**Late blowouts can catch up.** In regular-season Q4, extra pressure gradually comes in across margins of 17-24 points when attendance is still high for the score. Larger margins and less time remaining raise that pressure. It builds gradually rather than removing a fixed batch when a touchdown is scored; with 12-minute quarters, it responds over roughly 90 seconds of game time. Other quarter lengths scale with it.

Home-winning patience, matchup and weather adjustments, upset and comeback protection, crowd floors and quarter limits still apply. Fans already gone are counted, so a crowd that has thinned enough does not get another catch-up cut. Nothing is added for the earlier close part of the game. Actual changes still wait for play selection, breaks or verified offscreen Super Sim. Playoff and championship games keep their existing departure rules.

### Upsets and comebacks

An underdog staying competitive with a Top 25 opponent can slow departures. The benefit develops as the game progresses, with the strongest stage for an underdog leading a Top 10 opponent in Q4. A larger ranking gap creates a stronger upset effect.

Recovering at least 14 points after falling 21 behind can pause departures while the comeback remains alive. The protection can end if the rally deteriorates or the team builds a comfortable lead. A multi-score deficit with too little time for a realistic recovery can also end protection, even with home possession. Remaining timeouts, possession and field position help decide this when the game provides them. Special home retention benefits fade against vastly inferior opponents.

These protections keep additional fans from leaving; they do not bring departed fans back.

### Minimum crowd targets

| Lowest permanent-departure target | Dry conditions | Rain or snow |
| --- | --- | --- |
| Ordinary game | 25% | 15% |
| Playoff / conference championship | 60% | 50% |
| National championship | 80% | 70% |

A winning Top 25 home team also has a 35% floor, including against FCS opponents and in bad weather. A higher postseason floor still wins. If more fans left earlier while the team was losing, taking the lead does not refill those seats. Neutral venues use their regular floors.

Playoff, conference championship and national championship crowds are more patient, but multi-score deficits can still produce departures. During Q4, fans increasingly try to beat traffic when the result looks settled. The effect grows with the lead and time elapsed in the quarter. Championship crowds remain more patient than playoff crowds, and close games do not receive this extra traffic effect. These floors limit further departures and halftime seat absences. They do not override the smaller opening crowd or the game's original empty seats.

### Protection against sudden huge drops

The standard permanent-departure cap in each quarter is 45 percentage points of the original crowd in ordinary games, 20 in playoffs and conference championships, and 10 in the national championship. For example, a 45-point cap permits a change from 90% to 45%; it is not a 45% cut applied repeatedly to whoever remains.

Late-score departures share that cap. Departures blocked by the cap are discarded rather than waiting to hit all at once later. Temporary halftime absences have their own limits.

Departure decisions follow the score and game clock even while the field is visible. The crowd itself waits for play selection, a break or verified offscreen Super Sim to change. Long stretches without opening play selection no longer erase departures or change the formula simply because fewer updates were possible.

Arrivals and halftime seat changes normally share an allowance starting at four points per opportunity, growing with time since the previous opportunity up to 16 points by default. When permanent departures have already built beyond that allowance, the next hidden opportunity can catch up to their calculated target. Quarter caps and crowd floors still apply, so this does not create another departure penalty. Quarter, two-minute and late-score calculations cannot repeatedly renew an opportunity's allowance. Initial pregame setup applies the opening target directly. Comeback protection stops new departure decisions; it does not undo decisions already made before the comeback. Offscreen Super Sim can catch up immediately while still respecting the quarter cap and crowd floor.

### Late-game exits and overtime

Q4 has one additional departure checkpoint at the next safe break after 2:00. It follows the same formula and protections. Quarters lasting two minutes or less skip this checkpoint.

A live opponent score inside the final two minutes can send another five percentage points home if the home team falls at least nine points behind with one timeout or none. Extra points do not repeat the penalty. This extra rule does not run at neutral sites or in postseason games.

Verified possession and remaining home timeouts also shape late Q4 pressure. Having the ball, especially near the end zone, buys some patience. Giving possession back to the opponent while multiple scores behind with few timeouts encourages exits. If that information is unavailable, the normal score-and-clock formula applies. Neutral venues receive extra overall patience because supporters of the winning team still have reason to stay; the mod does not assume it knows each section's allegiance.

Ordinary departures stop in overtime. The existing crowd remains rather than being treated as another regulation quarter.

### Late-Q4 traffic

Home deficits of 17+ can start faster traffic departures in the last third of Q4. Deficits of 28+ can begin gently in the last half, reaching 35% of the traffic activation by the last third, then ramping up. With 12-minute quarters, those starts are 4:00 and 6:00 remaining. Postseason and smaller-deficit traffic windows keep their previous timing. A larger deficit, less time, fewer timeouts and the opponent having possession make departures more urgent. Home possession, a nearby scoring opportunity and a recent home score add patience. That recent-scoring boost fades with game time, and an opponent score cancels it. Unknown possession or timeout data uses a patient fallback.

The system moves toward a crowd target, so it does not subtract another percentage every time it updates. For a home deficit of 17 or more, urgency builds over the opening tenth of this window and severe losses move more quickly toward the applicable crowd floor. Its response takes roughly 10â€“15 game seconds with 12-minute quarters, scaled for other lengths. Hidden seat-update windows still control when changes become visible. Fans who already left do not return after a score. Winning home crowds keep their existing patience. Postseason crowds receive a smaller traffic target only in clear blowouts; neutral-site games retain their symmetric rules.

A home team losing by 28 or more can use a limited extra Q4 allowance in that late window. At the default setting, up to ten additional points of the original crowd may leave beyond the normal 45-point quarter cap. The amount responds to the deficit, remaining clock and recovery outlook; it is an allowance, not an automatic crowd cut. All crowd floors and comeback protection still apply. It never applies to winning home teams, neutral venues or postseason games. Set LateTrafficExtraQuarterAllowance to 0 to keep the ordinary cap absolute; LateTrafficEnabled=0 disables both traffic acceleration and this exception. Status shows the current quarter budget, minimum remaining crowd allowed and whether the floor, quarter limit or comeback protection is holding departures back.

A verified late home interception or lost fumble can also send a group home when it sharply reduces the recovery outlook. This can include a one-score loss when the opponent can run out the clock with kneel-downs. The group is capped at ten original-crowd percentage points by default and runs once for the turnover. Hopeful comebacks stay protected; neutral and postseason games are excluded from this event group. The old late-score group remains. A return touchdown uses the larger of its score and turnover groups, without adding both together. Settings allow the continuous traffic acceleration and turnover group to be disabled separately.

### End-of-third-quarter departures

A small group can permanently leave at the Q3/Q4 break. In an ordinary game with the home team trailing, the group is about one percentage point in a close game, two at a 14-point deficit, five at 21, and seven at 28 or more. The curve between those examples is smooth. These are points of the original crowd, not a fresh percentage taken off every update. Winning home crowds and postseason games get more patience; comeback/upset protection, weather, rankings, existing crowd floors and the shared quarter cap still apply. This replaces the old small Q4-break adjustment and runs once. Fans leave at the break or another verified hidden update, including Super Sim. Loading directly into Q4 does not charge a second group.

### Temporary halftime seat breaks

In an ordinary regular-season home game that reaches halftime down 18 or more, some seat-breakers may decide not to return. This applies only while more than 80% of the original crowd is still at the stadium, counting seated fans and people on a temporary break. About 20% of the seat-breakers stay away at an 18-point deficit, rising smoothly toward 25% at 28 or more. The effect eases between 90% and 80% remaining and excludes big games, postseason and neutral sites. These seats are already empty, so the change reduces later returns rather than removing another group on screen. Crowd floors and quarter limits still apply.

Some fans leave their seats for food, bathrooms and the concourse. At normal settings, the peak temporary absence is 20-30% of the fans still in the stadium, subject to the existing crowd floors. These are separate from people who permanently leave.

Each seeded game fixes its own halftime timetable. Temporary departures ramp over the last 20–30% of Q2. Most seat-breakers return during the first 15–25% of Q3, with a smaller 20% wave finishing 25–35% into Q3. With 12-minute quarters, departures start 3:36–2:24 before halftime, most returns finish 1:48–3:00 into Q3, and the later group finishes 3:00–4:12 into Q3. Unseeded legacy history keeps the former quarter-of-Q2/Q3 timetable. Changes happen at play selection, breaks and verified offscreen Super Sim updates. A missed window catches up at the next safe opportunity; fans never visibly refill during live action. Only temporarily withheld seats can return. In a one-score big game or packed stadium, only about one fifth of the usual early seat break develops before halftime; most of that group waits until the half ends. The main temporary exodus uses the safe halftime boundary. Timings scale with quarter length and survive matching saved-game resumes. This changes whole-crowd temporary timing; section-by-section variety is a separate future pass.

## Dynamic Gameday Soundscape

### Volume follows the fans

Sound uses measured occupied seating, including arrivals, departures and temporary seat breaks. It does not use the percentage of the game's original attendance or a listed real-world stadium capacity.

| Stadium fullness | Ordinary crowd volume | Chant volume |
| --- | --- | --- |
| 100% | +25% | +15% |
| 95% | +20% | +12% |
| 90% | +15% | +9% |
| 85% | +10% | +6% |
| 80% | +5% | +3% |
| 50-75% | Normal | Normal |
| 40% | Calmer crowd; 5% volume cut | 5% volume cut |
| 30% | Calmer crowd; 10% volume cut | 10% volume cut |
| 20% | Calmer crowd; 15% volume cut | 15% volume cut |
| 10% | Calmer crowd; 20% volume cut | 20% volume cut |
| 0% | Lowest excitement; 25% volume cut | 25% volume cut |

The volume cuts in this table apply only to venues with 12,000 or fewer mapped positions. Larger venues receive the calmer native crowd, with no added crowd-category volume cut. The boosts above 75% remain available at all stadium sizes.

Below 50% fullness, the mod selects calmer background ambience through the game's own excitement control. The background ceiling blends in progressively with empty seats and quieting strength, without switching fully on just below the threshold. Native big-play peaks progressively bypass that background ceiling while keeping the ordinary crowd-presence scaling; an empty stadium stays at its lowest excitement. This preserves the difference between a calmer bed and an earned cheer. The bed ceiling also rises with stadium size. These are mapped crowd positions, not a listed real-world seating capacity. Background changes blend over about six seconds.

Only venues with **12,000 or fewer verified mapped seating positions** receive the separate gentle crowd-volume cut. It starts below the same threshold and grows linearly to a maximum of 25% at zero fullness. This also reaches chants through the game's crowd category. It never mutes the category, compounds over time or changes the saved crowd-volume setting. Larger venues use native intensity quieting alone below 50% fullness. The cut can be reduced or disabled separately in the INI. Above 50% it adds no cut, and existing boost limits stay unchanged.

Native intensity changes crowd character rather than applying an exact loudness percentage. Quieting also applies during intros and postgame. A newly loaded presentation crowd starts at its measured level rather than briefly sounding full. Automatic replay clips use their recorded crowd fullness. Ordinary gameplay pauses and interactive replay return to native intensity and crowd-category volume. If a control is unavailable, that adjustment is skipped. Strength, starting fullness and the separate volume cut are adjustable in the INI. Zero intensity selects the lowest excitement; it is not a guaranteed mute of every crowd sound.

The boosts above 75% adjust the sound signal; perceived loudness depends on the sound and your settings. High-leverage moments can reach +35% total and eruptions +50% total. Boosts do not stack on top of those caps.

Full sound layers require a qualifying situation and its stadium-size-based fullness threshold. Below that gate, verified plays can receive one native cheer or disappointment sound, and skills/airborne passes can use short softer swells. This sound-only route adds no passive roar, follow-up cheer, shake, eruption or volume bonus. Full reactions and celebration movement retain their existing requirements. Native reactions, fullness-based volume and sparse-crowd quieting remain separate.

### Upset Watch sound

`UpsetWatchAtmosphere=1` adds a separate sound route from Q2 onward when a lower-ranked home team challenges a Top 25 visitor. Native game ranks must show a meaningful gap; unknown ranks cannot qualify. At neutral sites, either team's underdog bid can qualify. Records are not used by this sound route.

The required actual fullness is the standard stadium-size sound gate minus 35 percentage points multiplied by the relative rank discrepancy and the current upset stage, with an absolute 50% floor. The discrepancy is the rank gap divided by the gap plus 50. For example, #120 UNC leading #12 Notre Dame 7-3 at the start of Q2 needs about **54.9%** of its 27,663 mapped positions occupied, so the measured **55.6%** crowd qualifies. Its **83.3% packed-house requirement stays unchanged**.

Early bids get a modest lineup roar/swell. A closer score, a larger ranking gap and a later game stage strengthen excitement. The final 25% of Q4 can sustain tension for a close bid with a large ranking gap, using the existing high-leverage limit. A deficit beyond 14 points ends the route; a two-score deficit in that final window ends it too. The usual lopsided-game fade still applies. Layers settle when the bid or crowd stops qualifying.

The route admits the same earned play reactions and existing upset-win celebration; it does not create another cheer for the same event. Touchdowns and eruptions take priority. Away-offense play selection at home venues still uses roars/swells only. Fullness continues to scale sound strength, and all ordinary, high-leverage, chant and eruption volume caps stay unchanged. Packed-house classification, packed marquee reaction boosts, pre-snap camera requirements and attendance protection remain separate. Earned reaction motion retains its existing zoom compensation. Set this option to zero to remove only the additional sound admission, or turn off PlayCallAtmosphere to disable play-call layers altogether.

### Layered play-call buildup

Big games are both-Top-25 matchups, CFP games, conference championships and the national championship. Rivalry alone boosts starting attendance but does not make a game big. Thresholds use the full mapped seating layout, including originally empty positions. They scale in a straight line between these sizes:

| Atmosphere route | At 6,060 stored positions | At 61,888 stored positions |
| --- | --- | --- |
| Ordinary packed house | 98% fullness | 60% fullness |
| Opening drive, big game, close critical down or goal-line tension | 90% fullness | 55% fullness |
| Overtime or late winning-drive buildup | 85% fullness | 50% fullness |

Smaller and larger layouts use the nearest endpoint. Actual big games receive an opening allowance: buildup can start at 50% measured fullness during the intro and early Q1. The normal size-based requirement returns gradually between 10% and 75% of the way through Q1. This avoids a quiet opening while fans arrive; the absolute 50% safeguard still applies. Ordinary matchups can use the critical-down fullness threshold for the opening offensive possession, even below packed house. When a verified crowd plan expects at least 90% peak capacity, that first-drive threshold eases to 55% actual fullness and its newly admitted buildup stays gentle. This applies whichever team receives. It ends when that possession finishes with a score, punt or turnover; the return and whistle retain their earned reactions. Later ordinary drives need their usual qualifying situation. Resumed or already-progressed games do not gain a new opening drive. `OpeningAtmosphere=0` disables this allowance along with the other opening extras. A high-demand ordinary intro can use the same lighter 55% route; sparse intros/first drives can receive the modest sound-only welcome. Opening-kickoff motion keeps its existing marquee requirements. A stadium can only fill as far as the game originally intended; these gates never add fans. An optional INI floor can raise the big-game requirement. Ordinary pre-snap camera movement uses the big-game, packed-house or qualified opening-drive route. Qualifying close critical downs can receive their stronger movement independently, and qualifying overtime/late-drive buildup can receive gentle movement on ordinary downs. Goal-line sound alone does not unlock ordinary camera motion.

Most eligible play calls use an anticipation roar and supporting swell, without an added cheer. A single immediate cheer opportunity is reserved for a call inside the opponent's 10-yard line, the first home offensive call of the game, or the first call of a late home drive that can win. A late winning drive means a tied game or a deficit of up to eight points in the last three minutes of Q4, or overtime. Neutral venues allow either offense. All qualification and fullness checks still apply. Existing native cheers may cover the moment instead; missed opportunities are not played late.

When the visitors receive the opening possession at a home stadium, their first qualifying offensive call gets a stronger hostile welcome using the anticipation roar and supporting swell, without an added lineup cheer. The roar builds quickly, then follows the usual 40-second sequence and snap/whistle bridge. Opening shake scales from 5% in the smallest mapped venues to 15% in the largest, with an adjustable ceiling. This uses the existing high-leverage volume limit of +35%; it does not add another boost. The standard native sound and volume guards still apply. It never repeats on a later visitors' drive if the home team started with the ball. At neutral venues, each team's first qualifying Q1 offensive play gets a shared positive opening cheer and half-strength crowd movement instead of a home-defense welcome. Resumed scenarios, simulation and crowds below the qualifying threshold do not receive this sequence. Disabling OpeningAtmosphere removes it.

Play-call motion begins easing in during play selection and keeps the same ramp when the lineup appears. It applies only to the gameplay camera, so separate quarterback, sideline and other presentation cutaways do not receive the next play's buildup shake. A change to stronger motion also eases into its target. An earned play reaction or celebration keeps priority and can still follow live camera cuts; returning to ordinary motion does not abruptly jump to full strength. Recorded replays remain steady.

A qualifying big game's opening kickoff has its own immediate lineup cheer, followed by a roar and supporting swell throughout the kick and return. The layers fade over about four seconds after the whistle. A qualifying long return or eruption takes priority. Both kicking and receiving formations are supported, including neutral sites. This needs a verified fresh opening and the opening fullness requirement; resumed scenarios, later kickoffs and simulation cannot invent it. It does not add another volume bonus.

The roar builds over roughly 2.5-3.5 seconds. Each ordinary sequence lasts 40 seconds including its fade: roughly 26-30 seconds of buildup/sustain, then 10-14 seconds settling. Small timing and reaction-size differences reduce repetition. Opening energy fades through Q1; useful drives can build it again. Big games build on the same fullness-based foundation as ordinary packed games, with additional excitement and a larger native reaction profile when strength allows it. Close finishes add more emphasis during the last quarter of Q4; postseason games receive a small further lift. Ordinary-game strength is unchanged, and this adds no separate volume bonus. Blowout fading and all sound limits still apply.

If arrivals push the stadium past its requirement while players are already lined up, the buildup can begin then. It does not need to wait for another play call, and it cannot restart repeatedly after its 40 seconds end. Ordinary buildup uses the same rules on home and away offense. Standalone defensive-loss motion uses one verified loss of at least five yards on a normal offensive play, including sacks. Exactly five yards qualifies; the motion no longer waits for a native sack label. Shorter sacks and unknown yardage do not independently shake, but keep their existing sounds and replay classification. A separate goal-line, high-leverage or qualifying third/fourth-down stop can still earn motion. These routes favor the home defense, or either defense at a neutral site, and retain the play's atmosphere/fullness requirements. A result first confirmed at the next lineup can receive its one impact. Missing yardage, stale results and possession changes alone cannot establish a loss.

Snapping carries the remaining sound through the play, followed by a short bridge into the result. Ordinary pre-snap shake settles after the snap. As the score separates from 16 to 24 points, added layers fade; at 24 or more, new ordinary layers stop. The game's own sounds remain.

### Pregame and postgame atmosphere

Qualified big games and packed ordinary stadiums can begin their roar/swell atmosphere during the stadium intro. Big games keep the 50% measured-fullness opening safeguard. An ordinary game with at least 90% expected peak can receive a lighter intro and first-possession buildup from 55% actual fullness (or its lower normal size threshold). This route does not receive the stronger hostile first-snap surge. A modest welcome sound remains available below shake requirements, without added motion or volume premium; stronger play/result cues take priority. It requires a genuinely tracked fresh opening and cannot be invented by a resume, late attachment or simulation.

After a home win, or either side's win at a neutral venue, a qualifying crowd can keep its stadium atmosphere through the postgame scenes. Earned winning eruptions take priority and keep their existing carry into postgame. Ordinary closing atmosphere ends at the stats/highlights menu and has a two-minute safety limit. Intro atmosphere has a five-minute safety limit. A blowout still reduces or prevents extra layers. These scenes do not invent touchdowns, play reactions or camera cues.

### Reactions while the play is still live

These reactions need a play that qualified for layered atmosphere and enough fullness to meet that play's size-based requirement. Home-team successes qualify at a home venue; either side can qualify at a neutral venue.

- Long catches need 20 yards at the catch. An immediate tackle still gets a short reaction and settling shake. If the completion settles just after the whistle, a short confirmation window can catch it. A verified 20-yard completed gain can also qualify when the final live runner sample fell just short. Each play receives one added reaction.
- Runs, quarterback scrambles and short catch-and-runs start a gentle shake at 10 total yards: about 1%, rising to the normal 10% live-play strength by 15 yards. A roar and supporting gameplay swell build with the run; the cheer starts when it reaches 15 yards. Catch-and-runs need actual movement after the catch, so a 15-yard catch tackled immediately does not bypass the separate long-catch cutoff. Sideways or backward cuts do not cancel an established long-run reaction. Long air-yard catches keep their immediate reaction rather than this ramp. Custom long-gain settings move the five-yard ramp with the cutoff.
- Close offensive fourth-down conversions can react as soon as a held runner or confirmed catch reaches the first-down line, even on a short gain. Field goals and punts do not count as offensive conversions.
- Interceptions react to a fresh native interception signal without waiting for the slower possession label to update.
- While a verified pass is airborne, the existing native gameplay swell builds with a 0.5-second attack. It works alongside a lineup roar and can give an already-playing swell one fast refresh for that pass. If no roar remains, it uses only the gameplay swell, with the primary channel available as a fault fallback. It settles over the existing short pass-end fade; catches and other earned reactions take priority. A known stats reset at the line also covers a quick release whose live reset frame was missed. No extra volume bonus, pre-catch cheer or shake is added. Pause, replay, simulation, unknown play types and unqualified fullness still block new anticipation. `PassFlightAnticipation` requires `DuringPlayReactionsEnabled`.
- Kickoff and punt return sound/movement builds from 30 return yards to its existing peak at 40. The one-shot cheer is requested at 40, measured from the returner's catch/recovery. Observed kick flight and coherent carrier movement are required.
- Verified five-yard losses, forced fumbles and fourth-down goal-line stops inside the five get a short reaction impact. A sack uses the same five-yard loss requirement for standalone motion, while its sound and replay classification remain separate. Missing yardage, stale results, kicking plays and unverified possession changes cannot supply that loss proof. A lost fumble is not required for a forced-fumble impact.
- Fumble recoveries are sound-only. Neither a live pickup, a recovery buildup nor the post-play fallback can start new shake. The separate forced-fumble impact remains, and a confirmed decisive score/takeaway can still receive its higher-priority celebration.
- A verified home blocked field goal gets a defensive cheer and impact. When home is tied or leading by one to three points before the kick, it uses the touchdown-style 20% opening hit and 15% sustained motion by default. Wider margins use the regular defensive impact. At a neutral venue, either defense can qualify, using that team's lead. Existing fullness/atmosphere requirements still apply. Ordinary misses and blocked punts do not count as blocked field goals. A return touchdown or eruption takes over.

Qualifying live gains and returns use 10% movement, followed by a five-second settling fade after the whistle by default, with no extra time for longer plays. The fade is adjustable from one to eight seconds. Interceptions and qualifying long kickoff/punt returns keep excited roar/swell layers through the return. The initial cheer is not repeatedly restarted. Immediately tackled long catches use their own adjustable 8% impact. The whistle must already have happened or follow within 0.35 seconds of the catch; catches with a longer run keep the normal live-play motion. Verified five-yard losses and other defensive impacts keep their separate 10% strength. These short impacts rise quickly, hold their initial strength for up to a second, then ease out. They last about 6.4 seconds total by default. The extra short-impact time is adjustable separately. A confirmed forced fumble uses a sharper 35 ms attack and 50 ms handoff, at the same configured strength and duration. Its detector still waits for verified force proof. A fresh native forced-fumble event can react while the ball is loose; a defensive recovery adds sound without new shake. Forced-fumble motion requires its own verified proof; a pickup cannot substitute for it. Movement softens on zoomed post-play cameras. A touchdown or eruption takes priority, without stacking a second shake or volume bonus. Native voice limits and duplicate checks remain in place; pause, replay, simulation and stale play data cannot create new reactions.

The game's own Stadium Pulse camera shake is also suppressed below 50% actual seating fullness. This changes the HFA camera event only; the meter and gameplay advantage remain under game control. Fuller crowds keep native shake, and the original camera event is restored when the filter no longer applies. The threshold is adjustable, and setting it to zero disables this filter. DGA's added motion remains subject to its own eligibility and strength settings.

### Skill moves, hits and important stops

Defensive loss and third/fourth-down stop motion now share one completed-result decision per play. Early knockout/contact evidence waits for that confirmation; late contact flavor can use the same bounded result window without restarting or stacking the impact. Goal-line and high-leverage stops take priority over contact flavor. Routine third-down incompletions do not gain a new shake rule. Forced fumbles, interceptions and scoring reactions keep their separate priority paths.

Confirmed successful trucks, stiff arms and hurdles can receive a short sound and movement accent. Successful jukes and spins use a cheer without starting or restarting shake or an extra skill swell. Regular broken tackles use a short swell without adding or restarting shake. A separately earned long-gain or touchdown reaction can continue on the same play. Landed hit sticks get a sound accent; their shake requires a confirmed third- or fourth-down stop. Move attempts alone do not count. Sound-only accents can work below the atmosphere gate; movement keeps its existing fullness and home/neutral rules, with at least 0.9 seconds between accents and at most three per play. Success plus broken-tackle evidence from the same collision becomes one accent. Kneel-downs and clock-stopping spikes do not get added buildup or sack/stop reactions. Verified away takeaways clear the previous home-positive motion immediately. A skill accent never restarts an already-running long-gain shake; touchdowns and eruptions take priority. SkillMoveReactions turns these accents on or off. Truck, stiff-arm and hurdle success signals, broken tackles and landed hit sticks were observed live; final-build sound/motion and the remaining skill signals still need targeted checks.

A third-down defensive stop inside the five receives the normal impact. A fourth-down goal-line stand receives a slightly stronger impact. Fourth-down conversion reactions start when the live ball crosses the first-down line; a later qualifying run or catch-and-run remains eligible for its own accent. Native voice limits still apply.

PackedBigGameReactions adds extra context only when a ranked/championship game or rivalry also meets its stadium's packed-house threshold. Reactions strengthen with a close score, late clock, critical down or goal-line threat. A defensive play that actually seals a one-score win can become an eruption; a first down that lets the winning side kneel out the clock can receive a touchdown-style response. Possession, remaining time and known opponent timeouts must support that conclusion, or the game must already be final. An ordinary packed matchup keeps its standard reactions. These context bonuses do not raise the volume caps.

During play-call buildup, only the home offense receives the special lineup cheers. Away-offense buildup uses roars and swells, including the hostile opening. Neutral sites can cheer either offense. This does not mute the game's own cheers or earned defensive reactions.

### Fixed post-whistle settling

Runs, catch-and-runs, interceptions and qualifying kick/punt returns use the same five-second settling period after the whistle by default. Play length never adds time. Sound and camera hold briefly, then fade together. The shared fade remains adjustable from one to eight real seconds in the INI; the old distance-extension setting is removed.

Touchdowns, eruptions and short impacts retain their separate profiles. Camera cuts and live timeout presentation keep an earned reaction on its original timer. A new snap, recorded replay, regular pause menu or game change cannot restart an old reaction.

### Reactions after successful plays

Verified big gains, third/fourth-down conversions, sacks or tackles for loss, defensive stops, takeaways and ordinary touchdowns can receive a fuller cheer and a short sustained roar. They require qualifying layered atmosphere and the qualifying play's size-based fullness requirement. Routine shorter first downs can receive a smaller response within that same qualified play context.

Safeties receive the touchdown-style cheer and roar layers, with their hold and fade shortened to 70% of the normal touchdown response. They use the same atmosphere/fullness checks and home/neutral ownership rules. PATs and two-point conversions do not trigger this safety reaction. A safety that earns a winning eruption still gets that higher-priority celebration.

The response scales with the play and score. A long gain, fourth-down stop or takeaway in a close game gets more excitement than a routine success in a blowout. Stronger responses can combine two different cheer sounds and a supporting swell. First downs and conversions use a yardage cheer; an explosive completion or gain already measuring at least 40 yards at its trigger uses the larger tested cheer. Defensive stops and takeaways use a distinct defensive cheer. Added responses have a quick onset, settle slightly into a sustained cheer, then fade smoothly. Ordinary successful-play responses hold for roughly 5-8 seconds, then settle over roughly 7-9 seconds. Duration-based running/return reactions use the shared whistle fade instead of stacking another ordinary result envelope. It does not add another volume bonus or unlock the eruption volume allowance.

A close made field goal gets a positive home-crowd cheer, or a cheer for either side at a neutral venue. Existing late winning kicks keep their eruption. A close missed home field goal requests disappointment; an opponent miss gets a defensive cheer.

In a one-score game during the final two minutes of Q2 or Q4, a confirmed third/fourth-down defensive stop gets the larger layered cheer profile and a 10% settling impact. The qualifying atmosphere and fullness rules still apply. An earlier disappointed response to a deep catch cannot cancel the later defensive-stop celebration.

Earned roar/swell responses recover promptly if a native presentation stop interrupts their shared channel during a timeout or penalty scene. Recovery uses the existing play timer, even when a matching native cheer meant no extra cheer was needed. It does not replay the cheer or extend the response. Repeated stops remain rate-limited, and normal pause, replay, scene-retirement and new-snap cleanup still apply. This protects the shared looping layers; it cannot create a specific native cheer that the game never requested.

Cheer requests now distinguish native queue dispatch from a newly registered native voice. A request discarded by the native queue can get one recovery within 600 ms, only on the same active play and owner and within the existing two-voice budget. Retiring native voices do not count as active duplicates. A registered voice still does not prove audible output.

Confirmed turnover-on-downs stops get a small paired shake and cheer: half the Live-play setting for 1.8 seconds, with neutral scaling and the normal atmosphere gates. A scoring play, kick, interception or fumble is not reclassified as a turnover-on-downs stop.

A verified released forward pass cannot be classified as a sack merely because a native presentation sack flag persists from an earlier play.

The game keeps its own reactions. The mod never lowers the game's native crowd intensity to make room for an added sound, skips matching cheers already playing, and limits competing native reaction families to two at once. Extra reactions favor the home team, or either team at a neutral venue. Repeated checks of the same play cannot repeat its cheer. Separate reactions are at least four seconds apart, and a new live snap ends the previous result's extra excitement. Eruptions take priority.

When a fresh play result and updated down, possession or measured first-down information are available at the whistle, the reaction starts there. Otherwise it waits for the result to be confirmed; it does not guess from an unfinished play. Additional native disappointment is requested for failed home third/fourth-down conversions, deep home incompletions, home turnovers and visitor catches of at least 15 air yards. Home defensive deep incompletions/breakups and tackles for loss receive a defensive cheer. Deep incompletions require a verified pass that traveled at least 15 yards in the air; an overthrown pass can qualify too. These sounds use the same qualifying atmosphere, fullness and duplicate checks, add no volume bonus or extra shake, and can be disabled separately. The disappointment sound can be subtle depending on the native mix. At neutral sites, either defense can receive the positive response; the mod avoids a stadium-wide home-team letdown for one side's success.

### Successful drives build excitement

Consecutive home first downs gradually strengthen the existing buildup. Third- and fourth-down conversions add more, and useful shorter gains can contribute a little. Failed plays cool it. Drive excitement halves over one eighth of a quarter of game time: 90 game seconds with 12-minute quarters. A score, possession change, halftime or simulation clears it. The effect changes crowd excitement and the size of the existing layers; it does not add another volume bonus or unlock an otherwise unqualified stadium. Neutral venues allow either offense.

A new game clears drive excitement. Waiting at the line with the game clock stopped does not drain it. Simulation does not invent a run of observed big plays.

### Overtime and late scoring drives

The same layered system stays active across snaps and possession changes in overtime once its size-based **85%-to-50% fullness requirement** is met. The roar and swell stay continuous through the action. Cheers still require a special call inside the 10 or the start of a home drive that can win; ordinary overtime calls do not schedule a cheer. There are no recurring late cheer bursts simply because the roar has been running for a while. Pause and replay cleanup still apply.

In Q4's final three minutes, a home offense tied or trailing by no more than eight points can build anticipation once it reaches the opponent's 40-yard line. Its size-based fullness requirement scales from **85% in the smallest reference venue to 50% in the largest**. The effect grows nearer the goal line and as time runs out. At neutral venues, either team's qualifying drive can receive it. This can flow directly into a winning eruption.

The play-call system supplies the layered opening, overtime and late-drive atmosphere. Sparse fresh openings can use a separate modest sound-only welcome, with no shake or volume premium.

### Crowd eruptions

Eruptions require qualifying layered atmosphere and inherit that play's size-based fullness requirement. A qualifying big-game play can erupt at 55% in the largest reference venue; an eligible overtime/late-drive play can do so at 50%. The admitted threshold stays with the celebration through postgame, rather than reverting to an old flat floor. An opening return touchdown in a sparsely occupied stadium keeps the game's own celebration without added eruption or camera shake.

Eruptions can follow these home-team moments. At neutral venues, either team can receive them.

- A last-gasp touchdown pass snapped in the final **15 seconds of Q2 or Q4**, with at least **30 air yards** and a catch within five yards of the goal line. This uses confirmed catch/score data rather than assuming a play name; a short screen taken all the way does not count.
- A go-ahead score in Q4's final **30 seconds**, including field goals, extra points and two-point conversions, or a verified game-winning score.
- The **game's first touchdown within its first two plays**, including the opening kickoff. Offense, defense and return touchdowns count, and that play call must qualify for layered atmosphere. A previous touchdown by either team closes this opportunity. The mod needs to watch the game from the start; it does not guess the count after a resumed game or a stretch of simulation.
- An **overtime interception or defensive fumble recovery**, once the change of possession is confirmed, or an overtime winner.
- A game-sealing takeaway while protecting a **one-score lead in Q4's final minute**. With zero or one opponent timeout, the full minute qualifies; with two, 40 seconds or fewer qualify. Three timeouts can still stop the kneels, so that stays a normal takeaway reaction unless time has expired. This works anywhere on the field. Unknown timeout counts do not get guessed. An interception or defensive fumble recovery counts; a routine possession change or turnover on downs does not qualify as a takeaway.

A **60+ yard rushing or passing touchdown** with the score within eight points before the snap receives a modest touchdown accent when its play qualifies for atmosphere: 15% stronger touchdown shake and bounded extra crowd excitement, using normal touchdown timing and volume limits. Long distance alone does not trigger an eruption. A separate decisive-score rule can still take priority.

The celebration opens with a large native positive cheer, an excited roar and a supporting swell. The roar and swell use a fast half-second attack, including when they take over an existing buildup. With StaggeredMajorCheers enabled, a major play or eruption can add one extra cheer about 0.9 seconds later. The initial cheer can use an already-playing matching native reaction; the later accent may overlap it. Both native and added cheers count toward a two-reaction limit. The second accent has one brief opportunity and is never retried after a full voice budget, a new play, pause or replay. Crowd volume can rise toward a **50% maximum boost**, with the amount also responding to the remaining crowd. Chants keep their separate fullness-based boost of up to 15%.

Ordinary eruptions last about 45 seconds. The volume lift reaches its peak quickly, holds briefly, then settles over roughly 25 seconds; the roar and swell fade over the final 20 seconds. Overtime winners and takeaways get a longer, roughly 60-second eruption with the same slow settling shape. If an eruption carries into a victory, the roar and swell continue through postgame presentation until the stats/highlights menu opens. A home celebration can end if the opponent scores or takes the ball away. A winning confirmation for the same play does not start another cheer or restart the roar.

A close home upset over a Top 25 opponent can also trigger a final-whistle celebration when the ranking gap is large. It does not start an extra eruption if one is already carrying through. Neutral venues can celebrate the winning underdog on either side.

### Keeping the effects together

Eruptions take priority over ordinary buildup. The system uses native reaction checks and spacing between cheers to avoid piling up identical reactions. Pause menus, interactive replay, scene changes and disabled crowd audio release the added layers. The normal game reactions remain available.

The plugin does not boost commentary, referees' spoken calls, the stadium announcer, bands or stadium music. Team-specific sounds follow the audio group the game assigns them to. Boost percentages describe the volume adjustment; adding sound layers also changes how full the crowd feels.

Gameplay home-field advantage, the Stadium Pulse meter, player composure, play-art effects and clocks remain controlled by the game. The added shake is presentation only.

### Conference championship games

The game supplies a dedicated conference-championship flag. These games use playoff arrival baselines, departure patience, halftime treatment, the 20-point permanent-departure cap per quarter, and the 60% crowd floor in dry weather or 50% in rain/snow. They also qualify for big-game audio buildup. They do not use the national championship's higher floor. Saved crowd history retains the conference-game classification.

## Dynamic Gameday Broadcast

### Camera movement

Ordinary eligible play calls choose one stable random shake level from 3-5%. Critical downs choose 5-10% before a 15% reduction for the home offense (4.25-8.5% by default). Home-defense critical downs keep the full range. Neutral-site crowd movement is halved for either team. A special immediate-cheer call uses 5% unless the stronger critical-down range applies. These settings control camera movement, not volume, Stadium Pulse or gameplay home-field advantage.

Ordinary anticipation movement requires a qualifying big game, packed house or opening drive; eligible overtime and late winning drives can also use this gentle range. Close critical downs can receive 5-10% movement at their qualifying atmosphere threshold, without a packed-house label. Third down needs ten or fewer yards and an offensive play; fourth down needs an offensive go-for-it call. Overtime/late-drive ordinary downs stay gentle; their qualifying third/fourth downs can receive the critical range. Field goals can receive ordinary movement. Pre-punt buildup uses half the ordinary movement (1.5-2.5% by default); earned return reactions keep their own strength. PATs and ordinary kickoffs do not; a big game's opening kickoff can use the gentle level on either kicking or receiving formations and carry it briefly through the whistle. It uses the big-game opening fullness requirement, so packed-house status is not needed. A qualifying return builds from 30 yards to full reaction strength and one cheer at 40.

At neutral venues, all crowd-driven shake targets and opening impacts use **75% of their configured strengths** for either team, including ordinary/critical buildup, live plays, touchdowns and eruptions. Each team can receive its own first-Q1-offensive-play surge once; resuming or joining later cannot invent one. The independent cannon impact keeps its configured physical-blast strength. Sound volume caps and atmosphere/fullness requirements stay unchanged.

An isolated touchdown opens at 20%, settles to 15%, holds through about six seconds, then fades out by eleven seconds. An eruption opens at 30%, settles to 22.5%, holds through about eight seconds, and fades out by sixteen seconds. Play distance does not extend these profiles. Every added shake softens automatically on close-ups and compensates for wider views, with a bounded adjustment. The game keeps its camera angle and zoom; camera compensation never changes audio volume.

A safety uses the touchdown's configured opening and sustained strengths, but 70% of its camera duration: about 7.7 seconds at the default settings. Its movement follows the same camera-cut, timeout, replay and zoom rules.

If live run/catch movement is already active, a touchdown or eruption takes over its current strength and builds to the new sustained target over 300 ms, without a new opening impact, an amplitude dip or an oscillator restart. For example, an active 5% catch-and-run rises to the default 15% touchdown level. An isolated score retains the impact profile above.

Long gains, interceptions and returns use the separate 10% reaction strength. Verified five-yard losses, forced fumbles and fourth-down goal-line stops use a short impact/fade at that strength. The strongest applicable effect wins; a live reaction can blend smoothly into a touchdown or eruption. Live camera cuts, timeout scenes and play selection retain an earned reaction's original timer and movement phase. They do not restart or extend it. Each new view gets its own zoom compensation, including during live gain reactions. A new snap clears the previous play's earned movement; the current lineup buildup fades over 0.9 seconds and follows the new live camera view instead of cutting off at the snap. Recorded replays, including the play-selection inset replay, receive no added shake. Pause, interactive replay, a new game or disabled accessibility effects stop added motion.

Confirmed runs and returns keep their full settling time after the whistle, including when the game changes views during a tackle. The official post-play crowd response takes priority over the shorter during-play fade. A touchdown, safety or eruption replaces the previous play shake; the older shake stays retired when that celebration ends, even with custom shorter durations.

Automatic movement has a matching crowd layer. Short impacts retain their roar/swell through the impact's own hold and fade, even when a matching native cheer made another cheer unnecessary. A fumble pickup leaves any existing forced-fumble impact on its original timer, without starting or restarting motion. Losing possession ends the old return's extra celebration. Camera-only previews in the diagnostic tool are an intentional exception.

Camera controls are under Dynamic Gameday Broadcast in the INI. OrdinaryShakeStrengthPercent and CriticalShakeStrengthPercent set the random range ceilings. LineupCheerShakeStrengthPercent controls special cheer calls; DuringPlayShakeStrengthPercent controls live gains and defensive impacts; CatchImpactShakeStrengthPercent controls immediately tackled long catches. PlayReactionFadeSeconds controls their total settling time after the whistle, including the brief hold; PlayImpactExtraSeconds adds time to the short impact reactions. Touchdown and eruption settings control their sustained strengths, opening impacts and total durations. CameraShake=0 disables all added movement.

### More post-play replays

**Small inset replays:** routine runs and completions can appear in the replay window while you choose your next play, including short gains that normally miss the replay rules. You can keep selecting plays while they run.

**Full-screen replays:** qualifying first-down gains of 15+ yards can use the game's big-play sequence, with presentation graphics and the normal skip control. When the native play-call UI has been verified, a freshly confirmed touchdown or takeaway holds the authored inset request until the native full-screen clip sequence completes, then requests a separate inset repeat. Slow Sim keeps native full-screen playback without an added inset request; unknown UI states retain original inset handling. Mode changes cancel pending inset work. This uses existing recordings and needs live verification, particularly on tradition and RTG routes. DGA routine replay expansion excludes kneels and spikes.

**Defensive plays:** sacks favor the full-screen sack sequence, including when you control the offense. Tackles for loss can use the run/pass inset replay instead of the cutaway that normally has no replay. Interceptions and lost fumbles use their native full-screen sequence before the separate inset repeat; serial full-screen angles are kept together. Fumbles recovered by the offense can use routine run/pass insets where that play type is available; strip sacks can use the sack sequence.

These rules work for either team. They use the game's existing recordings and presentation sequences. Penalties, injuries, safeties, missing recordings and other special scenes can affect what plays. A replay is not forced after literally every snap. Native postgame highlight variety remains unresolved: repeated final-knee highlights were reported on 1.70; excluding DGA routine knee replay promotion does not by itself fix the native montage picker.

The replay controls under **Dynamic Gameday Broadcast** in the settings file lets you change the full-screen yardage cutoff, turn off either replay expansion, or disable the defensive routing.

### Attendance in Instant Replay and highlights

Instant Replay, the postgame highlights viewer and automatic presentation clips use attendance recorded near the selected play. This includes halftime and postgame replay sequences. An early highlight can show a fuller crowd than a late highlight, and changing the selected play updates the crowd accordingly.

Leaving replay restores the current match's crowd. Replaying an old comeback does not bring fans back into the live game. Only seats emptied by the mod can be restored.

This history changes the visible fans. Automatic presentation clips also use the selected play's crowd fullness for quieting and volume; they do not recreate old scoring reactions or run new departure decisions. An active winning celebration can continue through those clips. Opening the interactive replay viewer or pausing releases the added roar.

### Dynasty Midgame Save support

Resumed games and scenarios are treated as new starts, including when the game reuses parts of the previous match.

If a recent attendance record matches the teams, time, score, quarter length and matchup, the mod restores it. Otherwise, it estimates a starting crowd from the resumed score, time, rankings and weather. First-quarter scenarios also pick up the arrival curve at that point. Status and logs say whether attendance was restored or estimated.

The saved history keeps the last eight games in `Plugins/DynamicGamedayAtmosphere/History/`. Older entries are replaced automatically. Missing play history cannot be reconstructed exactly, and a resumed scenario records its own new replay history from that point onward.

## Timing, settings and logs

Normal attendance changes wait for play selection or breaks. The crowd stays fixed while you are lined up, during snaps and through the same play-selection screen. A clock reaching an arrival or departure threshold schedules the change for a suitable break.

Arrivals, ordinary departure timing and halftime seat-break windows scale with quarter length. The Q4 two-minute checkpoint and final-30-second go-ahead reactions use the actual clock. Audio fades and celebration durations use real seconds. Drive excitement decays with game time, scaled to quarter length.

Updates preserve supported settings and add new options automatically. Restart both MMC and the game after replacing the plugin so the new version is loaded. Keep one copy of the plugin installed.

Logs and status are in `Plugins/DynamicGamedayAtmosphere/Logs/`. They show the version, crowd target, measured crowd, renderer status, team IDs, supplied rankings, rivalry status and relevant events. Audio status separately lists the mapped seating baseline, live seating fullness, size-based packed-house threshold, native attendance/capacity reference values and drive excitement. It identifies when the seating measurement is not ready. A target is the mod's requested crowd; the measured figure is what the game actually shows. Native hiding and rounding can make them differ slightly.

The status report separates the matchup, visible attendance, current sound levels, active atmosphere, replays and settings into labeled sections. Live effects and configured limits appear separately, with one value per line. Extra technical counters only appear when debug logging is enabled.

Play summaries record the confirmed event and whether added reactions qualified. A numbered live-reaction entry records the gain, clock, fullness and camera cue. Matching audio entries say whether a cheer was sent to the game, was already playing, was skipped because native reaction layers were full, or missed its window during a scene change. Primary cheers are requested at the confirmed trigger. An optional single second accent for major plays has its own short, logged delay. A blocked cheer does not wait in a queue to pop up later. A sent sound event or prepared camera cue confirms dispatch, not what a particular speaker mix or camera ultimately showed. Pause, replay, injury and other presentation changes can interrupt effects.

Launcher failures name the failed operation, game process ID, relevant file and Windows error code/HRESULT where available. For example, a denied game start-time query is distinguished from a blocked loader executable, module enumeration or runtime-load operation. Reports include whether MMC and the game are running as administrator when Windows allows that read-only check. Repeated process-check failures back off through 5, 15, 30 and 60 seconds; unchanged failures are summarized rather than logged every second. A changed error or recovery gets its own notice. Runtime loading stays once per game-process/start-time identity, including uncertain failures, so a blocked or pending load cannot create repeated injection attempts.

The main log clears before it would pass its size limit, about 2 MB by default. The status file is overwritten. Crowd-history storage is also bounded.

The plugin checks the game's internal layout before activating and has no EXE hash requirement. Game updates can require a plugin update. The plugin DLL contains its native runtime and local preview tool; the short README and this feature list are separate. Installer scripts are not required.

Each setting in `Plugins/DynamicGamedayAtmosphere/DynamicGamedayAtmosphere.ini` has a plain-language explanation, including valid ranges and how related switches interact. First Update 3 startup resets the old INI to current defaults and saves the exact original in Backups. Settings edited afterward are preserved on later launches and routine updates.


For missing-audio troubleshooting, turn off `Enabled` under **Dynamic Gameday Soundscape**, then restart MMC and the game. With that switch off at launch, DGA leaves its audio hooks uninstalled while attendance and extra replays remain available. Added atmosphere motion depends on Soundscape. Re-enabling Soundscape after such a launch needs another restart. Status includes the game process ID, native audio callback count and native crowd-volume setting to help match a report to the correct session. Failed game-code checks now identify the expected and observed bytes and any recognizable existing detour; they still leave unsupported code untouched.

## Customization and diagnostics

The working build exposes the user controls in **Gameday Atmosphere**, a new tab at the far right of the game's Settings menu, after Audio Settings on the standard screen. The native page uses the game's controls and description pane. Changes save to the existing DGA INI; the INI remains available for direct editing. The tab and pause-menu fullness toggle were verified in game. Complete controller navigation, numeric edits and Reset remain on the final-build checklist. Unsupported native menu layouts retain the original tabs and INI controls.

The in-game page contains 35 essential controls: Attendance (10), Soundscape (12) and Broadcast (13). Each section puts On/Off switches before numeric controls. Attendance keeps arrivals, starting crowds, fan patience, departures and halftime breaks; Soundscape keeps its main atmosphere/reaction switches and background quieting; Broadcast keeps camera/replay switches, primary shake strengths and the replay yardage cutoff. There is no in-game troubleshooting section. Advanced arrival/comeback limits, volume caps, impact/duration tuning and logging remain in the INI, and editing the smaller menu preserves those values.

Every numeric setting has its default, accepted range, units and the effect of increasing/decreasing it beside the option. Use whole numbers where indicated and a period for decimal values. Invalid numbers use the default; valid out-of-range numbers are clamped. Configuration warnings identify the option and the value actually used. The one-time Update 3 reset clears previous values and obsolete keys with a backup. Custom settings made afterward are preserved within current limits; unknown keys are not additional controls.

Important combinations:

- Attendance `Enabled=0` leaves native attendance and disables the attendance-dependent soundscape. Extra replay settings remain separate. `ArrivalsEnabled=0` only skips arrivals; it does not force a sellout.
- Soundscape `Enabled=0` at launch skips its audio hooks. Turning it back on needs an MMC/game restart. `LayeredAtmosphereEnabled=0` disables added boost/layer effects while independently configured sparse-crowd quieting can remain.
- `MaximumBoostPercent=0` also disables added layers and added reaction motion. Use individual switches/strengths when only one feature should stop. Chants, ordinary crowd, high-leverage and eruption limits are separate totals, not percentages to add together.
- `FormulaStrength=0` disables formula pressure, not every independently configured event. Late-score exits, costly-turnover exits and halftime conversions have separate zero-disable settings. Temporary seat breaks are separate too.
- `CameraShake=0` stops the mod's added motion. The visual-only sparse native-Stadium-Pulse filter has its own threshold; zero disables that filter. Gameplay HFA stays unchanged.
- The post-whistle fade is shared by sound and camera. Short-impact extra time and touchdown/eruption durations remain separately adjustable. Play distance adds no extra time, and camera cuts cannot restart a reaction already earned.

Logs use readable categories such as `[play-summary]`, `[reaction-audio]`, `[departures]` and `[configuration]`. Play summaries show qualification, native sack confirmation, result phase, takeover/timeout context when relevant and the selected camera cue. Status shows the measured reaction distance and shared settling time. Repeated identical failures are throttled even if other messages appear between them. The diagnostic cache, crowd history and log sizes are bounded.

Disappointment requests can be subtle in the game's native mix. Offline Dynasty is the verified mode; other offline modes require their own live compatibility check rather than an assumed guarantee.

## Current Update 3 behavior and diagnostics

The in-game **Gameday Atmosphere** tab sits after Audio Settings and edits the DGA INI. The user verified the tab and pause-menu fullness toggle. Full keyboard/controller navigation, numeric edits and Reset remain on the final-build checklist. Labels and numeric help explain what the option changes and what higher/lower values do; FCS is capitalized consistently.

**Update 3 resets older DGA settings once.** The exact original INI is saved in `Plugins/DynamicGamedayAtmosphere/Backups` before current defaults replace it. Reapply desired custom settings using the new INI or in-game tab. Later launches and routine updates preserve new settings. Game saves and native game settings are unaffected.

All added camera motion respects the game's Dynamic Camera Effects switch. Ordinary buildup stays on gameplay views; earned play reactions may follow live presentation cuts with softer close-ups. Wide gameplay views, including the default RTG player camera, get bounded view compensation. Recorded replays stay steady. Offline Dynasty is the verified mode; other modes are not guaranteed.

Eligible deep passes by either offense use an airborne supporting swell with a 0.5-second rise. This is the attack time, not a half-second duration limit: the swell follows the verified flight and settles after it ends. It adds no pre-catch cheer, shake or separate volume bonus.

Crowd reactions carry their earned level through live gain, result, touchdown and eruption handoffs. Full-size continuous voices do not restart their attack, and lower-priority effects cannot reappear after the celebration. The existing voice budget, fullness gates and volume caps remain.

Successful skills use a gentle 0.45-second rise at half the configured live-play strength. Hurdles request a positive cheer; trucks request the native big-hit crowd family. Confirmed home-defense third/fourth-down pass breakups and catch knockouts use a small 1.8-second impact with sound. A normal incomplete pass is not deflection proof. Hit sticks keep their immediate sound, but only a completed third/fourth-down stop gets their shake.

A confirmed forced fumble can receive its own impact on any down. A delayed-signal fallback requires opposing contact, grounded possession, a held-to-loose transition and fresh native fumble confirmation. It retains the original carrier side through pickups. Recovery alone adds no shake, and recovering a team's own fumble cannot revive an old gain reaction. A qualifying touchdown or decisive eruption still takes priority.

Kickoff/punt return swells begin at 30 yards and reach their existing peak at 40, with one qualifying cheer. Sound and camera settle together after the whistle.

**Stadium Cannon Shake (%)** is independent of Live Play Shake: default 15%, range 0-30%, duration 0.9 seconds. Zero disables cannon movement while keeping native cannon audio. It covers all nine native cannon variants through the shared stadium route without a fullness requirement, using the stronger of cannon/crowd movement instead of adding them. Texas was tested live; other variants have offline coverage. Normal logs now include one numbered `[cannon]` entry per admitted blast, including variant and configured strength. These entries report a detected sound start and requested impact, not proof of visible output.

The standalone diagnostics tool offers bounded sound/camera previews and read-only compatibility reports. Previews do not change attendance, scores, clocks or gameplay HFA, and do not prove automatic event detection. The pause menu displays measured Stadium Fullness beside Resume. Logs, status, history, backups and extracted runtime files live inside the DGA folder; only the plugin DLL stays directly in Plugins. Launcher errors identify the failing operation and use bounded retry backoff.

See the [current release checklist](docs/DIAGNOSTICS.md) for confirmed observations, remaining live checks and support steps, and the [Update 3 candidate notes](docs/releases/Update3.md) for the changes since Update 2.

### Late rainy home wins (1.78 candidate)

In an ordinary home game, a lead above 21 in rain or snow gradually reduces winning-fan protection during the last half of Q4, reaching full severity at 35+. This strengthens the existing time-based departure target; a score does not directly remove a fixed number of fans. A narrow quarter-cap allowance of up to 10 extra points (less for low custom caps) is shared rather than stacked with home-loss allowances. Shared crowd floors, safe update windows, dry/close/early games, neutral venues and postseason rules retain their existing behavior. Live tuning verification is pending.
