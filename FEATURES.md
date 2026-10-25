# Dynamic Gameday Atmosphere
By Ace | Version 1.26.1 | Offline CFB 27

The crowd fills in, reacts to the matchup and thins as the game gets out of reach. Big games have a fuller sound, sustained opening and overtime roars, and longer celebrations for winning moments. More plays also get the game's own replay presentation.

## What the crowd percentages mean

Attendance targets and permanent-departure percentages are based on the crowd the game originally planned to show. If the game chose 40,000 fans, a 50% target means about 20,000 of those fans, even if the stadium holds 80,000. Temporary halftime break percentages refer to the fans still present.

The game still chooses the original attendance. This mod changes how much of that crowd is visible over the course of the game. Attendance rules apply to the stadium crowd as a whole.

Fans who permanently leave stay gone. First-quarter arrivals, temporary halftime seat breaks and viewing an earlier replay are separate from permanent departures.

## Where fans stay and leave

Thinning follows the seating layout. Seats farther from the field generally empty sooner, while closer seats keep more fans. Density tapers gradually across the rows. Areas around the stadium fill and empty at different rates, with stronger differences between fuller and thinner areas, including the closer seating. Small groups and individual stragglers remain among the emptier seats. The variation stays fixed for the game, so it does not reshuffle on each update or when viewing a replay.

This changes where the remaining fans sit, not how many the attendance rules allow. It never fills seats the game originally left empty.

Arrivals, permanent departures and temporary halftime absences share the same seating pattern. Earlier replays use that pattern with the attendance recorded for the play; leaving the replay restores the live crowd. Resumed games rebuild the pattern from the stadium's seats and use the restored or estimated attendance.

The pattern is prepared during pregame and stays in place for postgame stadium shots. If a cutscene loads a fresh crowd renderer, it receives the same final attendance. Automatic highlights can still show a fuller crowd when replaying an earlier part of the game.

Using Midgame Save to record a normal game preserves the smaller crowd prepared during pregame. Loading a saved scenario still restores or estimates the crowd for that scenario.

If the mod cannot verify a crowd's seating layout, that crowd keeps the previous scattered thinning. Small groups of people beside the field also keep their usual treatment.

## First-quarter arrivals

The smaller opening crowd and its seating pattern are prepared together during pregame, as soon as the stadium crowd is ready. Setup continues if the crowd finishes loading after the intro begins, and carries through kickoff. Quitting one game and loading another starts a fresh setup. These are the baselines before matchup and weather adjustments:

| Arrival group | Schools | Starting crowd | Baseline fill point with 12-minute quarters |
| --- | --- | --- | --- |
| Severe late arrivals | UCLA, Miami, Georgia Tech, Maryland | 50% | 3:00 remaining in Q1 |
| Moderate late arrivals | USC, Rutgers, Boston College, California, Stanford, Ole Miss, LSU, Florida State | 60% | 6:00 remaining in Q1 |
| Standard arrivals | Every other school | 70% | 9:00 remaining in Q1 |

Miami means the Hurricanes. Miami (OH) uses standard arrivals. Neutral venues use the standard arrival group. Playoffs start from a 65% baseline and the national championship starts from 75%, before adjustments.

The starting crowd and arrival speed respond together:

- Rivalries and evenly matched teams bring fans in earlier. Strong, closely matched teams receive a larger boost.
- Rain or snow lowers the starting crowd and slows arrivals.
- Large ranking gaps lower early attendance whichever team is hosting. No. 25 against No. 127 attracts a smaller opening crowd than a closely matched game, even when No. 127 is at home.
- FCS opponents lower early attendance too.

The ordinary starting adjustment ranges from 20 percentage points below the baseline to 20 above it. An FCS opponent subtracts another five points after that limit, in addition to its normal early-attendance penalty. Exceptional matchups can override a school's late-arrival habit: a dry rivalry can open around 92%, with similarly full playoff crowds and up to 95% for the championship. Weather can still lower these starts. The starting target has a hard minimum of 35%. Turning off the big-game arrival bonus also turns off its exceptional opening boost.

Arrivals are stronger early, followed by a smaller tail of stragglers. Matchup and weather have a milder effect on fill speed than on the opening crowd. An extreme first-quarter blowout can delay the last few arrivals slightly.

Other quarter lengths use the same proportions. Matchup and weather adjustments can move the fill point earlier or later. Actual crowd changes settle at play selection or a break, so reaching a time on the clock does not make fans appear while you are lined up.

Normal and Fast Super Sim keep attendance in step with the score and clock while the play-by-play screen is open. Arrivals, departures and temporary halftime absences follow the simulated game's progress. Slow Sim keeps the usual selection and break timing while the field is visible. Simulation does not refill seats that fans permanently left.

Arrivals fill toward the game's original crowd. They never fill seats the game originally left empty.

