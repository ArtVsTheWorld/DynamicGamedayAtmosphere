# Dynamic Gameday Atmosphere

By Ace | Update 3 development | Working build 1.55.0 | Offline CFB 27

One plugin, organized into Attendance, Soundscape and Broadcast settings.

The crowd fills in, reacts to the matchup and thins as the game gets out of reach. Play calls bring a buildup of crowd noise and pre-snap camera shake. Stadium fullness controls volume. Big games, packed ordinary stadiums, close critical downs, scoring threats, overtime and late scoring drives can get layered buildup. Winning moments receive longer celebrations. More plays also get the game's own replay presentation.

## Dynamic Gameday Attendance

### What the crowd percentages mean

Attendance targets and permanent-departure percentages are based on the crowd the game originally planned to show. If the game chose 40,000 fans, a 50% target means about 20,000 of those fans, even if the stadium holds 80,000. Temporary halftime break percentages refer to the fans still present.

**Sound follows the occupied share of the mapped stands.** The mod records how many crowd positions the game originally filled, then applies live arrivals, departures and temporary seat breaks to that baseline. If the game filled 90% of its mapped seating and 80% of that crowd remains, audio uses about 72% fullness.

This uses the whole verified seating layout, not just the fans visible to the current camera. Rotating the camera, changing the view or watching an earlier highlight does not change the live audio baseline. Replacement crowd renderers keep the original empty seats. A delayed Midgame Save announcement after Super Sim does not start a new crowd; actual scenario loads and new games still reset normally. Tiny groups beside the field are excluded, and duplicate renderers of the same seating layout are counted once. The game uses fewer crowd models than real seats, so this is a measure of visual density rather than a literal headcount.

Soundscape additions wait until the stadium's seating layout is verified. During that wait, the game keeps its own sound. Original-attendance percentages and the stadium's listed real-world capacity do not substitute for measured seating fullness.

The game still chooses the original attendance. This mod changes how much of that crowd is visible over the course of the game. Attendance rules apply to the stadium crowd as a whole.

Fans who permanently leave stay gone. First-quarter arrivals, temporary halftime seat breaks and viewing an earlier replay are separate from permanent departures.

### Where fans stay and leave

Thinning follows the seating layout. Seats farther from the field generally empty sooner, while closer seats keep more fans. Density tapers gradually across the rows. Areas around the stadium fill and empty at different rates, with stronger differences between fuller and thinner areas, including the closer seating. Small groups and individual stragglers remain among the emptier seats. The variation stays fixed for the game, so it does not reshuffle on each update or when viewing a replay.

This changes where the remaining fans sit, not how many the attendance rules allow. It never fills seats the game originally left empty.

Arrivals, permanent departures and temporary halftime absences share the same seating pattern. Earlier replays use that pattern with the attendance recorded for the play; leaving the replay restores the live crowd. Resumed games rebuild the pattern from the stadium's seats and use the restored or estimated attendance.

The pattern is prepared during pregame and stays in place for postgame stadium shots. If a cutscene loads a fresh crowd renderer, it receives the same final attendance. Automatic highlights can still show a fuller crowd when replaying an earlier part of the game.

Using Midgame Save to record a normal game preserves the smaller crowd prepared during pregame. Loading a saved scenario still restores or estimates the crowd for that scenario.

If the mod cannot verify a crowd's seating layout, that crowd keeps the previous scattered thinning. Small groups of people beside the field also keep their usual treatment.

### First-quarter arrivals

The smaller crowd and seating pattern are prepared during pregame and carried through kickoff. The game still chooses the peak crowd: the mod never fills seats the game originally left empty.

| Arrival group | Schools | Starting baseline | Finish point with 12-minute quarters |
| --- | --- | --- | --- |
| Normal | Every school except the four below, including all former moderate-late schools | 55% | 9:00 remaining in Q1 |
| Severe late arrivals | UCLA, Miami (FL), Georgia Tech, Maryland | 40% | 3:00 remaining in Q1 |

Miami (OH) uses normal arrivals. Neutral venues use the normal group. Other quarter lengths use the same proportions: normal arrivals finish one quarter of the way through Q1, severe arrivals three quarters of the way through. Actual changes wait for play selection, a break or verified offscreen Super Sim.

In ordinary matchups, ranking quality and separation, rivalry status, rain/snow and FCS opposition share a **combined adjustment limited to ten percentage points in either direction**. Large ranking gaps lower the start whichever team is hosting. Rivalry raises the start; weather and FCS opponents lower it. At the defaults, normal openings range from **45% to 65%** and severe-late openings from **30% to 50%**. Matchup and weather do not change the group's finish time.

**Big games use a shared 60% baseline and cannot start below 60%.** These are games between two Top 25 teams, CFP games, conference championships and the national championship. A rivalry adds ten points; rain or snow subtracts ten, with the 60% minimum applied last. That means 60% for a regular big game, 70% for a dry ranked rivalry and 60% for that rivalry in bad weather. Ordinary bowl games are not automatically in this category.

