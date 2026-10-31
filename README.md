# Dynamic Gameday Atmosphere

By Ace · **Update 3 working build — 2.0.20** · College Football 27 for Windows · MMC plugin

Make the stadium feel like the game being played. Fans arrive after kickoff, leave for halftime and head home when the result looks settled. Crowd sound follows the fans in the seats. Big plays bring cheers, camera shake and more replay opportunities.

**Update 3 is still being finalized and is not publicly released.** [Update 2 remains the latest public release](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/releases/latest). The [feature list](FEATURES.md) describes the current Update 3 working build.

## Install or update a provided build

1. Close College Football 27 and MMC.
2. Unzip the build package provided for testing. The public Releases page currently contains Update 2; Update 3 will be published after final approval.
3. Copy `DynamicGamedayAtmosphere.dll` from the ZIP's `Plugins` folder into your MMC `Plugins` folder. Replace the old copy. Keep only one DGA DLL installed.
4. Open MMC and launch the game through it.

This build supports the Windows game update checked on October 30, 2026. **Offline Dynasty and Road to Glory have been tested.** The revised peak-attendance formula is for fresh regular-season Dynasty games. Other modes are not guaranteed to support every feature.

**Update 3 resets old DGA settings once, including earlier Update 3 test builds.** Your old INI is backed up automatically. You do not need to delete it. Reapply any settings you want afterward. Your new settings are kept on later launches. Game saves and the game's own settings are not reset.

## What's new since Update 2

- **Rebuilt attendance:** expected crowds use school support, season performance and matchup demand. Openers, night games, local opponents, senior day and service-academy matchups can draw more fans.
- **Showcase rivalries:** eight major rivalry events receive sellout peak demand and fuller kickoffs, with dedicated starting bands for Army–Navy, Michigan–Ohio State and Oklahoma–Texas. Recognition follows the native rivalry event, including replacements that retain it.
- **More natural seating:** fans favor desirable seats, with smaller group gaps and less uniform thinning.
- **Attendance Profile:** keep Default, or choose Fuller Crowds for higher regular-season Dynasty peaks and fuller kickoffs. Select it before loading a fresh game.
- **A new crowd mix:** sparse crowds are quieter but still alive. Full stadiums can be louder than the base game, with dense background sound reaching +3.5 dB and the largest packed venues reaching +4 dB. Collective reactions keep their separate +30% signal ceiling. Chants follow the same background mix.
- **More reaction variety:** 80 listening-tested crowd sounds for existing plays and situations, including disappointment and visitor responses. Trucks favor the big-hit “ooh,” other successful skills get stronger cheers, and fuller crowds can keep a steady background roar between reactions.
- **Better shake and overtime:** strength follows a linear occupied-seat curve up to 1.25×. Independent effects can reinforce each other gently, close-up shake is less reduced, and earned effects finish through timeout/penalty scenes. Runs build from 11–15 yards; kickoff returns build from 25–35. Overtime covers more important results.
- **Camera correction:** ordinary views no longer multiply shake for a wide field of view. Native HFA camera shake yields while DGA movement is active; the larger RTG/player-camera allowance has a separate in-game toggle, off by default.
- **More control in game:** open Settings → Gameday Atmosphere. Attendance, Soundscape and Broadcast each have a short list of plainly explained options.
- **2020 Mode:** empty stands, no collective crowd or bands, quieter individual detail and a more open stadium sound.
- **Performance and reliability fixes:** fixes for the repeated post-whistle hitch, carried-over sack flags, duplicate reactions, saved crowds and replay cleanup.

## Settings

Open **Settings → Gameday Atmosphere** in game. Most people can keep the defaults. **Wide Player Camera Compensation** is in Broadcast, directly below Added Camera Shake. Leave it off for normal cameras. Turn on the game's **Dynamic Camera Effects** if you want added shake.

The advanced settings file is `Plugins/DynamicGamedayAtmosphere/DynamicGamedayAtmosphere.ini`. Each option includes an explanation. Restart the game and MMC after replacing the DLL or changing the main Crowd Sound Effects switch.

**2020 Mode is off by default.** It empties the stadium and mutes collective crowd, bands, drums and the PA announcer. Turn it off to restore normal attendance and sound.

Fullness means the share of seats currently filled. Kickoff settings use a share of the crowd expected for that game. For example, if the expected crowd fills half the stadium and half those fans start seated, kickoff fullness is about 25%.

## Problems

Check `Plugins/DynamicGamedayAtmosphere/Logs/status.txt` for the loaded version and activation status. If something is wrong, send the **whole Logs folder**, plus the teams, quarter/clock and what you saw or heard. The readable files are `status.txt` and `DynamicGamedayAtmosphere.log`; the technical files keep the detail needed for troubleshooting.

Replays use the game's own recordings and presentation system. Injuries, penalties, skipped scenes and missing recordings can interrupt them. Slow Sim uses full-screen replays without a play-selection inset. DGA does not fix the base game's repeated kneel-down postgame highlights.

The DLL belongs directly in MMC's `Plugins` folder. Settings, logs, backups, history and extracted tools stay inside `Plugins/DynamicGamedayAtmosphere/`. Keep that folder when updating so crowd history and backups remain available.
