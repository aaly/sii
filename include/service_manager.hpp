#ifndef SERVICE_MANAGER_HPP
#define SERVICE_MANAGER_HPP
#include <string>
#include "common.hpp"
#include "service.hpp"
#include <map>
#include "json.hpp"

extern "C"{
#include "zenoh-pico.h"
}

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <algorithm>
#include <fstream>

namespace SII {

    // add nlohmann::json as json for protability...
    using json = nlohmann::json;


     // divide used jiffies by total https://rosettacode.org/wiki/Linux_CPU_utilization#C
    std::vector<size_t> get_cpu_times();
    bool get_cpu_times(size_t &idle_time, size_t &total_time);



    // beautiofully done at https://ncona.com/2019/05/using-thread-pools-in-cpp/
    // This class manages a thread pool that will process requests
    class ThreadPool {
     public:
      ThreadPool() : done(false) {
        // This returns the number of threads supported by the system. If the
        // function can't figure out this information, it returns 0. 0 is not good,
        // so we create at least 1
        auto numberOfThreads = std::thread::hardware_concurrency();
        if (numberOfThreads == 0) {
          numberOfThreads = 1;
        }

        std::cerr << "threads : " << numberOfThreads << std::endl;

        for (unsigned i = 0; i < numberOfThreads; ++i) {
          // The threads will execute the private member `doWork`. Note that we need
          // to pass a reference to the function (namespaced with the class name) as
          // the first argument, and the current object as second argument
          threads.push_back(std::thread(&ThreadPool::doWork, this));
        }
      }

      // The destructor joins all the threads so the program can exit gracefully.
      // This will be executed if there is any exception (e.g. creating the threads)
      ~ThreadPool() {
        // So threads know it's time to shut down
        done = true;

        // Wake up all the threads, so they can finish and be joined
        workQueueConditionVariable.notify_all();
        for (auto& thread : threads) {
          if (thread.joinable()) {
            thread.join();
          }
        }
      }

      // This function will be called by the server every time there is a request
      // that needs to be processed by the thread pool
      void queueWork(std::function<void(const json message_json)> event_handler, const json& event) {
        // Grab the mutex
        std::lock_guard<std::mutex> g(workQueueMutex);

        // Push the request to the queue
        workQueue.push(std::pair<std::function<void(const json message_json)>, const json>(event_handler, event));

        // Notify one thread that there are requests to process
        workQueueConditionVariable.notify_one();
      }

     private:
      // This condition variable is used for the threads to wait until there is work
      // to do
      std::condition_variable_any workQueueConditionVariable;

      // We store the threads in a vector, so we can later stop them gracefully
      std::vector<std::thread> threads;

      // Mutex to protect workQueue
      std::mutex workQueueMutex;

      // Queue of requests waiting to be processed
      std::queue<std::pair<std::function<void(const json message_json)>, const json>> workQueue;

      // This will be set to true when the thread pool is shutting down. This tells
      // the threads to stop looping and finish
      bool done;

      // Function used by the threads to grab work from the queue
      void doWork() {
        // Loop while the queue is not destructing
        while (!done) {
          std::function<void(const json message_json)> service_callback;
//          queue_element([](const json msg) {;}, json());
          json service_json;
          // Create a scope, so we don't lock the queue for longer than necessary
          {
            std::unique_lock<std::mutex> g(workQueueMutex);
            workQueueConditionVariable.wait(g, [&]{
              // Only wake up if there are elements in the queue or the program is
              // shutting down
              return !workQueue.empty() || done;
            });

            // If we are shutting down exit witout trying to process more work
            if (done) {
              break;
            }

            auto pop_element =  workQueue.front();
            workQueue.pop();
            service_callback = pop_element.first;
            service_json = pop_element.second;
          }

          struct timespec old_ts = {};
          clock_gettime(CLOCK_THREAD_CPUTIME_ID, &old_ts);
          size_t old_idle_time = 0;
          size_t old_total_time = 0;
          get_cpu_times(old_idle_time, old_total_time);
          service_callback(service_json);
          struct timespec current_ts = {};
          clock_gettime(CLOCK_THREAD_CPUTIME_ID, &current_ts);
          size_t current_idle_time = 0;
          size_t current_total_time = 0;
          get_cpu_times(current_idle_time, current_total_time);
          auto tv_nsec = current_ts.tv_nsec - old_ts.tv_nsec;
          int nb = sysconf(_SC_NPROCESSORS_ONLN);
          long long tv_nsec_hertz = (1/(float)(tv_nsec/(float)1000000000));
          size_t cpu_useage = (nb * 100.0 * tv_nsec_hertz) / float(current_total_time - old_total_time);

          printf("Raw nanoseconds: %09ld\n", (intmax_t)tv_nsec);
          printf("Raw nanoseconds to hertz: %09ld\n", (intmax_t)tv_nsec_hertz);
          printf("CPU Utilization: %jd\n", (intmax_t)cpu_useage);


        }
      }

    };

    class serviceManager{
        public:
            serviceManager(const std::string& config_path);
           ~serviceManager();

           int Run(const std::string& router_address = "",  const std::string& topic_path = "/sii/services");
        private:
           static void dataHandler(const zn_sample_t *sample, const void *arg);
           void Process(const nlohmann::json& message_json);
           std::map<std::string, SII::Service> m_services;
           std::map<std::string, std::function<void(const json message_json)>> m_eventHandlers;
           ThreadPool m_threadPool;
    };
} //namespace SII
#endif //SERVICE_MANAGER_HPP
