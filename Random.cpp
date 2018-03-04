#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifndef _RANDOM_CPP_
#define _RANDOM_CPP_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Random.h"

/*Random Number generator  - Mersenne Twister
Essentially, the Mersenne Twister is a very large linear-feedback shift
register.
The algorithm operates on a 19,937 bit seed, stored in an 624-element array of
32-bit unsigned integers.
 The value 219937-1 is a Mersenne prime; the technique for manipulating the seed
is based on an older "twisting" algorithm
 -- hence the name "Mersenne Twister".

John Savard provides an excellent description of the algorithm here.
One of the appealing aspects of the Mersenne Twister is its use of binary
operations --
as opposed to time-consuming multiplication -- for generating numbers.
 The algorithm's period stems from the size of it's shift register -- 219937-1
(~106001),
 which is quite large when compared to a period of ~108 for the best variants of
the linear congruential methods.
 For most practical purposes, the Mersenne Twister doesn't repeat itself.
 And the basic algorithm generates 32-bit integers,
  which provide a greater granularity than 16-bit linear congruential
generators.

Granularity allows finer distinctions between individual values and is
 particularly important when dealing with floating-point numbers. PRNGs output
integers;
 to obtain a floating point number -- say, between 0 and 1 -- divide the random
integer by the maximum possible value for that size.
  When using a 16-bit PRNG, the biggest difference in floating-point values is
1/216, or 0.000015, while
  a 32-bit PRNG provides a granularity of 1/232, or 0.00000000023. While a
16-bit generator is really only useful for the 7-digit precision of a 32-bit
IEEE floating-point number, a 32-bit PRNG produces values suitable for the
64-bit IEEE "double" type. Smaller granularity means a broader range of results,
which is required by stochastic algorithms that solve large problems to a fine
degree.
*/
using namespace std;

Random::Random()
    : N(STATEVECTOR), MM(PERIOD), lbound(1), ubound(100), seed(5489UL) {
  Init();
}

Random::~Random() {}
inline void Random::setPeriod(int m) {
  MM = m;

#ifdef DEBUG_ON
  cout << endl << " setStateVector " << N << endl;
#endif
}
/*set the seed*/
/*
void Random::setSeed(unsigned long _seed)
{
seed = _seed;
}
*/
/*set the stateVector*/
inline void Random::setStateVector(int n) {
  N = n;

#ifdef DEBUG_ON
  cout << endl << " setStateVector " << N << endl;
#endif
}
void Random::Init() {
  mti = STATEVECTOR + 1;

  int length = 4;
  unsigned long init[4] = {0x123, 0x234, 0x345, 0x456};

  this->init_by_array(init, length);
}
/*print a vector*/
void Random::printVector(std::vector<unsigned long> *randVec) {
  unsigned long value;
#ifdef DEBUG_ON
  cout << endl << " printVector " << endl;
#endif
  vector<unsigned long>::const_iterator cii;
  for (cii = randVec->begin(); cii != randVec->end(); cii++) {
    value = (unsigned long)*cii;
    cout << " value " << value << endl;
  }
}

/*get value within a certain range Inclusive Version*/
int Random::getint32BoundedValue(int lbound, int ubound) {

  // RAND_MAX 2147483647
  // std::cout<<" RAND_MAX"<<RAND_MAX<<std::endl;

  // Neither endpoint excluded

  int value = (int)genrand_int32();
  int val = (int)(value / (RAND_MAX + 1.0) * (ubound - lbound + 1)) + lbound;
  // printf("A value between [%d and %d]: %d \n", lbound, ubound, abs(val));

  /*	 // Neither endpoint excluded
          int val = ( value/ (RAND_MAX + 1.0) * (ubound - lbound + 1)) + lbound;
        printf("A value between [%d and %d]: %d\n", lbound, ubound, val);

      // Upper endpoint excluded
       val = (rand() / (RAND_MAX + 1.0) * (ubound - lbound)) + lbound;
      printf("A value between [%d and %d): %d\n", lbound, ubound, val);

      // Lower endpoint excluded
       val = (rand() / (RAND_MAX + 1.0) * (ubound - lbound)) + lbound + 1;
      printf("A value between (%d and %d]: %d\n", lbound, ubound, val);

      // Both endpoints excluded
       val = (rand() / (RAND_MAX + 1.0) * (ubound - lbound - 1)) + lbound + 1;
      printf("A value between (%d and %d): %d\n", lbound, ubound, val);
    */
  return val;
}
/*add random values to a std::vector<ul> type*/
void Random::getint32Array(std::vector<unsigned long> *randVec, int num) {

#ifdef DEBUG_ON
  cout << " getint32Array " << endl;
#endif
  int length = 4;
  int i;

  unsigned long init[4] = {0x123, 0x234, 0x345, 0x456};

  this->init_by_array(init, length);

  cout << num << " outputs of genrand_int32()" << std::endl;

  for (i = 0; i < num; i++) {

    int value = (int)genrand_int32();

    randVec->push_back(value);
  }

#ifdef DEBUG_ON
  cout << "END getint32Array " << endl;
#endif
}
/*add values to a std::vector<float> type  num represents the number of values
 * requested*/
