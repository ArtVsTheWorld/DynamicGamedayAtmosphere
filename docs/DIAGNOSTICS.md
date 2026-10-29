# Update 3 testing and diagnostics

Current candidate **1.98.0**, staged only after its build/host checks pass. Update 2 (1.54.0) remains the public release. Confirm the installed runtime in status.txt; staging does not install it. Earlier tests do not establish live acceptance of later changes. Historical evidence is retained in [validation history](reviews/Update3-validation-history-through-1.97.md).

## First check for 1.98

Use matched regular-season day/night games with the same teams, venue, weather, quarter length and scoring path. Fresh launches are required for revised peak turnout; resume tests validate restored history. Night means verified stadium-local kickoff at or after 19:00.

- Atmosphere thresholds are 90% of their daytime values, with an absolute 50% measured-fullness floor. An 80% gate becomes 72%; 55% becomes 50%. Verify status reports the effective night gates. Actual packed-house classification, occupied-seat strengths, voice/volume budgets, native HFA cutoff and cannon behavior remain separate.
- During Q3 the night/scoring pressure ramps in, reaching its full effect at Q4. Night adds up to 20% pressure; a projected full-game total of 24 or less adds up to 15%, while 72 or more reduces it by up to 10%, with smooth transitions around 48. Pace is game-clock normalized. These multiply existing pressure/approach speed, not remaining crowd. They do not create a score-triggered group or lower existing floors.
- One-score games receive only a quarter of those modifiers; credible comeback/upset protection still owns admission. Check a low-scoring close game against a close shootout, and a low-scoring 21-point loss against a high-scoring 21-point loss. No refill, pause/replay accumulation or retroactive deduction across Super Sim jumps.
- Bowl, CFP and championship atmosphere/departure policies do not receive these modifiers. Neutral regular-season games receive symmetric night/scoring pressure; side-specific home-loss traffic remains excluded.

## Representative games

| Test | Main checks |
| --- | --- |
| Lower-demand daytime game, kept close | Revised peak can remain sparse after arrivals finish. Sound-only reactions below layered admission; modest permanent losses; temporary halftime returns. |
| High-demand large venue at night | Ordinary versus exceptional opening band, effective gates, occupied-seat strength as fans arrive, 13-to-20-yard gain buildup and scoring handoffs. Use a venue besides Kyle Field too. |
| Severe-late outdoor home in rain, developing into a rout | Utah/UCLA/Miami/Georgia Tech/Maryland baseline; late arrivals, weather reduction, halftime conversion and late traffic without refills or repeated deductions. |
| Neutral regular-season rivalry | Both teams' qualifying reactions at 75% crowd-motion strength; symmetric crowd policy; manual/Slow Sim transitions. |
| Bowl/CFP and championship spot checks | Native peak preserved; 70-80% and 80-90% opening fractions of peak, protected floors and no new regular-season modifiers. |

A rivalry with either side Top25 or both Top50 qualifies for the expanded opening/arrival rules. Weak #135-versus-#137 rivals do not get that shortcut. Legacy attendance/conversion and packed-reaction classification distinctions remain under review; no change to those policies is implied by 1.98.

## Attendance checkpoints

1. At kickoff, record EA/revised peak, actual capacity fullness, fraction still arriving and local kickoff. Confirm revised turnout applied rather than an EA fallback. Demand uses school grades and rank; rank alone does not dictate turnout.
2. Opening fractions are of expected peak: ordinary 50-60% day, 55-65% night; severe-late baseline 45%/50%; ordinary marquee ceiling70%, exceptional cap80%. Only weather can go below40%, with35% minimum. Postseason bands stay protected.
3. Arrivals finish after50/30/75/20% of Q1 for ordinary/marquee/severe/postseason. In five-minute quarters that is2:30/3:30/1:15/4:00 remaining. Changes wait for hidden windows; arrivals never restore permanent exits.
4. Halftime departures begin during the last20-30% of Q2. Most temporary fans return during the first15-25% of Q3; the smaller wave finishes25-35% into Q3. Close marquee/packed games hold more fans until halftime. Permanent and temporary absences must stay distinct.
5. Midgame Save: pause during arrivals, save/reload and compare the same clock. No paused drift or invented new opening. Keep history intact. A later-quarter save must not restart arrivals.
6. Replays: older clips may use a fuller historical crowd; leaving them restores live attendance. Streamed views/postgame use the proper crowd. The 1.97 seating pattern is accepted on one Kyle Field preview, but installed arrivals/replay/resume and other venue shapes still need checks.
7. At late Q4/final, compare the score's history and weather rather than expecting a universal final percentage. Floors/caps are percentages of peak; visible fullness is percentage of mapped capacity.

## Reactions and broadcast

- Below layered admission, earned sound-only reactions must remain audible without a new DGA event shake, passive roar, followup or volume premium. Native HFA can still shake above its independent50% cutoff; cannons are independent.
- Native big-play excitement should rise above the calmer sparse background without an abrupt collapse at50% fullness. Queued/registered audio logs are not listening proof.
- Run/scramble/catch-and-run buildup spans13-20 yards. Spins, jukes and ordinary broken tackles must not start shake. Check airborne anticipation for both offenses.
- Five-yard-or-greater defensive losses share one motion route; shorter sacks retain audio/replay classification. Retest a strip sack followed by an ordinary minus-two-yard run and genuine consecutive sacks.
- Home-defense forced fumbles can react on any down; defensive returns use interception-style motion, own pickups do not. Away takeaways at a home venue cancel positive home motion. Neutral ownership follows either side.
- Dedicated goal-line motion requires fourth down; special high-leverage stops require Q4. Ordinary third-down incompletion alone does not trigger the contact stop impact. Take contact/knockout/breakup opportunities naturally.
- Qualified home showboating builds slowly; away showboating requests boos. Cached/post-whistle tags must not repeat it.
- Check softer pre-punt/home-critical buildup, snap fade, return/gain-to-TD handoffs, and bounded final-play carry. Final menus stop effects; no late restart from postgame highlights.
- Manual TD/takeaway: let full-screen presentation finish, then inspect the separate inset. The user accepted current intermittent replay stability; it is an accepted limitation, not proof every native route is fixed. Slow Sim has no play-call inset destination; current mode-aware full-screen behavior still needs rendered confirmation.
- The settings menu was accepted. Check reset/persistence and native accessibility/DGA camera-off behavior if changing settings. No crowd-animation changes were adopted after the replay-offset A/B showed no noticeable benefit.

## Reporting

Use normal logging (Debug=0) for routine games. Note quarter/clock, side, down, result and what was visibly/audibly wrong. Preserve the complete Logs folder plus relevant status. The status gate and request explain eligibility; neither proves rendered/audible output. No files upload automatically.

Install only with the game and MMC closed, preserving INI/save/history files. A build version line alone is not activation proof. See [candidate scope](releases/Update3.md), [build instructions](BUILDING.md) and [support notes](../README.md#problems).
