// =============================================================================
// You must modify this file and then submit it for grading to D2L.
// =============================================================================

#include "detectPrimes.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <mutex>
//The simple_barrier is made by Pavol Federl and the use of a runner function was also made by Federl, but its been changed.
class simple_barrier {
  std::mutex m_;
  std::condition_variable cv_;
  int n_remaining_, count_;
  bool coin_;
public:
  simple_barrier(int count) {
    count_ = count;
    n_remaining_ = count_;
    coin_ = false;
  }
  bool wait() {
    std::unique_lock<std::mutex> lk(m_);
    if( n_remaining_ == 1) {
      coin_ = ! coin_;
      n_remaining_ = count_;
      cv_.notify_all();
      return true;
    }
    auto old_coin = coin_;
    n_remaining_ --;
    cv_.wait(lk, [&](){ return old_coin != coin_; });
    return false;
  }
};
//memory for threads
struct Task{
  int64_t start;
  int64_t end;
  int id;
 
};
//GLOBAL VARIABLES:
std::vector<int64_t> result;
std::atomic<bool> flag (false);
std::unordered_map<int,int64_t> dupNums;//DIDNT ADD THIS YET
int64_t num;//making a global num for all threads
int64_t m;
std::vector<int64_t> numsUsed;
int n_thread;
size_t iUsed;//another global
std::atomic<int> value (0);

// returns true if n is prime, otherwise returns false
// -----------------------------------------------------------------------------
// to get full credit for this assignment, you will need to adjust or even
// re-write the code in this function to make it multithreaded.
#if 1
void is_prime(int64_t n, int64_t start, int64_t end)
{
  // handle trivial cases
  if(value >= 1) return; // same as the one inside the loop
  if (n < 2) {value.fetch_add(1, std::memory_order_relaxed);return;}//
  if (n <= 3) {return;} // 2 and 3 are primes 
  if (n % 2 == 0) {value.fetch_add(1, std::memory_order_relaxed);return;} // handle multiples of 2
  if (n % 3 == 0){value.fetch_add(1, std::memory_order_relaxed);return; }// handle multiples of 3
  // try to divide n by every number 5 .. sqrt(n)
  int64_t i = start; //Starts at 5 originally
  int64_t max = end; //originally sqrt(n)
  while (i <= max) {
    if(value > 0) return;// if there ever was a false for this number then it will never be true
    if (n % i == 0) {value.fetch_add(1, std::memory_order_relaxed);return;}//flag2.store(false, std::memory_order_release);
    if (n % (i + 2) == 0){ value.fetch_add(1, std::memory_order_relaxed);return;}
    i += 6;
  }
  // didn't find any divisors, so it must be a prime
  
}
#endif
#if 0
static bool is_prime(int64_t n)
{
  // handle trivial cases
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
#endif
#if 1
//this section inspired by code from Xining's barrier2 and Pavol Federl aswell as other modifications made by myself.
void runner(Task & task, simple_barrier & barrier ) {
  while(1){
    int n = n_thread;
    if( barrier.wait()) {
      //serial code to get nums

      if(iUsed == numsUsed.size()){//if vector is empty end the runner
        flag.store(true, std::memory_order_release);// setting end of runner
      
      }
      else {
        num = numsUsed.at(iUsed);//global num for all threads single thread rights to it so its fine
        m = sqrt((num))/(6*n); // M from Xining's solution 2 for splitting the work
        iUsed++;
      }
      //serial code to get num
    }
   
    barrier.wait();// WAITING FOR SERIAL TASK TO FINISH (just in case)
    int64_t numUsed = num;//consider moving this
    int64_t mUsed = m;
    
    
    if(flag.load()) break;
   // if statement to stop if num isnt very big
    if((n_thread == 1|| mUsed <= 50)){//only one thread .&&task.id == 0
    //if statement to take in make
       task.start = 5;
       task.end = sqrt(numUsed);// the end for the last thread is the square of the number
       is_prime(numUsed,task.start,task.end);
    }
     else{
    if(task.id == 0){//first thread
      task.start = 5;
      task.end = 5 + 6*mUsed;//implied tid 0 so we put tid value of 1 (the id's start at zero )
      is_prime(numUsed,task.start,task.end);
     }
    else if(task.id == n-1 && n!=0){//last thread
       task.start = 5 +mUsed*6*task.id ;
       task.end = sqrt(numUsed);// the end for the last thread is the square of the number
       is_prime(numUsed,task.start,task.end);
      
     }
    
    else{ //all other threads
      task.start = 5 + mUsed*6*task.id;
      task.end = 5 + mUsed*6*(task.id +1);// The tid is +1 b/c the array of tasks starts at zero
      is_prime(numUsed,task.start,task.end);
    }
  }
    barrier.wait();
    if( barrier.wait()) {
        if(value==0)
          result.push_back(numUsed);
     value.store(0);//reset for each num
    }
    
  }
  barrier.wait();
}
#endif
// This function takes a list of numbers in nums[] and returns only numbers that
// are primes.
//
// The parameter n_threads indicates how many threads should be created to speed
// up the computation.
// -----------------------------------------------------------------------------
// You will most likely need to re-implement this function entirely.
// Note that the current implementation ignores n_threads. Your multithreaded
// implementation must use this parameter.
std::vector<int64_t>
detect_primes(const std::vector<int64_t> & nums, int n_threads)
{
  #if 1
  n_thread = n_threads;
  numsUsed = nums;// setting global vector
  simple_barrier barrier(n_threads);
  std::vector<std::thread> threads;
  Task tasks[n_threads];//preparing memory for each thread
  flag.store(false, std::memory_order_release);
  for( int i = 0 ; i < n_threads ; i ++) {
    tasks[i].id = i;//task ids will be (0,...,n_threads-1)
    //printf("maybe");
    threads.emplace_back(runner,std::ref(tasks[i]), std::ref(barrier));
  }
  for( auto & t : threads)// joining the threads
    t.join();
  #endif

  #if 0
  for (auto num : nums) {
    if (is_prime(num)) result.push_back(num);
  }
  #endif
  return result;
}
