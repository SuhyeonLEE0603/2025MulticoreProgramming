#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <atomic>

std::atomic<int> sum;
std::mutex mtx;

void worker(const int loop_count) 
{
	for (auto i = 0; i < loop_count; ++i) {
		//mtx.lock();
		sum += 2;
		//mtx.unlock();
	}
}

int main()
{
	using namespace std::chrono;

	auto start = high_resolution_clock::now();
	for (int i = 0; i < 50000000; ++i) {
		sum = sum + 2;
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
			threads.emplace_back(worker, 50000000 / num_threads);
		}
		for (auto& t : threads) {
			t.join();
		}
		high_resolution_clock::time_point t_end = high_resolution_clock::now();
		auto t_duration = duration_cast<milliseconds>(t_end - t_start).count();
		std::cout << num_threads << " threads duration: " << t_duration << " ms, SUM = " << sum << std::endl;
	}
}