#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <array>
#include <mutex>
#include <set>
#include <queue>

using namespace std;
using namespace chrono;

constexpr int MAX_THREADS = 16;


class NODE {
	std::mutex  nl;
public:
	int v;
	NODE* next;
	bool removed;
	int remove_point;
	NODE() : v(-1), next(nullptr), removed(false) {}
	NODE(int x) : v(x), next(nullptr), removed(false) {}
	void Lock()
	{
		nl.lock();
	}
	void Unlock()
	{
		nl.unlock();
	}
};

class null_mutex
{
public:
	void lock() {}
	void unlock() {}
};


class SET {
	NODE head, tail;
	null_mutex ll;
public:
	SET()
	{
		head.v = 0x80000000;
		tail.v = 0x7FFFFFFF;
		head.next = &tail;
	}
	bool ADD(int x)
	{
		NODE* prev = &head;
		ll.lock();
		NODE* curr = prev->next;
		while (curr->v < x) {
			prev = curr;
			curr = curr->next;
		}
		if (curr->v != x) {
			NODE* node = new NODE{ x };
			node->next = curr;
			prev->next = node;
			ll.unlock();
			return true;
		}
		else
		{
			ll.unlock();
			return false;
		}
	}

	bool REMOVE(int x)
	{
		NODE* prev = &head;
		ll.lock();
		NODE* curr = prev->next;
		while (curr->v < x) {
			prev = curr;
			curr = curr->next;
		}
		if (curr->v != x) {
			ll.unlock();
			return false;
		}
		else {
			prev->next = curr->next;
			delete curr;
			ll.unlock();
			return true;
		}
	}

	bool CONTAINS(int x)
	{
		NODE* prev = &head;
		ll.lock();
		NODE* curr = prev->next;
		while (curr->v < x) {
			prev = curr;
			curr = curr->next;
		}
		bool res = (curr->v == x);
		ll.unlock();
		return res;
	}
	void print20()
	{
		NODE* p = head.next;
		for (int i = 0; i < 20; ++i) {
			if (p == &tail) break;
			cout << p->v << ", ";
			p = p->next;
		}
		cout << endl;
	}

	void clear()
	{
		NODE* p = head.next;
		while (p != &tail) {
			NODE* t = p;
			p = p->next;
			delete t;
		}
		head.next = &tail;
	}
};

class STD_SET {
	std::set <int> std_set;
public:
	STD_SET()
	{
	}
	bool ADD(int x)
	{
		if (std_set.count(x) == 1)
			return false;
		std_set.insert(x);
		return true;
	}

	bool REMOVE(int x)
	{
		if (std_set.count(x) == 0)
			return false;
		std_set.erase(x);
		return true;
	}

	bool CONTAINS(int x)
	{
		return std_set.count(x) == 1;
	}
	void print20()
	{
		int count = 20;
		for (auto x : std_set) {
			cout << x << ", ";
			if (--count == 0) break;
		}
		cout << endl;
	}

	void clear()
	{
		std_set.clear();
	}
};

class F_SET {
	NODE head, tail;
public:
	F_SET()
	{
		head.v = 0x80000000;
		tail.v = 0x7FFFFFFF;
		head.next = &tail;
	}
	bool ADD(int x)
	{
		NODE* prev = &head;
		prev->Lock();
		NODE* curr = prev->next;
		curr->Lock();
		while (curr->v < x) {
			prev->Unlock();
			prev = curr;
			curr = curr->next;
			curr->Lock();
		}
		if (curr->v != x) {
			NODE* node = new NODE{ x };
			node->next = curr;
			prev->next = node;
			prev->Unlock(); curr->Unlock();
			return true;
		}
		else
		{
			prev->Unlock(); curr->Unlock();
			return false;
		}
	}

	bool REMOVE(int x)
	{
		NODE* prev = &head;
		prev->Lock();
		NODE* curr = prev->next;
		curr->Lock();
		while (curr->v < x) {
			prev->Unlock();
			prev = curr;
			curr = curr->next;
			curr->Lock();
		}
		if (curr->v != x) {
			prev->Unlock(); curr->Unlock();
			return false;
		}
		else {
			prev->next = curr->next;
			prev->Unlock(); curr->Unlock();
			delete curr;
			return true;
		}
	}

	bool CONTAINS(int x)
	{
		NODE* prev = &head;
		prev->Lock();
		NODE* curr = prev->next;
		curr->Lock();
		while (curr->v < x) {
			prev->Unlock();
			prev = curr;
			curr = curr->next;
			curr->Lock();
		}
		bool res = (curr->v == x);
		prev->Unlock(); curr->Unlock();
		return res;
	}
	void print20()
	{
		NODE* p = head.next;
		for (int i = 0; i < 20; ++i) {
			if (p == &tail) break;
			cout << p->v << ", ";
			p = p->next;
		}
		cout << endl;
	}

	void clear()
	{
		NODE* p = head.next;
		while (p != &tail) {
			NODE* t = p;
			p = p->next;
			delete t;
		}
		head.next = &tail;
	}
};

class O_SET {
	NODE head, tail;
public:
	O_SET()
	{
		head.v = 0x80000000;
		tail.v = 0x7FFFFFFF;
		head.next = &tail;
	}

	bool validate(const NODE* prev, const NODE* curr)
	{
		NODE* ptr = &head;
		while (ptr->v <= prev->v) {
			if (ptr == prev)
				return ptr->next == curr;
			ptr = ptr->next;
		}
		return false;
	}

	bool ADD(int x)
	{
		while (true) {
			NODE* prev = &head;
			NODE* curr = prev->next;
			while (curr->v < x) {
				prev = curr;
				curr = curr->next;
			}
			prev->Lock(); curr->Lock();
			if (true == validate(prev, curr)) {
				if (curr->v != x) {
					NODE* node = new NODE{ x };
					node->next = curr;
					prev->next = node;
					prev->Unlock(); curr->Unlock();
					return true;
				}
				else
				{
					prev->Unlock(); curr->Unlock();
					return false;
				}
			}
			else {
				prev->Unlock(); curr->Unlock();
			}
		}
	}

	bool REMOVE(int x)
	{
		while (true) {
			NODE* prev = &head;
			NODE* curr = prev->next;
			while (curr->v < x) {
				prev = curr;
				curr = curr->next;
			}
			prev->Lock(); curr->Lock();
			if (true == validate(prev, curr)) {
				if (curr->v != x) {
					prev->Unlock(); curr->Unlock();
					return false;
				}
				else {
					prev->next = curr->next;
					prev->Unlock(); curr->Unlock();
					// delete curr;
					return true;
				}
			}
			else {
				prev->Unlock(); curr->Unlock();
			}
		}
	}

