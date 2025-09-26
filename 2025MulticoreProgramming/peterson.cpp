#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <queue>

std::queue<int> g_queue;

volatile int sum = 0;

 std::atomic<bool> flags[2] = { false, false };
 std::atomic<int> victim;

void p_lock(int th_id)
{
	int other = 1 - th_id;
	flags[th_id] = true;
	victim = th_id;
	while ((flags[other] == true) && (victim == th_id));
}

void p_unlock(int th_id)
{
	flags[th_id] = false;
}


void worker_p(const int th_id, const int num_loop)
{
	for (auto i = 0; i < num_loop; ++i) {
		p_lock(th_id);
		sum = sum + 2;
		p_unlock(th_id);
	}
}

int main()
{
	using namespace std::chrono;

	for (int n = 1; n <= 8; n*=2) {
		sum = 0;
		std::vector<std::thread> tv;
		auto start_t = high_resolution_clock::now();
		for (int i = 0; i < n; ++i) {
			tv.emplace_back(worker_p, i, 500'0000 / n);
		}
		for (auto& th : tv)
			th.join();
		auto end_t = high_resolution_clock::now();
		auto exec_t = end_t - start_t;
		size_t ms = duration_cast<milliseconds>(exec_t).count();
		std::cout << n << " Threads, Peterson Sum = " << sum << ", " << ms << "ms.\n";
	}
}