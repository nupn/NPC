#include <stdio.h>
#include <thread>
#include <mutex>
using namespace std;

template <typename T, int C>
class MemPool
{
public:
	MemPool() {};
	~MemPool() {};
	
	static MemPool<T, C>* Getinstant() {
		if (m_mempool == nullptr)
		{
			m_mempool = new MemPool<T,C>();
		}

		return m_mempool;
	}


	static std::mutex m_mutext;
	static MemPool<T, C>* m_mempool;

	void* Allow(size_t size) {

		m_mutext.lock();
		void* p = malloc(size);

		m_mutext.unlock();
		return p;
	}

};


class TestCls
{
public:
	TestCls() {};
	~TestCls() {};

	

	void* operator new(size_t size)
	{
		MemPool<TestCls, 10>* mempool = MemPool<TestCls, 10>::Getinstant();

		void* p = mempool->Allow(size);

		
		return p;
	}


	int GetValue() {
		return a;
	}

private:
	int a = 0;


};


std::mutex MemPool<TestCls, 10>::m_mutext;
MemPool<TestCls, 10>* MemPool<TestCls, 10>::m_mempool;
void LoopCall()
{
	TestCls* pt[10];
	for (int j = 0; j < 10; ++j)
	{
		pt[j] = new TestCls;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	}

	for (int j = 0; j < 10; ++j)
	{
		delete pt[j];
	}

}


int main()
{
	std::thread thread[2];
	for (int i = 0; i < 2; ++i)
	{
		thread[i] = std::thread(LoopCall);
	}

	for (int i = 0; i < 2; ++i)
	{
		thread[i].join();
	}

	return 0;
}