Big games override the school's starting percentage, while the school still determines its fill time. A severe-late school in a ranked matchup therefore starts at 60% but continues filling through three quarters of Q1. Championship departure floors remain separate from the opening crowd; they do not force a 60% opening up to 80%.

Rivalry alone is no longer a big-game atmosphere trigger and no longer grants extra departure or halftime patience. It still raises opening attendance. A packed rivalry can qualify for the same packed-house effects as any other full stadium.

Arrivals are stronger early, followed by a smaller tail of stragglers. Permanent departures can keep the stadium below its original peak even after scheduled arrivals finish. Arrivals never bring back fans who permanently left.

Normal and Fast Super Sim follow the simulated score and clock while their play-by-play screen is open. Slow Sim keeps the usual selection and break timing while the field is visible. Replays use their recorded crowd and restore the live crowd on exit.

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
| Both teams ranked and the game is close | 0-5% | 20-30% |
| Close ordinary game | 5-10% | 20-30% |
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

Permanent departures are capped in each quarter at 45 percentage points of the original crowd in ordinary games, 20 in playoffs and conference championships, and 10 in the national championship. For example, a 45-point cap permits a change from 90% to 45%; it is not a 45% cut applied repeatedly to whoever remains.

Late-score departures share that cap. Departures blocked by the cap are discarded rather than waiting to hit all at once later. Temporary halftime absences have their own limits.

Departure decisions follow the score and game clock even while the field is visible. The crowd itself waits for play selection, a break or verified offscreen Super Sim to change. Long stretches without opening play selection no longer erase departures or change the formula simply because fewer updates were possible.

Arrivals and halftime seat changes normally share an allowance starting at four points per opportunity, growing with time since the previous opportunity up to 16 points by default. When permanent departures have already built beyond that allowance, the next hidden opportunity can catch up to their calculated target. Quarter caps and crowd floors still apply, so this does not create another departure penalty. Quarter, two-minute and late-score calculations cannot repeatedly renew an opportunity's allowance. Initial pregame setup applies the opening target directly. Comeback protection stops new departure decisions; it does not undo decisions already made before the comeback. Offscreen Super Sim can catch up immediately while still respecting the quarter cap and crowd floor.

### Late-game exits and overtime

Q4 has one additional departure checkpoint at the next safe break after 2:00. It follows the same formula and protections. Quarters lasting two minutes or less skip this checkpoint.

A live opponent score inside the final two minutes can send another five percentage points home if the home team falls at least nine points behind with one timeout or none. Extra points do not repeat the penalty. This extra rule does not run at neutral sites or in postseason games.

Verified possession and remaining home timeouts also shape late Q4 pressure. Having the ball, especially near the end zone, buys some patience. Giving possession back to the opponent while multiple scores behind with few timeouts encourages exits. If that information is unavailable, the normal score-and-clock formula applies. Neutral venues receive extra overall patience because supporters of the winning team still have reason to stay; the mod does not assume it knows each section's allegiance.

Ordinary departures stop in overtime. The existing crowd remains rather than being treated as another regulation quarter.

### Late-Q4 traffic

During the last quarter of Q4, home crowds can leave faster when a comeback is slipping away. With 12-minute quarters, this is the final three minutes. A larger deficit, less time, fewer timeouts and the opponent having possession make departures more urgent. Home possession, a nearby scoring opportunity and a recent home score add patience. That recent-scoring boost fades with game time, and an opponent score cancels it. Unknown possession or timeout data uses a patient fallback.

The system moves toward a crowd target, so it does not subtract another percentage every time it updates. It can catch up much faster than the ordinary blowout rule, while keeping the same crowd floors, quarter limit and hidden seat-update windows. Fans who already left do not return after a score. Winning home crowds keep their existing patience. Postseason crowds receive a smaller traffic target only in clear blowouts; neutral-site games retain their symmetric rules.

A verified late home interception or lost fumble can also send a group home when it sharply reduces the recovery outlook. This can include a one-score loss when the opponent can run out the clock with kneel-downs. The group is capped at ten original-crowd percentage points by default and runs once for the turnover. Hopeful comebacks stay protected; neutral and postseason games are excluded from this event group. The old late-score group remains. A return touchdown uses the larger of its score and turnover groups, without adding both together. Settings allow the continuous traffic acceleration and turnover group to be disabled separately.

### End-of-third-quarter departures

A small group can permanently leave at the Q3/Q4 break. In an ordinary game with the home team trailing, the group is about one percentage point in a close game, two at a 14-point deficit, five at 21, and seven at 28 or more. The curve between those examples is smooth. These are points of the original crowd, not a fresh percentage taken off every update. Winning home crowds and postseason games get more patience; comeback/upset protection, weather, rankings, existing crowd floors and the shared quarter cap still apply. This replaces the old small Q4-break adjustment and runs once. Fans leave at the break or another verified hidden update, including Super Sim. Loading directly into Q4 does not charge a second group.

