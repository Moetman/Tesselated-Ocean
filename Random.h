/*********************************************************************************
 * MONASH UNIVERSITY, School of Computer Science and Software Engineering
 * Student Declaration for CSE3301 Submission 2005
 *
 * This source code is the original work of Paul Pocock, ID: 18358667
 * My email address is pdpoc1@student.monash.edu
 * I, Paul Pocock declare that this submission is my own work and has not been
copied from
 * any other source without attribution. I have read the School and Faculty
 * rules on plagiarism and I acknowledge that severe penalties
 * exist for any copying of code without attribution, including a mark of 0
 * for this assessment.

 Version History
 AI Experiments  : Last Modified 31 July 8:23am


Mersenne Twister
The Mersenne twister is a pseudorandom number generator developed in 1997 by
Makoto Matsumoto (松本 眞, Makoto Matsumoto?) and Takuji Nishimura (西村 拓士,
Takuji Nishimura?)[1] that is based on a matrix linear recurrence over a finite
binary field F2. It provides for fast generation of very high-quality
pseudorandom numbers, having been designed specifically to rectify many of the
flaws found in older algorithms.
 *********************************************************************************/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define STATEVECTOR 624
#define PERIOD 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

// static unsigned long mt[STATEVECTOR]; /* the array for the state vector  */
// static int mti=STATEVECTOR+1; /* mti==N+1 means mt[N] is not initialized */

class Random {
public:
  Random();
  ~Random();
  /*set the stateVector*/
  inline void setStateVector(int n);
  /*initialize state vector*/
  void Init();
  /*set the period*/
  inline void setPeriod(int m);
  /*set the seed*/
  inline void setSeed(unsigned long _seed) { seed = _seed; }
  /*add random values to a std::vector<ul> type*/
  void getint32Array(std::vector<unsigned long> *randVec, int num);
  /*add values to a std::vector<float> type*/
  void getReal32Array(std::vector<double> *randVec, int num);
  /*print values in std::vector*/
  void printVector(std::vector<unsigned long> *randVec);
  /*get value within a certain range*/
  int getint32BoundedValue(int lbound, int ubound);
  /*get Real value witin a certain Range*/
  double getReal1BoundedValue(double lbound, double ubound);
  /* initializes mt[N] with a seed */
  void init_genrand(unsigned long s);
  /* initializes mt[N] with an array */
  void init_by_array(unsigned long init_key[], int key_length);
  /* generates a random number on [0,0xffffffff]-interval */
  unsigned long genrand_int32(void);
  /* generates a random number on [0,0x7fffffff]-interval */
  long genrand_int31(void);
  /* generates a random number on [0,1]-real-interval */
  double genrand_real1(void);
  /* generates a random number on [0,1)-real-interval */
  double genrand_real2(void);
  /* generates a random number on (0,1)-real-interval */
  double genrand_real3(void);
  /* generates a random number on [0,1) with 53-bit resolution*/
  double genrand_res53(void);
  /*value returned between [N,M-1]*/
  double genrand_NormalDist(const double &mean, const double &sd);
  /*gaussian distribution*/
  double randgauss(double min, double max, double sigma, double centre);
  /*save function using registers*/
  inline void save(unsigned long *saveArray) const;
  /*Box Muller*/
  float box_muller(float m, float s);

private:
  int N;
  int MM;

  int lbound;
  int ubound;

  unsigned long seed;

  unsigned long mt[STATEVECTOR];
  int mti;
};
#endif
