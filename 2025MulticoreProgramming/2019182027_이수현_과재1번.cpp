#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <numeric>

int no_lock_sum;
int mutex_sum;
int atomic_bread_sum;
int volatile_bread_sum;

std::atomic_bool atomic_flag[8];
std::atomic_int atomic_label[8];

volatile bool volatile_flag[8];
volatile int volatile_label[8];

std::mutex m;

void atomic_lock(int id, int num_thread)
{
	atomic_flag[id] = true;

	// int �ּҰ����� �ʱ�ȭ
	int max = std::numeric_limits<int>::min();
	for (int i = 0; i < num_thread; ++i) {
		if (atomic_label[i] > max)
			max = atomic_label[i];
	}
	// ���� ū ���� ���� �� + 1�� id�� ��
	atomic_label[id] = max + 1;

	bool stay = false;

	while (true) {
		stay = false;	// �ٽ� false�� reset
		for (int i = 0; i < num_thread; ++i) {
			if (i == id)
				continue;

			if (atomic_flag[i]) {
				if (atomic_label[i] < atomic_label[id]) {		// �󺧰��� i�� ������ �� ����, i�� ������ id�� ���
					stay = true;
				}
				else if (atomic_label[i] == atomic_label[id]) {	// �󺧰��� ������ id�� �� ������ �� ����
					if (i < id) {
						stay = true;
					}
				}
			}
		}

		if (!stay) // �� �̻� ����� �ʿ䰡 ������ Ż��
			break;
	}
}

void volatile_unlock(int id)
{
	volatile_flag[id] = false;
}

void volatile_lock(int id, int num_thread)
{
	volatile_flag[id] = true;

	// int �ּҰ����� �ʱ�ȭ
	int max = std::numeric_limits<int>::min();
	for (int i = 0; i < num_thread; ++i) {
		if (volatile_label[i] > max)
			max = volatile_label[i];
	}
	// ���� ū ���� ���� �� + 1�� id�� ��
	volatile_label[id] = max + 1;

	bool stay = false;

	while (true) {
		stay = false;	// �ٽ� false�� reset
		for (int i = 0; i < num_thread; ++i) {
			if (i == id)
				continue;

			if (volatile_flag[i]) {
				if (volatile_label[i] < volatile_label[id]) {		// �󺧰��� i�� ������ �� ����, i�� ������ id�� ���
					stay = true;
				}
				else if (volatile_label[i] == volatile_label[id]) {	// �󺧰��� ������ id�� �� ������ �� ����
					if (i < id) {
						stay = true;
					}
				}
			}
		}

		if (!stay) // �� �̻� ����� �ʿ䰡 ������ Ż��
			break;
	}
}

void atomic_unlock(int id)
{
	atomic_flag[id] = false;
}

void no_lock_worker(const int loop_count)
{
	for (int i = 0; i < loop_count; ++i) {
		no_lock_sum += 2;
	}
}

void mutex_worker(const int loop_count)
{
	for (int i = 0; i < loop_count; ++i) {
		m.lock();
		mutex_sum += 2;
		m.unlock();
	}

}

void atomic_bread_worker(const int thread_id, const int loop_count, int num_thread)
{
	for (int i = 0; i < loop_count; ++i) {
		atomic_lock(thread_id, num_thread);
		atomic_bread_sum += 2;
		atomic_unlock(thread_id);
	}
}

void volatile_bread_worker(const int thread_id, const int loop_count, int num_thread)
{
	for (int i = 0; i < loop_count; ++i) {
		volatile_lock(thread_id, num_thread);
		volatile_bread_sum += 2;
		volatile_unlock(thread_id);
	}
}
int main()
{
	using namespace std::chrono;

	// NO LOCK
	{
		for (int num_threads = 1; num_threads <= 8; num_threads *= 2) {
			no_lock_sum = 0;
			std::vector<std::thread> tv;

			auto start = high_resolution_clock::now();
			for (int i = 0; i < num_threads; ++i) {
				tv.emplace_back(no_lock_worker, 500'0000 / num_threads);
			}
			for (auto& th : tv)
				th.join();
			auto end = high_resolution_clock::now();
			auto duration = duration_cast<microseconds>(end - start).count();
			std::cout << num_threads << " Threads duration : " << duration << "��s, No Lock Sum = "<< no_lock_sum << std::endl;
		}
	}
	// MUTEX
	{
		for (int num_threads = 1; num_threads <= 8; num_threads *= 2) {
			mutex_sum = 0;
			std::vector<std::thread> tv;

			auto start = high_resolution_clock::now();
			for (int i = 0; i < num_threads; ++i) {
				tv.emplace_back(mutex_worker, 500'0000 / num_threads);
			}
			for (auto& th : tv)
				th.join();
			auto end = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(end - start).count();
			std::cout << num_threads << " Threads duration : " << duration << "ms, Mutex Sum = " << mutex_sum << std::endl;
		}
	}
	// ���� �˰��� (atomic)
	{
		for (int num_threads = 1; num_threads <= 8; num_threads *= 2) {
			atomic_bread_sum = 0;
			std::vector<std::thread> tv;
			for (int i = 0; i < num_threads; ++i) {
				atomic_flag[i] = 0;
				atomic_label[i] = 0;
			}
			auto start = high_resolution_clock::now();
			for (int i = 0; i < num_threads; ++i) {
				tv.emplace_back(atomic_bread_worker, i, 500'0000 / num_threads, num_threads);
			}
			for (auto& th : tv)
				th.join();
			auto end = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(end - start).count();
			std::cout << num_threads << " Threads duration : " << duration << "ms, bread Sum(atomic) = " << atomic_bread_sum << std::endl;
		}
	}
	// ���� �˰��� (volatile)
	{
		for (int num_threads = 1; num_threads <= 8; num_threads *= 2) {
			volatile_bread_sum = 0;
			std::vector<std::thread> tv;
			for (int i = 0; i < num_threads; ++i) {
				volatile_flag[i] = 0;
				volatile_label[i] = 0;
			}		
			auto start = high_resolution_clock::now();
			for (int i = 0; i < num_threads; ++i) {
				tv.emplace_back(volatile_bread_worker, i, 500'0000 / num_threads, num_threads);
			}
			for (auto& th : tv)
				th.join();
			auto end = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(end - start).count();
			std::cout << num_threads << " Threads duration : " << duration << "ms, bread Sum(volatile) = " << volatile_bread_sum << std::endl;
		}
	}
}