### Temporary halftime seat breaks

In an ordinary regular-season home game that reaches halftime down 18 or more, some seat-breakers may decide not to return. This applies only while more than 80% of the original crowd is still at the stadium, counting seated fans and people on a temporary break. About 20% of the seat-breakers stay away at an 18-point deficit, rising smoothly toward 25% at 28 or more. The effect eases between 90% and 80% remaining and excludes big games, postseason and neutral sites. These seats are already empty, so the change reduces later returns rather than removing another group on screen. Crowd floors and quarter limits still apply.

Some fans leave their seats for food, bathrooms and the concourse. At normal settings, the peak temporary absence is 20-30% of the fans still in the stadium, subject to the existing crowd floors. These are separate from people who permanently leave.

They gradually head out over the last quarter of Q2 and return over the first quarter of Q3. With 12-minute quarters, that means the final three minutes before halftime and the first three minutes afterward. Both windows scale to other quarter lengths. Changes happen at play selection, breaks and verified offscreen Super Sim updates. A missed window catches up at the next safe opportunity; fans never visibly refill during live action. Only temporarily withheld seats can return. In a one-score big game or packed stadium, only about one fifth of the usual early seat break develops before halftime; most of that group waits until the half ends. The main temporary exodus then uses the safe halftime boundary. Returns follow the usual first-quarter-of-Q3 window, with the same crowd floors. This changes temporary timing, not permanent departures.

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

Below 50% fullness, the mod selects calmer crowd ambience through the game's own excitement control. Native peaks are restrained too. Sparse venues with 10,000 or fewer mapped positions settle below 100 excitement; that ceiling rises smoothly with stadium size, reaching the normal 1,000 range at 61,888 positions. A large stadium with thousands of fans retains more presence than a tiny crowd. These are mapped crowd positions, not a listed real-world seating capacity. Changes blend over about six seconds.

Only venues with **12,000 or fewer verified mapped seating positions** receive the separate gentle crowd-volume cut. It starts below the same threshold and grows linearly to a maximum of 25% at zero fullness. This also reaches chants through the game's crowd category. It never mutes the category, compounds over time or changes the saved crowd-volume setting. Larger venues use native intensity quieting alone below 50% fullness. The cut can be reduced or disabled separately in the INI. Above 50% it adds no cut, and existing boost limits stay unchanged.

Native intensity changes crowd character rather than applying an exact loudness percentage. Quieting also applies during intros and postgame. A newly loaded presentation crowd starts at its measured level rather than briefly sounding full. Automatic replay clips use their recorded crowd fullness. Ordinary gameplay pauses and interactive replay return to native intensity and crowd-category volume. If a control is unavailable, that adjustment is skipped. Strength, starting fullness and the separate volume cut are adjustable in the INI. Zero intensity selects the lowest excitement; it is not a guaranteed mute of every crowd sound.

The boosts above 75% adjust the sound signal; perceived loudness depends on the sound and your settings. High-leverage moments can reach +35% total and eruptions +50% total. Boosts do not stack on top of those caps.

Added sound layers require a qualifying situation and its stadium-size-based fullness threshold. Added reactions and celebration movement inherit the requirement of the play that qualified. The game's own reactions, ordinary fullness-based volume and quieting continue independently.

### Layered play-call buildup

Big games are both-Top-25 matchups, CFP games, conference championships and the national championship. Rivalry alone boosts starting attendance but does not make a game big. Thresholds use the full mapped seating layout, including originally empty positions. They scale in a straight line between these sizes:

| Atmosphere route | At 6,060 stored positions | At 61,888 stored positions |
| --- | --- | --- |
| Ordinary packed house | 98% fullness | 60% fullness |
| Big game, close critical down or goal-line tension | 90% fullness | 55% fullness |
| Overtime or late winning-drive buildup | 85% fullness | 50% fullness |

Smaller and larger layouts use the nearest endpoint. Actual big games receive an opening allowance: buildup can start at 50% measured fullness during the intro and early Q1. The normal size-based requirement returns gradually between 10% and 75% of the way through Q1. This avoids a quiet opening while fans arrive; the absolute 50% safeguard still applies. Ordinary packed-house games keep their usual threshold. A stadium can only fill as far as the game originally intended; these gates never add fans. An optional INI floor can raise the big-game requirement. Ordinary pre-snap camera movement uses the big-game or packed-house venue route. Qualifying close critical downs can receive their stronger movement independently, and qualifying overtime/late-drive buildup can receive gentle movement on ordinary downs. Goal-line sound alone does not unlock ordinary camera motion.

