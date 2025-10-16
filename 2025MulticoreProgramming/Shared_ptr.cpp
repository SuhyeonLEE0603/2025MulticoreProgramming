#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <numeric>

const int MAX_THREADS = 16;

#include <queue>

//
//class NODE_SP {
//public:
//	int value;
//	std::shared_ptr<NODE_SP> next;
//	std::mutex mtx;
//	volatile bool removed;
//	NODE_SP(int x) : next(nullptr), value(x), removed(false) {}
//	void lock() { mtx.lock(); }
//	void unlock() { mtx.unlock(); }
//};

class NODE_ASP {
public:
	int value;
	std::atomic<std::shared_ptr<NODE_ASP>> next;
	std::mutex mtx;
	volatile bool removed;
	NODE_ASP(int x) : next(nullptr), value(x), removed(false) {}
	void lock() { mtx.lock(); }
	void unlock() { mtx.unlock(); }
};


//class L_SET_SP{
//private:
//	std::shared_ptr<NODE_SP> head, tail;
//public:
//	L_SET_SP() {
//		head = std::make_shared<NODE_SP>(std::numeric_limits<int>::min());
//		tail = std::make_shared<NODE_SP>(std::numeric_limits<int>::max());
//
//		head->next = tail;
//	}
//
//	~L_SET_SP()
//	{
//	}
//
//	void recycle()
//	{
//
//	}
//
//	void clear()
//	{
//		head->next = tail;
//	}
//
//	bool validate(const std::shared_ptr<NODE_SP> &p, 
//		const std::shared_ptr<NODE_SP> &c)
//	{
//		return (p->removed == false)
//			&& (c->removed == false)
//			&& (std::atomic_load(&p->next) == c);
//	}
//
//	bool add(int x)
//	{
//		while (true) {
//			std::shared_ptr<NODE_SP> prev = head;
//			std::shared_ptr<NODE_SP> curr = std::atomic_load(&prev->next);
//
//			while (curr->value < x) {
//				prev = curr;
//				curr = std::atomic_load(&curr->next);
//			}
//
//			prev->lock();
//			curr->lock();
//
//			if (!validate(prev, curr)) {
//				prev->unlock(); curr->unlock();
//				continue;
//			}
//
//			if (curr->value == x) {
//				prev->unlock(); curr->unlock();
//				return false; 
//			}
//			else {
//				auto newNode = std::make_shared<NODE_SP>(x);
//				newNode->next = curr;
//				std::atomic_exchange(&prev->next,newNode);
//				prev->unlock();	curr->unlock();
//				return true;
//			}
//		}
//	}
//
//	bool remove(int x)
//	{
//		while (true) {
//			std::shared_ptr<NODE_SP> prev = head;
//			std::shared_ptr<NODE_SP> curr = std::atomic_load(&prev->next);
//			while (curr->value < x) {
//				prev = curr;
//				curr = std::atomic_load(&curr->next);
//			}
//
//			prev->lock(); curr->lock();
//			if (false == validate(prev, curr)) {
//				prev->unlock(); curr->unlock();
//				continue;
//			}
//			if (curr->value != x) {
//				prev->unlock(); curr->unlock();
//				return false;
//			}
//			else {
//				curr->removed = true;
//				std::atomic_thread_fence(std::memory_order_release);
//				std::atomic_exchange(&prev->next,std::atomic_load(&curr->next));
//				prev->unlock(); curr->unlock();
//				return true;
//			}
//		}
//	}
//
//	bool contains(int x)
//	{
//		std::shared_ptr<NODE_SP> curr = std::atomic_load(&head->next);
//		while (curr->value < x) {
//			curr = std::atomic_load(&curr->next);
//		}
//		return (curr->value == x && !curr->removed);
//	}
//
//	void print20()
//	{
//		auto curr = std::atomic_load(&head->next);
//		for (int i = 0; i < 20 && curr != tail; ++i) {
//			std::cout << curr->value << ", ";
//			curr = curr->next;
//		}
//		std::cout << std::endl;
//	}
//};

class L_SET_ASP {
private:
	std::shared_ptr<NODE_ASP> head, tail;
public:
	L_SET_ASP() {
		head = std::make_shared<NODE_ASP>(std::numeric_limits<int>::min());
		tail = std::make_shared<NODE_ASP>(std::numeric_limits<int>::max());

		head->next = tail;
	}

	~L_SET_ASP()
	{
	}

	void recycle()
	{

	}

	void clear()
	{
		head->next = tail;
	}

	bool validate(const std::shared_ptr<NODE_ASP>& p,
		const std::shared_ptr<NODE_ASP>& c)
	{
		return (p->removed == false)
			&& (c->removed == false)
			&& (p->next.load() == c);
	}

	bool add(int x)
	{
		while (true) {
			auto prev = head;
			std::shared_ptr<NODE_ASP> curr = prev->next;

			while (curr->value < x) {
				prev = curr;
				curr = curr->next;
			}

			prev->lock();
			curr->lock();

			if (!validate(prev, curr)) {
				prev->unlock(); curr->unlock();
				continue;
			}

			if (curr->value == x) {
				prev->unlock(); curr->unlock();
				return false;
			}
			else {
				auto newNode = std::make_shared<NODE_ASP>(x);
				newNode->next = curr;
				prev->next = newNode;
				prev->unlock();	curr->unlock();
				return true;
			}
		}
	}

