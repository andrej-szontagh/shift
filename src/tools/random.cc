
#include <limits.h>
#include <time.h>
#include <math.h>

#include "types.h"
#include "random.h"


// Mersenne Twister random generator

#define TWIST(m, s0, s1) (m ^ (((s0 & 0x80000000UL) | (s1 & 0x7fffffffUL)) >> 1) ^ ((s1 & 0x00000001UL) ? 0x9908b0dfUL : 0x0UL))

#define RAND_N 624
#define RAND_M 397


struct TRand 
{		
	UINT_32		state [RAND_N];	// internal state
	UINT_32P	next;			// next value to get from state

	INT_32		left;			// number of values left before reload needed

} random_generator;


VOID randInitialize (UINT_32 seed)
{
	register UINT_32P s = random_generator.state;
	register UINT_32P r = random_generator.state;

	register INT_32 i = 1;

	* s++ = seed & 0xffffffffUL;

	for (; i < RAND_N; ++i) {

		*s++ = (1812433253UL * (*r ^ (*r >> 30)) + i) & 0xffffffffUL; r++;
	}
}

VOID randReload ()
{
	static const INT_32 MmN = INT_32 (RAND_M) - INT_32 (RAND_N);

	register UINT_32P p = random_generator.state;

	register INT_32 i;

	for (i = RAND_N - RAND_M; i--;	++p)	*p = TWIST (p [RAND_M],	p [0], p [1]);
	for (i = RAND_M;		   --i;	++p)	*p = TWIST (p [MmN],	p [0], p [1]);

	*p = TWIST (p [MmN], p [0], random_generator.state [0]);

	random_generator.next = random_generator.state;

	random_generator.left = RAND_N;
}

VOID randSeed (UINT_32 oneSeed)
{
	randInitialize (oneSeed);
	randReload ();
}

VOID randSeed (UINT_32P bigSeed, UINT_32 seedLength)
{
	randInitialize (19650218UL);

	register INT_32 i = 1;
	register INT_32 k = (RAND_N > seedLength ? RAND_N : seedLength);

	register UINT_32 j = 0;

	for (; k; --k) {

		random_generator.state [i] =
		random_generator.state [i] ^ ((random_generator.state [i - 1] ^ (random_generator.state [i - 1] >> 30)) * 1664525UL);
		random_generator.state [i] += (bigSeed [j] & 0xffffffffUL) + j;
		random_generator.state [i] &= 0xffffffffUL;

		++ i;  ++ j;

		if (i >= RAND_N) { random_generator.state [0] = random_generator.state [RAND_N - 1];  i = 1; }

		if (j >= seedLength) j = 0;
	}

	for (k = RAND_N - 1; k; -- k) {

		random_generator.state [i] =
		random_generator.state [i] ^ ((random_generator.state [i - 1] ^ (random_generator.state [i - 1] >> 30)) * 1566083941UL);
		random_generator.state [i] -= i;
		random_generator.state [i] &= 0xffffffffUL;

		++ i;

		if (i >= RAND_N) { random_generator.state [0] = random_generator.state [RAND_N - 1];  i = 1; }
	}

	random_generator.state [0] = 0x80000000UL; 
	
	randReload ();
}

VOID randSeed ()	{

	time_t t = time  (NULL);
	clock_t c = clock ();

	static UINT_32 differ = 0;
	
	UINT_32 h1 = 0;

	UINT_8P p = (UINT_8P) &t;

	for (size_t i = 0; i < sizeof (t); ++ i) {

		h1 *= UCHAR_MAX + 2U;
		h1 += p [i];
	}

	UINT_32 h2 = 0;		p = (UINT_8P) &c;

	for (UINT_32 j = 0; j < sizeof (c); ++ j) {

		h2 *= UCHAR_MAX + 2U;
		h2 += p [j];
	}

	randSeed ((h1 + differ ++) ^ h2);
}

UINT_32 randInt ()
{	

	if (random_generator.left == 0) randReload ();

	-- random_generator.left;
	
	register UINT_32 s1;

	s1 = *random_generator.next ++;

	s1 ^= (s1 >> 11);
	s1 ^= (s1 <<  7) & 0x9d2c5680UL;
	s1 ^= (s1 << 15) & 0xefc60000UL;

	return (s1 ^ (s1 >> 18));
}

FLOAT_64 randDouble () { 

	return (FLOAT_64 (randInt ()) * (1.0 / 4294967295.0)); 
}