	bool CONTAINS(int x)
	{
		while (true) {
			NODE* prev = &head;
			NODE* curr = prev->next;
			while (curr->v < x) {
				prev = curr;
				curr = curr->next;
			}
			prev->Lock(); curr->Lock();
			if (true == validate(prev, curr)) {
				bool res = (curr->v == x);
				prev->Unlock(); curr->Unlock();
				return res;
			}
			else {
				prev->Unlock(); curr->Unlock();
			}
		}
	}

	void print20()
	{
		NODE* p = head.next;
		for (int i = 0; i < 20; ++i) {
			if (p == &tail) break;
			cout << p->v << ", ";
			p = p->next;
		}
		cout << endl;
	}

	void clear()
	{
		NODE* p = head.next;
		while (p != &tail) {
			NODE* t = p;
			p = p->next;
			delete t;
		}
		head.next = &tail;
	}
};

class Z_SET {
	NODE head, tail;
public:
	Z_SET()
	{
		head.v = 0x80000000;
		tail.v = 0x7FFFFFFF;
		head.next = &tail;
	}

	bool validate(const NODE* prev, const NODE* curr)
	{
		return (false == prev->removed)
			&& (false == curr->removed)
			&& (prev->next == curr);
	}

	bool ADD(int x)
	{
		while (true) {
			NODE* prev = &head;
			NODE* curr = prev->next;
			while (curr->v < x) {
				prev = curr;
				curr = curr->next;
			}
			prev->Lock(); curr->Lock();
			if (true == validate(prev, curr)) {
				if (curr->v != x) {
					NODE* node = new NODE{ x };
					node->next = curr;
					prev->next = node;
					prev->Unlock(); curr->Unlock();
					return true;
				}
				else
				{
					prev->Unlock(); curr->Unlock();
					return false;
				}
			}
			else {
				prev->Unlock(); curr->Unlock();
			}
		}
	}

	bool REMOVE(int x)
	{
		while (true) {
			NODE* prev = &head;
			NODE* curr = prev->next;
			while (curr->v < x) {
				prev = curr;
				curr = curr->next;
			}
			prev->Lock(); curr->Lock();
			if (true == validate(prev, curr)) {
				if (curr->v != x) {
					prev->Unlock(); curr->Unlock();
					return false;
				}
				else {
					curr->removed = true;
					prev->next = curr->next;
					prev->Unlock(); curr->Unlock();
					// delete curr;
					return true;
				}
			}
			else {
				prev->Unlock(); curr->Unlock();
			}
		}
	}

	bool CONTAINS(int x)
	{
		NODE* curr = head.next;
		while (curr->v < x) {
			curr = curr->next;
		}
		return (curr->v == x) && (false == curr->removed);
	}

	void print20()
	{
		NODE* p = head.next;
		for (int i = 0; i < 20; ++i) {
			if (p == &tail) break;
			cout << p->v << ", ";
			p = p->next;
		}
		cout << endl;
	}

	void clear()
	{
		NODE* p = head.next;
		while (p != &tail) {
			NODE* t = p;
			p = p->next;
			delete t;
		}
		head.next = &tail;
	}
};

class SP_NODE {
	std::mutex  nl;
public:
	int v;
	shared_ptr<SP_NODE> next;
	bool removed;
	SP_NODE() : v(-1), next(nullptr), removed(false) {}
	SP_NODE(int x) : v(x), next(nullptr), removed(false) {}
	void Lock()
	{
		nl.lock();
	}
	void Unlock()
	{
		nl.unlock();
	}
};
class ZSP_SET {
	std::shared_ptr <SP_NODE> head, tail;
public:
	ZSP_SET()
	{
		head = make_shared<SP_NODE>(0x80000000);
		tail = make_shared<SP_NODE>(0x7FFFFFFF);
		head->next = tail;
	}

	bool validate(const shared_ptr<SP_NODE>& prev,
		const shared_ptr<SP_NODE>& curr)
	{
		return (false == prev->removed)
			&& (false == curr->removed)
			&& (prev->next == curr);
	}

	bool ADD(int x)
	{
		while (true) {
			shared_ptr<SP_NODE> prev = head;
			shared_ptr<SP_NODE> curr = prev->next;
			while (curr->v < x) {
				prev = curr;
				curr = curr->next;
			}
			prev->Lock(); curr->Lock();
			if (true == validate(prev, curr)) {
				if (curr->v != x) {
					shared_ptr<SP_NODE> node = make_shared<SP_NODE>(x);
					node->next = curr;
					prev->next = node;
					prev->Unlock(); curr->Unlock();
					return true;
				}
				else
				{
					prev->Unlock(); curr->Unlock();
					return false;
				}
			}
			else {
				prev->Unlock(); curr->Unlock();
			}
		}
	}

	bool REMOVE(int x)
	{
		while (true) {
			shared_ptr<SP_NODE> prev = head;
			shared_ptr<SP_NODE> curr = prev->next;
			while (curr->v < x) {
				prev = curr;
				curr = curr->next;
			}
			prev->Lock(); curr->Lock();
			if (true == validate(prev, curr)) {
				if (curr->v != x) {
					prev->Unlock(); curr->Unlock();
					return false;
				}
				else {
					curr->removed = true;
					prev->next = curr->next;
					prev->Unlock(); curr->Unlock();
					return true;
				}
			}
			else {
				prev->Unlock(); curr->Unlock();
			}
		}
	}

	bool CONTAINS(int x)
	{
		shared_ptr<SP_NODE> curr = head->next;
		while (curr->v < x) {
			curr = curr->next;
		}
		return (curr->v == x) && (false == curr->removed);
	}

	void print20()
	{
		shared_ptr<SP_NODE> p = head->next;
		for (int i = 0; i < 20; ++i) {
			if (p == tail) break;
			cout << p->v << ", ";
			p = p->next;
		}
		cout << endl;
	}

	void clear()
	{
		head->next = tail;
	}
};

