# Update 3 / 2.0.7 test candidate

First validate the new optional mode and overtime coverage. At normal play selection, turn Attendance > 2020 Mode on: expect empty stands, zero pause-menu fullness, no crowd or band audio, and clean commentary/field sounds. Watch a play and replay, then turn it off and verify the current crowd/audio returns. Repeat in Play Now if convenient. Normal Attendance/Soundscape switches should not defeat this independent mode. It defaults off.

For qualifying overtime, check a plain third-down stop without fresh knockout proof, a missed opponent field goal, and successful/failed two-point attempts. Expect one ownership-correct reaction per result, a modest 10% crowd-shake lift and steadier background across kicks/tries. At neutral sites either side can react. Away successes/home failures at a home venue should receive disappointed crowd sound instead of a positive home shake. Do not use native requests as proof of audible or visible output.

Keep Additional Replays on for a short normal sequence, with no whistle hitch and a TD full-screen/inset check. Preserve logs/status and exact play/clock if anything fails. The broader attendance/audio checks below remain useful; their original fixtures and policies are retained.

These fixtures are verified in the supplied DYNASTY-DGA183 schedule,2026. The save remains unchanged. Use fresh launches for the starting-attendance formula; use a Midgame Save separately for restore validation. Fullness means the logged **Audio stadium fullness / mapped seating**, not Crowd target (% of the original peak) or listed real-world capacity.

| Week | Game | Purpose |
|---|---|---|
|1|FCS East at Buffalo|Small lower-demand crowd; opening arrivals, quiet background, preserved reaction peaks and the mix-strength setting. The saved kickoff is evening; confirm local night detection in the runtime log.|
|1|Clemson at LSU|Large evening crowd; the earlier vanilla crossover, fuller/dense audio above vanilla and the30% gain cap. Keep commentary/field sounds clean.|
|2|FCS Midwest at Akron|Small noon crowd, low-demand sparse extreme; test whether the below30% bed gets too quiet. Watch late-game emptying and compare with Buffalo.|
|2|Oklahoma at Michigan|Top25 pairing and large afternoon venue; opening/layered atmosphere, fuller audio, live-play/TD handoffs, and attendance retention in a close game.|

## First checks

1. Close game and MMC before swapping the staged adapter DLL. Keep your normal settings for the comparison. Confirm runtime2.0.7 in status, 2020 Mode off, Crowd mix = eight native buses, Old excitement limiter = removed, and Legacy crowd parameter writes = disabled. Do not load private research helpers.
2. Start with Buffalo. At kickoff and lateQ1 note mapped fullness, occupied seats, crossover and Beds/Reactions/detail dB. Arrivals should increase the crowd without an abrupt snap-time silence. A normal first down or defensive stop should still have dynamic native reactions. Commentary, field sounds, referee and music should stay clean. Below layered admission, added reactions remain sound-only: audio alone does not unlock shake.
3. Let halftime and an automatic replay play without skipping. Compare current/recorded crowd behavior. Ordinary pause and interactive replay retire owned mixer overrides, then return to the current mix afterward. Check manual play and one Slow Sim segment; mode-aware replay behavior remains unchanged. Native dispatch alone is not proof of an audible reaction or a visible replay.
4. For Michigan/LSU, stay through arrivals if possible. A genuinely packed large crowd can reach+30% signal gain. A small venue at100% is intentionally a smaller boost (about+15.5%). Check a TD, turnover, big gain and a late critical down for continuous handoffs and unchanged ownership/shake admission. Keep volume sliders unchanged between comparisons.
5. At Akron, use a large fourth-quarter deficit or an ordinary low-scoring night comparison if readily available. Fans should leave through safe windows; the sound should thin with occupied seats, not jump to silence. Midgame Save should restore the same crowd plan, arrivals and audio level rather than a fresh kickoff.

The current nonzero audio floors are40% background/chant,55% collective reactions and95% individual detail. The smallest venue at25% fullness requests50% background. Larger venues still cross vanilla earlier and the largest packed venue reaches+30%. The old steep lower tail is gone; see [audio ranges](AUDIO-MIX-RANGES.md). Individual-detail routing/effective audibility remains unresolved.

## Record when something looks or sounds wrong

Game/quarter/clock, side/down/play result, mapped fullness/occupied count, what you heard or saw, and whether you skipped, paused, opened replay or returned from Sim. Keep the normal DGA logs/status. The agent did not install the candidate or modify the save.


## 2.0.2 focused checks

Start with a fresh small-venue regular-season game. The starting-attendance entry should show access points (up to8), event multiplier, breakout points, native school state IDs and senior-day flag availability. Same-state schools get a local bonus; unknown/FCS location data does not invent one. A home opener/night game can draw more fans but need not sell out. Repeated launches of the same fixture should not reroll bonuses. Compare an available final true-home regular-season game: expect a10% senior bonus only when EA sets its senior-day flag. Postseason/resumed games keep their existing peak policy.

For reactions, use a short fourth-down conversion with a lead larger than8 and qualifying layers; it should now have one impact. A confirmed defensive fumble recovery should get a fallback impact if the live detector missed it. Own recoveries must not gain recovery motion. Live verified returns keep their interception-style continuation, and existing force/return impacts should not duplicate. Not every hit stick gets shake; DGA cheers should now request over matching/multiple native voices; repeated callbacks must not resend the same event. Listen for clean output on major plays.

If an end-of-play hitch recurs, note the play/clock and preserve status before exiting. New fields show DGA atmosphere/attendance callback maximum wall time and counts >=4ms/16ms, plus deferred log overflow. Logging is now drained by the worker. Those counters do not measure FPS, GPU/driver stalls or all plugin/native work. The new15% departure capacity floor must never refill existing empty seats or raise an intentionally sparse kickoff.
