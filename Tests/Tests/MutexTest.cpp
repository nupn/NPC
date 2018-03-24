#include <stdio.h>
#include <thread>
#include <mutex>
using namespace std;

template <int Cnt>
class TestCls
{
public:
	TestCls() {};
	~TestCls() {};

	static std::mutex m_mutext;

	void Work()
	{
		if (Cnt == 1)
		{
			TestCls<Cnt>::m_mutext.lock();
			a++;
			TestCls<Cnt>::m_mutext.unlock();
		}
		else
		{
			a++;
		}

	}

	int GetValue() {
		return a;
	}

private:
	int a = 0;


};


std::mutex TestCls<1>::m_mutext;
std::mutex TestCls<2>::m_mutext;

/*
void LoopCall(void* testCls)
{
	TestCls* pTest = (TestCls*)testCls;

	for (int i = 0; i < 100; ++i)
	{
		pTest->Work();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
*/

int main()
{
	printf("Start\n");
	const int nSize = 5;
	TestCls<1>* pTest1 = new TestCls<1>();
	TestCls<2>* pTest2 = new TestCls<2>();

	std::thread threads1[nSize];
	std::thread threads2[nSize];
	for (int i = 0; i < nSize; ++i)
	{
		threads1[i] = thread([pTest1]()->void {
			for (int i = 0; i < 100; ++i)
			{
				pTest1->Work();
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		});
		threads2[i] = thread([pTest2]()->void {
			for (int i = 0; i < 100; ++i)
			{
				pTest2->Work();
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		});
	}

	for (int i = 0; i < nSize; ++i)
	{
		threads1[i].join();
		threads2[i].join();
	}

	printf("%d\n", pTest1->GetValue());
	printf("%d\n", pTest2->GetValue());

	return 0;
}