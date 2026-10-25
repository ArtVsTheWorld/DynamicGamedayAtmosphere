# Dynamic Gameday Atmosphere

By Ace — Update 2 (build 1.54.0)

Fans arrive, leave and take halftime seat breaks as the game unfolds. Fuller stadiums get more involved sound, play reactions and camera effects. Big moments get bigger celebrations.

## Install or update

1. Close both the game and MMC.
2. Copy `DynamicGamedayAtmosphere.dll` into MMC's `Plugins` folder, replacing your previous copy.
3. Open MMC and launch the game through it.

Keep your settings file. Supported settings update automatically. Restart both programs after every DLL update.

## Settings

Open `Plugins/DynamicGamedayAtmosphere.ini`. Every option explains its default, range and what turning it up or down does.

- **Dynamic Gameday Attendance:** arrivals, permanent departures, halftime breaks and seating patterns.
- **Dynamic Gameday Soundscape:** crowd sound, buildup, play reactions and celebrations.
- **Dynamic Gameday Broadcast:** camera movement and extra replay opportunities.

Leave the four General update markers alone. Audio fullness is how full the mapped stands are; attendance is the share of this game's original crowd still present. They can differ.

Read `FEATURES.md` for the full feature list and important setting combinations. The game still decides the original peak crowd; this mod never creates a sellout the game did not plan.

## Troubleshooting

Check `Plugins/DynamicGamedayAtmosphere/Logs/status.txt` for the loaded version, game process and active features. Logs are in the same folder. The main log clears at its configured size; status is replaced each second.

For missing audio, turn off **Enabled** under **Dynamic Gameday Soundscape**, then restart MMC and the game. This skips the mod's audio hooks; attendance and extra replays remain available. Added atmosphere motion depends on Soundscape. Restart again when re-enabling Soundscape.

For a bug report, include the matchup/stadium, game mode, quarter/clock, what happened, your INI, `status.txt`, the main log and `launcher.log`. Include a crash dump if one was created.

The plugin has no EXE hash requirement. It verifies the game functions it uses; a game update or another hook can make a particular control unavailable. Added motion changes presentation, not gameplay home-field advantage.
