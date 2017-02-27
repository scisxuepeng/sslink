#include "ff.h"

int main(){

  ff::initialize(20);

  std::vector<void *> mem;

  for(int i = 0; i < 20; i++){
    mem.push_back(malloc(...)); //init mem
  }

  ff::paragroup pc;
  pc.for_each(0, 50, [&mem](int i){
      int id = ff::rt::get_thrd_id();
      void * m = mem[id];
      //manipulate m here
      });

  ff::ff_wait(all(pc));


  return 0;
}