## How fans decide to leave

Departures use a formula that follows the score, time remaining, rankings, matchup, weather and home team's fortunes. It also remembers how the game unfolded. A lead that suddenly grows late in the game produces different attendance from a blowout that has lasted all afternoon.

**Close games keep more people interested.** Routine departures remain modest when the result is still in doubt. Rivalries and games between two Top 25 teams get extra patience. Both teams must be ranked for the ranked-matchup benefit.

**Home wins get more patience.** Fans are more willing to stay for a comfortable home victory than an equally large home deficit. This benefit becomes smaller as the opponent gets weaker, but some basic home-win patience remains even against FCS opponents. Special rivalry patience still fades in those mismatches. Comfortable wins against similarly ranked teams get more patience than wins against vastly weaker opponents.

**Ranking gaps matter throughout the rankings.** No. 1 against No. 50 differs from No. 1 against No. 127. A gap near the top of the rankings also has more weight than the same numerical gap farther down. Larger mismatches increase departure pressure when the favorite is winning. The game supplies the rankings; missing FBS rankings are not invented. An FCS opponent without a ranking is treated as below the supported FBS ranks.

**Weather encourages earlier exits.** Rain and snow increase ordinary departure pressure. Special-matchup patience generally takes priority over the usual weather multiplier, although that patience fades against a vastly weaker opponent. Weather can still lower the minimum crowd targets listed below.

### Halftime is the main departure window

Departures build across Q2 toward situation-based halftime targets, instead of forcing every ordinary game to lose at least 10%. A close defensive game receives the same basic consideration as a close high-scoring game. Smaller groups can continue leaving in Q3 and Q4.

| Halftime situation | Permanent departures: share of original crowd | Temporary seat breaks: share of fans still present |
| --- | --- | --- |
| Close rivalry or both teams ranked | 0–5% | 10–15% |
| Close ordinary game | 5–10% | 15–20% |
| Margin of 17–24 points | 10–20% | 15–25% |
| Margin of 25–34 points | 20–30% | 15–25% |
| Margin of 35 or more | 30–40% | 10–20% |
| Playoff or championship | 0–5% | 10–15% |

These are general targets; comparable-opponent home wins can lose fewer people. They are not a guaranteed cut at one moment. A margin of 9–16 points bridges the close-game and blowout targets. Home wins, weather, protection, the way the score developed, crowd floors and safe update opportunities affect the result. A small amount of game-to-game variation keeps identical scorelines from always producing identical crowds; it stays fixed throughout the game and survives a matching saved-game resume.

Moderate home deficits are treated more gently during Q3, with departures building faster in Q4. The change from a 21-point game to a 24-point game is gentler, with stronger exits building across a wider range of margins. Large blowouts still generate stronger pressure.

**Late blowouts can catch up.** In regular-season Q4, extra pressure gradually comes in across margins of 17–24 points when attendance is still high for the score. Larger margins and less time remaining raise that pressure. It builds gradually rather than removing a fixed batch when a touchdown is scored; with 12-minute quarters, it responds over roughly 90 seconds of game time. Other quarter lengths scale with it.

Home-winning patience, matchup and weather adjustments, upset and comeback protection, crowd floors and quarter limits still apply. Fans already gone are counted, so a crowd that has thinned enough does not get another catch-up cut. Nothing is added for the earlier close part of the game. Actual changes still wait for play selection, breaks or verified offscreen Super Sim. Playoff and championship games keep their existing departure rules.

### Upsets and comebacks

An underdog staying competitive with a Top 25 opponent can slow departures. The benefit develops as the game progresses, with the strongest stage for an underdog leading a Top 10 opponent in Q4. A larger ranking gap creates a stronger upset effect.

Recovering at least 14 points after falling 21 behind can pause departures while the comeback remains alive. The protection can end if the rally deteriorates or the team builds a comfortable lead. A multi-score deficit with too little time for a realistic recovery can also end protection, even with home possession. Remaining timeouts, possession and field position help decide this when the game provides them. Special home retention benefits fade against vastly inferior opponents.

These protections keep additional fans from leaving; they do not bring departed fans back.

### Minimum crowd targets

| Lowest permanent-departure target | Dry conditions | Rain or snow |
| --- | --- | --- |
| Ordinary game | 25% | 15% |
| Playoff | 60% | 50% |
| National championship | 80% | 70% |

A winning Top 25 home team also has a 35% floor, including against FCS opponents and in bad weather. A higher postseason floor still wins. If more fans left earlier while the team was losing, taking the lead does not refill those seats. Neutral venues use their regular floors.

Playoff and championship crowds are more patient, but multi-score deficits can still produce departures. During Q4, fans increasingly try to beat traffic when the result looks settled. The effect grows with the lead and time elapsed in the quarter. Championship crowds remain more patient than playoff crowds, and close games do not receive this extra traffic effect. These floors limit further departures and halftime seat absences. They do not override the smaller opening crowd or the game's original empty seats.

