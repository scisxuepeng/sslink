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
#include "runtime/rtcmn.h"
#include "runtime/runtime.h"
#include "utilities/mutex.h"
#include <iostream>

namespace ff {
  extern bool g_initialized_flag;
  void initialize(size_t concurrency)
  {
    rt::set_concurrency(concurrency);
    rt::runtime::instance();
  }

  namespace rt {
    std::shared_ptr<runtime_deletor> runtime_deletor::s_pInstance(nullptr);
    runtime_ptr runtime::s_pInstance(nullptr);
    std::once_flag		runtime::s_oOnce;

    void schedule(task_base_ptr p)
    {   static runtime_ptr r = runtime::instance();
      r->schedule(p);
    }
    void yield()
    {
      std::this_thread::yield();
    }


    runtime::runtime()
      : m_pTP(new threadpool())
        , m_oQueues()
        , m_bAllThreadsQuit(false) {
          for(int i = 0; i < concurrency(); ++i){
            m_pred_conflict_times.push_back(0);
            m_may_conflict_times.push_back(0);
            m_reschedule_times.push_back(0);
            m_steal_times.push_back(0);
            m_good_times.push_back(0);
          }
        };

    runtime::~runtime()
    {
      m_bAllThreadsQuit = true;
      m_pTP->join();
#if _DEBUG
#define OUT(s) std::cout<<#s<<std::endl; \
      for(size_t i = 0; i <m_pred_conflict_times.size(); ++i){ \
        std::cout<<"\t "<<s[i]; \
      }\
      std::cout<<std::endl;
#else
#define OUT(s) ;
#endif

      OUT(m_pred_conflict_times);
      OUT(m_may_conflict_times);
      OUT(m_reschedule_times);
      OUT(m_steal_times);
      OUT(m_good_times);
    }

    runtime_ptr 	runtime::instance()
    {
      if(!s_pInstance)
        std::call_once(s_oOnce, runtime::init);
      return s_pInstance;
    }

    void			runtime::init()
    {
      s_pInstance = new runtime();
      runtime_deletor::s_pInstance = std::make_shared<runtime_deletor>(s_pInstance);
      auto thrd_num = concurrency();
      for(int i = 0; i<thrd_num; ++i)
      {
        s_pInstance->m_oQueues.push_back(std::unique_ptr<work_stealing_queue>(new work_stealing_queue()));
        s_pInstance->m_oWQueues.push_back(std::unique_ptr<simo_queue_t>(new simo_queue_t()));
      }

      set_local_thrd_id(0);

      for(int i = 1; i< thrd_num; ++i)
      {
        s_pInstance->m_pTP->run([i]() {
            auto r = runtime::instance();
            set_local_thrd_id(i);
            r->thread_run();
            });
      }
      g_initialized_flag = true;
    }

    void	runtime::schedule(task_base_ptr p)
    {
      thread_local static int i = get_thrd_id();
      if(!m_oQueues[i] ->push_back(p))
      {
        std::cout<<"no this"<<std::endl;
        run_task(p);
      }
    }