	bool remove(int x)
	{
		while (true) {
			std::shared_ptr<NODE_ASP> prev = head;
			std::shared_ptr<NODE_ASP> curr = prev->next;
			while (curr->value < x) {
				prev = curr;
				curr = curr->next;
			}

			prev->lock(); curr->lock();
			if (false == validate(prev, curr)) {
				prev->unlock(); curr->unlock();
				continue;
			}
			if (curr->value != x) {
				prev->unlock(); curr->unlock();
				return false;
			}
			else {
				curr->removed = true;
				std::atomic_thread_fence(std::memory_order_release);
				prev->next = curr->next.load();
				prev->unlock(); curr->unlock();
				return true;
			}
		}
	}

	bool contains(int x)
	{
		std::shared_ptr<NODE_ASP> curr = head->next;
		while (curr->value < x) {
			curr = curr->next;
		}
		return (curr->value == x && !curr->removed);
	}

	void print20()
	{
		std::shared_ptr<NODE_ASP> curr = head->next;
		for (int i = 0; i < 20 && curr != tail; ++i) {
			std::cout << curr->value << ", ";
			curr = curr->next;
		}
		std::cout << std::endl;
	}
};

L_SET_ASP set;

const int LOOP = 400'0000;
const int RANGE = 1000;

#include <array>

class HISTORY {
public:
	int op;
	int i_value;
	bool o_value;
	HISTORY(int o, int i, bool re) : op(o), i_value(i), o_value(re) {}
};

std::array<std::vector<HISTORY>, MAX_THREADS> history;

void check_history(int num_threads)
{
	std::array <int, RANGE> survive = {};
	std::cout << "Checking Consistency : ";
	if (history[0].size() == 0) {
		std::cout << "No history.\n";
		return;
	}
	for (int i = 0; i < num_threads; ++i) {
		for (auto& op : history[i]) {
			if (false == op.o_value) continue;
			if (op.op == 3) continue;
			if (op.op == 0) survive[op.i_value]++;
			if (op.op == 1) survive[op.i_value]--;
		}
	}
	for (int i = 0; i < RANGE; ++i) {
		int val = survive[i];
		if (val < 0) {
			std::cout << "ERROR. The value " << i << " removed while it is not in the set.\n";
			exit(-1);
		}
		else if (val > 1) {
			std::cout << "ERROR. The value " << i << " is added while the set already have it.\n";
			exit(-1);
		}
		else if (val == 0) {
			if (set.contains(i)) {
				std::cout << "ERROR. The value " << i << " should not exists.\n";
				exit(-1);
			}
		}
		else if (val == 1) {
			if (false == set.contains(i)) {
				std::cout << "ERROR. The value " << i << " shoud exists.\n";
				exit(-1);
			}
		}
	}
	std::cout << " OK\n";
}

void benchmark_check(int num_threads, int th_id)
{
	for (int i = 0; i < LOOP / num_threads; ++i) {
		int op = rand() % 3;
		switch (op) {
		case 0: {
			int v = rand() % RANGE;
			history[th_id].emplace_back(0, v, set.add(v));
			break;
		}
		case 1: {
			int v = rand() % RANGE;
			history[th_id].emplace_back(1, v, set.remove(v));
			break;
		}
		case 2: {
			int v = rand() % RANGE;
			history[th_id].emplace_back(2, v, set.contains(v));
			break;
		}
		}
	}
}
void benchmark(const int num_threads)
{
	for (int i = 0; i < LOOP / num_threads; ++i) {
		int value = rand() % RANGE;
		int op = rand() % 3;
		if (op == 0) set.add(value);
		else if (op == 1) set.remove(value);
		else set.contains(value);
	}
}

int main()
{
	using namespace std::chrono;

	for (int num_thread = 1; num_thread <= MAX_THREADS; num_thread *= 2) {
		set.clear();
		std::vector<std::thread> threads;
		auto start = high_resolution_clock::now();
		for (int i = 0; i < num_thread; ++i)
			threads.emplace_back(benchmark, num_thread);
		for (auto& th : threads)
			th.join();
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		std::cout << "Threads: " << num_thread
			<< ", Duration: " << duration.count() << " ms.\n";
		std::cout << "Set: "; set.print20();

	}

	std::cout << "\n\nConsistency Check\n";

	for (int num_thread = 1; num_thread <= MAX_THREADS; num_thread *= 2) {
		set.clear();
		std::vector<std::thread> threads;
		for (int i = 0; i < MAX_THREADS; ++i)
			history[i].clear();
		auto start = high_resolution_clock::now();
		for (int i = 0; i < num_thread; ++i)
			threads.emplace_back(benchmark_check, num_thread, i);
		for (auto& th : threads)
			th.join();
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		std::cout << "Threads: " << num_thread
			<< ", Duration: " << duration.count() << " ms.\n";
		std::cout << "Set: "; set.print20();
		check_history(num_thread);

	}
}