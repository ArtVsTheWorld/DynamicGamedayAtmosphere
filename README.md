# Dynamic Gameday Atmosphere

By Ace. A College Football 27 plugin for MMC.

This branch contains Update 3 diagnostics work (1.55.0). Update 2 is still the latest public release. The settings path below describes the working build; Update 2 keeps its INI directly in Plugins.

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

## Problems?

[Open a bug report](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/issues/new/choose) with your plugin/game/MMC versions, matchup, settings and logs. Logs are in `Plugins/DynamicGamedayAtmosphere/Logs/`.

For missing audio, disable Soundscape's `Enabled` option and restart MMC and the game. Re-enabling it also needs a restart. There is no EXE hash requirement, but an EA update may still require new game bindings.

## Source and updates

[Build instructions](docs/BUILDING.md) · [Updating after an EA patch](docs/UPDATING-THE-GAME.md) · [Release history](RELEASES.md)

[Working diagnostics and remaining live checks](docs/DIAGNOSTICS.md)
