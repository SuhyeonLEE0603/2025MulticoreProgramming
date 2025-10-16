#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <numeric>
#include <queue>

const int MAX_THREADS = 16;

class NODE {
public:
	int value;
	NODE* volatile next;
	std::mutex mtx;
	volatile bool removed;
	NODE(int x) : next(nullptr), value(x), removed(false) {}	
	void lock() { mtx.lock(); }
	void unlock() { mtx.unlock(); }
};

class DUMMY_MTX {
	public:
	void lock() {}
	void unlock() {}
};	

class C_SET {
private:
	NODE* head, * tail;
	std::mutex mtx;
public:
	C_SET() {
		head = new NODE(std::numeric_limits<int>::min());
		tail = new NODE(std::numeric_limits<int>::max());
		head->next = tail;
	}

	~C_SET()
	{
		clear();
		delete head;
		delete tail;
	}

	void clear()
	{
		NODE* curr = head->next;
		while (curr != tail) {
			NODE* temp = curr;
			curr = curr->next;
			delete temp;
		}
		head->next = tail;
	}

	bool add(int x)
	{
		auto prev = head;
		mtx.lock();
		auto curr = prev->next;

		while (curr->value < x) {
			prev = curr;
			curr = curr->next;
		}

		if (curr->value == x) {
			mtx.unlock();
			return false;
		}
		else {
			auto newNode = new NODE(x);
			newNode->next = curr;
			prev->next = newNode;
			mtx.unlock();
			return true;
		}
	}

	bool remove(int x)
	{
		auto prev = head;
		mtx.lock();
		auto curr = prev->next;

		while (curr->value < x) {
			prev = curr;
			curr = curr->next;
		}

		if (curr->value != x) {
			mtx.unlock();
			return false;
		}
		else {
			prev->next = curr->next;
			mtx.unlock();
			delete curr;
			return true;
		}
	}

	bool contains(int x)
	{
		auto prev = head;
		mtx.lock();
		auto curr = prev->next;

		while (curr->value < x) {
			prev = curr;
			curr = curr->next;
		}

		if (curr->value == x) {
			mtx.unlock();
			return true;
		} else {
			mtx.unlock();
			return false;
		}
	}

	void print20()
	{
		auto curr = head->next;
		for (int i = 0; i < 20 && curr != tail; ++i) {
			std::cout << curr->value << ", ";
			curr = curr->next;
		}
		std::cout << std::endl;
	}
};

class F_SET {
private:
	NODE* head, * tail;
public:
	F_SET() {
		head = new NODE(std::numeric_limits<int>::min());
		tail = new NODE(std::numeric_limits<int>::max());
		head->next = tail;
	}

	~F_SET()
	{
		clear();
		delete head;
		delete tail;
	}

	void clear()
	{
		NODE* curr = head->next;
		while (curr != tail) {
			NODE* temp = curr;
			curr = curr->next;
			delete temp;
		}
		head->next = tail;
	}

	bool add(int x)
	{
		auto prev = head;
		prev->lock();
		auto curr = prev->next;
		curr->lock();
		while (curr->value < x) {
			prev->unlock();
			prev = curr;
			curr = curr->next;
			curr->lock();
		}

		if (curr->value == x) {
			prev->unlock();	curr->unlock();
			return false;
		}
		else {
			auto newNode = new NODE(x);
			newNode->next = curr;
			prev->next = newNode;
			prev->unlock();	curr->unlock();
			return true;
		}
	}

	bool remove(int x)
	{
		auto prev = head;
		prev->lock();
		auto curr = prev->next;
		curr->lock();
		while (curr->value < x) {
			prev->unlock();
			prev = curr;
			curr = curr->next;
			curr->lock();
		}

		if (curr->value != x) {
			prev->unlock();	curr->unlock();
			return false;
		}
		else {
			prev->next = curr->next;
			prev->unlock();	curr->unlock();
			delete curr;
			return true;
		}
	}

	bool contains(int x)
	{
		auto prev = head;
		prev->lock();
		auto curr = prev->next;
		curr->lock();
		while (curr->value < x) {
			prev->unlock();
			prev = curr;
			curr = curr->next;
			curr->lock();
		}

		if (curr->value == x) {
			prev->unlock();	curr->unlock();
			return true;
		}
		else {
			prev->unlock();	curr->unlock();
			return false;
		}
	}

