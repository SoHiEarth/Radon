#pragma once
#include <engine/interface.h>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <cstdint>

class IJobSystem;
enum class Priority : std::uint8_t {
  kLow = 0,
  kMedium = 1,
  kHigh = 2,
  kCritical = 3,
};

class Job {
private:
  std::function<void()> function_;
  Priority priority_;
  Job* parent_ = nullptr;
  std::atomic<std::int32_t> unfinished_jobs_{};
  std::vector<Job*> dependents_;

public:
  Job(std::function<void()> func, Priority priority)
      : function_(func), priority_(priority) {}
  
  void Execute() {
    if (function_) {
      function_();
    }
    Finish();
  }

  void Finish() {
    std::int32_t unfinished = unfinished_jobs_.fetch_sub(1) - 1;
    if (unfinished == 0 && parent_) {
      parent_->Finish();
    }
  }

  void AddDependent(Job* dependent) {
    dependents_.push_back(dependent);
    dependent->unfinished_jobs_.fetch_add(1);
  }

  const std::vector<Job*>& GetDependents() const {
    return dependents_;
  }

  bool IsComplete() const {
    return unfinished_jobs_.load() == 0;
  }

  Priority GetPriority() const {
    return priority_;
  }

  void SetParent(Job* parent) {
    parent_ = parent;
    if (parent) {
      parent->unfinished_jobs_.fetch_add(1);
    }
  }
};

class JobHandle {
private:
  Job* job_;

public:
  JobHandle() : job_(nullptr) {}
  explicit JobHandle(Job* job) : job_(job) {}

  bool IsComplete() const {
    return !job_ || job_->IsComplete();
  }
  
  Job* GetJob() const {
    return job_;
  }
};

class WorkerThread {
private:
  void ThreadLoop();
  std::uint32_t id_;
  IJobSystem* job_system_ = nullptr;
  std::thread thread_;
  std::atomic<bool> running_{};

public:
  WorkerThread(std::uint32_t id, class IJobSystem* job_system);
  ~WorkerThread();
  void Start();
  void Stop();
  bool IsRunning() const {
    return running_.load();
  }
  std::uint32_t GetID() const {
    return id_;
  }
};

class IJobSystem : public Interface {
private:
  struct JobComparator {
    bool operator()(Job* a, Job* b) const {
      return static_cast<std::uint8_t>(a->GetPriority()) <
             static_cast<std::uint8_t>(b->GetPriority());
    }
  };

  std::priority_queue<Job*, std::vector<Job*>, JobComparator> job_queue_;
  std::mutex queue_mutex_;
  std::condition_variable queue_cv_;
  std::vector<std::unique_ptr<Job>> job_pool_;
  std::mutex pool_mutex_;
  std::vector<std::unique_ptr<WorkerThread>> workers_;
  std::uint32_t worker_count_;
  std::atomic<bool> shutdown_;
  std::vector<std::thread::id> worker_thread_ids_;
  std::mutex thread_id_mutex_;
  Job* AllocateJob(const std::function<void()>& function, Priority priority);
  void FreeJob(Job* job);

protected:
  void IInit() override;
  void IUpdate() override;
  void IQuit() override;

public:
  const char* Name() override {
    return "JobSystem";
  }
  void RegisterWorkerThreadId(std::thread::id id);
  JobHandle Schedule(const std::function<void()>& function, Priority priority);
  JobHandle ScheduleDependent(const JobHandle& parent,
    const std::function<void()>& function,
    Priority priority);
  void Wait(const JobHandle& handle);
  void WaitForAll();
  void HelpUntil(const JobHandle& handle);
  Job* internal_GetNextJob();
  void ProcessDependents(Job* job);
  std::uint32_t GetWorkerCount() const {
    return worker_count_;
  }
  std::size_t GetPendingJobCount();
  bool IsWorkerThread();
};

// Updated macros to use engine context pattern
#define PARALLEL_FOR(engine, start, end, body)                                 \
  {                                                                            \
    auto& job_system = (engine)->GetJobSystem();                               \
    const std::size_t total = (end) - (start);                                 \
    const std::size_t worker_count = job_system.GetWorkerCount();              \
    const std::size_t chunk_size = (total + worker_count - 1) / worker_count;  \
    std::vector<JobHandle> handles;                                            \
    for (std::size_t i = 0; i < worker_count; ++i) {                           \
      std::size_t chunk_start = (start) + i * chunk_size;                      \
      std::size_t chunk_end = std::min((start) + (i + 1) * chunk_size, (end)); \
      if (chunk_start >= chunk_end)                                            \
        break;                                                                 \
      handles.push_back(job_system.Schedule([=]() {                            \
        for (std::size_t index = chunk_start; index < chunk_end; ++index) {    \
          body;                                                                \
        }                                                                      \
      }, Priority::kMedium));                                                  \
    }                                                                          \
    for (auto& handle : handles) {                                             \
      job_system.Wait(handle);                                                 \
    }                                                                          \
  }

#define SCHEDULE_JOB(engine, func, priority) \
  (engine)->GetJobSystem().Schedule(func, priority)

#define SCHEDULE_JOB_DEPENDENT(engine, parent, func, priority) \
  (engine)->GetJobSystem().ScheduleDependent(parent, func, priority)

#define WAIT_FOR_JOB(engine, handle) \
  (engine)->GetJobSystem().Wait(handle)

#define WAIT_FOR_ALL_JOBS(engine) \
  (engine)->GetJobSystem().WaitForAll()