### Protection against sudden huge drops

Permanent departures are capped in each quarter at 45 percentage points of the original crowd in ordinary games, 20 in playoffs and 10 in the championship. For example, a 45-point cap permits a change from 90% to 45%; it is not a 45% cut applied repeatedly to whoever remains.

Late-score departures share that cap. Departures blocked by the cap are discarded rather than waiting to hit all at once later. Temporary halftime absences have their own limits.

Departure decisions follow the score and game clock even while the field is visible. The crowd itself waits for play selection, a break or verified offscreen Super Sim to change. Long stretches without opening play selection no longer erase departures or change the formula simply because fewer updates were possible.

Arrivals and halftime seat changes normally share an allowance starting at four points per opportunity, growing with time since the previous opportunity up to 16 points by default. When permanent departures have already built beyond that allowance, the next hidden opportunity can catch up to their calculated target. Quarter caps and crowd floors still apply, so this does not create another departure penalty. Quarter, two-minute and late-score calculations cannot repeatedly renew an opportunity's allowance. Initial pregame setup applies the opening target directly. Comeback protection stops new departure decisions; it does not undo decisions already made before the comeback. Offscreen Super Sim can catch up immediately while still respecting the quarter cap and crowd floor.

### Late-game exits and overtime

Q4 has one additional departure checkpoint at the next safe break after 2:00. It follows the same formula and protections. Quarters lasting two minutes or less skip this checkpoint.

A live opponent score inside the final two minutes can send another five percentage points home if the home team falls at least nine points behind with one timeout or none. Extra points do not repeat the penalty. This extra rule does not run at neutral sites or in postseason games.

Verified possession and remaining home timeouts also shape late Q4 pressure. Having the ball, especially near the end zone, buys some patience. Giving possession back to the opponent while multiple scores behind with few timeouts encourages exits. If that information is unavailable, the normal score-and-clock formula applies. Neutral venues receive extra overall patience because supporters of the winning team still have reason to stay; the mod does not assume it knows each section's allegiance.

Ordinary departures stop in overtime. The existing crowd remains rather than being treated as another regulation quarter.

## Temporary halftime seat breaks

Some fans leave their seats before halftime and return afterward. With 12-minute quarters, these breaks build during the final two minutes of Q2 and return over the first three minutes of Q3. The situation-based percentages above apply to the fans still present, so an already depleted crowd sends fewer people to the concourse. The full planned break is prepared at halftime even if there was no late-Q2 play-selection screen. Safe-update limits and crowd floors still apply. After that peak, temporary absences can only shrink, even if the score or weather changes.

Both windows scale with quarter length. Changes mix with ordinary between-play departures and wait for play selection or breaks. Permanent departures stay empty, and the applicable crowd floor still limits the seat break.

## Crowd sound and atmosphere

A big game means a rivalry, a matchup between two Top 25 teams, a playoff or the national championship. Kickoff time is not a big-game trigger.

### Fuller big-game ambience

Big games get a stronger continuous crowd rumble between plays, alongside a general crowd-volume lift. Close games get the strongest effect, particularly early and late. The lift eases as the score gets out of hand and fewer fans remain.

Steady ambience receives more of the available boost than general reactions, helping a packed stadium sound involved between big plays. Ordinary games keep their normal baseline, with the special overtime and winning-moment effects still available.

### Sustained opening roar

Big games can begin roaring during the intro and continue through the opening offensive possession. The added roar normally fades over about 20 seconds when that opening phase ends. A five-minute real-time safety limit prevents it from running indefinitely.

The roar uses the game's own crowd sound. Its excitement is raised as well as its volume, giving it a fuller character.

### Overtime atmosphere

Overtime gets a continuous excited roar, including between snaps and through possession changes. It follows the same pause, replay and scene-change safeguards as the opening roar.

### Winning eruptions

Home go-ahead scores in the final 1:30, game-winning scores and overtime winners can trigger an extended eruption. Field goals count. At neutral venues, either team can receive the effect.

The celebration combines a native crowd cheer, a sustained roar and a stronger volume lift. During play it lasts about 45 seconds, with a gradual release. If it carries into a victory, the roar continues through the postgame presentation until the stats/highlights menu opens. An opponent score can end a home celebration.

A close home upset over a Top 25 opponent can also trigger a final-whistle celebration when the ranking gap is large. It does not start an extra eruption if one is already carrying through. Neutral venues can celebrate the winning underdog on either side.

The added cheer is not duplicated when the game is already playing the same reaction.

### Volume limits and smaller crowds

All public volume controls and status readings use percentages. At the default settings, ordinary crowd and ambience boosts share a maximum of about +58.5%. Winning eruptions can reach about +99.5%. The layers share one limit, so boosting the rumble and reactions together does not add both limits on top of each other.