	void print20()
	{
		auto curr = head->next;
		for (int i = 0; i < 20 && curr != tail; ++i) {
			std::cout << curr->value << ", ";
			curr = curr->next;
		}
		std::cout << std::endl;
	}
};

class O_SET {
private:
	NODE* head, * tail;
public:
	O_SET() {
		head = new NODE(std::numeric_limits<int>::min());
		tail = new NODE(std::numeric_limits<int>::max());
		head->next = tail;
	}

	~O_SET()
	{
		clear();
		delete head;
		delete tail;
	}

	void clear()
	{
		NODE* curr = head->next;
		while (curr != tail) {
			NODE* temp = curr;
			curr = curr->next;
			delete temp;
		}
		head->next = tail;
	}

	bool validate(int x, NODE* p, NODE* c)
	{
		auto prev = head;
		auto curr = prev->next;
		while (curr->value < x) {
			prev = curr;
			curr = curr->next;
		}
		return ((prev == p) && (curr == c));
	}

	bool add(int x)
	{
		while (true) {
			auto prev = head;
			auto curr = prev->next;
			while (curr->value < x) {
				prev = curr;
				curr = curr->next;
			}

			prev->lock(); curr->lock();
			if (false == validate(x, prev, curr)) {
				prev->unlock(); curr->unlock();
				continue;
			}
			if (curr->value == x) {
				prev->unlock();	curr->unlock();
				return false;
			}
			else {
				auto newNode = new NODE(x);
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
			auto prev = head;
			auto curr = prev->next;
			while (curr->value < x) {
				prev = curr;
				curr = curr->next;
			}

			prev->lock(); curr->lock();
			if (false == validate(x, prev, curr)) {
				prev->unlock(); curr->unlock();
				continue;
			}
			if (curr->value != x) {
				prev->unlock();	curr->unlock();
				return false;
			}
			else {
				prev->next = curr->next;
				prev->unlock();	curr->unlock();
				//delete curr;
				return true;
			}
		}
	}

	bool contains(int x)
	{
		while (true) {
			auto prev = head;
			auto curr = prev->next;
			while (curr->value < x) {
				prev = curr;
				curr = curr->next;
			}
			prev->lock(); curr->lock();
			if (false == validate(x, prev, curr)) {
				prev->unlock(); curr->unlock();
				continue;
			}
			if (curr->value == x) {
				prev->unlock();	curr->unlock();
				return true;
			}
			else {
				prev->unlock();	curr->unlock();
				return false;
			}
		}
	}

	void print20()
	{
		auto curr = head->next;
		for (int i = 0; i < 20 && curr != tail; ++i) {
			std::cout << curr->value << ", ";
			curr = curr->next;
		}
		std::cout << std::endl;
	}
};

class L_SET {
private:
	NODE* head, * tail;
public:
	L_SET() {
		head = new NODE(std::numeric_limits<int>::min());
		tail = new NODE(std::numeric_limits<int>::max());
		head->next = tail;
	}

	~L_SET()
	{
		clear();
		delete head;
		delete tail;
	}

	void clear()
	{
		NODE* curr = head->next;
		while (curr != tail) {
			NODE* temp = curr;
			curr = curr->next;
			delete temp;
		}
		head->next = tail;
	}

	bool validate(int x, NODE* p, NODE* c)
	{
		return (p->removed == false) 
			&& (c->removed == false)
			&& (p->next == c);
	}

