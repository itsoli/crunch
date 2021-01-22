#include "crn_core.h"

#include <algorithm>

#include "crn_threading.h"

namespace crnlib {
uint g_number_of_processors = 1;

void crn_threading_init() {
  g_number_of_processors = std::thread::hardware_concurrency();
}

semaphore::semaphore(long initialCount)
    : m_count(initialCount){};

semaphore::~semaphore() {}

void semaphore::release(long releaseCount) {
  CRNLIB_ASSERT(releaseCount >= 1);

  while (releaseCount > 0) {
    std::lock_guard<decltype(m_mutex)> lock(m_mutex);
    ++m_count;
    m_condition.notify_one();
    --releaseCount;
  }
}

void semaphore::try_release(long releaseCount) {
  release(releaseCount);
}

bool semaphore::wait(uint32 milliseconds) {
  std::unique_lock<decltype(m_mutex)> lock(m_mutex);
  if (milliseconds == cUINT32_MAX) {
    while (m_count <= 0) {
      m_condition.wait(lock);
    }
    --m_count;
    return true;
  } else if (
      m_count > 0 ||
      m_condition.wait_for(lock, std::chrono::milliseconds(milliseconds)) != std::cv_status::timeout) {
    --m_count;
    return true;
  }
  return false;
}

task_pool::task_pool(uint num_threads)
    : m_tasks_available(0),
      m_total_submitted_tasks(0),
      m_total_completed_tasks(0),
      m_exit_flag(false) {
  if (num_threads > 0) {
    bool status = init(num_threads);
    CRNLIB_VERIFY(status);
  }
}

task_pool::~task_pool() {
  deinit();
}

bool task_pool::init(uint num_threads) {
  CRNLIB_ASSERT(num_threads <= cMaxThreads);
  num_threads = std::min(num_threads, cMaxThreads);

  deinit();

  for (auto i = 0; i < num_threads; ++i) {
    m_threads.push_back(std::thread(thread_func, this));
  }

  return true;
}

void task_pool::deinit() {
  if (!m_threads.empty()) {
    join();

    m_exit_flag = true;

    m_tasks_available.release(m_threads.size());

    for (auto& thread : m_threads) {
      thread.join();
    }

    m_threads.clear();

    m_exit_flag = false;
  }

  m_task_stack.clear();
  m_total_submitted_tasks = 0;
  m_total_completed_tasks = 0;
}

bool task_pool::queue_task(task_callback_func pFunc, uint64 data, void* pData_ptr) {
  CRNLIB_ASSERT(pFunc);

  task tsk;
  tsk.m_callback = pFunc;
  tsk.m_data = data;
  tsk.m_pData_ptr = pData_ptr;
  tsk.m_flags = 0;

  m_total_submitted_tasks.fetch_add(1);
  if (!m_task_stack.try_push(tsk)) {
    m_total_completed_tasks.fetch_add(1);
    return false;
  }

  m_tasks_available.release(1);

  return true;
}

// It's the object's responsibility to delete pObj within the execute_task() method, if needed!
bool task_pool::queue_task(executable_task* pObj, uint64 data, void* pData_ptr) {
  CRNLIB_ASSERT(pObj);

  task tsk;
  tsk.m_pObj = pObj;
  tsk.m_data = data;
  tsk.m_pData_ptr = pData_ptr;
  tsk.m_flags = cTaskFlagObject;

  m_total_submitted_tasks.fetch_add(1);
  if (!m_task_stack.try_push(tsk)) {
    m_total_completed_tasks.fetch_add(1);
    return false;
  }

  m_tasks_available.release(1);

  return true;
}

void task_pool::process_task(task& tsk) {
  if (tsk.m_flags & cTaskFlagObject) {
    tsk.m_pObj->execute_task(tsk.m_data, tsk.m_pData_ptr);
  } else {
    tsk.m_callback(tsk.m_data, tsk.m_pData_ptr);
  }

  if (++m_total_completed_tasks == m_total_submitted_tasks) {
    // Try to signal the semaphore (the max count is 1 so this may actually fail).
    m_all_tasks_completed.release();
  }
}

void task_pool::join() {
  // Try to steal any outstanding tasks. This could cause one or more worker threads to wake up and immediately go back to sleep, which is wasteful but should be harmless.
  task tsk;
  while (m_task_stack.pop(tsk)) {
    process_task(tsk);
  }

  // At this point the task stack is empty.
  // Now wait for all concurrent tasks to complete. The m_all_tasks_completed semaphore has a max count of 1, so it's possible it could have saturated to 1 as the tasks
  // where issued and asynchronously completed, so this loop may iterate a few times.
  const uint32 total_submitted_tasks = m_total_submitted_tasks;
  while (m_total_completed_tasks != total_submitted_tasks) {
    // If the previous (m_total_completed_tasks != total_submitted_tasks) check failed the semaphore MUST be eventually signalled once the last task completes.
    // So I think this can actually be an INFINITE delay, but it shouldn't really matter if it's 1ms.
    m_all_tasks_completed.wait(1);
  }
}

void task_pool::thread_func(task_pool* pPool) {
  task tsk;

  for (;;) {
    if (!pPool->m_tasks_available.wait()) {
      break;
    }
    if (pPool->m_exit_flag) {
      break;
    }
    if (pPool->m_task_stack.pop(tsk)) {
      pPool->process_task(tsk);
    }
  }
}

}  // namespace crnlib