Most eligible play calls use an anticipation roar and supporting swell, without an added cheer. A single immediate cheer opportunity is reserved for a call inside the opponent's 10-yard line, the first home offensive call of the game, or the first call of a late home drive that can win. A late winning drive means a tied game or a deficit of up to eight points in the last three minutes of Q4, or overtime. Neutral venues allow either offense. All qualification and fullness checks still apply. Existing native cheers may cover the moment instead; missed opportunities are not played late.

A qualifying big game's opening kickoff has its own immediate lineup cheer, followed by a roar and supporting swell throughout the kick and return. The layers fade over about four seconds after the whistle. A qualifying long return or eruption takes priority. Both kicking and receiving formations are supported, including neutral sites. This needs a verified fresh opening and the opening fullness requirement; resumed scenarios, later kickoffs and simulation cannot invent it. It does not add another volume bonus.

The roar builds over roughly 2.5-3.5 seconds. Each ordinary sequence lasts 40 seconds including its fade: roughly 26-30 seconds of buildup/sustain, then 10-14 seconds settling. Small timing and reaction-size differences reduce repetition. Opening energy fades through Q1; useful drives can build it again. Big games build on the same fullness-based foundation as ordinary packed games, with additional excitement and a larger native reaction profile when strength allows it. Close finishes add more emphasis during the last quarter of Q4; postseason games receive a small further lift. Ordinary-game strength is unchanged, and this adds no separate volume bonus. Blowout fading and all sound limits still apply.

If arrivals push the stadium past its requirement while players are already lined up, the buildup can begin then. It does not need to wait for another play call, and it cannot restart repeatedly after its 40 seconds end. Ordinary buildup uses the same rules on home and away offense. Added sack impacts require a confirmed home defensive sack, or either defense at a neutral site, plus the play's atmosphere/fullness qualification. A result confirmed only at the next lineup can still receive its one impact. A fresh sack confirmation can also trigger while its yardage is still unavailable; an ordinary loss of yards does not prove a sack.

Snapping carries the remaining sound through the play, followed by a short bridge into the result. Ordinary pre-snap shake settles after the snap. As the score separates from 16 to 24 points, added layers fade; at 24 or more, new ordinary layers stop. The game's own sounds remain.

### Pregame and postgame atmosphere

Qualified big games and packed ordinary stadiums can begin their roar/swell atmosphere during the stadium intro. Big games use their opening allowance; ordinary games keep their packed-house threshold. First-quarter play-call energy takes over as gameplay begins. Sparse intros stay calmer instead.

After a home win, or either side's win at a neutral venue, a qualifying crowd can keep its stadium atmosphere through the postgame scenes. Earned winning eruptions take priority and keep their existing carry into postgame. Ordinary closing atmosphere ends at the stats/highlights menu and has a two-minute safety limit. Intro atmosphere has a five-minute safety limit. A blowout still reduces or prevents extra layers. These scenes do not invent touchdowns, play reactions or camera cues.

### Reactions while the play is still live

These reactions need a play that qualified for layered atmosphere and enough fullness to meet that play's size-based requirement. Home-team successes qualify at a home venue; either side can qualify at a neutral venue.

- Long catches need 20 yards at the catch. An immediate tackle still gets a short reaction and settling shake. If the completion settles just after the whistle, a short confirmation window can catch it. A verified 20-yard completed gain can also qualify when the final live runner sample fell just short. Each play receives one added reaction.
- Runs, quarterback scrambles and short catch-and-runs start a gentle shake at 10 total yards: about 1%, rising to the normal 10% live-play strength by 15 yards. A roar and supporting gameplay swell build with the run; the cheer starts when it reaches 15 yards. Catch-and-runs need actual movement after the catch, so a 15-yard catch tackled immediately does not bypass the separate long-catch cutoff. Sideways or backward cuts do not cancel an established long-run reaction. Long air-yard catches keep their immediate reaction rather than this ramp. Custom long-gain settings move the five-yard ramp with the cutoff.
- Close offensive fourth-down conversions can react as soon as a held runner or confirmed catch reaches the first-down line, even on a short gain. Field goals and punts do not count as offensive conversions.
- Interceptions react to a fresh native interception signal without waiting for the slower possession label to update.
- Passes in flight can lift anticipation on an eligible layered play. This reuses the roar/swell, adds no pre-catch cheer or camera shake, and settles when the ball is caught or the pass ends. A catch still needs the game's confirmation; the intended receiver alone is not treated as a completion.
- Kickoff and punt returns can react at 30 return yards, measured from the returner's catch/recovery. Kick flight must be observed, so a touchback or the kick's flight distance cannot count as a long return.
- Confirmed sacks, forced fumbles and fourth-down goal-line stops inside the five get a short reaction impact after the play. These use the game's own result checks, not a guess based on negative yardage. A lost fumble is not required for a forced-fumble impact.