	bool add(int x)
	{


		while (true) {
			auto prev = head;
			auto curr = prev->next;
			while (curr->value < x) {
				prev = curr;
				curr = curr->next;
			}

			prev->lock(); curr->lock();
			if (false == validate(x, prev, curr)) {
				prev->unlock(); curr->unlock();
				continue;
			}
			if (curr->value == x) {
				prev->unlock();	curr->unlock();
				return false;
			}
			else {
				auto newNode = new NODE(x);
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
			auto prev = head;
			auto curr = prev->next;
			while (curr->value < x) {
				prev = curr;
				curr = curr->next;
			}

			prev->lock(); curr->lock();
			if (false == validate(x, prev, curr)) {
				prev->unlock(); curr->unlock();
				continue;
			}
			if (curr->value != x) {
				prev->unlock();	curr->unlock();
				return false;
			}
			else {
				curr->removed = true;
				std::atomic_thread_fence(std::memory_order_seq_cst);
				prev->next = curr->next;
				prev->unlock();	curr->unlock();
				return true;
			}
		}
	}

	bool contains(int x)
	{
		auto curr = head;
		while (curr->value < x) {
			curr = curr->next;
		}
		return (x == curr->value) && (curr->removed == false);
	}

	void print20()
	{
		auto curr = head->next;
		for (int i = 0; i < 20 && curr != tail; ++i) {
			std::cout << curr->value << ", ";
			curr = curr->next;
		}
		std::cout << std::endl;
	}
};

class L_SET_FL {
	private:
		NODE* head, * tail;
		std::queue<NODE*> free_list;
		std::mutex fl_mtx;
public:

	void my_delete(NODE* node) {
		std::lock_guard<std::mutex> lg(fl_mtx);
		free_list.push(node);
	}

	void recycle() {

	}

	L_SET_FL() {
		head = new NODE(std::numeric_limits<int>::min());
		tail = new NODE(std::numeric_limits<int>::max());
		head->next = tail;
	}

	~L_SET_FL()
	{
		clear();
		delete head;
		delete tail;
	}

	void clear()
	{
		NODE* curr = head->next;
		while (curr != tail) {
			NODE* temp = curr;
			curr = curr->next;
			delete temp;
		}
		head->next = tail;
	}

	bool validate(int x, NODE* p, NODE* c)
	{
		return (p->removed == false)
			&& (c->removed == false)
			&& (p->next == c);
	}

	bool add(int x)
	{


		while (true) {
			auto prev = head;
			auto curr = prev->next;
			while (curr->value < x) {
				prev = curr;
				curr = curr->next;
			}

			prev->lock(); curr->lock();
			if (false == validate(x, prev, curr)) {
				prev->unlock(); curr->unlock();
				continue;
			}
			if (curr->value == x) {
				prev->unlock();	curr->unlock();
				return false;
			}
			else {
				auto newNode = new NODE(x);
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
			auto prev = head;
			auto curr = prev->next;
			while (curr->value < x) {
				prev = curr;
				curr = curr->next;
			}

			prev->lock(); curr->lock();
			if (false == validate(x, prev, curr)) {
				prev->unlock(); curr->unlock();
				continue;
			}
			if (curr->value != x) {
				prev->unlock();	curr->unlock();
				return false;
			}
			else {
				curr->removed = true;
				std::atomic_thread_fence(std::memory_order_seq_cst);
				prev->next = curr->next;
				prev->unlock();	curr->unlock();
				return true;
			}
		}
	}

	bool contains(int x)
	{
		auto curr = head;
		while (curr->value < x) {
			curr = curr->next;
		}
		return (x == curr->value) && (curr->removed == false);
	}

	void print20()
	{
		auto curr = head->next;
		for (int i = 0; i < 20 && curr != tail; ++i) {
			std::cout << curr->value << ", ";
			curr = curr->next;
		}
		std::cout << std::endl;
	}

};

class NODE_SP {
public:
	int value;
	std::shared_ptr<NODE_SP> next;
	std::mutex mtx;
	volatile bool removed;
	NODE_SP(int x) : next(nullptr), value(x), removed(false) {}
	void lock() { mtx.lock(); }
	void unlock() { mtx.unlock(); }
};

class L_SET_SP {
private:
	std::shared_ptr<NODE_SP> head, tail;
public:

	L_SET_SP() {
		head = std::make_shared<NODE_SP>(std::numeric_limits<int>::min());
		tail = std::make_shared<NODE_SP>(std::numeric_limits<int>::max());
		head->next = tail;
	}

	~L_SET_SP()
	{
	}

	bool validate(const std::shared_ptr<NODE_SP> &p,
		const std::shared_ptr<NODE_SP> &c)
	{
		return (p->removed == false)
			&& (c->removed == false)
			&& (p->next == c);
	}

