#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

volatile int  sum = 0;

 std::atomic_bool flags[2] = { false, false };
 std::atomic_int victim;

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

	for (int n = 2; n <= 2; n++) {
		sum = 0;
		std::vector<std::thread> tv;
		auto start_t = high_resolution_clock::now();
		for (int i = 0; i < n; ++i) {
			int num_loop = 5000000 / n;
			if (i == (n - 1))
				num_loop += 5000000 % num_loop;
			tv.emplace_back(worker_p, i, num_loop);
		}
		for (auto& th : tv)
			th.join();
		auto end_t = high_resolution_clock::now();
		auto exec_t = end_t - start_t;
		size_t ms = duration_cast<milliseconds>(exec_t).count();
		std::cout << n << " Threads, Peterson Sum = " << sum << ", " << ms << "ms.\n";
	}
}