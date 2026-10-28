# Update 3 - candidate

Working build **1.70.0**. Not published; final full-game checks are pending. Update 2 (1.54.0) remains the latest public release.

- **In-game settings:** A Gameday Atmosphere tab after Audio Settings, simpler option names/help, and Stadium Fullness beside Resume in the pause menu.
- **Smoother sound and shake:** Live runs, catches, returns, touchdowns and eruptions hand off without restarting a full-size swell. Added movement is paired with crowd sound, follows appropriate live camera cuts, respects Dynamic Camera Effects and stays out of recorded replay.
- **Deep-pass anticipation:** A fast supporting swell while either offense's verified deep throw is airborne, with no pre-catch cheer/shake or extra volume bonus.
- **More precise reactions:** Gentler skill buildup, a cheer for hurdles, big-hit crowd sound for trucks, and small home-defense third/fourth-down breakup/knockout reactions. Regular broken tackles are sound-only. Hit-stick shake requires a confirmed third/fourth-down stop. Kneels and clock-stopping spikes are excluded from added buildup and sack/stop reactions. Away takeaways clear previous home-positive shake immediately. Delayed forced-fumble detection tracks the original carrier; recovery alone no longer adds shake.
- **Sparse crowds keep their reaction peaks:** Calmer background sound remains, but native big-play peaks can rise above the bed ceiling. The ceiling blends with empty seats and strength instead of switching fully on just below 50%. Crowd scaling and volume limits remain.
- **Quieter games still react:** Verified plays can receive a single native cheer or disappointment sound below the atmosphere gate, with softer skill/pass swells. Shake, fuller roar layers, follow-up cheers, eruptions and volume bonuses keep their existing requirements.
- **Better game stories:** Upset Watch sound for credible home bids against Top 25 visitors and faster beat-the-traffic departures late in a home blowout. Existing packed-house requirements, volume caps and crowd floors remain.
- **More selective eruptions:** The opening-touchdown exception covers only the game's first two tracked plays, including kickoff. A 60+ yard close-game touchdown gets a modest standard-TD accent; decisive winning plays can still erupt.
- **Stadium cannons:** Their own strength control, default 15%, using the shared route for all nine native variants. Normal logs report detected blasts once, with sequence, variant and strength.
- **Clearer troubleshooting:** Exact activation failure stages, throttled retries, organized plugin-owned files, controlled previews and read-only compatibility reports.

**Updating resets old DGA INI values once.** The original is backed up automatically under `Plugins/DynamicGamedayAtmosphere/Backups`. Current defaults replace the old values; reapply desired settings afterward. Later launches and routine updates preserve those new settings. Close MMC and the game before replacing the plugin DLL, then restart both.

Offline Dynasty is the verified mode. Other modes, every cannon venue and every rare play are not implied to have been tested. See the [current checklist](../DIAGNOSTICS.md) for the distinction between automated coverage, observed game signals and confirmed live behavior.