	bool add(int x)
	{

		while (true) {
			auto prev = head;
			auto curr = prev->next;
			while (curr->value < x) {
				prev = curr;
				curr = curr->next;
			}

			prev->lock(); curr->lock();
			if (false == validate(prev, curr)) {
				prev->unlock(); curr->unlock();
				continue;
			}
			if (curr->value == x) {
				prev->unlock();	curr->unlock();
				return false;
			}
			else {
				auto newNode = new NODE_SP(x);
				newNode->next = curr;
				prev->next = std::shared_ptr<NODE_SP>(newNode);
				prev->unlock();	curr->unlock();
				return true;
			}
		}
	}

	bool remove(int x)
	{
		while (true) {
			auto prev = head;
			auto curr = prev->next;
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
				prev->unlock();	curr->unlock();
				return false;
			}
			else {
				curr->removed = true;
				std::atomic_thread_fence(std::memory_order_seq_cst);
				prev->next = curr->next;
				prev->unlock();	curr->unlock();
				return true;
			}
		}
	}

	bool contains(int x)
	{
		auto curr = head;
		while (curr->value < x) {
			curr = curr->next;
		}
		return (x == curr->value) && (curr->removed == false);
	}

	void print20()
	{
		auto curr = head->next;
		for (int i = 0; i < 20 && curr != tail; ++i) {
			std::cout << curr->value << ", ";
			curr = curr->next;
		}
		std::cout << std::endl;
	}

};

class LF_NODE;

class AMR // Atomic Markable Reference
{
public:
	volatile long long ptr_and_mark;
public:
	AMR(LF_NODE* ptr = nullptr, bool mark = false) {
		long long val = reinterpret_cast<long long>(ptr);
		if (true == mark) val |= 1;
		else { std::cout << "ERROR"; exit(-1); }
		ptr_and_mark = val;
	}

	LF_NODE* get_ptr() {
		long long val = ptr_and_mark;
		return reinterpret_cast<LF_NODE*>(val & ~1ULL);
	}
	bool get_mark() {
		return (1 == (ptr_and_mark & 1));
	}
	LF_NODE* get_ptr_and_mark(bool& mark) {
		long long val = ptr_and_mark;	// 지역변수로 복사를 해야 atomic 보장
		mark = (1 == (val & 1));
		return reinterpret_cast<LF_NODE*>(val & ~1ULL);
	}

	bool attempt_mark(LF_NODE* expected_ptr, bool new_mark) 
	{
		return CAS(expected_ptr, expected_ptr, false, new_mark);
	}

	bool CAS(LF_NODE* old_ptr, LF_NODE* new_ptr,
		bool old_mark, bool new_mark) 
	{
		long long old_val = reinterpret_cast<long long>(old_ptr);
		if (true == old_mark) old_val |= 1;
		else { std::cout << "ERROR"; exit(-1); }

		long long new_val = reinterpret_cast<long long>(new_ptr);
		if (true == new_mark) new_val |= 1;
		else { std::cout << "ERROR"; exit(-1); }
		
		return std::atomic_compare_exchange_strong(
			reinterpret_cast<volatile std::atomic<long long> *>(& ptr_and_mark), &old_val, new_val);
	}
	
};

class LF_NODE {
public:
	int value;
	AMR next;
	LF_NODE(int x) : value(x) {}
};

class LF_SET {
private:
	LF_NODE* head, * tail;
public:
	LF_SET() {
		head = new LF_NODE(std::numeric_limits<int>::min());
		tail = new LF_NODE(std::numeric_limits<int>::max());
		head->next = tail;
	}

	~LF_SET()
	{
		clear();
		delete head;
		delete tail;
	}

	void clear()
	{
		LF_NODE* curr = head->next.get_ptr();
		while (curr != tail) {
			LF_NODE* temp = curr;
			curr = curr->next.get_ptr();
			delete temp;
		}
		head->next = tail;
	}

