#include <engine/debug.h>
#include <engine/job_system.h>
#include <algorithm>

WorkerThread::WorkerThread(std::uint32_t thread_id, IJobSystem* job_system)
    : id_(thread_id), job_system_(job_system), running_(false) {}

WorkerThread::~WorkerThread() {
  Stop();
}

void WorkerThread::Start() {
  running_.store(true);
  thread_ = std::thread(&WorkerThread::ThreadLoop, this);
}

void WorkerThread::Stop() {
  if (running_.load()) {
    running_.store(false);
    if (thread_.joinable()) {
      thread_.join();
    }
  }
}

void WorkerThread::ThreadLoop() {
  job_system_->RegisterWorkerThreadId(std::this_thread::get_id());
  fmt::print("Worker thread {} started\n", id_);
  while (running_.load()) {
    Job* job = job_system_->internal_GetNextJob();
    if (job) {
      job->Execute();
      job_system_->ProcessDependents(job);
    } else {
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
  }
  fmt::print("Worker thread {} stopped", id_);
}

void IJobSystem::IInit() {
  auto& debug = engine_->GetDebug();
  debug.Log("Initializing Job System");
  shutdown_.store(false);
  worker_count_ = std::max(1u, std::thread::hardware_concurrency() - 1);
  debug.Log(fmt::format("Creating {} worker threads", worker_count_));
  workers_.reserve(worker_count_);
  for (std::uint32_t i = 0; i < worker_count_; ++i) {
    auto worker = std::make_unique<WorkerThread>(i, this);
    worker->Start();
    workers_.push_back(std::move(worker));
  }
  {
    std::lock_guard<std::mutex> lock(thread_id_mutex_);
    for (const auto& worker : workers_) {
    }
  }

  state_ = InterfaceState::kInitialized;
}

void IJobSystem::IUpdate() {}

void IJobSystem::IQuit() {
  auto& debug = engine_->GetDebug();
  debug.Log("Shutting down Job System");
  shutdown_.store(true);
  queue_cv_.notify_all();
  for (auto& worker : workers_) {
    worker->Stop();
  }
  workers_.clear();
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    while (!job_queue_.empty()) {
      job_queue_.pop();
    }
  }
  {
    std::lock_guard<std::mutex> lock(pool_mutex_);
    job_pool_.clear();
  }
  state_ = InterfaceState::kStopped;
}

void IJobSystem::RegisterWorkerThreadId(std::thread::id id) {
  std::lock_guard<std::mutex> lock(thread_id_mutex_);
  worker_thread_ids_.push_back(id);
}

JobHandle IJobSystem::Schedule(const std::function<void()>& function, Priority priority) {
  Job* job = AllocateJob(function, priority);
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    job_queue_.push(job);
  }
  queue_cv_.notify_one();
  return JobHandle(job);
}

JobHandle IJobSystem::ScheduleDependent(const JobHandle& parent,
                                        const std::function<void()>& function,
                                        Priority priority) {
  Job* job = AllocateJob(function, priority);
  if (parent.GetJob()) {
    parent.GetJob()->AddDependent(job);
    if (parent.IsComplete()) {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      job_queue_.push(job);
      queue_cv_.notify_one();
    }
  } else {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    job_queue_.push(job);
    queue_cv_.notify_one();
  }
  return JobHandle(job);
}

void IJobSystem::Wait(const JobHandle& handle) {
  if (!handle.GetJob() || handle.IsComplete()) {
    return;
  }
  HelpUntil(handle);
  while (!handle.IsComplete()) {
    std::this_thread::yield();
  }
}

void IJobSystem::WaitForAll() {
  while (GetPendingJobCount() > 0) {
    Job* job = internal_GetNextJob();
    if (job) {
      job->Execute();
      ProcessDependents(job);
    } else {
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
  }
}

void IJobSystem::HelpUntil(const JobHandle& handle) {
  while (!handle.IsComplete()) {
    Job* job = internal_GetNextJob();
    if (job) {
      job->Execute();
      ProcessDependents(job);
    } else {
      std::this_thread::yield();
    }
  }
}

Job* IJobSystem::internal_GetNextJob() {
  std::unique_lock<std::mutex> lock(queue_mutex_);
  if (job_queue_.empty()) {
    return nullptr;
  }
  Job* job = job_queue_.top();
  job_queue_.pop();
  return job;
}

void IJobSystem::ProcessDependents(Job* job) {
  if (!job)
    return;
  const auto& dependents = job->GetDependents();
  if (!dependents.empty()) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    for (Job* dependent : dependents) {
      if (dependent->IsComplete()) {
        job_queue_.push(dependent);
        queue_cv_.notify_one();
      }
    }
  }
  FreeJob(job);
}

std::size_t IJobSystem::GetPendingJobCount() {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  return job_queue_.size();
}

bool IJobSystem::IsWorkerThread() {
  std::lock_guard<std::mutex> lock(thread_id_mutex_);
  auto current_id = std::this_thread::get_id();

  return std::find(worker_thread_ids_.begin(), worker_thread_ids_.end(), current_id) !=
         worker_thread_ids_.end();
}

Job* IJobSystem::AllocateJob(const std::function <void()>& function, Priority priority) {
  std::lock_guard<std::mutex> lock(pool_mutex_);

  // Try to reuse from pool
  if (!job_pool_.empty()) {
    auto job = std::move(job_pool_.back());
    job_pool_.pop_back();

    // Reset job state
    Job* raw_job = job.release();
    new (raw_job) Job(function, priority);
    return raw_job;
  }

  // Allocate new job
  auto job = std::make_unique<Job>(function, priority);
  return job.release();
}

void IJobSystem::FreeJob(Job* job) {
  if (!job)
    return;

  std::lock_guard<std::mutex> lock(pool_mutex_);

  // Return to pool for reuse
  job_pool_.push_back(std::unique_ptr<Job>(job));
}
