# Update3 / 2.0.0 test candidate: Week1 and Week2

These fixtures are verified in the supplied DYNASTY-DGA183 schedule,2026. The save remains unchanged. Use fresh launches for the starting-attendance formula; use a Midgame Save separately for restore validation. Fullness means the logged **Audio stadium fullness / mapped seating**, not Crowd target (% of the original peak) or listed real-world capacity.

| Week | Game | Purpose |
|---|---|---|
|1|FCS East at Buffalo|Small lower-demand crowd; opening arrivals, quiet background, preserved reaction peaks and the mix-strength setting. The saved kickoff is evening; confirm local night detection in the runtime log.|
|1|Clemson at LSU|Large evening crowd; the earlier vanilla crossover, fuller/dense audio above vanilla and the30% gain cap. Keep commentary/field sounds clean.|
|2|FCS Midwest at Akron|Small noon crowd, low-demand sparse extreme; test whether the below30% bed gets too quiet. Watch late-game emptying and compare with Buffalo.|
|2|Oklahoma at Michigan|Top25 pairing and large afternoon venue; opening/layered atmosphere, fuller audio, live-play/TD handoffs, and attendance retention in a close game.|

## First checks

1. Close game and MMC before swapping the staged adapter DLL. Keep your normal settings for the comparison. Confirm runtime2.0.0 in status, Crowd mix = eight native buses, Old excitement limiter = removed, and Legacy crowd parameter writes = disabled. Do not load private research helpers.
2. Start with Buffalo. At kickoff and lateQ1 note mapped fullness, occupied seats, crossover and Beds/Reactions/detail dB. Arrivals should increase the crowd without an abrupt snap-time silence. A normal first down or defensive stop should still have dynamic native reactions. Commentary, field sounds, referee and music should stay clean. Below layered admission, added reactions remain sound-only: audio alone does not unlock shake.
3. Let halftime and an automatic replay play without skipping. Compare current/recorded crowd behavior. Ordinary pause and interactive replay retire owned mixer overrides, then return to the current mix afterward. Check manual play and one Slow Sim segment; mode-aware replay behavior remains unchanged. Native dispatch alone is not proof of an audible reaction or a visible replay.
4. For Michigan/LSU, stay through arrivals if possible. A genuinely packed large crowd can reach+30% signal gain. A small venue at100% is intentionally a smaller boost (about+15.5%). Check a TD, turnover, big gain and a late critical down for continuous handoffs and unchanged ownership/shake admission. Keep volume sliders unchanged between comparisons.
5. At Akron, use a large fourth-quarter deficit or an ordinary low-scoring night comparison if readily available. Fans should leave through safe windows; the sound should thin with occupied seats, not jump to silence. Midgame Save should restore the same crowd plan, arrivals and audio level rather than a fresh kickoff.

The30% small-venue bed is the final user-directed-9.5dB, with matching chants; reactions-7.75dB and individual detail+0.75dB. The50/70/90% anchors retain the accepted listening results. Individual detail routing/effective gain remains unresolved after the high-gain test; do not expect a newly discovered sparse-voice track.

## Record when something looks or sounds wrong

Game/quarter/clock, side/down/play result, mapped fullness/occupied count, what you heard or saw, and whether you skipped, paused, opened replay or returned from Sim. Keep the normal DGA logs/status. The agent did not install the candidate or modify the save.
