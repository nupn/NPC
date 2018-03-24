#include <stdio.h>

using namespace std;

class TestCls1
{
public:
	TestCls1() {printf("TestCls1()\n");};
	~TestCls1() { printf("~TestCls1()\n"); };


	TestCls1(const TestCls1& ref)
	{
		printf("TestCls1(const TestCls& ref)\n");
	}


	TestCls1& operator = (const TestCls1& ref) {
		printf("operator = (const TestCls1& ref)\n");
		return *this;
	}


	void SetValue(int value) {
		nValue = value;
	}

private:
	int nValue;

};

class TestCls2
{
public:
	TestCls2() { printf("TestCls2()\n"); };
	~TestCls2() { printf("~TestCls2()\n"); };


	TestCls2(const TestCls2& ref)
	{
		value = ref.value;
		printf("TestCls2(const TestCls2& ref)\n");
	}


	TestCls2& operator = (const TestCls2& ref) {
		printf("operator = (const TestCls2& ref)\n");
		this->value = ref.value;
		return *this;
	}

private:
	TestCls1 value;
};

TestCls1 GetTestClass(int value) {
	TestCls1 t1;
	t1.SetValue(value);
	return t1;
}

int main() {
	TestCls1 t1;
	printf("----------------\n");
	t1 = GetTestClass(10);
	printf("----------------\n");
}