Qualifying live gains and returns use 10% movement, followed by a gradual 4.5-second base settling fade after the whistle by default; verified longer plays can add the distance-based time described below. The fade is adjustable from one to eight seconds. Interceptions, defensive fumble recoveries and qualifying long kickoff/punt returns keep excited roar/swell layers through the return. The initial cheer is not repeatedly restarted. Immediately tackled long catches use their own adjustable 8% impact. The whistle must already have happened or follow within 0.35 seconds of the catch; catches with a longer run keep the normal live-play motion. Sacks and other defensive impacts keep their separate 10% strength. These short impacts last about 5.9 seconds total by default. This is one second longer than the earlier impact profile and is adjustable separately. A fresh native forced-fumble event can react while the ball is loose; a defensive recovery adds a return reaction, with shake building over about 1.5 seconds. If the optional live forced-fumble signal cannot be verified, the confirmed post-play fallback remains. Movement softens on zoomed post-play cameras. A touchdown or eruption takes priority, without stacking a second shake or volume bonus. Native voice limits and duplicate checks remain in place; pause, replay, simulation and stale play data cannot create new reactions.

The game's own Stadium Pulse camera shake is also suppressed below 50% actual seating fullness. This changes the HFA camera event only; the meter and gameplay advantage remain under game control. Fuller crowds keep native shake, and the original camera event is restored when the filter no longer applies. The threshold is adjustable, and setting it to zero disables this filter. DGA's added motion remains subject to its own eligibility and strength settings.

### Longer plays settle for longer

Duration-based runs, catch-and-runs, interception/fumble returns and qualifying long kick/punt returns keep a short settling fade after the whistle. At the activation distance they keep the existing base timing. Longer verified gains add time smoothly, reaching four extra real seconds at 99 yards with the default setting. Gains beyond 99 yards receive the same maximum.

For a normal run or short catch-and-run at the default 15-yard threshold:

| Play distance | Extra settling time | Base 4.5-second return fade becomes |
| --- | --- | --- |
| 15 yards | None | 4.5 seconds |
| 30 yards | About 0.7 seconds | About 5.2 seconds |
| 57 yards | 2 seconds | 6.5 seconds |
| 75 yards | About 2.9 seconds | About 7.4 seconds |
| 99 yards | 4 seconds | 8.5 seconds |

The distance starts from the relevant point: total gain for runs/catch-and-runs, the interception or recovery for turnover returns, and the catch for kick/punt returns. An interception that is immediately downed keeps its base timing. Immediately tackled long catches retain their separate short impact; they are not treated as long running returns. Actual run-after-catch movement can earn the longer tail.

Touchdowns use their confirmed offensive distance or a verified return distance. Long touchdowns and qualifying eruptions receive the same one-time extra settling allowance. Their opening hit and base hold are unchanged; short scores do not automatically receive a longer celebration. Safeties keep their separate shorter touchdown-style profile. An earned winning postgame roar still follows its existing menu-based ending.

Sound and shake use the same bounded extra time. A native fade begins at the whistle rather than waiting and starting another fade later. Touchdown/eruption sound takes over from a running-return layer. There is no repeated extra cheer or timer added every frame. A new snap, replay interruption, disabled feature, invalid context or new game cannot inherit an old reaction. Recorded replay footage remains steady.

`ReactionDistanceExtraSeconds` controls the shared maximum, from 0 to 8 real seconds; default 4. Higher means a longer long-play fade, lower means shorter. Zero keeps base timing. It does not change activation yardage, volume caps or the separate base fade settings. Missing/unreliable position data keeps the base timing rather than guessing a long return.

### Reactions after successful plays

Verified big gains, third/fourth-down conversions, sacks or tackles for loss, defensive stops, takeaways and ordinary touchdowns can receive a fuller cheer and a short sustained roar. They require qualifying layered atmosphere and the qualifying play's size-based fullness requirement. Routine shorter first downs can receive a smaller response within that same qualified play context.

Safeties receive the touchdown-style cheer and roar layers, with their hold and fade shortened to 70% of the normal touchdown response. They use the same atmosphere/fullness checks and home/neutral ownership rules. PATs and two-point conversions do not trigger this safety reaction. A safety that earns a winning eruption still gets that higher-priority celebration.

The response scales with the play and score. A long gain, fourth-down stop or takeaway in a close game gets more excitement than a routine success in a blowout. Stronger responses can combine two different cheer sounds and a supporting swell. First downs and conversions use a yardage cheer; defensive stops and takeaways use a distinct defensive cheer. Ordinary successful-play responses hold for roughly 5-8 seconds, then settle over roughly 7-9 seconds. Duration-based running/return reactions use the shared whistle fade instead of stacking another ordinary result envelope. It does not add another volume bonus or unlock the eruption volume allowance.

A close made field goal gets a positive home-crowd cheer, or a cheer for either side at a neutral venue. Existing late winning kicks keep their eruption. A close missed home field goal requests disappointment; an opponent miss gets a defensive cheer.

In a one-score game during the final two minutes of Q2 or Q4, a confirmed third/fourth-down defensive stop gets the larger layered cheer profile and a 10% settling impact. The qualifying atmosphere and fullness rules still apply. An earlier disappointed response to a deep catch cannot cancel the later defensive-stop celebration.

