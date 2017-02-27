/***********************************************
  The MIT License (MIT)

  Copyright (c) 2012 Athrun Arthur <athrunarthur@gmail.com>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
 *************************************************/
#ifndef FF_COMMON_SPIN_LOCK_H_
#define FF_COMMON_SPIN_LOCK_H_
#include "common/common.h"
#include "runtime/rtcmn.h"
namespace ff{
  class spinlock
  {
    std::atomic_flag locked = ATOMIC_FLAG_INIT ;
    public:
    void lock() {
      while (locked.test_and_set(std::memory_order_acquire)) { ; }
    }
    void unlock() {
      locked.clear(std::memory_order_release);
    }
    spinlock(){}

    spinlock(const spinlock & ) = delete;
    spinlock & operator=(const spinlock &) = delete;
  };//end class spin_lock
}//end namespace ff

#endif
