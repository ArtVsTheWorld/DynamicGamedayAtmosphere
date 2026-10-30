# Releases

Staged Update 3 test candidate: **2.0.6**. Removes duplicate audio-context/camera checks, filters unrelated cannon events earlier and adds sound/camera timing. Retains the 2.0.5 replay fix that passed the user's whistle-hitch, arrivals and touchdown replay check. Includes the prior crowd, attendance, audio and shake changes. The new candidate needs a short smoke test; see the [performance audit](docs/reviews/Update3-2.0.6-performance.md).

Public names identify downloads. Numbered builds identify source, DLL metadata, runtime folders and bug reports.

| Public release | Working build | Source tag | Notes |
| --- | --- | --- | --- |
| Release | 1.20.1 | [v1.20.1](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/tree/v1.20.1) | [Initial release](docs/releases/Release.md) |
| Update 1 | 1.26.1 | [v1.26.1](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/tree/v1.26.1) | [Changes](docs/releases/Update1.md) |
| **Update 2 â€” latest** | **1.54.0** | [v1.54.0](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/tree/v1.54.0) | [Changes](docs/releases/Update2.md) |

Downloads are under [GitHub Releases](https://github.com/ArtVsTheWorld/DynamicGamedayAtmosphere/releases). Archives are preserved byte-for-byte from the original packages. SHA256SUMS.txt identifies each download.

Older tags were imported from preserved source snapshots. Initial commits record that import; they do not recreate every historical working build. `main` includes current source and portable build tools. No game binaries, MMC assemblies, saves, private diagnostics or development backlog are included.

Update 3 is a **working candidate** and has not been published; 1.99.0 was the earlier testing baseline. Public Update 3 is planned as **2.0.0** after test-game fixes. See its [candidate notes](docs/releases/Update3.md) and [remaining test checklist](docs/DIAGNOSTICS.md).
