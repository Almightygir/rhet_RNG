# rhet_RNG
Storing an RNG header created by RomuDuoJr at http://rhet.dev
WayBackMachine: https://web.archive.org/web/20220704174727/https://rhet.dev/wheel/rng-battle-royale-47-prngs-9-consoles/

PRNG Battle Royale: 47 PRNGs × 9 consoles
30 March, 2020
In this “Ultimate Pseudo Random Number Generator Battle Royale for video game development” almost 50 fast, small, non-cryptographic PRNGs compete in multiple categories across 9 game consoles: Nintendo Switch, PlayStation (4/4Pro/5), Xbox (One/One X/Series X), Stadia (1080p/4K).

What’s the point? Well, it’s 2020 and many games still use MT19937 for random number generation. At the same time there are a lot of brilliant, small PRNGs and range mapping techniques that bring speed, memory and shiny features to the table.

Let’s showcase the available wheels!

The different wheels
I am inclined to group most PRNGs by state advancement. State advancement tells us really interesting properties about the PRNG such as: the expected period length, whether it supports streams, if it is trivially debuggable, resizable or has a fixed point at zero.

Consider checking out a primer on randomness for a recent take on PRNG and getting started in PRNG development.

Without further ado, the gladiators:

Chaotic: jsf,romu2jr,romu2,romu3,romu4,overdrive64,movercounter64,mover64
Period: seed dependant, may end up in a short or empty cycle
Stream: by seeding (unreliable)
First 12~18 rolls are low quality
Chaotic counter: gjrand,sfc
Period: seed dependant, guaranteed minimum, high average
Stream: changing sequence (untested), by seeding (unreliable)
First 12~18 rolls are low quality
Debuggable by default (can see the distance between two states)
MCG: Lehmer64,Lehmer128
Period: guaranteed
Seed: must be odd
XLCG based: Diver,Quixotic
Period: guaranteed
Stream: changing the xor number (untested), sequence
LCG based: PCG,Mizuchi,Linnorm
Period: guaranteed
Stream: changing sequence
Sequence based: wyrand,splitmix,velvet,thrust(alt),tangle,spiral,jab63,duelist,dervish,bright,orbit
Period: guaranteed
Stream: by teleporting, xoring before a multiply in mixer (untested)
Can transform into increment based
Increment based: Pulley,Pelican,Topping,Dirk,NASAM
Period: guaranteed
Stream: by teleporting, xoring before a multiply in mixer (untested)
Trivially resizable: you can make a good prng with smaller/bigger period by adjusting the state size while keeping output size
Debuggable by default (can see the distance between two states)
Other: TinyMT, Bell
Other and seed must not be zero: Lanyard, xoshiro256plusplus, xoshiro256starstar, xoshiro256plus, xoroshiro128plusplus, xoroshiro128starstar, xoroshiro128plus, seiran128, shioi128, Seaslater
Approaches for mapping bits to a range
One of the most overlooked factors in PRNG is that the process of mapping a random number into a range is more relevant to latency and throughput than the PRNG used.

In this Battle Royale we will benchmark three methods of mapping numbers to a range:

Modulo (biased): the most common way to map an integer to a range rand()%5, it introduces a small bias in the lower part of generating range
Masking with discard (unbiased): a fair mapping approach attributed to Apple I found about a few years ago in this post of Melissa O’Neill
Fastrange (biased): an alternative to modulo reduction based on fixed-point multiplication by Lemire
Lemire also proposed a nearly division-free unbiased reduction but I believe Travis Downs later pointed out in the comments section that the method may not be fully unbiased, so I will not include it in this round of the Battle Royale.

The Battle Royale
The purpose of the PRNG Battle Royale is to find the one PRNG to rule game development : A PRNG that is statistically sound, with great average performance and has the best worst case performance.

In order to guarantee the best worst case performance all PRNGs have been benchmarked in all game consoles using multiple tests. Each using the numbers in different ways in order to provide empirical guarantee that the winning PRNG worst case doesn’t underperform other alternatives when performing one certain operation.

Some notes regarding each individual test:

An individual test is defined by a combination of: console, PRNG, mapping, category and operation
Each test generates at least 256GB+ of random data.
Range tests are performed on a big amount of ranges (2~2^16).
At least one operation is used on the data (store, addition, multiplication, xoring, masking…).
Barriers were used to guarantee behaviour measurement.
The tests also verify the integrity and determinism of the code in the different consoles.
About the charts, important notes:

In all charts higher is faster
Charts are in a candlestick format where:
The upper shadow (highest point of the line): is the best result across all consoles.
The lower shadow (lowest point of the line): is the worst result across all consoles.
The actual body (open/close) tells the 33% to 66% percentiles across all consoles
All charts share the same scale and are computed in relative speed compared to the slowest test result of the console. This allows cross-chart and cross-console data comparisons of the PRNGs without allowing comparisons of the different consoles.
Charts use as source data the relative time to complete a test in a console compared to the slowest test that ran in that console, so for example: 2.2 implies “the test was 2.2 faster than the slowest test that ran in this game console”.
This battle royale has been designed to meet NDA requirements, so I will keep the code private and the results will never ever allow comparison between different consoles.
Regarding the two benchmarked categories:

Latency tests measure how fast each generator provides a single random number which is immediately operated on.
Throughput tests measure how fast can a generator continuously generate random numbers.
Also, I want to insist that this is a hunt for the one : In a more particular situation a tailored solution will always reign supreme. If you need more throughput, you can use multiple generators or a vectorized approach (some PRNGs such as Shishua excel at this). As for latency, you may consider improving latency at the cost of additional register/memory pressure by computing next roll and returning the previous one (multiple PRNG families such as Romu use this technique to improve latency).

Raw 64-bit generation
This mapping measures generating any power-of-two amount of data at once, from 1-bit to GBs.

I found the results of lehmer128 very interesting and after a thorough investigation I discovered that two of the SDKs were generating sub-optimal code for the 128-bit operations that this PRNG performed. At this point I could have hand-rolled an optimized version for that compiler but because all tests were built with top tier configuration presets using the latest versions of each platform SDK and Visual Studio 2019 I decided it was important to keep all the PRNGs as-is. Also, this issue regarding 128-bit operations impacts, to a much lesser extent, both pcg64 and lehmer64 PRNGs.

Raw 64-bit generation (throughput)
Raw 64-bit generation (latency)
Biased modulo generation in range
This mapping measures the traditional way of generating numbers in a range, performing a modulo operation on the raw 64-bit value to map that 64-bit value to a range [0,N)

As you can see, while latency feels ok throughput is really bad.

Biased modulo to range (throughput)
Biased modulo to range (latency)
Unbiased masking to range
This mapping generates numbers in a range rolling numbers again and again until the number, truncating with a bitmask calculated as nextPowerOfTwo-1 fits the target range.

The charts show that generating a fair mapping puts a high overhead on latency, making it slower than modulo. However, the throughput is still way better than modulo.

Unbiased masking to range (throughput)
Unbiased masking to range (latency)
Biased Fixed-Point generation
With floats you can map any random number Y=[0.00,1.00) to integer range [0,X) multiplying Y*X and truncating.

This technique popularized by Lemire beats all alternatives by applying the previous concept to generate integer numbers using fixed-point math. The process is straightforward: you can map any 32-bit value (0.32) to the range [0,5) by multiplying by 5 (32.0) which will result in a number in the 32.32 format that after being divided by (2^32) will result in a 32.0 value, this is (rand()*5ULL)>>32

Biased fixed-point to range (throughput)
Biased fixed-point to range (latency)
PractRand
Regarding PRNG quality, after running PractRand for a few days the following results were obtained, which mostly match what is expected (X! fails at X, X+ reached X):

PractRand
Keep in mind that failing is not the end of the world, very well known PRNGs fail PractRand and some such as the xoshiros/xoroshiros only fail because of the linearity of specific bits and are still used by people that understand what they’re doing. Also, take into consideration that a PRNG that is capable of passing PractRand 0.95b may not be flawless and it may fail future versions of PractRand.

Because the costs of running all the tests were ramping up and I had another set of “particular PractRand” I decided to stop the “regular” PractRand.

The gist of this other approach is that given an amount of random numbers, a sufficiently large subset of the random numbers that has been selected and/or shuffled without observing the random numbers themselves should also be a solid generator.

To explain it with an image:

SubPRNG
I believe that this property is very interesting for PRNGs in videogames because we share random number generators between multiple roll sources.

An example of this may be: each enemy has 2 fields that are rolled from the random number generator (enemyType, itemDrop). If rolling with a PRNG that doesn’t meet this criteria, the results of each individual field may not be statistically random and a specific enemyType may appear more than the rest. This is trivially solvable by using a separate PRNG for each roll but I believe the one should have this property.

To test for this I ran 16GB of PractRand multiple times, with each test picking a subset of the random numbers generated (using as the only selection criteria the index of that roll). The results were quite surprising:

PractRand SubPRNG results
Woah! Holy ****! when I started this I had no idea such a simple change would end up like this… Some notes in case you want to perform an independent review of this fact:

All the tests were performed with PractRand 0.95b.
Multiple selection criteria were tested:
One out of every X random numbers:
Where: X<512
Where: X = Y^2 and Y < 23
Where: X = random()
All random numbers except one every X random numbers:
Where: X<512
Where: X = Y^2 and Y < 23
Where: X = random()
Winner
My takeaway after measuring bias with 64-bit generators and performing the Battle Royale is that for game development only the fixed-point method is relevant: the bias is too small to warrant the additional latency of a fair mapping and modulo is just as biased but way slower than fastrange.

