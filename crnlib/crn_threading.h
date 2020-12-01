// File: crn_threading.h
// See Copyright Notice and license at the end of inc/crnlib.h

#if 1

#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include <cstdlib>

#include "crn_atomics.h"

namespace crnlib
{
   // g_number_of_processors defaults to 1. Will be higher on multicore machines.
   extern uint g_number_of_processors;

   void crn_threading_init();

   typedef std::thread::id crn_thread_id_t;

   inline crn_thread_id_t crn_get_current_thread_id() {
      return std::this_thread::get_id();
   }

   inline void crn_sleep(uint milliseconds) {
      std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
   }

   class mutex
   {
      mutex(const mutex&) = delete;
      mutex& operator= (const mutex&) = delete;

   public:
      inline mutex() {}
      inline ~mutex() {}
      inline void lock() { m_mutex.lock(); }
      inline void unlock() { m_mutex.unlock(); };

   private:
      std::mutex m_mutex;
   };

   class scoped_mutex
   {
      scoped_mutex(const scoped_mutex&) = delete;
      scoped_mutex& operator= (const scoped_mutex&) = delete;

   public:
      inline scoped_mutex(mutex& m) : m_mutex(m) { m_mutex.lock(); }
      inline ~scoped_mutex() { m_mutex.unlock(); }

   private:
      mutex& m_mutex;
   };

   class semaphore
   {
      semaphore(const semaphore&) = delete;
      semaphore& operator= (const semaphore&) = delete;

   public:
      semaphore(long initialCount = 0);
      ~semaphore();

      void release(long releaseCount = 1);
      void try_release(long releaseCount = 1);
      bool wait(uint32 milliseconds = cUINT32_MAX);

   private:
      std::mutex m_mutex;
      std::condition_variable m_condition;
      long m_count;
   };

   class spinlock
   {
      spinlock(const spinlock&) = delete;
      spinlock& operator= (const spinlock&) = delete;

   public:
      spinlock() {}
      ~spinlock() {}

      void lock() {
         while (m_lock.test_and_set(std::memory_order_acquire));
      }
      void unlock() {
         m_lock.clear(std::memory_order_release);
      }

   private:
      std::atomic_flag m_lock = ATOMIC_FLAG_INIT;
   };

   class scoped_spinlock
   {
      scoped_spinlock(const scoped_spinlock&) = delete;
      scoped_spinlock& operator= (const scoped_spinlock&) = delete;

   public:
      scoped_spinlock(spinlock& lock) : m_lock(lock) { m_lock.lock(); }
      ~scoped_spinlock() { m_lock.unlock(); }

   private:
      spinlock& m_lock;
   };

   template<typename T, uint cMaxSize>
   class tsstack
   {
   public:
      inline tsstack() :
         m_top(0)
      {
      }

      inline ~tsstack()
      {
      }

      inline void clear()
      {
         m_spinlock.lock();
         m_top = 0;
         m_spinlock.unlock();
      }

      inline bool try_push(const T& obj)
      {
         bool result = false;
         m_spinlock.lock();
         if (m_top < (int)cMaxSize)
         {
            m_stack[m_top++] = obj;
            result = true;
         }
         m_spinlock.unlock();
         return result;
      }

      inline bool pop(T& obj)
      {
         bool result = false;
         m_spinlock.lock();
         if (m_top > 0)
         {
            obj = m_stack[--m_top];
            result = true;
         }
         m_spinlock.unlock();
         return result;
      }

   private:
      spinlock m_spinlock;
      T m_stack[cMaxSize];
      int m_top;
   };

   class task_pool
   {
   public:
      task_pool(uint num_threads = 0);
      ~task_pool();

      static const uint cMaxThreads = 16;

      bool init(uint num_threads);
      void deinit();

      inline uint get_num_threads() const { return m_threads.size(); }
      inline uint32 get_num_outstanding_tasks() const { return m_total_submitted_tasks - m_total_completed_tasks; }

      // C-style task callback
      typedef void (*task_callback_func)(uint64 data, void* pData_ptr);
      bool queue_task(task_callback_func pFunc, uint64 data = 0, void* pData_ptr = nullptr);

      class executable_task
      {
      public:
         virtual void execute_task(uint64 data, void* pData_ptr) = 0;
      };

      // It's the caller's responsibility to delete pObj within the execute_task() method, if needed!
      bool queue_task(executable_task* pObj, uint64 data = 0, void* pData_ptr = nullptr);

      template<typename S, typename T>
      inline bool queue_object_task(S* pObject, T pObject_method, uint64 data = 0, void* pData_ptr = nullptr);

