# Dynamic Gameday Atmosphere

By Ace. A College Football 27 plugin for MMC.

This branch contains Update 3 diagnostics work (1.61.0). Update 2 is still the latest public release. The settings path below describes the working build; Update 2 keeps its INI directly in Plugins.

Fans arrive late, take halftime breaks and leave as games get out of reach. Crowds react to big plays, full stadiums get more involved sound, and winning moments get bigger celebrations.

**[Download the latest release](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/releases/latest)** · [Full feature list](FEATURES.md)

## Install or update

1. Close the game and MMC.
2. Download the ZIP from Releases.
3. Copy `DynamicGamedayAtmosphere.dll` from its `Plugins` folder into MMC's `Plugins` folder.
4. Start MMC and launch the game through it.

Keep your existing INI when updating. Settings migrate automatically. Restart both programs after replacing the DLL.

## Settings

Edit `Plugins/DynamicGamedayAtmosphere/DynamicGamedayAtmosphere.ini`. Each option explains its default, range and what it changes.

- **Attendance:** arrivals, departures, halftime seat breaks and seating patterns.
- **Soundscape:** crowd sound, anticipation, play reactions and celebrations.
- **Broadcast:** camera movement and extra replay opportunities.

The game still chooses the original peak attendance. Sound uses how full the mapped stands actually look. Gameplay home-field advantage stays unchanged. Offline Dynasty is the verified mode; other modes are not guaranteed.

Deep throws use a fast native swell while the pass is airborne. It works alongside lineup buildup, settles after the pass ends and adds no pre-catch cheer, shake or volume bonus. `PassFlightAnticipation=0` disables it.

The working build adds a **Gameday Atmosphere** tab at the far right of the game's Settings menu. Its attendance, soundscape, broadcast and logging controls save to the existing DGA INI. The tab still needs its integrated in-game navigation and visual check after installation. A long-run touchdown also carries its earned crowd level into the celebration instead of restarting a full-size native swell's attack.

The working build also adds **Upset Watch sound** for credible home underdog bids against Top 25 visitors from Q2 onward. Ranking gap, score and game stage can qualify roars/swells and earned reactions below the normal sound gate, never below 50% actual fullness. Packed-house requirements and volume caps stay unchanged. `UpsetWatchAtmosphere=0` disables this route.

The working build shows **Stadium Fullness** beside Resume in the regular pause menu. This is how full the mapped stands are, not attendance remaining. Set `PauseMenuFullness=0` under Dynamic Gameday Broadcast to hide it, then reopen the menu.

## Problems?

[Open a bug report](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/issues/new/choose) with your plugin/game/MMC versions, matchup, settings and logs. Logs are in `Plugins/DynamicGamedayAtmosphere/Logs/`. Activation errors in `launcher.log` name the failed step and Windows error. Compare the reported MMC/game administrator levels when access is denied.

For missing audio, disable Soundscape's `Enabled` option and restart MMC and the game. Re-enabling it also needs a restart. There is no EXE hash requirement, but an EA update may still require new game bindings.

## Source and updates

[Build instructions](docs/BUILDING.md) · [Updating after an EA patch](docs/UPDATING-THE-GAME.md) · [Release history](RELEASES.md)

[Working diagnostics and remaining live checks](docs/DIAGNOSTICS.md)