The following chart amalgamates all the important information for what I believe are the most common scenarios in game development (generating numbers in a range). The chart also shows which PRNGs failed a PractRand (0.95b) test, where: * Hue indicates the amount of fails and my subjective perceived severity of the failures * Opacity indicates whether the fail was running the PRNG or a selection of the PRNG values based on index.

TLDR
Thus, in the first round ever of the PRNG Battle Royale, I, a random game developer on the internet, declare Romu2jr winner of the PRNG Battle Royale. Huge disclaimer: As stated previously, a PRNG that passes PractRand may fail a future version of PractRand, so wise people may be interested in picking an older alternative with guaranteed period such as xoshiro256++ or sfc64.

As for myself, when I processed the results I was really sad that Romu2jr won. Not that I’m biased against Mark Overton or romu itself. I just hate getting such a fast PRNG just to be told “Yeah, it’s almost impossible that it happens but a seed may exist which results in a shitty period”. I can only shudder at this happening when a popular youtuber is streaming your game…

Lots of people play video games every hour of every day, so we just can’t risk this scenario, no matter how unlikely it may be.

So, what can we do about it? Should we just forget about such an awesome PRNG? I think not.

Some implementations avoid this issue by providing a table of seeds known to have a huge cycle. I don’t like this approach that much, even less so when there is no “jump” function provided by the generator. So I went ahead and grabbed two magic constants from one of the generators, used them to initialize the seed, wrote some CUDA code and I can empirically guarantee by the mighty power of brute force that with that initializer function:

All 33-bit seeds (including 0) have no cycles within the first 2^24 bytes
All 17-bit seeds (including 0) have no cycles within the first 2^36 bytes
A separate issue I originally forgot to tackle is that chaotic PRNGs generate a few low quality rolls after the initial seeding. This can be tested by with a generic FirstRollsPRNG which seeds a separate PRNG and rolls two or three times before reseeding with previous_seed+1. Most chaotic generators overcome this by skipping a few rolls, in xromu2jr case it took 16 rolls to get from 2^12 bytes to more than 2^34 bytes.

However, because this is the winner, I came up with a faster alternative: Using a separate increment based PRNG to mix the seed which also allows for statistically independent sequential seeding.

After evaluating multiple mixers such as After evaluating multiple mixers that worked perfectly such as dwyrand (3m,2k), ThrustRNG (2m,2k), SplitMix64 (3m,3k), even Chris Wellons prospected functions I ended up using the NASAM permutation function by Pelle Evensen. The resulting PRNG initializer is much less expensive than 16 PRNG rolls and packs quite a nice feature set:

Makes xromu2jr generators trivially seedable with an uint32_t counter.
Doesn’t require 128-bit integer operations, so it’s easier to probe seed space in a CUDA kernel.
Only 3 multiplications used to generate two 64-bit values for the initial state.
Only 1 constant to make the initializer more inlineable (and perhaps faster on ARM).
No fixed point at zero thanks to a small hack
The initial value of Y is NASAM without the initial rotate, rotate, xor.
The initial value of X is a rotate-multiply of the last multiplication, so it can be pipelined.
Here is the winning prng code wrapped in an API for provinding fast: range, sign and floats

Future Work
I’m still running another batch of PractRand tests, will update the data when the tests finish running.

I hope to hold another PRNG Battle Royale at a later date (probably in a year or two, as this test took almost 3 months of work), including new PRNGs and PRNGs that I missed such as NASAM. Also, I’m interested into expanding this test to 32-bit generators (because most of the time, for game development 32 bits of output may be enough, and it might hurt to run 64 bit generators in javascript).

Also, in case I left out your PRNG: I’m sorry, feel free to contact me at @xnwdd and I promise to add it to a future round.

Regarding comments, this blog is 100% JavaScript free, so feel free to reply to the twitter announcement, the /r/prng post at reddit or Hacker News.

Special Thanks
To everyone who has helped making the first round of PRNG Battle Royale possible: Chui who got the benchmark running in two additional platforms, Dew from GaiaByte who explained and offered tips and pointers that got the CUDA code faster, Lean Andrew who helped updating and getting consoles up, Tony Cabello for proofreading this blog post, BlitWorks for being such an awesome company to work at and Tath who did a great job editing the first edition of this post (with very little time) and will continue to improve it over time.

Also to the authors of the all the incredible techniques and research used in this post that have been worked on for years. A lot of effort and compute power went into them and some of their discoveries are still pretty obscure: Daniel Lemire, Iñigo Quilez, Chandler Carruth, Wang Yi, Mark A.Overton, Bob Jenkins, M.E. O’Neill, Chris Doty-Humphrey (who is also the creator of PractRand), Tommy Ettinger, Pelle Evensen, Sebastiano Vigna, David Blackman, Andante, espadrine, Mutsuo Saito & Makoto Matsumoto, Takuji Nishimura and George Marsaglia [F]
