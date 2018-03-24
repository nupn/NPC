#include "NPCMemoryPool/NPCMemoryPool.h"

#pragma comment(lib, "NPCMemoryPool.lib")
#include <thread>
using namespace NPCL;
using namespace std;


class TestCls
{
public:
	TestCls(int value) { a = value; };
	~TestCls() {};

	MEMPOOL_OPERATOR(TestCls, 20)

private:
	int a = 0;
};

MEMPOOL_STATIC(TestCls, 20)


class TestClsTwo
{
public:
	TestClsTwo(int value) { a = value; };
	~TestClsTwo() {};

	MEMPOOL_OPERATOR(TestClsTwo, 20)

private:
	int a = 0;
};

MEMPOOL_STATIC(TestClsTwo, 20)

//1 교차
//2 클래스
//3 dfine 정리

void LoopCall(int id)
{
	if (id % 2 == 0)
	{
		TestCls* tests[100] = { nullptr, };


		for (int i = 0; i < 100; ++i)
		{
			int nidx = rand() % 100;

			if (tests[i] == nullptr)
			{
				tests[i] = new TestCls(i);
			}
			else
			{
				delete tests[i];
			}


			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		for (int i = 0; i < 100; ++i)
		{
			if (tests[i] != nullptr)
			{
				delete tests[i];
			}
		}
	}
	else
	{
		TestClsTwo* tests[100] = { nullptr, };


		for (int i = 0; i < 100; ++i)
		{
			int nidx = rand() % 100;

			if (tests[i] == nullptr)
			{
				tests[i] = new TestClsTwo(i);
			}
			else
			{
				delete tests[i];
			}


			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		for (int i = 0; i < 100; ++i)
		{
			if (tests[i] != nullptr)
			{
				delete tests[i];
			}
		}
	}
}

int main()
{
	printf("Start\n");
	const int nSize = 5;
	std::thread threads[nSize];
	for (int i = 0; i < nSize; ++i)
	{
		threads[i] = thread(LoopCall, i);
	}

	for (int i = 0; i < nSize; ++i)
	{
		threads[i].join();
	}

	int blockSize = NPCMemoryPool<TestCls, 20>::GetInstance()->BlockSize();
	int remainSize = NPCMemoryPool<TestCls, 20>::GetInstance()->Size();
	printf("%d : %d\n", blockSize, remainSize);


	blockSize = NPCMemoryPool<TestClsTwo, 20>::GetInstance()->BlockSize();
	remainSize = NPCMemoryPool<TestClsTwo, 20>::GetInstance()->Size();
	printf("%d : %d\n", blockSize, remainSize);

	return 0;
}