The game keeps its own reactions. The mod never lowers the game's native crowd intensity to make room for an added sound, skips matching cheers already playing, and limits competing native reaction families to two at once. Extra reactions favor the home team, or either team at a neutral venue. Repeated checks of the same play cannot repeat its cheer. Separate reactions are at least four seconds apart, and a new live snap ends the previous result's extra excitement. Eruptions take priority.

When a fresh play result and updated down, possession or measured first-down information are available at the whistle, the reaction starts there. Otherwise it waits for the result to be confirmed; it does not guess from an unfinished play. Additional native disappointment is requested for failed home third/fourth-down conversions, deep home incompletions, home turnovers and visitor catches of at least 15 air yards. Home defensive deep incompletions/breakups and tackles for loss receive a defensive cheer. Deep incompletions require a verified pass that traveled at least 15 yards in the air; an overthrown pass can qualify too. These sounds use the same qualifying atmosphere, fullness and duplicate checks, add no volume bonus or extra shake, and can be disabled separately. The disappointment sound can be subtle depending on the native mix. At neutral sites, either defense can receive the positive response; the mod avoids a stadium-wide home-team letdown for one side's success.

### Successful drives build excitement

Consecutive home first downs gradually strengthen the existing buildup. Third- and fourth-down conversions add more, and useful shorter gains can contribute a little. Failed plays cool it. Drive excitement halves over one eighth of a quarter of game time: 90 game seconds with 12-minute quarters. A score, possession change, halftime or simulation clears it. The effect changes crowd excitement and the size of the existing layers; it does not add another volume bonus or unlock an otherwise unqualified stadium. Neutral venues allow either offense.

A new game clears drive excitement. Waiting at the line with the game clock stopped does not drain it. Simulation does not invent a run of observed big plays.

### Overtime and late scoring drives

The same layered system stays active across snaps and possession changes in overtime once its size-based **85%-to-50% fullness requirement** is met. The roar and swell stay continuous through the action. Cheers still require a special call inside the 10 or the start of a home drive that can win; ordinary overtime calls do not schedule a cheer. There are no recurring late cheer bursts simply because the roar has been running for a while. Pause and replay cleanup still apply.

In Q4's final three minutes, a home offense tied or trailing by no more than eight points can build anticipation once it reaches the opponent's 40-yard line. Its size-based fullness requirement scales from **85% in the smallest reference venue to 50% in the largest**. The effect grows nearer the goal line and as time runs out. At neutral venues, either team's qualifying drive can receive it. This can flow directly into a winning eruption.

There is no separate continuous intro roar or dedicated overtime-only sound layer. The play-call system supplies the opening, overtime and late-drive atmosphere.

### Crowd eruptions

Eruptions require qualifying layered atmosphere and inherit that play's size-based fullness requirement. A qualifying big-game play can erupt at 55% in the largest reference venue; an eligible overtime/late-drive play can do so at 50%. The admitted threshold stays with the celebration through postgame, rather than reverting to an old flat floor. An opening return touchdown in a sparsely occupied stadium keeps the game's own celebration without added eruption or camera shake.

Eruptions can follow these home-team moments. At neutral venues, either team can receive them.

- A last-gasp touchdown pass snapped in the final **15 seconds of Q2 or Q4**, with at least **30 air yards** and a catch within five yards of the goal line. This uses confirmed catch/score data rather than assuming a play name; a short screen taken all the way does not count.
- A go-ahead score in Q4's final **30 seconds**, including field goals, extra points and two-point conversions, or a verified game-winning score.
- The **game's first touchdown within its first five plays**, including the opening kickoff. Offense, defense and return touchdowns count, and that play call must qualify for layered atmosphere. A previous touchdown by either team closes this opportunity. The mod needs to watch the game from the start; it does not guess the count after a resumed game or a stretch of simulation.
- A **60+ yard rushing or passing touchdown** when the score was within eight points before the snap and that play call qualified for layered atmosphere. Letting the lineup sound settle before snapping does not remove this eligibility. The distance comes from the verified line of scrimmage. The mod does not guess a defensive or special-teams return distance from that line. Return touchdowns can still qualify through the other eruption rules.
- An **overtime interception or defensive fumble recovery**, once the change of possession is confirmed, or an overtime winner.
- A game-sealing takeaway while protecting a **one-score lead in Q4's final minute**. With zero or one opponent timeout, the full minute qualifies; with two, 40 seconds or fewer qualify. Three timeouts can still stop the kneels, so that stays a normal takeaway reaction unless time has expired. This works anywhere on the field. Unknown timeout counts do not get guessed. An interception or defensive fumble recovery counts; a routine possession change or turnover on downs does not qualify as a takeaway.

