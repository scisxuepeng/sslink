
#include "ff.h"
#include "common/log.h"

int main(int argc, char *argv[])
{
    std::vector<int> s;
    int sum = 0;
    ff::paragroup pg1;
    pg1.for_each(s.begin(), s.end(), [&sum](int x){
        sum += x;
    });
    ff::ff_wait(all(pg1));
    return 0;
}