    bool runtime::take_one_task(task_base_ptr & pTask){
      bool b = false;
      thread_local static int i = get_thrd_id();
      thread_local static work_stealing_queue * q = m_oQueues[i].get();
      int reschedule_times = 0;
      int steal_counter = 0;
      int reschedule_max = m_oQueues[i]->size();
      const static int steal_max = 4;

      b = q->pop(pTask);
      if(!b){
        b = steal_one_task(pTask);
      }
      if(!b) return b;
#if 1
      ff::mutex * m = (ff::mutex *) pTask->getHoldMutex();
      while(b && m != invalid_mutex_id){
        reschedule_times ++;
        thrd_id_t lock_thrd = m->lock_thrd_id();
        if(lock_thrd == -1){
          break;
        }else if(reschedule_times > reschedule_max){
          break;
        }else{
            if(q->push_front(pTask)){
              b = q->pop(pTask);
            }else{
              b = steal_one_task(pTask);
            }
        }
      }
#endif
#if 0
      ff::mutex * m = (ff::mutex *) pTask->getHoldMutex();
      while( b && m != invalid_mutex_id){
        reschedule_times ++;
        m_reschedule_times[i] ++;
        //m_oHPMutex.get_hazard_pointer().store(pTask->getHoldMutex());
        if( m->is_locked() &&
            reschedule_times < reschedule_max){
          m_pred_conflict_times[i] ++;
          if(m_oQueues[i]->push_front(pTask)){
            if(reschedule_times& (steal_max - 1)== 0){
              b = steal_one_task(pTask);
              m_steal_times[i] ++;
              if(!b){
                b = q->pop(pTask);
              }
            }
            else{
              b = q->pop(pTask);
            }
          }
          else{
              b = steal_one_task(pTask);
          }
        }
        else{
          if(reschedule_times >= reschedule_max){
            m_may_conflict_times[i] ++;
            break; //reschedule enough times
          }
          m_good_times[i] ++;
          break; //can hold the mutex
        }
        m = (ff::mutex *) pTask->getHoldMutex();
      }
#endif
      return b;
    }

#if 0
    bool		runtime::take_one_task(task_base_ptr & pTask)
    {
      bool b = false;
      thread_local static int i = get_thrd_id();
      thread_local static uint64_t ct = 0;
      b = m_oQueues[i]->pop(pTask);
      if(!b)
      {
        ct ++;
        if(ct &1 == 0)
        {
          b = m_oWQueues[i]->pop(pTask);
          if(!b){
            b = steal_one_task(pTask);
          }
        }
        else
        {
          b = steal_one_task(pTask);
          if(!b){
            b = m_oWQueues[i]->pop(pTask);
          }
        }
      }
      return b;
    }
#endif

    void 			runtime::run_task(task_base_ptr & pTask)
    {
      thread_local static int cur_id = get_thrd_id();
      pTask->run();
      while(pTask->need_to_reschedule() && ! m_oWQueues[cur_id]->push(pTask)) pTask->run();
#if 0
      int take_times = 0;
      int steal_times = 0;
START:

      //_DEBUG(LOG_INFO(rt)<<"run_task() id:"<<get_thrd_id()<<" get task... "<<pTask.get();)
      if(pTask->getHoldMutex() != invalid_mutex_id)
      {
        m_oHPMutex.get_hazard_pointer().store(pTask->getHoldMutex());
        if(m_oHPMutex.outstanding_hazard_pointer_for(pTask->getHoldMutex()))
        {
          if(take_times >= m_oQueues[i]->size()+1 && steal_times > 2*rt::rt_concurrency())
          {
            pTask->run();
            m_oHPMutex.get_hazard_pointer().store(invalid_mutex_id);
          }
          else {
            m_oHPMutex.get_hazard_pointer().store(invalid_mutex_id);
            m_oQueues[i]->push_back(pTask);
            if(take_times & 0x1 )
            {
              steal_times ++;
              if(!steal_one_task(pTask))
              {
                take_one_task(pTask);
                take_times ++;
              }
            }
            else
            {
              take_one_task(pTask);
              take_times ++;
            }
            goto START;
          }

        }
        else
        {
          pTask->run();
          m_oHPMutex.get_hazard_pointer().store(invalid_mutex_id);
        }
      }
      else
        pTask->run();

#endif
    }
    void			runtime::thread_run()
    {
      bool flag = false;
      thread_local static int cur_id = get_thrd_id();
      task_base_ptr pTask;
      while(!m_bAllThreadsQuit)
      {
        flag = take_one_task(pTask);
        if(flag){
          run_task(pTask);
        }
        if(!flag)
          yield();
      }
    }

    bool		runtime::steal_one_task(task_base_ptr & pTask)
    {
      thread_local static int cur_id = get_thrd_id();
      size_t dis = 1;
      size_t ts = m_oQueues.size();
      while((cur_id + dis)%ts !=cur_id)
      {
        if(m_oQueues[(cur_id + dis)%ts]->steal(pTask))
        {
          return true;
        }
        else if (m_oWQueues[(cur_id + dis)%ts]->pop(pTask))
        {
          return true;
        }
        dis ++;
      }
      return false;
    }
  }//end namespace rt
}//end namespace ff