	void find(LF_NODE*& prev, LF_NODE*& curr, int x)
	{
		while(true) {
			retry:
			prev = head;
			curr = prev->next.get_ptr();
			while (true) {
				bool marked;
				auto succ = curr->next.get_ptr_and_mark(&marked);
				if (true == marked) {
					if (false == prev->next.CAS(curr, succ, false, false)) {
						goto retry; 
						curr = succ;


					}
					
				}
				else {
					if (curr->value >= x) {
						return;
					}
					prev = curr;
					curr = succ;
				}
			}
		}
	}

	bool add(int x)
	{
		while (true) {
			LF_NODE* prev, * curr;
			find(prev, curr, x);
			
			if (curr->value == x) {
				return false;
			}
			else {
				auto newNode = new LF_NODE(x);
				newNode->next = curr;
				if(true == prev->next.CAS(curr, newNode, false, false))
					return true;
			}
		}
	}

	bool remove(int x)
	{
		while (true) {
			auto prev = head;
			auto curr = prev->next;
			while (curr->value < x) {
				prev = curr;
				curr = curr->next;
			}

			prev->lock(); curr->lock();
			if (false == validate(x, prev, curr)) {
				prev->unlock(); curr->unlock();
				continue;
			}
			if (curr->value != x) {
				prev->unlock();	curr->unlock();
				return false;
			}
			else {
				curr->removed = true;
				std::atomic_thread_fence(std::memory_order_seq_cst);
				prev->next = curr->next;
				prev->unlock();	curr->unlock();
				return true;
			}
		}
	}

	bool contains(int x)
	{
		auto curr = head;
		while (curr->value < x) {
			curr = curr->next;
		}
		return (x == curr->value) && (curr->removed == false);
	}

