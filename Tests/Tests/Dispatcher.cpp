#include <functional>
#include <stdio.h>
#include <unordered_map>
#include <list>
#include <string.h>


#include <algorithm>


class CalleeA
{
public:
	void Echo(void *p)
	{
		printf("Echo A\n");
	}

	void Echo2(void *p, int b)
	{
		printf("Echo A\n");
	}
};

class CalleeB
{
public:
	void Echo(void *p)
	{
		printf("Echo B\n");
	}
};


struct FunctionObject
{
public:
	void operator()(void *p) {
		printf("Function Object\n");
	}
};

struct FunctionObject2
{
public:
	void operator()() {
		printf("Function Object2\n");
	}
};


template <typename _Key, typename _Ret>
class EventDispatcher
{
};


template <typename _Key, typename _Ret, typename... Args>
class EventDispatcher<_Key, _Ret(Args...)>
{
public:
	using Handler = std::tuple<void *, std::function<_Ret(Args...)>>;

	template <typename T>
	bool AddEventListener(_Key event, T *p, _Ret(T::*func)(Args...))
	{
		Handler newHandler = { p, std::bind(func, p, std::placeholders::_1) };

		auto iter = m_mapEventToHandle.find(event);
		if (iter == end(m_mapEventToHandle))
		{
			m_mapEventToHandle.insert({ event,{ newHandler } });
			return true;
		}

		std::list<Handler>& handlers = iter->second;
		for (auto& handler : handlers)
		{
			if (std::get<0>(handler) == p &&
				std::get<1>(handler).target_type() == std::get<1>(newHandler).target_type())
			{
				printf("Add Memeber Funciton fail\n");
				return false;
			}
		}

		iter->second.push_back(newHandler);
		return true;
	}

	void AddEventListener(_Key event, _Ret(*func)(Args...))
	{
		Handler newHandler = { nullptr, std::bind(func, std::placeholders::_1) };

		auto iter = m_mapEventToHandle.find(event);
		if (iter == end(m_mapEventToHandle))
		{
			m_mapEventToHandle.insert({ event,{ newHandler } });
			return;
		}

		iter->second.push_back(newHandler);
	}

	template<typename T>
	void AddEventListener(_Key event, T&& funcitonObject)
	{
		Handler newHandler = { nullptr, std::bind(funcitonObject, std::placeholders::_1) };

		auto iter = m_mapEventToHandle.find(event);
		if (iter == end(m_mapEventToHandle))
		{
			m_mapEventToHandle.insert({ event,{ newHandler } });
			return;
		}

		iter->second.push_back(newHandler);
	}

	_Ret Dispatch(_Key event, Args... args)
	{
		auto iter = m_mapEventToHandle.find(event);
		if (iter != end(m_mapEventToHandle))
		{
			std::list<Handler>& handlers = iter->second;
			for (auto& handler : handlers)
			{
				return std::get<1>(handler)((args)...);
			}
		}

		return _Ret();
	}

	std::unordered_map<_Key, std::list<Handler>> m_mapEventToHandle;
};

enum class MEventType : int
{
	kStart = 0,
	kEnd,
};

class Malibu : public EventDispatcher<MEventType, void(void*)>
{
public:
	//Malibu() {};
	void Start(void *p)
	{
		printf("malibu Start\n");
	}

	void End(void *p)
	{
		printf("malibu end\n");

	}


};

/*
class Cruze : public EventDispatcher<std::string, void(int, int, int)>
{
public:
	//Malibu() {};
	void Start(int a, int b, int c)
	{
		printf("cruze Start\n");

	}

	void End(int a, int b, int c)
	{
		printf("cruze Start\n");

	}


};
*/

void StaticFunction(void *p)
{
	printf("staticCall\n");
}

int main()
{
	//Test 1
	{
		CalleeA aa;
		CalleeA aa2;
		CalleeB bb;
		//void(CalleeA::*func)(void*) = &CalleeA::Echo;
		EventDispatcher<std::string, void(void*)> dispatcher;
		dispatcher.AddEventListener("event", &aa, &CalleeA::Echo);
		dispatcher.AddEventListener("event", &aa2, &CalleeA::Echo);
		dispatcher.AddEventListener("event", &aa, &CalleeA::Echo);
		dispatcher.AddEventListener("event", &bb, &CalleeB::Echo);

		//Error
		//dispatcher.AddEventListener("event", &bb, &CalleeA::Echo);
		//dispatcher.AddEventListener("event", &aa, &CalleeA::Echo2);

		dispatcher.Dispatch("event", &dispatcher);
	}

	printf("--Test2--------------\n");
	//Test2
	{
		CalleeA startHandler;
		CalleeB endHandler;

		Malibu malibu1;
		malibu1.AddEventListener(MEventType::kStart, &startHandler, &CalleeA::Echo);
		malibu1.AddEventListener(MEventType::kEnd, &endHandler, &CalleeB::Echo);
		malibu1.AddEventListener(MEventType::kEnd, &startHandler, &CalleeA::Echo);

		malibu1.Dispatch(MEventType::kStart,nullptr);
		malibu1.Dispatch(MEventType::kEnd, nullptr);
	}

	printf("--Test3--------------\n");
	//Test3 - Lambda
	{
		Malibu malibu1;
		malibu1.AddEventListener(MEventType::kStart, [](void *p)->void {
			printf("Start\n");
		});

		auto lambda1 = [](void *p)->void {
			printf("lambda Start\n");
		};

		auto lambda2 = [](void *p)->void {
			printf("lambda Start\n");
		};
		malibu1.AddEventListener(MEventType::kStart, lambda1);
		malibu1.AddEventListener(MEventType::kStart, lambda1);
		malibu1.AddEventListener(MEventType::kStart, lambda2);


		malibu1.Dispatch(MEventType::kStart, nullptr);
		malibu1.Dispatch(MEventType::kEnd, nullptr);
	}

	printf("--Test4--------------\n");
	// Test4 - function object
	{
		Malibu malibu1;
		malibu1.AddEventListener(MEventType::kStart, FunctionObject());

		FunctionObject func1;
		malibu1.AddEventListener(MEventType::kStart, func1);

		//malibu1.AddEventListener(MEventType::kStart, FunctionObject2());


		malibu1.Dispatch(MEventType::kStart, nullptr);
	}


	printf("--Static--------------\n");
	// Test4 - function object
	{
		Malibu malibu1;
		malibu1.AddEventListener(MEventType::kStart, StaticFunction);

		malibu1.Dispatch(MEventType::kStart, nullptr);
	}

	printf("--Other Type--------------\n");

	// Other Type
	{
		/*
		Cruze cruze;
		cruze.AddEventListener("start", &cruze, &Cruze::Start);
		cruze.AddEventListener("start", &cruze, &Cruze::Start);
		cruze.AddEventListener("end", &cruze, &Cruze::End);
		*/
	}

	return 0;
}