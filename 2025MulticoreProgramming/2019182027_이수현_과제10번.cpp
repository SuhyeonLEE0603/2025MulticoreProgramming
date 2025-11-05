#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>

const int MAX_THREADS = 16;

class NODE {
public:
	int value;
	NODE* next;
	NODE(int x) : next(nullptr), value(x) {}
};

class DUMMY_MTX {
public:
	void lock() {}
	void unlock() {}
};

class C_QUEUE {
private:
	NODE* head, * tail;
	std::mutex mtx;
public:
	C_QUEUE() {
		head = tail = new NODE(-1);
	}

	~C_QUEUE() {
		clear();
		delete head;
	}

	void clear()
	{
		NODE* curr = head->next;
		while (nullptr != curr) {
			NODE* next = curr->next;
			delete curr;
			curr = next;
		}
		tail = head;
		head->next = nullptr;
	}

	void enqueue(int x)
	{
		NODE* new_node = new NODE(x);
		mtx.lock();
		tail->next = new_node;
		tail = new_node;
		mtx.unlock();
	}

	int dequeue()
	{
		NODE* temp;
		mtx.lock();
		if (nullptr == head->next) {
			mtx.unlock();
			return -1;
		}
		int res = head->next->value;
		temp = head;
		head = head->next;
		mtx.unlock();
		delete temp;
		return res;
	}

	void print20()
	{
		auto curr = head->next;
		for (int i = 0; i < 20 && curr != nullptr; ++i, curr->next) {
			std::cout << curr->value << ", ";
			curr = curr->next;
		}
		std::cout << std::endl;
	}
};

C_QUEUE my_queue;

const int NUM_TEST = 1000'0000;

void benchmark(const int num_threads, int th_id)
{
	const int loop_count = NUM_TEST / num_threads;

	int key = 0;
	for (int i = 0; i < loop_count; ++i) {
		if ((i < 32) || (rand() % 2 == 0))
			my_queue.enqueue(key++);
		else
			my_queue.dequeue();
	}
}

int main()
{
	using namespace std::chrono;

	for (int num_thread = 1; num_thread <= MAX_THREADS; num_thread *= 2) {
		my_queue.clear();
		auto start = high_resolution_clock::now();
		std::vector<std::thread> threads;
		for (int i = 0; i < num_thread; ++i)
			threads.emplace_back(benchmark, num_thread, i);
		for (auto& th : threads)
			th.join();
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start).count();
		std::cout << "Thread Count =  " << num_thread << ", Exec Time = " << duration << " ms.\n";
		std::cout << "Result : "; my_queue.print20();
	}
}