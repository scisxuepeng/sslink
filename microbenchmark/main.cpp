#include "ff.h"
#include <cmath>
#include <chrono>
#include <iostream>
#include <vector>

typedef ff::mutex TMutex;
typedef std::shared_ptr<TMutex> TMutex_ptr;
std::vector<TMutex_ptr> ms;


unsigned int fib( int n ) {
  switch(n){
    case 0:
    case 1:
      return 1;
    case 2:
      return 2;
    case 3:
      return 3;
    case 4:
      return 5;
    default:
      return fib(n - 1) + fib(n - 2);
  }
}
const int64_t total = 100000;//1000000;
int64_t total_per_thread;

int random_fib()
{
  int i = rand()%5 + 25;
  return fib(i);
}

void task_fun(int j) {
  int t = j;
  //t = ff::rt::get_thrd_id();
  //t = 0;
  ms[t]->lock();
  random_fib();
  ms[t]->unlock();
  //std::this_thread::yield();
}

std::atomic_int all_reach(0);

std::chrono::time_point<std::chrono::system_clock> start, end;

void dispatch() {
  ff::paracontainer p;
  int concurrency = ff::rt::concurrency();
  total_per_thread = total/concurrency;
  ff::thrd_id_t id = ff::rt::get_thrd_id();

  all_reach ++;
  while(all_reach != 4) std::this_thread::yield();
  if(id == 0){
    start = std::chrono::system_clock::now();
  }

  int64_t per_mutex_num = total_per_thread/concurrency;

  for(int64_t i = 0; i < total_per_thread; ++i){
    int mutex_index = (i/per_mutex_num)%concurrency;
    ff::para<> ptf;
#ifdef SSLINK
    ptf([mutex_index](){task_fun(mutex_index);}, ms[mutex_index]->id());
#else
    ptf([mutex_index](){task_fun(mutex_index);});
#endif
    p.add(ptf);
  }
  ff::ff_wait(ff::all(p));
}

int main(int argc, char *argv[])
{
  ff::rt::set_concurrency(4);//Set concurrency

  double elapsed_seconds;
  int concurrency = ff::rt::concurrency();
  //ff initialize runtime system
  ff::para<> a;
  a([]() {});
  ff::ff_wait(a);

  for(int i = 0; i< ff::rt::concurrency(); i++)
  {
    ms.push_back(std::make_shared<TMutex>());
  }

  ff::paracontainer p;
  for(int i = 0; i< ff::rt::concurrency(); i++)
  {
    ff::para<> ptf;
    ptf([](){dispatch();});
    p.add(ptf);
  }
  ff::ff_wait(ff::all(p));

  end = std::chrono::system_clock::now();
  elapsed_seconds = 1.0 * std::chrono::duration_cast<std::chrono::microseconds>
    (end-start).count()/1000000;
#ifdef SSLINK
  std::cout << "*SSLINK version,  ";
#endif
  std::cout << "elapsed time: " << elapsed_seconds << "s" << std::endl;
  return 0;
}