void Random::getReal32Array(std::vector<double> *randVec, int num) {

#ifdef DEBUG_ON
  cout << " getint32Array " << endl;
#endif
  int length = 4;
  int i;

  unsigned long init[4] = {0x123, 0x234, 0x345, 0x456};

  this->init_by_array(init, length);

  cout << num << " outputs of genrand_real1()" << std::endl;

  for (i = 0; i < num; i++) {

    double value = (double)genrand_real1(); // inclusive [] version
    cout << " real1() " << value << std::endl;
    randVec->push_back(fabs(value));
  }

#ifdef DEBUG_ON
  cout << "END getint32Array " << endl;
#endif
}
/* initializes mt[N] with a seed */
void Random::init_genrand(unsigned long s) {
#ifdef DEBUG_ON
  cout << " init_genrand " << s << endl;
#endif

  mt[0] = s & 0xffffffffUL;
  for (mti = 1; mti < N; mti++) {
    mt[mti] = (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, MSBs of the seed affect   */
    /* only MSBs of the array mt[].                        */
    /* 2002/01/09 modified by Makoto Matsumoto             */
    mt[mti] &= 0xffffffffUL;
    /* for >32 bit machines */
  }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void Random::init_by_array(unsigned long init_key[], int key_length) {
  int i, j, k;
  init_genrand(seed); // 19650218UL
  i = 1;
  j = 0;
  k = (N > key_length ? N : key_length);
  for (; k; k--) {
    mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1664525UL)) +
            init_key[j] + j; /* non linear */
    mt[i] &= 0xffffffffUL;   /* for WORDSIZE > 32 machines */
    i++;
    j++;
    if (i >= N) {
      mt[0] = mt[N - 1];
      i = 1;
    }
    if (j >= key_length)
      j = 0;
  }
  for (k = N - 1; k; k--) {
    mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1566083941UL)) -
            i;             /* non linear */
    mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
    i++;
    if (i >= N) {
      mt[0] = mt[N - 1];
      i = 1;
    }
  }

  mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
#ifdef DEBUG_ON
  cout << "END init_by_array" << endl;
#endif
}
/*get value within a certain range Inclusive Version*/
double Random::getReal1BoundedValue(double lbound, double ubound) {

  double value = (double)genrand_real1();
  double val = ((double)value * (ubound - lbound)) +
               lbound; //( (double)value * (ubound - lbound )+1) ;

  //  printf("A value between [%f and %f]: %f \n", lbound, ubound, (val));

  return val;
}
/* generates a random number on [0,0xffffffff]-interval */
unsigned long Random::genrand_int32(void) {
  unsigned long y;
  static unsigned long mag01[2] = {0x0UL, MATRIX_A};
  /* mag01[x] = x * MATRIX_A  for x=0,1 */

  if (mti >= N) { /* generate N words at one time */
    int kk;

    if (mti == N + 1)     /* if init_genrand() has not been called, */
      init_genrand(seed); /* a default initial seed is used */

    for (kk = 0; kk < N - MM; kk++) {
      y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
      mt[kk] = mt[kk + MM] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for (; kk < N - 1; kk++) {
      y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
      mt[kk] = mt[kk + (MM - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
    mt[N - 1] = mt[MM - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

    mti = 0;
  }

  y = mt[mti++];

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);

  return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long Random::genrand_int31(void) { return (long)(genrand_int32() >> 1); }

/* generates a random number on [0,1]-real-interval */
double Random::genrand_real1(void) {
  return genrand_int32() * (1.0 / 4294967295.0);
  /* divided by 2^32-1 */
}

/* generates a random number on [0,1)-real-interval */
double Random::genrand_real2(void) {
  return genrand_int32() * (1.0 / 4294967296.0);
  /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double Random::genrand_real3(void) {
  return (((double)genrand_int32()) + 0.5) * (1.0 / 4294967296.0);
  /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double Random::genrand_res53(void) {
  unsigned long a = genrand_int32() >> 5, b = genrand_int32() >> 6;
  return (a * 67108864.0 + b) * (1.0 / 9007199254740992.0);
}
/* These real versions are due to Isaku Wada, 2002/01/09 added */

/*generate Normally Distributed values */
double Random::genrand_NormalDist(const double &mean, const double &sd) {
  double randValue = double(genrand_real1());

  double N = ((double)1 / (double)sd * sqrt(2 * M_PI)) *
             exp(((double)-1 / (double)2) * pow(((randValue - mean) / sd), 2));

  return N;
}
/*gaussian distribution*/
double Random::randgauss(double min, double max, double sigma, double centre) {
  double random =
      (min +
       (max - min) *
           double(genrand_real1())); // create random domain between [min,max]

  double tmp = (random - centre) / sigma;
  double gauss = exp(-tmp * tmp / 2); // gaussian formula

  return gauss;
}
/*Save State array into registers*/
inline void Random::save(unsigned long *saveArray) const {
  register unsigned long *sa = saveArray;
  register const unsigned long *s = mt;
  register int i = N;
  for (; i--; *sa++ = *s++) {
  }
  *sa = mti;
}
/*Bos Muller*/
float Random::box_muller(float m, float s) /* normal random variate generator */
{                                          /* mean m, standard deviation s */
  float x1, x2, w, y1;
  static float y2;
  static int use_last = 0;

  if (use_last) /* use value from previous call */
  {
    y1 = y2;
    use_last = 0;
  } else {
    do {
      x1 = 2.0 * genrand_real1() - 1.0;
      x2 = 2.0 * genrand_real1() - 1.0;
      w = x1 * x1 + x2 * x2;
    } while (w >= 1.0);

    w = sqrt((-2.0 * log(w)) / w);
    y1 = x1 * w;
    y2 = x2 * w;
    use_last = 1;
  }

  return (m + y1 * s);
}
#endif
/*
inline void MTRand::load( unsigned long* const loadArray )
{
        register uint32 *s = state;
        register uint32 *la = loadArray;
        register int i = N;
        for( ; i--; *s++ = *la++ ) {}
        mti = *la; //left
        pNext = &state[N-left];
}*/
