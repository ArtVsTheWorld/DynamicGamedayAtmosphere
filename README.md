# Gameday Atmosphere

**By Ace.** Crowd arrivals and exits, traveling supporters, reactive sound and camera shake.

**[Download 2.1.9 ZIP](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/releases/download/v2.1.9/GamedayAtmosphere-2.1.9.zip)** · [Full feature list](FEATURES.md) · [What's new](RELEASES.md)

Update 4. Ace reported the standalone packaging working; other setups still need their own checks.

## Install or update

1. Close the game and MMC Mod Manager.
2. Put **GamedayAtmosphere.dll** directly in the versioned Mod Manager's **Plugins** folder. The ZIP contains that DLL, README.txt and FEATURES.md. Matching support files are embedded and prepared automatically.
3. Move older duplicate plugin DLLs outside Plugins, keeping settings/history folders. Restart MMC and launch normally. Use a writable MMC folder outside Program Files.

Read README.txt in the ZIP for this module's menu, old filenames, logs and removal steps. Use one provider of each feature. If keeping COT for other modules, disable this feature in COT before using the standalone. You can install any combination of matching current standalone modules.

**Want one feature? Download its standalone ZIP. Want the collection? [Download Dynasty COT](https://github.com/ArtVsTheWorld/DynastyCOT/releases).** See the [compatibility and saved-data guide](https://github.com/ArtVsTheWorld/DynastyCOT/blob/0c387c580473a84b7e5e5e8ff5ee44f8117fb5cf/docs/standalones.md).

## Source and verification

This repository is the standalone download/help front door. Current releases are built from the [single maintained COT workspace](https://github.com/ArtVsTheWorld/DynastyCOT/tree/0c387c580473a84b7e5e5e8ff5ee44f8117fb5cf), including [this module's source](https://github.com/ArtVsTheWorld/DynastyCOT/tree/0c387c580473a84b7e5e5e8ff5ee44f8117fb5cf/Modules/GamedayAtmosphere) and shared build/packaging code. `source-release.json` pins the exact source revision, baseline and artifact hashes.

The older source tree retained in this repository is historical; it is **not** the build input for this standalone release. Fixes are maintained in COT rather than separately editing four forks. Release assets include integrity metadata; successful offline checks do not certify every game/overhaul combination.
