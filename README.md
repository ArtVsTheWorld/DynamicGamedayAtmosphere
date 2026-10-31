# Dynamic Gameday Atmosphere

By Ace · **Update 3 Hotfix — 2.0.30** · College Football 27 for Windows · MMC

Crowds arrive, take halftime breaks and leave early. Crowd sound follows the fans in the seats, while big plays bring extra reactions, camera shake and replay opportunities.

[Download](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/releases/latest) · [Full feature list](FEATURES.md)

## Install or update

1. Close the game and MMC.
2. Unzip the download and open the extracted folder.
3. Copy **both** of these into your MMC **Plugins** folder:

   - `DynamicGamedayAtmosphere.dll`
   - The `DynamicGamedayAtmosphere` folder beside it

4. If Windows asks, merge the folders and replace matching files. Then open MMC and launch the game normally.

Your MMC folder should look like this:

```text
MMC/
  Plugins/
    DynamicGamedayAtmosphere.dll
    DynamicGamedayAtmosphere/
```

The matching folder contains files the plugin needs to run. **Copy it along with the DLL every time you update.** Keep your existing folder and merge the new files into it so your settings and crowd history stay in place.

You do not need to copy the `.md` files into MMC. They are just instructions and the feature list.

## Settings

Open **Gameday Atmosphere** from either:

- **Dynasty/RTG Central → User Settings**
- **Pause → Options → Settings**, at the far right after Auto-Subs

Most people can keep the defaults. Turn on the game's **Dynamic Camera Effects** if you want added shake. Broadcast includes separate shake sliders for Standard, Zoom, All-22, Wide and Broadcast cameras.

**Away Team Reaction Shake (%)** starts at 0 (off); 50 is half the matching home reaction and 100 is the same strength. **Neutral-Site Crowd Shake (%)** controls both teams at neutral stadiums and starts at the usual 75. Existing On settings become 35% when updated; existing Off settings stay at 0%.

**Existing Update 3 settings are kept.** Updating from Update 2 resets the old settings once and saves a backup automatically. You do not need to delete any INIs.

## If something is wrong

Send the whole **`Plugins/DynamicGamedayAtmosphere/Logs`** folder and briefly describe what happened. For a problem during a game, include the teams and quarter/clock.

If MMC reports a missing runtime loader, copy the matching `DynamicGamedayAtmosphere` folder from the ZIP again.

Offline Dynasty and Road to Glory have been tested on the game update checked October 30, 2026. The revised peak-attendance formula applies to fresh regular-season Dynasty games. See the full feature list for mode differences and replay limitations.
