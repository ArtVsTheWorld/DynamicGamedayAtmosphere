# Audio mix range / 2.0.1

These are production-calculator outputs at default 100% mix strength, not recordings or perceived-loudness measurements. Fullness and counts use mapped seating. Beds and Chants share the shown gain; collective reactions and individual detail can differ.

| Mapped venue | Fullness | Occupied positions | Bed gain vs native | Bed signal vs native |
|---:|---:|---:|---:|---:|
|6,060|0%|0|Muted|0.0%|
|6,060|1%|60|-60.00 dB|0.1%|
|6,060|10%|606|-30.25 dB|3.1%|
|6,060|30%|1,818|-3.50 dB|66.8%|
|6,060|50%|3,030|-3.25 dB|68.8%|
|6,060|70%|4,242|+0.00 dB|100.0%|
|6,060|90%|5,454|+1.00 dB|112.2%|
|6,060|100%|6,060|+1.25 dB|115.5%|
|61,888|30%|18,566|+0.00 dB|100.0%|
|61,888|50%|30,944|+0.50 dB|105.9%|
|61,888|90%|55,699|+2.25 dB|129.6%|
|61,888|100%|61,888|+2.28 dB|130.0%|

At 30% fullness in the smallest venue, the previous -9.5 dB bed/chant gain kept about 33.5% of native signal. Halving the missing signal gives about 66.7%, rounded to -3.5 dB / 66.8% in production. Negative subgroup gains at both sparse anchors are softened this way; the 50% point is adjusted too so the background/reaction curves keep rising with arrivals. Positive individual-detail gain remains +0.75 dB at the sparse anchor.

The strongest finite bed cut remains -60 dB (0.1% signal) near empty, with explicit mute at zero fans. The below-anchor logarithmic tail still becomes much quieter toward empty. In the largest venue, vanilla balance remains 30% full; 90% is about +29.6% signal gain and fully packed is exactly +30%. In the smallest venue, vanilla is 70%, and packed is about +15.5%.

This revised sparse tuning needs installed-build listening validation. Individual-detail routing/effective gain remains unresolved after the earlier high-gain audition. No diagnostic 9/18 dB gain is included.
