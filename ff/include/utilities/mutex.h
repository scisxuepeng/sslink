#pragma once

#include "common/common.h"
#include "runtime/runtime.h"
namespace ff{
  class mutex{
    public:
      mutex(): m_mutex(){}

      mutex (const mutex & )=delete;
      mutex & operator = (const mutex & ) = delete;

      inline void lock(){m_locked = true; m_mutex.lock(); m_lock_thrd_id = rt::get_thrd_id(); }
      inline void unlock(){
        ::ff::rt::runtime::instance()->clear_mutex_id();
        m_lock_thrd_id = -1;
        m_locked = false;
        m_mutex.unlock(); }

      inline mutex_id_t id(){return this;}
      inline bool is_locked(){ return m_locked; }
      inline int lock_thrd_id(){return m_lock_thrd_id;}


    protected:
      std::mutex m_mutex;
      std::atomic_bool m_locked;
      std::atomic_int m_lock_thrd_id;
  };
}
