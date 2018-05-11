#include <boost\serialization\singleton.hpp>
#include <stdio.h>
#include <thread>
#include <vector>

class TestCls : public boost::serialization::singleton<TestCls>
{
	friend class boost::serialization::singleton<TestCls>;
public:
	TestCls() {};
	~TestCls() {}

	int value;
	std::vector<int> vecs;
};

int main()
{
	{
		TestCls& ref = boost::serialization::singleton<TestCls>::get_mutable_instance();
		ref.value = 10;
		ref.vecs.push_back(10);
	}

	
	std::thread thread1([]()->void
	{
		const TestCls& ref = boost::serialization::singleton<TestCls>::get_const_instance();
		printf("%d\n", ref.value);
		printf("%d\n", ref.vecs.size());

	});

	{
		TestCls aa;
		aa.vecs.push_back(10);
	}

	thread1.join();
}