Volume percentages work like a volume knob. What you hear also depends on your in-game settings and speakers.

General crowd quieting starts below 80% attendance and changes ease in over six seconds. Temporary arrivals and halftime absences can affect this level too; opening hype softens their effect during the big-game opening.

Chants get no extra reduction until permanent attendance falls below 70%. Their separate cut then grows smoothly, reaching its default maximum at 25%. Temporary first-quarter arrivals and halftime seat breaks do not add that separate chant cut.

| Permanent crowd remaining | General crowd-volume cut | Extra chant-volume cut |
| --- | --- | --- |
| 100% | 0% | 0% |
| 80% | 0% | 0% |
| 70% | 2.5% | 0% |
| 50% | 7.5% | About 21% |
| 25% | About 13.8% | 50% |
| 10% | 17.5% | 50% |

These are the default cuts before any atmosphere boost and with no additional temporary seat absences. The chant cut applies after the general crowd level. A completely empty crowd is silent with the default audio settings. You can change or disable the cuts in the settings file.

The plugin does not boost commentary, referees' spoken calls, the stadium announcer, bands or stadium music. Team-specific sounds follow the audio group the game assigns them to. Gameplay home-field advantage and Stadium Pulse remain controlled by the game.

## More post-play replays

**Small inset replays:** routine runs and completions can appear in the replay window while you choose your next play, including short gains that normally miss the replay rules. You can keep selecting plays while they run.

**Full-screen replays:** qualifying first-down gains of 15+ yards can use the game's big-play sequence, with presentation graphics and the normal skip control. Touchdowns and other special plays keep their own presentation routes.

**Defensive plays:** sacks favor the full-screen sack sequence, including when you control the offense. Tackles for loss can use the run/pass inset replay instead of the cutaway that normally has no replay. Interceptions and lost fumbles retain their existing replay sequences without an extra duplicate. Fumbles recovered by the offense can use routine run/pass insets where that play type is available; strip sacks can use the sack sequence.

These rules work for either team. They use the game's existing recordings and presentation sequences. Penalties, injuries, safeties, missing recordings and other special scenes can affect what plays. A replay is not forced after literally every snap.

The `Replays` section in the settings file lets you change the full-screen yardage cutoff, turn off either replay expansion, or disable the defensive routing.

## Attendance in Instant Replay and highlights

Instant Replay, the postgame highlights viewer and automatic presentation clips use attendance recorded near the selected play. This includes halftime and postgame replay sequences. An early highlight can show a fuller crowd than a late highlight, and changing the selected play updates the crowd accordingly.

Leaving replay restores the current match's crowd. Replaying an old comeback does not bring fans back into the live game. Only seats emptied by the mod can be restored.

This history changes the visible fans. Sound continues to use the current match's atmosphere. Automatic presentation clips preserve an active winning celebration; opening the interactive replay viewer or pausing releases the added roar.

## Dynasty Midgame Save support

Resumed games and scenarios are treated as new starts, including when the game reuses parts of the previous match.

If a recent attendance record matches the teams, time, score, quarter length and matchup, the mod restores it. Otherwise, it estimates a starting crowd from the resumed score, time, rankings and weather. First-quarter scenarios also pick up the arrival curve at that point. Status and logs say whether attendance was restored or estimated.

The saved history keeps the last eight games in `Plugins/DynamicGamedayAtmosphere/History/`. Older entries are replaced automatically. Missing play history cannot be reconstructed exactly, and a resumed scenario records its own new replay history from that point onward.

## Timing, settings and logs

Normal attendance changes wait for play selection or breaks. The crowd stays fixed while you are lined up, during snaps and through the same play-selection screen. A clock reaching an arrival or departure threshold schedules the change for a suitable break.

Arrivals, ordinary departure timing and halftime seat-break windows scale with quarter length. The Q4 two-minute checkpoint and final-90-second scoring reactions use the actual clock. Audio fades and celebration durations use real seconds.

Updates preserve supported settings and add new options automatically. Restart both MMC and the game after replacing the plugin so the new version is loaded. Keep one copy of the plugin installed.

Logs and status are in `Plugins/DynamicGamedayAtmosphere/Logs/`. They show the version, crowd target, measured crowd, renderer status, team IDs, supplied rankings, rivalry status and relevant events. A target is the mod's requested crowd; the measured figure is what the game actually shows. Native hiding and rounding can make them differ slightly.

The main log clears before it would pass its size limit, about 2 MB by default. The status file is overwritten. Crowd-history storage is also bounded.

The plugin checks the game's internal layout before activating and has no EXE hash requirement. Game updates can require a plugin update. This ZIP contains the plugin DLL, the short README and this feature list. Test tools, showcase controls and installer scripts are not included.