// Atomic_load와 Atomic_Exchange를 사용한 atomic 구현
//class ZSP_SET2 {
//	std::shared_ptr <SP_NODE> head, tail;
//public:
//	ZSP_SET2()
//	{
//		head = make_shared<SP_NODE>(0x80000000);
//		tail = make_shared<SP_NODE>(0x7FFFFFFF);
//		head->next = tail;
//	}
//
//	bool validate(const shared_ptr<SP_NODE>& prev,
//		const shared_ptr<SP_NODE>& curr)
//	{
//		return (false == prev->removed)
//			&& (false == curr->removed)
//			&& (prev->next == curr);
//	}
//
//	bool ADD(int x)
//	{
//		while (true) {
//			shared_ptr<SP_NODE> prev = head;
//			shared_ptr<SP_NODE> curr = atomic_load(&prev->next);
//			while (curr->v < x) {
//				prev = curr;
//				curr = atomic_load(&curr->next);
//			}
//			prev->Lock(); curr->Lock();
//			if (true == validate(prev, curr)) {
//				if (curr->v != x) {
//					shared_ptr<SP_NODE> node = make_shared<SP_NODE>(x);
//					node->next = curr;
//					atomic_exchange(&prev->next, node);
//					prev->Unlock(); curr->Unlock();
//					return true;
//				}
//				else
//				{
//					prev->Unlock(); curr->Unlock();
//					return false;
//				}
//			}
//			else {
//				prev->Unlock(); curr->Unlock();
//			}
//		}
//	}
//
//	bool REMOVE(int x)
//	{
//		while (true) {
//			shared_ptr<SP_NODE> prev = head;
//			shared_ptr<SP_NODE> curr = atomic_load(&prev->next);
//			while (curr->v < x) {
//				prev = curr;
//				curr = atomic_load(&curr->next);
//			}
//			prev->Lock(); curr->Lock();
//			if (true == validate(prev, curr)) {
//				if (curr->v != x) {
//					prev->Unlock(); curr->Unlock();
//					return false;
//				}
//				else {
//					curr->removed = true;
//					atomic_exchange(&prev->next, atomic_load(&curr->next));
//					prev->Unlock(); curr->Unlock();
//					return true;
//				}
//			}
//			else {
//				prev->Unlock(); curr->Unlock();
//			}
//		}
//	}
//
//	bool CONTAINS(int x)
//	{
//		shared_ptr<SP_NODE> curr = atomic_load(&head->next);
//		while (curr->v < x) {
//			curr = atomic_load(&curr->next);
//		}
//		return (curr->v == x) && (false == curr->removed);
//	}
//
//	void print20()
//	{
//		shared_ptr<SP_NODE> p = head->next;
//		for (int i = 0; i < 20; ++i) {
//			if (p == tail) break;
//			cout << p->v << ", ";
//			p = p->next;
//		}
//		cout << endl;
//	}
//
//	void clear()
//	{
//		head->next = tail;
//	}
//};

// C++20의 atomic<shared_ptr<T>>를 사용한 세밀한 동기화 구현
//class ASP_NODE {
//	std::mutex  nl;
//public:
//	int v;
//	atomic<shared_ptr<ASP_NODE>> next;
//	bool removed;
//	ASP_NODE() : v(-1), next(nullptr), removed(false) {}
//	ASP_NODE(int x) : v(x), next(nullptr), removed(false) {}
//	void Lock()
//	{
//		nl.lock();
//	}
//	void Unlock()
//	{
//		nl.unlock();
//	}
//};
//class ZSP_SET3 {
//	std::atomic<std::shared_ptr <ASP_NODE>> head, tail;
//public:
//	ZSP_SET3()
//	{
//		head = make_shared<ASP_NODE>(0x80000000);
//		tail = make_shared<ASP_NODE>(0x7FFFFFFF);
//		head.load()->next = tail.load();
//	}
//
//	bool validate(const shared_ptr<ASP_NODE>& prev,
//		const shared_ptr<ASP_NODE>& curr)
//	{
//		return (false == prev->removed)
//			&& (false == curr->removed)
//			&& (prev->next.load() == curr);
//	}
//
//	bool ADD(int x)
//	{
//		while (true) {
//			shared_ptr<ASP_NODE> prev = head;
//			shared_ptr<ASP_NODE> curr = prev->next;
//			while (curr->v < x) {
//				prev = curr;
//				curr = curr->next;
//			}
//			prev->Lock(); curr->Lock();
//			if (true == validate(prev, curr)) {
//				if (curr->v != x) {
//					shared_ptr<ASP_NODE> node = make_shared<ASP_NODE>(x);
//					node->next = curr;
//					prev->next = node;
//					prev->Unlock(); curr->Unlock();
//					return true;
//				}
//				else
//				{
//					prev->Unlock(); curr->Unlock();
//					return false;
//				}
//			}
//			else {
//				prev->Unlock(); curr->Unlock();
//			}
//		}
//	}
//
//	bool REMOVE(int x)
//	{
//		while (true) {
//			shared_ptr<ASP_NODE> prev = head;
//			shared_ptr<ASP_NODE> curr = prev->next;
//			while (curr->v < x) {
//				prev = curr;
//				curr = curr->next;
//			}
//			prev->Lock(); curr->Lock();
//			if (true == validate(prev, curr)) {
//				if (curr->v != x) {
//					prev->Unlock(); curr->Unlock();
//					return false;
//				}
//				else {
//					curr->removed = true;
//					prev->next.store(curr->next);
//					prev->Unlock(); curr->Unlock();
//					return true;
//				}
//			}
//			else {
//				prev->Unlock(); curr->Unlock();
//			}
//		}
//	}
//
//	bool CONTAINS(int x)
//	{
//		shared_ptr<ASP_NODE> curr = head.load()->next;
//		while (curr->v < x) {
//			curr = curr->next;
//		}
//		return (curr->v == x) && (false == curr->removed);
//	}
//
//	void print20()
//	{
//		shared_ptr<ASP_NODE> p = head.load()->next;
//		for (int i = 0; i < 20; ++i) {
//			if (p == tail.load()) break;
//			cout << p->v << ", ";
//			p = p->next;
//		}
//		cout << endl;
//	}
//
//	void clear()
//	{
//		head.load()->next = tail.load();
//	}
//};

class LF_NODE;
class STPTR {
	LF_NODE* ptr;		// 주소와 marking이 같이 존재
public:
	STPTR() : ptr(nullptr) {}
	explicit STPTR(LF_NODE* p) : ptr(p) {}
	void setptr(LF_NODE* p)
	{
		ptr = p;
	}
	LF_NODE* getptr()
	{
		long long n = reinterpret_cast<long long>(ptr);
		n = n & 0xFFFFFFFFFFFFFFFE;
		return reinterpret_cast<LF_NODE*>(n);
	}

