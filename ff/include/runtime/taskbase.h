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
#ifndef FF_RUNTIME_TASK_BASE_H_
#define FF_RUNTIME_TASK_BASE_H_

#include "common/common.h"
namespace ff {
namespace rt {
class task_base {
public:
    virtual ~task_base(){}
    task_base():m_b_reschedule_flag(false), m_hold_mutex(invalid_mutex_id){}

    virtual void	run() = 0;
    bool & need_to_reschedule() {return m_b_reschedule_flag;}
    inline mutex_id_t 		getHoldMutex() const {return m_hold_mutex;}
    inline void			setHoldMutex(mutex_id_t id){m_hold_mutex = id;}
protected:
    bool m_b_reschedule_flag;
    mutex_id_t	m_hold_mutex;
};//end class task_base;
typedef std::shared_ptr<task_base>   task_base_ptr;

}//end namespace rt
}//end namespace ff

#endif