      template<typename S, typename T>
      inline bool queue_multiple_object_tasks(S* pObject, T pObject_method, uint64 first_data, uint num_tasks, void* pData_ptr = nullptr);

      void join();

   private:
      struct task
      {
         inline task() : m_data(0), m_pData_ptr(nullptr), m_pObj(nullptr), m_flags(0) { }

         uint64 m_data;
         void* m_pData_ptr;

         union
         {
            task_callback_func m_callback;
            executable_task* m_pObj;
         };

         uint m_flags;
      };

      tsstack<task, cMaxThreads> m_task_stack;

      std::vector<std::thread> m_threads;

      // Signalled whenever a task is queued up.
      semaphore m_tasks_available;

      // Signalled when all outstanding tasks are completed.
      semaphore m_all_tasks_completed;

      enum task_flags
      {
         cTaskFlagObject = 1
      };

      volatile std::atomic_uint32_t m_total_submitted_tasks;
      volatile std::atomic_uint32_t m_total_completed_tasks;
      volatile std::atomic_bool m_exit_flag;

      void process_task(task& tsk);

      static void thread_func(task_pool *pPool);
   };

   enum object_task_flags
   {
      cObjectTaskFlagDefault = 0,
      cObjectTaskFlagDeleteAfterExecution = 1
   };

   template<typename T>
   class object_task : public task_pool::executable_task
   {
   public:
      object_task(uint flags = cObjectTaskFlagDefault) :
         m_pObject(nullptr),
         m_pMethod(nullptr),
         m_flags(flags)
      {
      }

      typedef void (T::*object_method_ptr)(uint64 data, void* pData_ptr);

      object_task(T* pObject, object_method_ptr pMethod, uint flags = cObjectTaskFlagDefault) :
         m_pObject(pObject),
         m_pMethod(pMethod),
         m_flags(flags)
      {
         CRNLIB_ASSERT(pObject && pMethod);
      }

      virtual ~object_task() {}

      void init(T* pObject, object_method_ptr pMethod, uint flags = cObjectTaskFlagDefault)
      {
         CRNLIB_ASSERT(pObject && pMethod);

         m_pObject = pObject;
         m_pMethod = pMethod;
         m_flags = flags;
      }

      T* get_object() const { return m_pObject; }
      object_method_ptr get_method() const { return m_pMethod; }

      virtual void execute_task(uint64 data, void* pData_ptr)
      {
         (m_pObject->*m_pMethod)(data, pData_ptr);

         if (m_flags & cObjectTaskFlagDeleteAfterExecution)
            crnlib_delete(this);
      }

   protected:
      T* m_pObject;

      object_method_ptr m_pMethod;

      uint m_flags;
   };

   template<typename S, typename T>
   inline bool task_pool::queue_object_task(S* pObject, T pObject_method, uint64 data, void* pData_ptr)
   {
      object_task<S> *pTask = crnlib_new< object_task<S> >(pObject, pObject_method, cObjectTaskFlagDeleteAfterExecution);
      if (!pTask)
         return false;
      return queue_task(pTask, data, pData_ptr);
   }

   template<typename S, typename T>
   inline bool task_pool::queue_multiple_object_tasks(S* pObject, T pObject_method, uint64 first_data, uint num_tasks, void* pData_ptr)
   {
      CRNLIB_ASSERT(pObject);
      CRNLIB_ASSERT(num_tasks);
      if (!num_tasks)
         return true;

      bool status = true;

      uint i;
      for (i = 0; i < num_tasks; i++)
      {
         task tsk;

         tsk.m_pObj = crnlib_new< object_task<S> >(pObject, pObject_method, cObjectTaskFlagDeleteAfterExecution);
         if (!tsk.m_pObj)
         {
            status = false;
            break;
         }

         tsk.m_data = first_data + i;
         tsk.m_pData_ptr = pData_ptr;
         tsk.m_flags = cTaskFlagObject;

         m_total_submitted_tasks.fetch_add(1);

         if (!m_task_stack.try_push(tsk))
         {
            m_total_completed_tasks.fetch_add(1);

            status = false;
            break;
         }
      }

      if (i)
      {
         m_tasks_available.release(i);
      }

      return status;
   }

} // namespace crnlib

#else

#if CRNLIB_USE_WIN32_API
   #include "crn_threading_win32.h"
#elif CRNLIB_USE_PTHREADS_API
   #include "crn_threading_pthreads.h"
#else
   #include "crn_threading_null.h"
#endif

#endif
