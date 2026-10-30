# Audio mix range / 2.0.2

Default mix strength is 100%. Values below are native-bus signal gains, not perceived-loudness percentages or recordings. Actual mapped seating determines fullness and venue size. Beds and Chants always share one gain curve.

| Mapped positions | Fullness | Background / chants | Collective reactions | Individual detail |
|---:|---:|---:|---:|---:|
|6,060|0%|Muted|Muted|Muted|
|6,060|1%|40.0%|55.0%|95.0%|
|6,060|10%|42.2%|56.6%|95.2%|
|6,060|20%|47.2%|60.4%|95.6%|
|6,060|25%|50.0%|62.5%|95.8%|
|6,060|30%|53.9%|65.5%|96.2%|
|6,060|50%|81.9%|86.4%|98.5%|
|6,060|70%|100.0%|100.0%|100.0%|
|6,060|90%|111.1%|111.1%|111.1%|
|6,060|100%|115.0%|115.0%|115.0%|
|61,888|25%|92.9%|94.7%|99.4%|
|61,888|30%|100.0%|100.0%|100.0%|
|61,888|50%|105.9%|105.9%|105.9%|
|61,888|90%|128.3%|128.3%|128.3%|
|61,888|100%|130.0%|130.0%|130.0%|

Nonzero floors are 40% background/chant signal, 55% collective-reaction signal, and 95% individual-detail signal. At the smallest venue, 25% fullness requests exactly 50% background signal. A monotone Hermite curve connects the floor, anchor and vanilla crossover without the previous steep logarithmic tail. Requests use .001 signal-gain steps, followed by synchronized native fades, to bound insignificant preset churn. Exactly zero attendance still explicitly mutes the Crowd parent.

The vanilla crossover remains 70% fullness at 6,060 mapped positions through 30% at 61,888. Positive gain still tops out at exactly +30% in the largest packed venue. Subgroups keep distinct lower-density curves; individual-detail routing/effective audibility remains unresolved after the earlier audition. No independent chant cut or diagnostic 9/18 dB boost is included.