The celebration hits together: two different large native positive cheers, an excited roar and a supporting swell. The roar and swell use a fast half-second attack, including when they take over an existing buildup. The cheers are one initial burst, rather than a delayed series. Matching native cheers are left in place instead of being duplicated. Crowd volume can rise toward a **50% maximum boost**, with the amount also responding to the remaining crowd. Chants keep their separate fullness-based boost of up to 15%.

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

Ordinary eligible play calls choose one stable random shake level from 3-5%. Critical downs choose 5-10%. A special immediate-cheer call uses 5% unless the stronger critical-down range applies. These settings control camera movement, not volume, Stadium Pulse or gameplay home-field advantage.

Ordinary anticipation movement requires a qualifying big game or packed house, except that eligible overtime and late winning drives can also use this gentle range. Close critical downs can receive 5-10% movement at their qualifying atmosphere threshold, without a packed-house label. Third down needs ten or fewer yards and an offensive play; fourth down needs an offensive go-for-it call. Overtime/late-drive ordinary downs stay gentle; their qualifying third/fourth downs can receive the critical range. Field goals and punts can receive ordinary movement. PATs and ordinary kickoffs do not; a big game's opening kickoff can use the gentle level on either kicking or receiving formations and carry it briefly through the whistle. It uses the big-game opening fullness requirement, so packed-house status is not needed. A qualifying 30-yard return has its own separate reaction.

At base duration, a touchdown opens at 20%, settles to 15%, holds through about six seconds, then fades out by eleven seconds. An eruption opens at 30%, settles to 22.5%, holds through about eight seconds, and fades out by sixteen seconds. Verified long plays can lengthen only the settling fade as described above. Celebration and post-play impact movement soften automatically on zoomed cameras. The game keeps its camera angle and zoom.

A safety uses the touchdown's configured opening and sustained strengths, but 70% of its camera duration: about 7.7 seconds at the default settings. Its movement follows the same camera-cut, timeout, replay and zoom rules.

Long gains, interceptions and returns use the separate 10% reaction strength. Sacks, forced fumbles and fourth-down goal-line stops use a short impact/fade at that strength. The strongest applicable effect wins; a live reaction can blend smoothly into a touchdown or eruption. Live camera cuts, timeout scenes and play selection retain an earned reaction's original timer and movement phase. They do not restart or extend it. Each new view gets its own zoom compensation, including during live gain reactions. A new snap clears the previous play's remaining movement. Recorded replays, including the play-selection inset replay, receive no added shake. Pause, interactive replay, a new game or disabled accessibility effects stop added motion.

Camera controls are under Dynamic Gameday Broadcast in the INI. OrdinaryShakeStrengthPercent and CriticalShakeStrengthPercent set the random range ceilings. LineupCheerShakeStrengthPercent controls special cheer calls; DuringPlayShakeStrengthPercent controls live gains and defensive impacts; CatchImpactShakeStrengthPercent controls immediately tackled long catches. PlayReactionFadeSeconds controls their settling time after the whistle; PlayImpactExtraSeconds adds time to the short impact reactions. Touchdown and eruption settings control their sustained strengths, opening impacts and total durations. CameraShake=0 disables all added movement.

### More post-play replays

**Small inset replays:** routine runs and completions can appear in the replay window while you choose your next play, including short gains that normally miss the replay rules. You can keep selecting plays while they run.

**Full-screen replays:** qualifying first-down gains of 15+ yards can use the game's big-play sequence, with presentation graphics and the normal skip control. Touchdowns and other special plays keep their own presentation routes.

**Defensive plays:** sacks favor the full-screen sack sequence, including when you control the offense. Tackles for loss can use the run/pass inset replay instead of the cutaway that normally has no replay. Interceptions and lost fumbles retain their existing replay sequences without an extra duplicate. Fumbles recovered by the offense can use routine run/pass insets where that play type is available; strip sacks can use the sack sequence.

These rules work for either team. They use the game's existing recordings and presentation sequences. Penalties, injuries, safeties, missing recordings and other special scenes can affect what plays. A replay is not forced after literally every snap.

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

Play summaries record the confirmed event and whether added reactions qualified. A numbered live-reaction entry records the gain, clock, fullness and camera cue. Matching audio entries say whether a cheer was sent to the game, was already playing, was skipped because native reaction layers were full, or missed its window during a scene change. Cheers are requested at the confirmed trigger without a scheduled delay. A blocked cheer does not wait in a queue to pop up later. A sent sound event or prepared camera cue confirms dispatch, not what a particular speaker mix or camera ultimately showed. Pause, replay, injury and other presentation changes can interrupt effects.

The main log clears before it would pass its size limit, about 2 MB by default. The status file is overwritten. Crowd-history storage is also bounded.

The plugin checks the game's internal layout before activating and has no EXE hash requirement. Game updates can require a plugin update. This release contains the plugin DLL, the short README and this feature list. Test tools, showcase controls and installer scripts are not included.

