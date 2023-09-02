#pragma once

#include <chrono>
#include <iostream>

class Timer
{
    std::chrono::high_resolution_clock::time_point start;

public:
    Timer() : start(std::chrono::high_resolution_clock::now())
    {
        printf("Starting Timer!\n");
    }

    ~Timer()
    {
        const auto end_time = std::chrono::high_resolution_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start);
        const double elapsed_time_ms = static_cast<double>(elapsed.count()) / 1000.0;
        std::cout << "Time taken for operation to execute" << ": " << elapsed_time_ms << " ms" << std::endl;
    }

    void Start()
    {
        printf("Starting Timer!\n");
        start = std::chrono::high_resolution_clock::now();
    }

    void Stop(const std::string& str = "operation to execute") const
    {
        const auto end_time = std::chrono::high_resolution_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start);
        std::cout << "Time taken for " << str << ": " << elapsed.count() << " ms" << std::endl;
    }
};

//#include <chrono>
//#include <atomic>
//
//template <typename Clock = std::chrono::high_resolution_clock>
//class stopwatch
//{
//	const typename Clock::time_point start_point;
//public:
//	stopwatch() :
//		start_point(Clock::now())
//	{}
//
//	template <typename Units = typename Clock::duration>
//	void start() const
//	{
//		start_point = Clock::now();
//	}
//
//	template <typename Rep = typename Clock::duration::rep, typename Units = typename Clock::duration>
//	Rep elapsed_time() const
//	{
//		std::atomic_thread_fence(std::memory_order_relaxed);
//		auto counted_time = std::chrono::duration_cast<Units>(Clock::now() - start_point).count();
//		std::atomic_thread_fence(std::memory_order_relaxed);
//		return static_cast<Rep>(counted_time);
//	}
//};
//
//using precise_stopwatch = stopwatch<>;
//using system_stopwatch = stopwatch<std::chrono::system_clock>;
//using monotonic_stopwatch = stopwatch<std::chrono::steady_clock>;

