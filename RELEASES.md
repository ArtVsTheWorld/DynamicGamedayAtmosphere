# Aceâ€™s Gameday Atmosphere 2.1.38

Changes since public release 2.1.11.

- On new Dynasty home dates, ticket demand, travel and each school's following help set turnout. A visiting section can stretch across nearby blocks, with mixed edges and scattered groups of friends. Saved matchups keep their old plan unless you choose Regenerate Next Crowd for a newly started game.
- An added positive visiting cheer at a home stadium needs at least one visitor in five among the supporters actually present. A visiting touchdown can still hush the home background below that mark. A big lead can calm the roar.
- Supported neutral games keep the game's turnout and split the crowd between both schools. In supported Dynasty Extensions spring games, the actual school's fans wear its colors, take halftime breaks and drift out in the second half, whatever the score.
- Set Big Play Replay Minimum Yards in the menu from 10 to 60. It starts at 12. Supported Broadcast Universe setups can receive a big-play request when a clip is available. The game handles touchdown replays.
- A guide in the ZIP shows how to read the stands. The status report can help explain a missed cheer. The new section layouts, spring boos and in-game frame rate still need live checks.

Download the standalone ZIP, close MMC and the game, and copy **GamedayAtmosphere.dll** into MMCâ€™s **Plugins** folder. The ZIP includes a full feature list and an illustrated player guide.


---

# Gameday Atmosphere 2.1.11 â€” Play Now replay hotfix

Compared with the previous public standalone release, **2.1.9**.

- Fixed a replay eligibility check that admitted Dynasty but left offline Play Now out. With **Additional Replays** and **Full-Screen Big Play Replays** enabled, qualifying non-touchdown big plays in Play Now can use the added replay coverage.
- Replay diagnostics now admit offline Play Now too, so those decisions can be checked when troubleshooting.
- Existing crowd behavior, sound, camera shake, replay thresholds and saved settings/history are retained.
- Touchdown presentation remains controlled by the game. The unsuccessful private touchdown experiment and temporary timing helpers are **not included**. This release does not guarantee a replay after every touchdown.

**Update:** close both MMC and the game, then replace `Plugins/GamedayAtmosphere.dll` with the DLL from the ZIP. Keep your settings/history folders. Restart both applications. Use one Gameday provider; disable COT's Gameday module when using this standalone for the feature. The ZIP contains the DLL, README.txt and FEATURES.md; matching support files are embedded.

**Verification:** the production replay-root tests exercise Dynasty and Play Now, nested gain comparisons, blocked scopes/options, and unchanged touchdown predicates/shortcuts/insets. Replay, scope, menu and isolated managed packaging/storage/duplicate-provider checks pass. Settings and history remain byte-for-byte intact in upgrade fixtures. A 20-yard non-touchdown Play Now completion showed both full-screen and inset replays in the private test; the clean production ZIP was not live-installed during this publishing step. Other game/mod combinations remain unverified.

Version 2.1.10 was reserved for private touchdown tests. Public 2.1.11 uses a distinct native filename/cache so it cannot reuse that private runtime. No change to native fingerprints, ownership checks or supported offline scope.

Rollback: close MMC and the game and restore the 2.1.9 plugin DLL. Keep the existing data folders. The prior release remains available on GitHub.


---

## Earlier release notes

# Gameday Atmosphere 2.1.9 - Update 4

Compared with the last published standalone release, **v2.0.35 (Update 3 Hotfix)**.

- **Traveling supporters:** new visitor sections and scattered groups use fanbase demand, distance and matchup appeal. Big fanbases handle long trips better; strong home crowds resist takeovers. Main visitor blocks mix in some home fans rather than becoming one solid block.
- **Visiting Fan Strength:** a 0-200% control, with 100% using the normal model. It affects new plans, while existing plans and protected groups stay intact.
- **More natural late crowds:** winning visitors can still leave during a blowout, at a slower rate than the losing home crowd.
- **Sound and shake:** added shake requests accompanying crowd sound. Scoring cheers receive priority, background roar weakens with a growing home deficit, and away supporters have their own reactions.
- **Basic offline Play Now:** arrivals, departures, sound and shake work without requiring Dynasty's travel/season calculations. With arrivals enabled, fresh games use a 75% opening crowd model; visiting-fan appearance remains native.
- **Touchdown presentation:** extra replay requests skip touchdowns so native touchdown presentation remains in control. Other eligible extra replays stay optional.
- **Simpler installation:** put `GamedayAtmosphere.dll` in Plugins. Matching runtime files are embedded; the external loader EXE is unnecessary. Existing settings/history are retained without a reset or retune.

The ZIP contains only the DLL, a short README and a complete code-checked feature list. Move the old `DynamicGamedayAtmosphere.dll` outside Plugins and keep its data folder. Use one Gameday provider; disable COT's Gameday module if using this standalone alongside its other features.

Validation: 90 packaging/storage/guard checks including four existing regression suites, plus all-four standalone coexistence fixtures. Ace reported the standalone packaging working. This does not certify every game build, stadium or overhaul. The crowd engine is the same 2.1.9 engine shipped with COT 1.0.2.

Source comparison: old tag `v2.0.35`, commit `193662a9af440ef9cdc6903171e1a30b8acb0164`; current maintained source is pinned in `source-release.json`. User-facing changes were checked against `AwayCrowd.h`, `VisitorPlans.h`, `SettingsMenuOptions.inc`, `NativeRuntime.cpp`, `ReactionAudioRuntime.h` and `MajorReplay.h`, not inferred from version numbers. Existing arrival, turnout, Fuller Crowds, halftime, 2020 Mode and general replay features are retained rather than presented as new.


All four standalone releases were confirmed verified working by Ace on November 9, 2026. The tested ZIP/DLL bytes are unchanged; other game/overhaul combinations are not automatically certified.


---

## Earlier release notes

# Releases

**Update 3 Hotfix is the latest public release**

| Release | Build | Source tag | Notes |
| --- | --- | --- | --- |
| **Update 3 Hotfix â€” latest** | **2.0.35** | [v2.0.35](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/tree/v2.0.35) | [Changes since 2.0.21](docs/releases/Update3Hotfix.md) Â· [Discord notes](docs/releases/Update3Hotfix-Discord.md) |
| Update 3 | 2.0.21 | [v2.0.21](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/tree/v2.0.21) | [Changes](docs/releases/Update3.md) Â· [Discord notes](docs/releases/Update3-Discord.md) |
| Update 2 | 1.54.0 | [v1.54.0](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/tree/v1.54.0) | [Changes](docs/releases/Update2.md) |
| Update 1 | 1.26.1 | [v1.26.1](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/tree/v1.26.1) | [Changes](docs/releases/Update1.md) |
| Release | 1.20.1 | [v1.20.1](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/tree/v1.20.1) | [Initial release](docs/releases/Release.md) |

Public downloads are under [GitHub Releases](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/releases). The separate October 13 draft is an older private development snapshot.

Historical source tags were imported from preserved snapshots. They do not recreate every development build. Game binaries, MMC assemblies, saves and private captures are excluded from source and downloads.
