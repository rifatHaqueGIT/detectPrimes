// =============================================================================
// You must modify this file and then submit it for grading to D2L.
// =============================================================================

#include "detectPrimes.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>

// returns true if n is prime, otherwise returns false
// -----------------------------------------------------------------------------
// you may need to modify this function, or write your own equivalent
// in order to parallelize this test
static bool is_prime(int64_t n)
{
  if (n < 2) return false;
  if (n <= 3) return true; // 2 and 3 are primes
  if (n % 2 == 0) return false; // handle multiples of 2
  if (n % 3 == 0) return false; // handle multiples of 3
  // try to divide n by every number 5 .. sqrt(n)
  int64_t i = 5;
  int64_t max = sqrt(n);
  while (i <= max) {
    if (n % i == 0) return false;
    if (n % (i + 2) == 0) return false;
    i += 6;
  }
  // didn't find any divisors, so it must be a prime
  return true;
}

// this funciton tests all numbers in mums[] and returns a new vector containing
// only those numbers that are primes
// -----------------------------------------------------------------------------
// you will most likely need to re-implement this function entirely
std::vector<int64_t>
detect_primes(const std::vector<int64_t> & nums, int n_threads)
{
  std::vector<int64_t> result;
  for (auto num : nums) {
    if (is_prime(num)) result.push_back(num);
  }
  return result;
}