Each setting in `Plugins/DynamicGamedayAtmosphere/DynamicGamedayAtmosphere.ini` has a plain-language explanation, including valid ranges and how related switches interact. Updating from an older version refreshes help for known settings once while preserving values, custom disables and unknown settings.


For missing-audio troubleshooting, turn off `Enabled` under **Dynamic Gameday Soundscape**, then restart MMC and the game. With that switch off at launch, DGA leaves its audio hooks uninstalled while attendance and extra replays remain available. Added atmosphere motion depends on Soundscape. Re-enabling Soundscape after such a launch needs another restart. Status includes the game process ID, native audio callback count and native crowd-volume setting to help match a report to the correct session. Failed game-code checks now identify the expected and observed bytes and any recognizable existing detour; they still leave unsupported code untouched.

## Customization and diagnostics

All user controls live in the INI. Attendance options cover formula strength, quarter/transition limits, comeback thresholds, late exits, normal/severe/big-game openings, fill timing, weather/rivalry/FCS adjustments and halftime breaks. Soundscape options cover its master switch, quieting threshold/strength, the small-venue volume cut, volume caps, play-call/packed-house/OT/drive layers, event cutoffs, situational sounds and the shared long-play settling allowance. Broadcast options cover its camera switch, each motion strength/impact/duration, native sparse-crowd shake suppression and each replay expansion. Logging controls its size limit and technical detail.

Every numeric setting has its default, accepted range, units and the effect of increasing/decreasing it beside the option. Use whole numbers where indicated and a period for decimal values. Invalid numbers use the default; valid out-of-range numbers are clamped. Configuration warnings identify the option and the value actually used. Existing supported custom values are preserved within current limits; unknown legacy keys can remain in the file but are not new controls.

Important combinations:

- Attendance `Enabled=0` leaves native attendance and disables the attendance-dependent soundscape. Extra replay settings remain separate. `ArrivalsEnabled=0` only skips arrivals; it does not force a sellout.
- Soundscape `Enabled=0` at launch skips its audio hooks. Turning it back on needs an MMC/game restart. `LayeredAtmosphereEnabled=0` disables added boost/layer effects while independently configured sparse-crowd quieting can remain.
- `MaximumBoostPercent=0` also disables added layers and added reaction motion. Use individual switches/strengths when only one feature should stop. Chants, ordinary crowd, high-leverage and eruption limits are separate totals, not percentages to add together.
- `FormulaStrength=0` disables formula pressure, not every independently configured event. Late-score exits, costly-turnover exits and halftime conversions have separate zero-disable settings. Temporary seat breaks are separate too.
- `CameraShake=0` stops the mod's added motion. The visual-only sparse native-Stadium-Pulse filter has its own threshold; zero disables that filter. Gameplay HFA stays unchanged.
- Long-play extra time is shared by sound and camera. Base reaction fade, short-impact extra time and touchdown/eruption base durations remain separately adjustable. Changes do not keep extending a reaction already earned.

Logs use readable categories such as `[play-summary]`, `[reaction-audio]`, `[departures]` and `[configuration]`. Play summaries show qualification, native sack confirmation, result phase, takeover/timeout context when relevant and the selected camera cue. Status shows the measured reaction distance and additional settling allowance. Repeated identical failures are throttled even if other messages appear between them. The diagnostic cache, crowd history and log sizes are bounded.

Disappointment requests can be subtle in the game's native mix. Offline Dynasty is the verified mode; other offline modes require their own live compatibility check rather than an assumed guarantee.

## Diagnostics (working build)

Settings now live inside `Plugins/DynamicGamedayAtmosphere/`, alongside the Logs, History, Runtime and Tools folders. The launcher preserves current custom values and moves the former loose settings into Backups after successful migration. It waits for the game to close before moving settings that an older running DLL may still use.

`Tools/DgaDiagnostics.exe` opens a small local control window with the current stadium fullness, preset selector, duration, separate sound/camera choices and a Stop button. Roar, swell, cheer, conversion, defensive cheer, disappointment, lineup, critical-down, touchdown and eruption previews use the verified native routes and current strength/volume settings. A preview starts only in a verified offline match while unpaused at the line. Requests expire; the test ends automatically and cannot queue into a later game. A snap, pause, replay or game change cancels it. One-shot cheers finish naturally, and native voice limits/accessibility settings still apply. Previews do not change attendance, scores, clocks or gameplay HFA. They bypass the usual play-situation requirement for controlled comparison, without permanently changing gameplay eligibility.

The native pause-menu fullness widget is not implemented yet. The percentage in the diagnostics window uses the current measured seating value; it is separate from the planned in-game readout. Working previews still need an in-game listening/camera check.

The same tool can save a read-only compatibility report even when DGA did not activate. It records build metadata, loaded-module names and small code samples at the existing DGA addresses. Reports stay in DGA's Diagnostics folder, and nothing is sent automatically. No game EXE, save or full memory dump is included.