	void print20()
	{
		auto curr = head->next.get_ptr();
		for (int i = 0; i < 20 && curr != tail; ++i) {
			std::cout << curr->value << ", ";
			curr = curr->next.get_ptr();
		}
		std::cout << std::endl;
	}
};
C_SET c_set;
F_SET f_set;
O_SET o_set;
L_SET l_set;

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

//void check_history(int num_threads)
//{
//	std::array <int, RANGE> survive = {};
//	std::cout << "Checking Consistency : ";
//	if (history[0].size() == 0) {
//		std::cout << "No history.\n";
//		return;
//	}
//	for (int i = 0; i < num_threads; ++i) {
//		for (auto& op : history[i]) {
//			if (false == op.o_value) continue;
//			if (op.op == 3) continue;
//			if (op.op == 0) survive[op.i_value]++;
//			if (op.op == 1) survive[op.i_value]--;
//		}
//	}
//	for (int i = 0; i < RANGE; ++i) {
//		int val = survive[i];
//		if (val < 0) {
//			std::cout << "ERROR. The value " << i << " removed while it is not in the set.\n";
//			exit(-1);
//		}
//		else if (val > 1) {
//			std::cout << "ERROR. The value " << i << " is added while the set already have it.\n";
//			exit(-1);
//		}
//		else if (val == 0) {
//			if (set.contains(i)) {
//				std::cout << "ERROR. The value " << i << " should not exists.\n";
//				exit(-1);
//			}
//		}
//		else if (val == 1) {
//			if (false == set.contains(i)) {
//				std::cout << "ERROR. The value " << i << " shoud exists.\n";
//				exit(-1);
//			}
//		}
//	}
//	std::cout << " OK\n";
//}

//void benchmark_check(int num_threads, int th_id)
//{
//	for (int i = 0; i < LOOP / num_threads; ++i) {
//		int op = rand() % 3;
//		switch (op) {
//		case 0: {
//			int v = rand() % RANGE;
//			history[th_id].emplace_back(0, v, set.add(v));
//			break;
//		}
//		case 1: {
//			int v = rand() % RANGE;
//			history[th_id].emplace_back(1, v, set.remove(v));
//			break;
//		}
//		case 2: {
//			int v = rand() % RANGE;
//			history[th_id].emplace_back(2, v, set.contains(v));
//			break;
//		}
//		}
//	}
//}

void benchmark(const int num_threads, int set_num)
{
	if(1 == set_num) {
		for (int i = 0; i < LOOP / num_threads; ++i) {
			int value = rand() % RANGE;
			int op = rand() % 3;
			if (op == 0) c_set.add(value);
			else if (op == 1) c_set.remove(value);
			else c_set.contains(value);
		}
		return;
	}else if(2 == set_num) {
		for (int i = 0; i < LOOP / num_threads; ++i) {
			int value = rand() % RANGE;
			int op = rand() % 3;
			if (op == 0) f_set.add(value);
			else if (op == 1) f_set.remove(value);
			else f_set.contains(value);
		}
		return;
	}else if(3 == set_num) {
		for (int i = 0; i < LOOP / num_threads; ++i) {
			int value = rand() % RANGE;
			int op = rand() % 3;
			if (op == 0) o_set.add(value);
			else if (op == 1) o_set.remove(value);
			else o_set.contains(value);
		}
		return;
	}else if(4 == set_num) {
		for (int i = 0; i < LOOP / num_threads; ++i) {
			int value = rand() % RANGE;
			int op = rand() % 3;
			if (op == 0) l_set.add(value);
			else if (op == 1) l_set.remove(value);
			else l_set.contains(value);
		}
		return;
	}
	//for (int i = 0; i < LOOP / num_threads; ++i) {
	//	int value = rand() % RANGE;
	//	int op = rand() % 3;
	//	if (op == 0) set.add(value);
	//	else if (op == 1) set.remove(value);
	//	else set.contains(value);
	//}
}

int main()
{
	using namespace std::chrono;
	// 성긴 동기화
	{
		std::cout << "=========================성긴 동기화===============================" << std::endl;
		for (int num_thread = MAX_THREADS; num_thread >= 1; num_thread /= 2) {
			c_set.clear();
			std::vector<std::thread> threads;
			auto start = high_resolution_clock::now();
			for (int i = 0; i < num_thread; ++i)
				threads.emplace_back(benchmark, num_thread, 1);
			for (auto& th : threads)
				th.join();
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(stop - start);
			std::cout << "Threads: " << num_thread
				<< ", Duration: " << duration.count() << " ms.\n";
			std::cout << "Set: "; c_set.print20();
		}
	}

	// 세밀한 동기화
	{
		std::cout << "=========================세밀한 동기화===============================" << std::endl;
		for (int num_thread = MAX_THREADS; num_thread >= 1; num_thread /= 2) {
			f_set.clear();
			std::vector<std::thread> threads;
			auto start = high_resolution_clock::now();
			for (int i = 0; i < num_thread; ++i)
				threads.emplace_back(benchmark, num_thread, 2);
			for (auto& th : threads)
				th.join();
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(stop - start);
			std::cout << "Threads: " << num_thread
				<< ", Duration: " << duration.count() << " ms.\n";
			std::cout << "Set: "; f_set.print20();
		}
	}
	// 낙천적 동기화
	{
		std::cout << "=========================낙천적 동기화===============================" << std::endl;
		for (int num_thread = MAX_THREADS; num_thread >= 1; num_thread /= 2) {
			o_set.clear();
			std::vector<std::thread> threads;
			auto start = high_resolution_clock::now();
			for (int i = 0; i < num_thread; ++i)
				threads.emplace_back(benchmark, num_thread, 3);
			for (auto& th : threads)
				th.join();
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(stop - start);
			std::cout << "Threads: " << num_thread
				<< ", Duration: " << duration.count() << " ms.\n";
			std::cout << "Set: "; o_set.print20();
		}
	}
	{
		// Consistency check
		//std::cout << "\n\nConsistency Check\n";

		//for (int num_thread = MAX_THREADS; num_thread >= 1; num_thread /= 2) {
		//	set.clear();
		//	std::vector<std::thread> threads;
		//	for (int i = 0; i < MAX_THREADS; ++i)
		//		history[i].clear();
		//	auto start = high_resolution_clock::now();
		//	for (int i = 0; i < num_thread; ++i)
		//		threads.emplace_back(benchmark_check, num_thread, i);
		//	for (auto& th : threads)
		//		th.join();
		//	auto stop = high_resolution_clock::now();
		//	auto duration = duration_cast<milliseconds>(stop - start);
		//	std::cout << "Threads: " << num_thread
		//		<< ", Duration: " << duration.count() << " ms.\n";
		//	std::cout << "Set: "; set.print20();
		//	check_history(num_thread);
		//}
	}
}