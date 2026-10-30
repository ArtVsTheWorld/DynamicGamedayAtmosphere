# Update 3

Build **2.0.12** · [Download](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/releases/tag/v2.0.12) · [Full feature list](../../FEATURES.md)

Changes since Update 2 (1.54.0):

- **Rebuilt attendance formula.** Expected turnout now uses school support, AP rank, season record and opponent demand, calibrated against real attendance-to-capacity trends. Home openers, night games, senior day, in-state games and service-academy matchups can draw bigger crowds without making every game a sellout. Postseason peak turnout stays with EA.
- **Better arrivals and departures.** Smaller venues start with more fans seated, Utah joins the late-arriving schools, halftime breaks vary by game, and close games hold fans longer. Night games, scoring pace, weather and late blowouts affect how quickly people leave. Permanent exits respect a capacity floor.
- **More natural seating.** Midfield sidelines and useful viewing elevation keep fans longer. End zones and less desirable seats tend to clear earlier, with smaller group gaps. The fuller 90% look is preserved.
- **New crowd audio mix.** Uses the native mixer and occupied seats. Sparse backgrounds thin out without flattening big-play excitement; larger full stadiums rise above vanilla, up to +30% signal gain. Chants follow the background instead of getting a separate cut. The old excitement limiter is removed.
- **Much more sound variety.** 80 listening-tested native sounds across 27 usable families are matched to existing triggers and scaled by crowd size and play importance. Visitor versions add sound-only responses. DGA cheers can overlap the game's own cheers.
- **Rebalanced camera shake.** Occupied fans determine strength after the crowd qualifies. Smaller venues get more noticeable effects, neutral sites use 75% strength, and run/catch reactions blend into touchdowns and major celebrations. Eligible final-play effects can finish through postgame.
- **More responsive plays and overtime.** A confirmed five-yard loss can react without waiting for a sack label. Defensive fumble returns, fourth-down conversions, turnover on downs, supported skill moves and showboating get more consistent treatment. Overtime adds stop, kick and two-point coverage with a small shake boost.
- **Replay improvements.** Manual touchdown/takeaway sequences wait for full-screen playback before asking for a separate inset. Slow Sim keeps full-screen playback without asking for an inset. Replays retain recorded attendance and restore the live crowd afterward.
- **New in-game settings page.** 36 plain-language controls across Attendance, Soundscape and Broadcast, with toggles first. Stadium Fullness appears beside Resume in the pause menu.
- **2020 Mode.** Optional empty stands with collective crowd, bands, drums and PA muted; quiet individual detail and a more open ambience/reverb mix remain.
- **Fixes and performance.** Corrected repeated post-whistle replay checks, carried sack flags, resume-history selection, duplicate conversion/visitor reactions, launch weather handling and repeated crowd lookups. Readable logs sit alongside full technical logs, with disk writes kept off gameplay callbacks.
- **Updated game support.** Native bindings updated for the Windows title update checked October 30, 2026.

## Installation

Close the game and MMC. Replace `Plugins/DynamicGamedayAtmosphere.dll` with the one in the ZIP, then launch through MMC. **Old DGA settings reset once with an automatic backup; no INI deletion is needed.** Your saves and native game settings are unaffected.

## Limits

Offline Dynasty is the tested mode. Native recording availability, injuries, penalties and skip controls still affect replays; no replay is guaranteed after every play. Slow Sim has no play-call inset. The base game's repeated final-kneel postgame montage is not fixed. Quiet individual-detail routing in 2020 Mode has automated coverage; its final balance still needs a live check on this build. Custom stadium music and new crowd animations are not included.
