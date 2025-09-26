#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <atomic>

const int MAX_THREADS = 16;
const int CACHE_LINE_SIZE_INT = 64;
int sum = 0;
std::atomic_bool lock = false;

std::mutex mtx;

bool CAS(std::atomic_bool* lock_flag, bool old_value, bool new_value)
{
	return std::atomic_compare_exchange_strong(
		lock_flag, &old_value, new_value);
}

void cas_lock()
{
	while (!CAS(&lock, false, true));
}

void cas_unlock()
{
	CAS(&lock, true, false);
}

void worker3(const int thread_id, const int loop_count)
{
	int total = 0;
	for (int i = 0; i < loop_count; ++i) {
		cas_lock();
		sum += 2;
		cas_unlock();
	}

}


int main()
{
	using namespace std::chrono;

	for(int num_threads = 1; num_threads <= 8; num_threads*=2) {
		sum = 0;
		high_resolution_clock::time_point t_start = high_resolution_clock::now();
		std::vector<std::thread> threads;
		
		for(int i = 0; i < num_threads; ++i) {
			threads.emplace_back(worker3, i, 5000'0000 / num_threads);
		}

		for (auto& th : threads) {
			th.join();
		}

		high_resolution_clock::time_point t_end = high_resolution_clock::now();
		auto t_duration = duration_cast<milliseconds>(t_end - t_start).count();
		std::cout << num_threads << " threads duration: " << t_duration << " ms, SUM = " << sum << std::endl;
	}
}