	bool getptr(LF_NODE*& p)
	{
		long long n = reinterpret_cast<long long>(ptr);
		bool removed = 1 == (n & 1);
		n = n & 0xFFFFFFFFFFFFFFFE;
		p = reinterpret_cast<LF_NODE*>(n);
		return removed;
	}

	bool is_removed()
	{
		long long n = reinterpret_cast<long long>(ptr);
		return 1 == (n & 1);
	}

	bool CAS(LF_NODE* old_p, LF_NODE* new_p, bool old_mark, bool new_mark)
	{
		long long old_v = reinterpret_cast<long long>(old_p);
		if (true == old_mark)
			old_v = old_v | 1;
		long long new_v = reinterpret_cast<long long>(new_p);
		if (true == new_mark)
			new_v = new_v | 1;
		return std::atomic_compare_exchange_strong(
			reinterpret_cast<atomic_llong*>(&ptr),
			&old_v,
			new_v);
	}
};

class LF_NODE {
public:
	int v;
	STPTR next;
	LF_NODE() : v(-1), next(nullptr) {}
	LF_NODE(int x) : v(x), next(nullptr) {}
};

class LF_SET {
	LF_NODE head, tail;
public:
	LF_SET()
	{
		head.v = 0x80000000;
		tail.v = 0x7FFFFFFF;
		head.next.setptr(&tail);
	}

	void FIND(int x, LF_NODE*& prev, LF_NODE*& curr)
	{
	start:
		prev = &head;
		curr = prev->next.getptr();

		while (true) {
			LF_NODE* succ = nullptr;
			bool removed = curr->next.getptr(succ);
			while (true == removed) {
				if (false == prev->next.CAS(curr, succ, false, false))
					goto start;
				curr = succ;
				removed = curr->next.getptr(succ);
			}
			if (curr->v >= x) return;
			prev = curr;
			curr = succ;
		}
	}

