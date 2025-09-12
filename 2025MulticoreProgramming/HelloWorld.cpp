#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <atomic>

const int MAX_THREADS = 16;
const int CACHE_LINE_SIZE_INT = 64;
volatile int sum = 0;
struct NUM
{
	alignas(64) volatile int value;
};

NUM array_sum[MAX_THREADS] = { 0 };
std::mutex mtx;

void worker(const int thread_id, const int loop_count) 
{	
	
	for (auto i = 0; i < loop_count; ++i) {
		array_sum[thread_id].value =
			array_sum[thread_id].value + 2;
	}
}

int main()
{
	using namespace std::chrono;

	auto start = high_resolution_clock::now();
	for (int i = 0; i < 50000000; ++i) {
		sum += 2;
	}
	auto end = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(end - start).count();
	std::cout << "Single thread duration: " << duration << " ms,  SUM = "<< sum << std::endl;

	sum = 0;
	for(int num_threads = 1; num_threads <= 16; num_threads*=2) {
		sum = 0;
		high_resolution_clock::time_point t_start = high_resolution_clock::now();
		std::vector<std::thread> threads;
		for(int i = 0; i < num_threads; i++) {
			threads.emplace_back(worker, i, 50000000 / num_threads);
		}
		for (int i = 0; i < num_threads; ++i) {
			threads[i].join();
			sum = sum + array_sum[i].value;
			array_sum[i].value = 0;
		}

		high_resolution_clock::time_point t_end = high_resolution_clock::now();
		auto t_duration = duration_cast<milliseconds>(t_end - t_start).count();
		std::cout << num_threads << " threads duration: " << t_duration << " ms, SUM = " << sum << std::endl;
	}
}