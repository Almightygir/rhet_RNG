struct xromu2jr { //Wrapper released by NWDD into the public domain - https://rhet.dev
	//Sugar methods
	float    rf32(float c) { return rf32() * c; }
	double   rf64(double c) { return rf64() * c; }
	float    rf32(float f, float c) { return f + rf32() * (c - f); }
	double   rf64(double f, double c) { return f + rf64() * (c - f); }
	int32_t  ri32() { return as<int32_t>(ru64()); }
	int64_t  ri64() { return as<int64_t>(ru64()); }
	uint32_t ru32() { return uint32_t(ru64()); }
	int32_t  ri32(int32_t f, int32_t c) { return f + ru32(c - f); }
	int64_t  ri64(int64_t f, int64_t c) { return f + ru64(c - f); }
	uint32_t ru32(uint32_t f, uint32_t c) { return f + ru32(c - f); }
	uint64_t ru64(uint64_t f, uint64_t c) { return f + ru64(c - f); }
	//Bit twiddling methods
	template<class D, class S> static D as(S v) { return *reinterpret_cast<D*>(reinterpret_cast<uint8_t*>(&v)); }
	int32_t  si32() { return (ru32()&2) - 1; } //generate fast 32-bit integer sign
	int64_t  si64() { return (ri64()&2) - 1LL; } //generate fast 64-bit integer sign
	float    sf32() { return as<float>(0x3F800000U | (ru32()<<31)); } //generate fast float sign
	double   sf64() { return as<double>(0x3FF0000000000000ULL | (ru64()<<63)); } //previous adapted for doubles
	uint64_t bits(uint64_t b) { return ru64() >> (64-b); } //get a specific amount of bits
	//iq fast float generation - https://iquilezles.org/www/articles/sfrand/sfrand.htm
	float    rf32() { return as<float>(0x3F800000U | bits(23)) - 1.0f; }
	double   rf64() { return as<double>(0x3FF0000000000000ULL | bits(55)) - 1.0; } //previous adapted for doubles
	//Lemire FastRange using fixed-point - https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
	uint32_t ru32(uint32_t N) { return (uint32_t)(((uint64_t)N * ru32())>>32);  }
	uint64_t ru64(uint64_t N) { //Lemire FastRange using fixed-point
#		if defined(__SIZEOF_INT128__)
			return (uint64_t)(((N) * (__uint128_t)ru64()) >> 64);
#		else
			return __umulh(ru64(), N);
#		endif
	}
	//RomuDuoJr: a 128-bit state 64-bit output prng designed by Mark Overton - https://romu-random.org/romupaper.pdf
	uint64_t x,y;
	uint64_t ru64() {
		uint64_t xp = x;
		x = y * 0xD3833E804F4C574BULL;
		y = y - xp;
		y = y << 27 | y >> 37;
		return xp;
	}
	//Dirty init: first ~12 rolls are bad, no cycles found (33-bit seeds: 2^24 bytes, 17-bit seeds: 2^36 bytes)
	//xromu2jr(uint64_t s) : x(s^0x9E3779B97F4A7C15ULL), y(s^0xD1B54A32D192ED03ULL) {}
	//Finer init: based on Pelle Evensen - https://mostlymangling.blogspot.com/2020/01/nasam-not-another-strange-acronym-mixer.html
	xromu2jr(uint64_t s) : x(0x9E6C63D0676A9A99ULL), y(!s-s) { // no cycles found (32-bit seeds: 2^24 bytes, 16-bit seeds: 2^36 bytes)
		y *= x;
		y ^= y >> 23 ^ y >> 51;
		y *= x;
		x *= y << 27 | y >> 37;
		y ^= y >> 23 ^ y >> 51;
	}
};
