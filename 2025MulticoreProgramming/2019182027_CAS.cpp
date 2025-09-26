#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

volatile int sum;
volatile int LOCK = 0;

bool CAS(volatile int* addr, int expected, int new_val)
{
	return std::atomic_compare_exchange_strong(
		reinterpret_cast<volatile std::atomic_int*>(addr),
		&expected, new_val);
}

void CAS_LOCK()
{
	while (!CAS(&LOCK, 0, 1));
}

void CAS_UNLOCK()
{
	CAS(&LOCK, 1, 0);
}

void CAS_worker(int num_threads)
{
	const int loop_count = 5000'0000 / num_threads;
	for (auto i = 0; i < loop_count; ++i) {
		CAS_LOCK();
		sum = sum + 2;
		CAS_UNLOCK();
	}
}

int main()
{
	using namespace std::chrono;

	for (int n = 1; n <= 8; n *= 2) {
		sum = 0;
		std::vector<std::thread> tv;
		auto start_time = high_resolution_clock::now();
		for (int i = 1; i <= n; ++i) {
			tv.emplace_back(CAS_worker, n);
		}
		for (auto& th : tv) {
			th.join();
		}
		auto end_time = high_resolution_clock::now();
		auto exec_time = end_time - start_time;
		size_t ms = duration_cast<milliseconds>(exec_time).count();
		std::cout << n << " Threads, CAS Sum = " << sum << ", " << ms << "ms.\n";
	}
}