	bool ADD(int x)
	{
		while (true) {
			LF_NODE* prev = nullptr;
			LF_NODE* curr = nullptr;
			FIND(x, prev, curr);
			if (curr->v != x) {
				LF_NODE* node = new LF_NODE{ x };
				node->next.setptr(curr);
				if (false == prev->next.CAS(curr, node, false, false)) {
					delete node;
					continue;
				}
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	bool REMOVE(int x)
	{
		while (true) {
			LF_NODE* prev = nullptr;
			LF_NODE* curr = nullptr;
			FIND(x, prev, curr);
			if (curr->v != x) {
				return false;
			}
			else {
				LF_NODE* succ = curr->next.getptr();
				if (false == curr->next.CAS(succ, succ, false, true))
					continue;
				prev->next.CAS(curr, succ, false, false);
				// delete curr;
				return true;
			}
		}
	}


	bool CONTAINS(int x)
	{
		LF_NODE* curr = head.next.getptr();
		while (curr->v < x) {
			curr = curr->next.getptr();
		}
		return (curr->v == x) && (false == curr->next.is_removed());
	}

	void print20()
	{
		LF_NODE* p = head.next.getptr();
		for (int i = 0; i < 20; ++i) {
			if (p == &tail) break;
			cout << p->v << ", ";
			p = p->next.getptr();
		}
		cout << endl;
	}

	void clear()
	{
		LF_NODE* p = head.next.getptr();
		while (p != &tail) {
			LF_NODE* t = p;
			p = p->next.getptr();
			delete t;
		}
		head.next.setptr(&tail);
	}
};

thread_local std::queue<NODE*> freelist;
thread_local int g_th_id;
class EBR {
	atomic_int e_counter;
	atomic_int th_e_counter[MAX_THREADS * 16];
public:
	EBR()
	{
		clear();
	}

	void clear()
	{
	}

	void start()
	{
		int my_epoch = e_counter++;
		th_e_counter[g_th_id * 16] = my_epoch;
	}

	void end()
	{
		th_e_counter[g_th_id * 16] = 0;
	}

	void remove(NODE* p)
	{
		int max_epoch = 0;
		for (int i = 0; i < MAX_THREADS; ++i)
			max_epoch = max(max_epoch, static_cast<int>(th_e_counter[i * 16]));
		p->remove_point = max_epoch;
		freelist.push(p);
	}

	NODE* new_node(int x)
	{
		if (freelist.empty()) return new NODE(x);
		NODE* n = freelist.front();

		int curr_epoch = 0x7FFFFFFF;
		for (int i = 0; i < MAX_THREADS; ++i) {
			int ep = th_e_counter[i * 16];
			if (0 != ep)
				curr_epoch = min(curr_epoch, ep);
		}

		if (n->remove_point >= curr_epoch) return new NODE(x);
		freelist.pop();
		n->v = x;
		n->removed = false;
		n->next = nullptr;
		return n;
	}

};

EBR ebr;

class EBR_Z_SET {
	NODE head, tail;
public:
	EBR_Z_SET()
	{
		head.v = 0x80000000;
		tail.v = 0x7FFFFFFF;
		head.next = &tail;
	}

	bool validate(const NODE* prev, const NODE* curr)
	{
		return (false == prev->removed)
			&& (false == curr->removed)
			&& (prev->next == curr);
	}

	bool ADD(int x)
	{
		ebr.start();
		while (true) {
			NODE* prev = &head;
			NODE* curr = prev->next;
			while (curr->v < x) {
				prev = curr;
				curr = curr->next;
			}
			prev->Lock(); curr->Lock();
			if (true == validate(prev, curr)) {
				if (curr->v != x) {
					NODE* node = ebr.new_node(x);
					node->next = curr;
					prev->next = node;
					prev->Unlock(); curr->Unlock();
					ebr.end();
					return true;
				}
				else
				{
					prev->Unlock(); curr->Unlock();
					ebr.end();
					return false;
				}
			}
			else {
				prev->Unlock(); curr->Unlock();
			}
		}
	}

	bool REMOVE(int x)
	{
		ebr.start();
		while (true) {
			NODE* prev = &head;
			NODE* curr = prev->next;
			while (curr->v < x) {
				prev = curr;
				curr = curr->next;
			}
			prev->Lock(); curr->Lock();
			if (true == validate(prev, curr)) {
				if (curr->v != x) {
					prev->Unlock(); curr->Unlock();
					ebr.end();
					return false;
				}
				else {
					curr->removed = true;
					prev->next = curr->next;
					prev->Unlock(); curr->Unlock();
					ebr.remove(curr);
					ebr.end();
					return true;
				}
			}
			else {
				prev->Unlock(); curr->Unlock();
			}
		}
	}

	bool CONTAINS(int x)
	{
		ebr.start();
		NODE* curr = head.next;
		while (curr->v < x) {
			curr = curr->next;
		}
		bool result = (curr->v == x) && (false == curr->removed);
		ebr.end();
		return result;
	}

	void print20()
	{
		NODE* p = head.next;
		for (int i = 0; i < 20; ++i) {
			if (p == &tail) break;
			cout << p->v << ", ";
			p = p->next;
		}
		cout << endl;
	}

	void clear()
	{
		NODE* p = head.next;
		while (p != &tail) {
			NODE* t = p;
			p = p->next;
			delete t;
		}
		head.next = &tail;

		ebr.clear();
	}
};

///////////////
enum METHOD_TYPE { OP_ADD, OP_REMOVE, OP_CONTAINS, OP_CLEAR, OP_EMPTY, OP_GET20 };

struct Invocation {
	METHOD_TYPE  m_op;
	int		v;
};

struct Response {
	bool res;
	std::vector<int> get20;
};

class SeqObject_set {
public:
	std::set<int> m_set;
	Response apply(Invocation& inv)
	{
		Response res;
		switch (inv.m_op) {
		case OP_ADD: res.res = (m_set.count(inv.v) == 0);
			if (res.res == true) m_set.insert(inv.v);
			break;

		case OP_REMOVE: res.res = (m_set.count(inv.v) == 1);
			if (res.res == true) m_set.erase(inv.v);
			break;
		case OP_CONTAINS: res.res = (m_set.count(inv.v) == 1);
			break;
		case OP_CLEAR:m_set.clear();
			break;
		case OP_EMPTY: res.res = m_set.empty();
			break;
		case OP_GET20: {
			int count = 20;
			for (auto v : m_set) {
				res.get20.push_back(v);
				if (count-- == 0) break;
			}
			break;
		}
		default:
			cout << "Unknown Method!!\n";
			exit(-1);
		}
		return res;
	}
};

struct UNODE;
class CONSENSUS {
	UNODE* ptr;
public:
	CONSENSUS() : ptr(nullptr) {}
	void CAS(UNODE* old_p, UNODE* new_p)
	{
		atomic_compare_exchange_strong(
			reinterpret_cast<atomic_llong*>(&ptr),
			reinterpret_cast<long long*>(&old_p),
			reinterpret_cast<long long>(new_p));
	}
	UNODE* decide(UNODE* v)
	{
		CAS(nullptr, v);
		return ptr;
	}
	void clear()
	{
		ptr = nullptr;
	}
};

struct UNODE
{
	Invocation inv;
	UNODE* next;
	CONSENSUS decide_next;
	volatile int seq;
	UNODE() : next(nullptr), seq(0) {}
	UNODE(Invocation& v) : next(nullptr), inv(v), seq(0) {}
};

class STD_SET2 {
	SeqObject_set std_set;
public:
	bool ADD(int x)
	{
		Invocation inv{ OP_ADD, x };
		Response a = std_set.apply(inv);
		return a.res;
	}

	bool REMOVE(int x)
	{
		Invocation inv{ OP_REMOVE, x };
		Response a = std_set.apply(inv);
		return a.res;
	}

	bool CONTAINS(int x)
	{
		Invocation inv{ OP_CONTAINS, x };
		Response a = std_set.apply(inv);
		return a.res;
	}

	void print20()
	{
		Invocation inv{ OP_GET20, 0 };
		Response a = std_set.apply(inv);
		for (auto v : a.get20)
			cout << v << ", ";
		cout << endl;
	}

	void clear()
	{
		Invocation inv{ OP_CLEAR, 0 };
		Response a = std_set.apply(inv);
	}
};
///////////////

thread_local int tl_id;

int Thread_id()
{
	return tl_id;
}

class LF_UNIV_STD_SET {
private:
	UNODE* head[MAX_THREADS];
	UNODE tail;
public:
	LF_UNIV_STD_SET() {
		tail.seq = 0;
		for (int i = 0; i < MAX_THREADS; ++i) head[i] = &tail;
	}
	void clear() {
		tail.seq = 0;
		for (int i = 0; i < MAX_THREADS; ++i) head[i] = &tail;
		auto p = tail.next;
		while (p != nullptr) {
			auto pp = p->next;
			delete p;
			p = pp;
		}
		tail.next = nullptr;
		tail.decide_next.clear();
	}
	UNODE* max_node()
	{
		UNODE* max_n = head[0];
		for (int i = 1; i < MAX_THREADS; ++i)
			if (head[i]->seq > max_n->seq) max_n = head[i];
		return max_n;
	}
	Response apply(Invocation invoc) {
		int  i = Thread_id();
		UNODE* prefer = new UNODE(invoc);
		while (prefer->seq == 0) {
			UNODE* before = max_node();
			UNODE* after = before->decide_next.decide(prefer);
			before->next = after; after->seq = before->seq + 1;
			head[i] = after;
		}
		SeqObject_set myObject;
		UNODE* current = tail.next;
		while (current != prefer) {
			myObject.apply(current->inv);
			current = current->next;
		}
		return myObject.apply(current->inv);
	}
};

class LF_STD_SET {
	LF_UNIV_STD_SET std_set;
public:
	bool ADD(int x)
	{
		Invocation inv{ OP_ADD, x };
		Response a = std_set.apply(inv);
		return a.res;
	}

	bool REMOVE(int x)
	{
		Invocation inv{ OP_REMOVE, x };
		Response a = std_set.apply(inv);
		return a.res;
	}

	bool CONTAINS(int x)
	{
		Invocation inv{ OP_CONTAINS, x };
		Response a = std_set.apply(inv);
		return a.res;
	}

	void print20()
	{
		Invocation inv{ OP_GET20, 0 };
		Response a = std_set.apply(inv);
		for (auto v : a.get20)
			cout << v << ", ";
		cout << endl;
	}

	void clear()
	{
		// Invocation inv{ OP_CLEAR, 0 };
		// Response a = std_set.apply(inv);
		std_set.clear();
	}
};

constexpr int TOP_LEVEL = 9;

class SKNODE {
public:
	int v;
	SKNODE* volatile next[TOP_LEVEL + 1];
	volatile bool fullylinked;              // 게으른 동기화
	volatile bool removed;					// 게으른 동기화
	recursive_mutex nlock;					// 게으른 동기화
	int top_level;
	SKNODE() : v(-1), top_level(0), fullylinked(false), removed(false)
	{
		for (auto& n : next) n = nullptr;
	}
	SKNODE(int x, int top) : v(x), top_level(top), fullylinked(false), removed(false)
	{
		for (auto& n : next) n = nullptr;
	}
};

class C_SKSET {
	SKNODE head{ int(0x80000000), TOP_LEVEL };
	SKNODE tail{ int(0x7FFFFFFF), TOP_LEVEL };
	mutex ll;
public:
	C_SKSET()
	{
		for (auto& n : head.next)
			n = &tail;
	}

	void Find(int x, SKNODE* prev[], SKNODE* curr[])
	{
		for (int cl = TOP_LEVEL; cl >= 0; --cl) {
			if (cl == TOP_LEVEL)
				prev[cl] = &head;
			else
				prev[cl] = prev[cl + 1];
			curr[cl] = prev[cl]->next[cl];
			while (curr[cl]->v < x) {
				prev[cl] = curr[cl];
				curr[cl] = curr[cl]->next[cl];
			}
		}
	}

	bool ADD(int x)
	{
		SKNODE* prev[TOP_LEVEL + 1], * curr[TOP_LEVEL + 1];
		ll.lock();
		Find(x, prev, curr);
		if (curr[0]->v != x) {
			int level = 0;
			for (level = 0; level < TOP_LEVEL; ++level)
				if (rand() % 2 == 1) break;
			SKNODE* node = new SKNODE{ x, level };
			for (int i = 0; i <= level; ++i) {
				node->next[i] = curr[i];
				prev[i]->next[i] = node;
			}
			ll.unlock();
			return true;
		}
		else
		{
			ll.unlock();
			return false;
		}
	}

	bool REMOVE(int x)
	{
		SKNODE* prev[TOP_LEVEL + 1], * curr[TOP_LEVEL + 1];
		ll.lock();
		Find(x, prev, curr);
		if (curr[0]->v == x) {
			for (int i = 0; i <= curr[0]->top_level; ++i) {
				prev[i]->next[i] = curr[i]->next[i];
			}
			ll.unlock();
			delete curr[0];
			return true;
		}
		else
		{
			ll.unlock();
			return false;
		}
	}

	bool CONTAINS(int x)
	{
		SKNODE* prev[TOP_LEVEL + 1], * curr[TOP_LEVEL + 1];
		ll.lock();
		Find(x, prev, curr);
		if (curr[0]->v != x) {
			ll.unlock();
			return false;
		}
		else
		{
			ll.unlock();
			return true;
		}
	}
	void print20()
	{
		SKNODE* p = head.next[0];
		for (int i = 0; i < 20; ++i) {
			if (p == &tail) break;
			cout << p->v << ", ";
			p = p->next[0];
		}
		cout << endl;
	}

	void clear()
	{
		SKNODE* p = head.next[0];
		while (p != &tail) {
			SKNODE* t = p;
			p = p->next[0];
			delete t;
		}
		for (auto& n : head.next)
			n = &tail;
	}
};

class L_SKLIST {
	SKNODE head, tail;
public:
	L_SKLIST()
	{
		head.v = 0x80000000;
		tail.v = 0x7FFFFFFF;
		for (auto& n : head.next)
			n = &tail;
		head.fullylinked = tail.fullylinked = true;
	}
	~L_SKLIST()
	{
		clear();
	}

	int Find(int x, SKNODE* prev[TOP_LEVEL + 1], SKNODE* curr[TOP_LEVEL + 1])
	{
		int f_level = -1;
		prev[TOP_LEVEL] = &head;
		for (int cl = TOP_LEVEL; cl >= 0; --cl) {
			if (cl != TOP_LEVEL)
				prev[cl] = prev[cl + 1];
			curr[cl] = prev[cl]->next[cl];
			while (curr[cl]->v < x) {
				prev[cl] = curr[cl];
				curr[cl] = curr[cl]->next[cl];
			}
			if ((curr[cl]->v == x) && (f_level == -1))
				f_level = cl;
		}
		return f_level;
	}

	bool ADD(int x)
	{
		SKNODE* prev[TOP_LEVEL + 1], * curr[TOP_LEVEL + 1];

		while (true) {
			int f_level = Find(x, prev, curr);

			if (f_level != -1) {
				if (curr[f_level]->removed == true)
					continue;
				while (curr[f_level]->fullylinked == false);
				return false;;
			}

			int top_level = 0;
			while (rand() % 2 == 1) {
				top_level++;
				if (TOP_LEVEL == top_level)
					break;
			}

			bool valid = true;
			int locked_level = -1;
			for (int i = 0; i <= top_level; ++i) {
				prev[i]->nlock.lock();
				locked_level = i;
				if ((prev[i]->removed == true) || (curr[i]->removed == true) || (prev[i]->next[i] != curr[i])) {
					valid = false;
					break;
				}
			}

			if (false == valid) {
				for (int i = 0; i <= locked_level; ++i)
					prev[i]->nlock.unlock();
				continue;
			}

			SKNODE* new_node = new SKNODE{ x, top_level };

			for (int i = 0; i <= top_level; ++i)
				new_node->next[i] = curr[i];
			for (int i = 0; i <= top_level; ++i)
				prev[i]->next[i] = new_node;
			new_node->fullylinked = true;
			for (int i = 0; i <= top_level; ++i)
				prev[i]->nlock.unlock();
			return true;
		}
	}

	bool REMOVE(int x)
	{
		SKNODE* prev[TOP_LEVEL + 1], * curr[TOP_LEVEL + 1];

		int f_level = Find(x, prev, curr);
		if (f_level == -1) return false;

		SKNODE* victim = curr[f_level];

		if (victim->fullylinked == false) return false;
		if (victim->removed == true) return false;
		if (victim->top_level != f_level) return false;

		victim->nlock.lock();
		if (victim->removed == false)
			victim->removed = true;
		else {
			victim->nlock.unlock();
			return false;
		}

		int top_level = victim->top_level;

		while (true) {
			// Locking
			bool invalid = false;
			int locked_top = 0;
			for (int i = 0; i <= top_level; i++) {
				prev[i]->nlock.lock();
				if ((prev[i]->removed == true) || (prev[i]->next[i] != victim)) {
					invalid = true;
					locked_top = i;
					break;
				}
			}
			if (true == invalid) {
				for (int i = 0; i <= locked_top; ++i)
					prev[i]->nlock.unlock();
				Find(x, prev, curr);
				continue;
			}
			for (int i = top_level; i >= 0; --i)
				prev[i]->next[i] = victim->next[i];

			for (int i = top_level; i >= 0; --i)
				prev[i]->nlock.unlock();
			victim->nlock.unlock();
			return true;
		}
	}

	bool CONTAINS(int x)
	{
		SKNODE* prev[TOP_LEVEL + 1], * curr[TOP_LEVEL + 1];
		int f_level = Find(x, prev, curr);
		if (f_level == -1) return false;
		SKNODE* target = curr[f_level];
		return (target->fullylinked == true)
			&& (target->removed == false);
	}

	void print20()
	{
		SKNODE* p = head.next[0];
		for (int i = 0; i < 20; ++i) {
			if (p == &tail) break;
			cout << p->v << ", ";
			p = p->next[0];
		}
		cout << endl;
	}

	void clear()
	{
		SKNODE* p = head.next[0];
		while (p != &tail) {
			SKNODE* t = p;
			p = p->next[0];
			delete t;
		}
		for (auto& n : head.next)
			n = &tail;
	}
};


class LFSKNODE;
class STPTRSK {
	LFSKNODE* ptr;		// 주소와 marking이 같이 존재
public:
	STPTRSK() : ptr(nullptr) {}
	explicit STPTRSK(LFSKNODE* p) : ptr(p) {}
	void setptr(LFSKNODE* p)
	{
		ptr = p;
	}
	void setptr(LFSKNODE* node, bool mark)
	{
		long long addr = reinterpret_cast<long long>(node);
		if (mark)
			addr = addr | 0x1;
		else
			addr = addr & 0xFFFFFFFFFFFFFFFE;
		ptr = reinterpret_cast<LFSKNODE*>(addr);
	}

	LFSKNODE* getptr()
	{
		long long n = reinterpret_cast<long long>(ptr);
		n = n & 0xFFFFFFFFFFFFFFFE;
		return reinterpret_cast<LFSKNODE*>(n);
	}

	bool getptr(LFSKNODE*& p)
	{
		long long n = reinterpret_cast<long long>(ptr);
		bool removed = 1 == (n & 1);
		n = n & 0xFFFFFFFFFFFFFFFE;
		p = reinterpret_cast<LFSKNODE*>(n);
		return removed;
	}

	bool is_removed()
	{
		long long n = reinterpret_cast<long long>(ptr);
		return 1 == (n & 1);
	}

	bool CAS(LFSKNODE* old_p, LFSKNODE* new_p, bool old_mark, bool new_mark)
	{
		long long old_v = reinterpret_cast<long long>(old_p);
		if (true == old_mark)
			old_v = old_v | 1;
		long long new_v = reinterpret_cast<long long>(new_p);
		if (true == new_mark)
			new_v = new_v | 1;
		return std::atomic_compare_exchange_strong(
			reinterpret_cast<atomic_llong*>(&ptr),
			&old_v,
			new_v);
	}
};

class LFSKNODE {
public:
	int v;
	STPTRSK next[TOP_LEVEL + 1];
	int top_level;
	LFSKNODE() : v(-1), top_level(0)
	{
		for (auto& n : next) {
			n.setptr(nullptr);
		}
	}
	LFSKNODE(int x, int top) : v(x), top_level(top)
	{
		for (auto& n : next) {
			n.setptr(nullptr);
		}
	}
};


class LFSKLIST {
	LFSKNODE head { int(0x80000000), TOP_LEVEL };
	LFSKNODE tail { int(0x7FFFFFFF), TOP_LEVEL };

public:
	LFSKLIST()
	{
		for (auto& n : head.next)
			n.setptr(&tail);
	}

	bool Find(int x, LFSKNODE* prev[], LFSKNODE* curr[])
	{
		int bottomLevel = 0;
		bool marked = false;
		bool snip;
		LFSKNODE* pred = nullptr;
		LFSKNODE* cur = nullptr;
		LFSKNODE* succ = nullptr;
	retry:
		while (true)
		{
			pred = &head;
			for (int level = TOP_LEVEL; level >= bottomLevel; level--)
			{
				cur = pred->next[level].getptr();
				while (true)
				{
					succ = cur->next[level].getptr();
					while (true == cur->next[0].is_removed())
					{
						snip = pred->next[level].CAS(cur, succ, false, false);
						if (!snip) goto retry;
						cur = pred->next[level].getptr();
						succ = cur->next[level].getptr();
					}
					if(cur->v < x)
					{
						pred = cur;
						cur = succ;
					}
					else
					{
						break;
					}
				}
				prev[level] = pred;
				curr[level] = cur;
			}
			return (cur->v == x);
		}
	}

	bool ADD(int x)
	{
		LFSKNODE* prev[TOP_LEVEL + 1], * curr[TOP_LEVEL + 1];
		int bottomLevel = 0;
		int topLevel = 0;
		
		for (topLevel = 0; topLevel < TOP_LEVEL; ++topLevel)
			if (rand() % 2 == 1) break;
		while(true){
			bool found = Find(x, prev, curr);
			if (found) return false;
			else
			{
				LFSKNODE* node = new LFSKNODE{ x, topLevel };
				for (int i = bottomLevel; i <= topLevel; ++i) {
					LFSKNODE* succ = curr[i];
					node->next[i].setptr(succ, false);
				}
				LFSKNODE* pred = prev[bottomLevel];
				LFSKNODE* succ = curr[bottomLevel];
				node->next[bottomLevel].setptr(succ, false);
				if (false == pred->next[bottomLevel].CAS(succ, node, false, false))
				{
					continue;
				}
				for(int i = bottomLevel + 1; i <= topLevel; ++i)
				{
					while(true)
					{
						pred = prev[i];
						succ = curr[i];
						if(pred->next[i].CAS(succ, node, false, false))
						{
							break;
						}
						Find(x, prev, curr);
					}
				}
				return true;
			}
		}
		

	}

	bool REMOVE(int x)
	{
		int bottomLevel = 0;
		LFSKNODE* prev[TOP_LEVEL + 1], *curr[TOP_LEVEL + 1];
		STPTRSK succ;
		while (true)
		{
			bool found = Find(x, prev, curr);
			if (!found) return false;
			else
			{
				LFSKNODE* node = curr[bottomLevel];
				for (int i = node->top_level; i >= bottomLevel + 1; --i)
				{
					succ = node->next[i];
					while(false == succ.is_removed())
					{
						node->next[i].CAS(succ.getptr(), succ.getptr(), false, true);
						succ = node->next[i];
					}
				}
				succ = node->next[bottomLevel];
				while (true)
				{
					bool snip = node->next[bottomLevel].CAS(succ.getptr(), succ.getptr(), false, true);
					succ = curr[bottomLevel]->next[bottomLevel];
					if(snip)
					{
						Find(x, prev, curr);
						return true;
					}
					else if (succ.is_removed())
					{
						return false;
					}
				}
			}
		}
		
	}



	bool CONTAINS(int x)
	{
		int bottomLevel = 0;
		bool marked = false;
		LFSKNODE* pred = &head;
		LFSKNODE* curr = nullptr;
		LFSKNODE* succ = nullptr;

		for(int level = TOP_LEVEL; level >= bottomLevel; level--)
		{
			curr = pred->next[level].getptr();
			while(true)
			{
				succ = curr->next[level].getptr();
				while(true == curr->next[0].is_removed())
				{
					curr = curr->next[level].getptr();
					succ = curr->next[level].getptr();
				}
				if(curr->v < x)
				{
					pred = curr;
					curr = succ;
				}
				else
				{
					break;
				}
			}
		}
		return (curr->v == x);

	}
	void print20()
	{
		LFSKNODE* p = head.next[0].getptr();
		for(int i = 0; i < 20; ++i) {
			if (p == &tail) break;
			cout << p->v << ", ";
			p = p->next[0].getptr();
		}
	}

	void clear()
	{
		LFSKNODE* p = head.next[0].getptr();
		while (p != &tail) {
			LFSKNODE* t = p;
			p = p->next[0].getptr();
			delete t;
		}
		for(auto& n : head.next)
			n.setptr(&tail);
	}
};

//SET my_set;   // 성긴 동기화
//F_SET my_set;	// 세밀한 동기화
//O_SET my_set;	// 세밀한 동기화
//Z_SET my_set;	// 세밀한 동기화
//ZSP_SET my_set;
//ZSP_SET2 my_set;
//ZSP_SET3 my_set;
//LF_SET my_set;
//STD_SET my_set;
EBR_Z_SET my_set;
//STD_SET2 my_set;
//LF_STD_SET my_set;
//L_SKLIST my_set;
//C_SKSET my_set;

//LFSKLIST my_set;	// LF_SKLIST


constexpr int RANGE = 1000;
constexpr int LOOP = 4000000;

class HISTORY {
public:
	int op;
	int i_value;
	bool o_value;
	HISTORY(int o, int i, bool re) : op(o), i_value(i), o_value(re) {}
};

void worker_check(int thread_id, vector<HISTORY>* history, int num_threads)
{
	g_th_id = thread_id;
	tl_id = thread_id;
	for (int i = 0; i < LOOP / num_threads; ++i) {
		int op = rand() % 3;
		switch (op) {
		case 0: {
			int v = rand() % RANGE;
			history->emplace_back(0, v, my_set.ADD(v));
			break;
		}
		case 1: {
			int v = rand() % RANGE;
			history->emplace_back(1, v, my_set.REMOVE(v));
			break;
		}
		case 2: {
			int v = rand() % RANGE;
			history->emplace_back(2, v, my_set.CONTAINS(v));
			break;
		}
		}
	}
}

void check_history(array <vector <HISTORY>, MAX_THREADS>& history, int num_threads)
{
	array <int, RANGE> survive = {};
	cout << "Checking Consistency : ";
	if (history[0].size() == 0) {
		cout << "No history.\n";
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
			cout << "ERROR. The value " << i << " removed while it is not in the set.\n";
			exit(-1);
		}
		else if (val > 1) {
			cout << "ERROR. The value " << i << " is added while the set already have it.\n";
			exit(-1);
		}
		else if (val == 0) {
			if (my_set.CONTAINS(i)) {
				cout << "ERROR. The value " << i << " should not exists.\n";
				exit(-1);
			}
		}
		else if (val == 1) {
			if (false == my_set.CONTAINS(i)) {
				cout << "ERROR. The value " << i << " shoud exists.\n";
				exit(-1);
			}
		}
	}
	cout << " OK\n";
}

void worker(int thread_id, int num_threads)
{
	g_th_id = thread_id;
	tl_id = thread_id;
	for (int i = 0; i < LOOP / num_threads; ++i) {
		int op = rand() % 3;
		switch (op) {
		case 0: {
			int v = rand() % RANGE;
			my_set.ADD(v);
			break;
		}
		case 1: {
			int v = rand() % RANGE;
			my_set.REMOVE(v);
			break;
		}
		case 2: {
			int v = rand() % RANGE;
			my_set.CONTAINS(v);
			break;
		}
		}
	}
}

int main()
{
	cout << "Benchmarking...\n";
	for (int num_threads = 1; num_threads <= MAX_THREADS; num_threads *= 2) {
		vector <thread> threads;
		my_set.clear();
		auto start_t = high_resolution_clock::now();
		for (int i = 0; i < num_threads; ++i)
			threads.emplace_back(worker, i, num_threads);
		for (auto& th : threads)
			th.join();
		auto end_t = high_resolution_clock::now();
		auto exec_t = end_t - start_t;
		auto exec_ms = duration_cast<milliseconds>(exec_t).count();
		my_set.print20();
		cout << num_threads << " Threads.  Exec Time : " << exec_ms << endl;
	}

	cout << "\n\nChecking Error...\n";
	for (int num_threads = 1; num_threads <= MAX_THREADS; num_threads *= 2) {
		vector <thread> threads;
		array<vector<HISTORY>, MAX_THREADS> history;
		my_set.clear();
		auto start_t = high_resolution_clock::now();
		for (int i = 0; i < num_threads; ++i)
			threads.emplace_back(worker_check, i, &history[i], num_threads);
		for (auto& th : threads)
			th.join();
		auto end_t = high_resolution_clock::now();
		auto exec_t = end_t - start_t;
		auto exec_ms = duration_cast<milliseconds>(exec_t).count();
		my_set.print20();
		cout << num_threads << " Threads.  Exec Time : " << exec_ms << endl;
		check_history(history, num_threads);
	}
}