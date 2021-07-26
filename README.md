KRTPluginA 
===

Version: `RACK_VERSION.WORKING_MACHINES_COUNT.FIX_INCREMENT` supplied in tags. Any commit not tagged is not a version for compile, although it may work. The index is numbered so that `WORKING_MACHINES_COUNT` makes sense given the development ordering of the index.

Index
===

1. [A](#a) - No demo (it's a filter, so easy to try)
2. [u](#μ)
3. [T](#t) - [I shat the she riff](https://github.com/jackokring/KRTPluginADemo/blob/master/I%20shat%20the%20she%20riff..vcv)
4. [L](#l) - No demo (it's a delay which can be slightly shortened for latency timing)
5. [;D](#d)
6. [R](#r) - No demo required

[Easy Access Demo Archive](https://github.com/jackokring/KRTPluginADemo/archive/refs/heads/master.zip) includes all the demos above as a **.zip** file for easy downloading and none of that cut, paste, save and rename business. 

A
===

*49% Tranparent Module - Made with sexspex plastique*

Parameters
---
* Frequency (-12dB) `FRQ`
* Resonance `REZ`
* Frequency Modulation Gain `MOD`
* Relative Plate (-6 dB) `OFF`
* Plate Gain `AM`
* Plate Modulation Gain `MAM`

Ins
---
* -18 dB In (HP on `XP12`) `IN1`
* -18 dB Predictive (LP on `XP12`) `LP1`
* Frequency CV (Polyphonic master) `CV`
* -12 dB In (No Plate) `IN2`
* -6 dB HP In (Plate Ring) `HP1`
* Modulation `MOD`

Outs
---
* -6 dB LP Out `LP1`
* Tracking Out `CV`
* -12/-18 dB Out `LP12`
* -6 dB HP Out `HP1`
* Plate Out `AM`
* -12/-18 dB Inverse Extended Filter Out `XP12`

A generic filter with some nice fun additions for a characteristic sound. How much fun can you have with 4 poles and some other DSP functions? Yes, it is by design digital, although it might be possible to make an analogue version of most if not all of the implied circuit.

The filter poles are ZDF (Zero Delay Filter) arithmetic. The `LP1` predictive input creates a
ninth order estimate one sample into the future. This has the effect of a bit of phasing and given that it is not equally fed into the ring modulation too, makes it spectrally different. The 45 degree lead lag of the ring makes for a ninety on the AM, for even harmonic self AM power.

If some controls appear not to operate use `IN1` and `LP12` out, with `CV` for tracking, and then all the controls work. Some specific inserts are pre or post certain controls. The plate AM tracking stacks on the filter corner tracking.

μ
===

*The Gold One - Summation generatve additive sythesis*

Parameters
---
* Master Gain `dB`
* Slew Limit `Hz`
* Halflife `λ`
* Gain 1 `G1`
* Gain 2 `G2`
* Gain 3 `G3`

Ins
---
* 1V/6dB `CVdB`
* 1V/Oct (Polyphonic master) `CVHz`
* 1V/halflife  `CVλ`
* In 1 `IN1`
* In 2 `IN2`
* In 3 `IN3`

Outs
---
* Differential `Δ`
* Second Differential `ΔΔ`
* Third Differential `ΔΔΔ`
* Integral Linear `Σ`
* Integral Pole `1/Σ`
* Integral Log `LNΣ`

Some weird calculus module. It really depends what it can be made to do. It might become unstable if you feedback connections. It might not as well. The `λ` control only affect the integral outputs, as the 3 representations should converge to the same value and `λ` affects the effective sample timing offset which defaults to 1.

All the estimators are predictive one sample into the future (with `λ` kind of being a sophisticated constant of integration), and the slew filter is a 6 dB/Oct zero delay filter.

So the inputs `IN1`, `IN3` and `IN3` are controlled by **centred dB scaling** gainuverters for gain, amplified by an exponetial `dB`, slew limited and then processed for calculus estimates (9th order FIR).

The integrals are further processed by 3 point series accelerations (a tiny effect but interesting) to occur at a sample in the future. `λ` applies an effective modulation of this integral time step from a fraction of a sample to multiple samples. The halflife then is measured in exponetial samples.

As the future sample prediction method is used, essentially there is zero delay when an output is fedback into an input. This is potentially interesting for making a `ΔΔ` to `IN1` with negative `G1` forced oscillator using `IN2` or other inputs for forcing. The module can be setup to "solve" various integra-differential equations, some of which maybe chaotic in nature.

I mean why a 9th order FIR, and the number 27 of the sporadic simple groups connection? From 2 comes 2 to the power of all N. From 3 comes all odd numbers?

He's irisponsible :D https://en.wikipedia.org/wiki/Jacques_Tits ... roll on (Monster Moonshine)[https://en.wikipedia.org/wiki/Monstrous_moonshine] and let's not confuse maths with physics "speculated" on maths. Hope I got all the integration by parts and collection of approximants in limited terms right? (Some 2's, some 3's, some hopeful sporadics on the elliptic). Did I mention the congruants div 3 and the determinats (count 44 (18n+26 (Groupies)[https://en.wikipedia.org/wiki/Sporadic_group])) with Z(p) and alternating groups to a 42 which the circle and ellipse are the alternating or not on the major minor radius?

Initialize the big bang with differential estimates of the future captain, so obivating the definitional requirements at the singularity Captain. Well Gamma Bob, well gamma. A form of precision series truncation crusher, nice! None of that chucking away an abstract precision and getting right in there with truncation of summation generatve additive sythesis, for that timbral union.

But isn't it about the singularity context of integration in a set of limits x, x+y where x is unkown but gestimated in a relative context Captain? Yes Bob all error in the calculus should be reported as this module is not about error, and others maybe built.

But shouldn't a sine converge to sin(x)? And and truncation of terms provide a controlled deviation from sin(x) for Fauxrier harminics? Exactly Boz, the sweet low power law alias summands.

"Proxy off a dirty hole of unknown gain? How dare you? Must fix the incomming to be at least plesant." -- Captain of the Exponential.

T
===

*The White One - A harmonic pitch down and up delay*

Parameters
---
* Course Tune `NOTE`
* Fine Tune `FINE`

Ins
---
* In `IN`
* Trigger (Polyphonic master) `TRIG`

Outs
---
* High Trigger `HI`
* Out `OUT`

The trigger `TRIG` resets the delay and produces a down shifted `OUT` until the up shift starts causing `HI` to output a rising edge and the delay switches an octave up until the expected retrigger point. The delay plays regular speed until a retrigger. The delay will reset itself if the delay buffer becomes full.

Going down is always a prerequisite before rising to the top! And normal is very overdue. The maximal delay length is divided by polyphony. What's the cache flush GB/s on this thing? I'm thinking 4 bars and a bit or so?

Polyphonic triggers occur at the optimal octave intercept per trigger, and so the outputs become phasic to the triggers. `HI` can off trigger a trigger to sync with the up speed clearing of the precision down beat zero delay. No trigger equals monophony and no delay after the "blip" as there is a trigger need on `TRIG`.

Try `NOTE` at 5 semitones, and alter the trigger rate for a sync chord arpy sound. In this sense other semitone values are better at lower `TRIG` rates. Try some drums on the `HI` out for an interesting timing effect too.

L
===

*A White One - A synchronized delay with pre-trigger end*

Parameters
---
* Pre-trigger Delay `PRE`
* Fine Tune `FINE`

Ins
---
* In `IN`
* Trigger (Polyphonic master) `TRIG`

Outs
---
* Pre-trigger `PRE`
* Out `OUT`

A synchronized delay with a `PRE` control to remove a time dependant on sample latency so as to match timing one synchronization trigger later. So suppose you wish to sychronize to some live play and are prepared to time up one `TRIG` later (say a bar for arguments sake), then you can increase `PRE` to achive that less than the bar delay with latency compensating the rest of the bar delay which `PRE` removed on `OUT`. The output `PRE` gates high before the bar end to allow down module to trigger other things with the need to be triggered before the bar start. 

;D
===

*Another White One - A future estimator VCA with error*

Parameters
---
* Level `dB`
* Level Modulation `CVdB`

Ins
---
* In (Polyphonic master) `IN`
* Level Modulation `CVdB`

Outs
---
* Error `ERR`
* Out `OUT`

Exponetial amplitude modulation with a future projection estimate 2 samples into the future for phase critical all pass when used with modules that have a 1 sample delay. Some estimate of the error in the current sample based on a future prediction is also re-estimated 2 samples into the future and output on `ERR`. The `CVdB` control is a **centred dB scaling** gainuverter.

R
===

*The Final White One - A simple mixer split*

Parameters
---
* On Level `ON`
* As Level `AS`

Ins
---
* In (Polyphonic master) `IN`
* Mix Some On `ON`

Outs
---
* Get As `AS`
* Out `OUT`

Mix some `ON` into the `IN` making an `OUT` with control of it `AS` for easy modulation mixing. The controls are **centred dB scaling** gainuverters.