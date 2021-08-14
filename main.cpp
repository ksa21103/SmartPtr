#include <iostream>
#include <vector>
#include <future>

#include "shared_ptr.h"

using namespace another_smart_pointer;

static long sCounter = 0;

class CounterHelper
{
public:

    CounterHelper(size_t instanceNo)
        : m_instanceNo(instanceNo)
    {
        ++sCounter;
    }

    ~CounterHelper()
    {
        --sCounter;
    }

    size_t getInstanceNo() const
    {
        return m_instanceNo;
    }

private:

    size_t m_instanceNo = 0;
};
using CounterHelperPtr = shared_ptr<CounterHelper>;

int main()
{
    {
        assert(sCounter == 0);

        CounterHelperPtr pCounterHelper;

        assert(pCounterHelper.use_count() == 0);
        assert(sCounter == 0);
    }

    {
        assert(sCounter == 0);

        CounterHelperPtr pCounterHelper(new CounterHelper(1));
        assert(sCounter == 1);
        assert(pCounterHelper.use_count() == 1);

        pCounterHelper.reset();
        assert(pCounterHelper.get() == nullptr);

        assert(pCounterHelper.use_count() == 0);
        assert(sCounter == 0);
    }

    {
    	assert(sCounter == 0);

    	CounterHelperPtr pCounterHelper;
    	pCounterHelper.reset(new CounterHelper(1));
    	assert(sCounter == 1);
    	assert(pCounterHelper.use_count() == 1);

    	pCounterHelper.reset();
    	assert(pCounterHelper.get() == nullptr);

    	assert(pCounterHelper.use_count() == 0);
    	assert(sCounter == 0);
    }

    {
        assert(sCounter == 0);

        CounterHelperPtr pCounterHelper1(new CounterHelper(1));
        assert(sCounter == 1);
        assert(pCounterHelper1.use_count() == 1);

        assert((*pCounterHelper1).getInstanceNo() == 1);

        CounterHelperPtr pCounterHelper2;
        pCounterHelper2 = pCounterHelper1;
        assert(pCounterHelper2.get() == pCounterHelper1.get());
        assert(pCounterHelper1.use_count() == 2);
        assert(pCounterHelper2.use_count() == 2);
        assert(sCounter == 1);
        assert((*pCounterHelper2).getInstanceNo() == 1);

        assert(sCounter == 1);

        pCounterHelper1.reset();
        assert(pCounterHelper1.get() == nullptr);
        assert(pCounterHelper2.get() != nullptr);
        assert(pCounterHelper1.use_count() == 0);
        assert(pCounterHelper2.use_count() == 1);

        pCounterHelper2.reset();
        assert(pCounterHelper1.get() == nullptr);
        assert(pCounterHelper2.get() == nullptr);
        assert(pCounterHelper1.use_count() == 0);
        assert(pCounterHelper2.use_count() == 0);

        assert(sCounter == 0);
    }

    {
        assert(sCounter == 0);

        CounterHelperPtr pCounterHelper1(new CounterHelper(1));
        assert(sCounter == 1);
        assert(pCounterHelper1.use_count() == 1);

        CounterHelperPtr pCounterHelper2(pCounterHelper1);
        assert(pCounterHelper1.get() == pCounterHelper2.get());
        assert(pCounterHelper1.use_count() == 2);
        assert(pCounterHelper2.use_count() == 2);
        assert(sCounter == 1);

        assert((*pCounterHelper1).getInstanceNo() == 1);
        assert((*pCounterHelper2).getInstanceNo() == 1);

        pCounterHelper1.reset();
        assert(pCounterHelper1.get() == nullptr);
        assert(pCounterHelper2.get() != nullptr);
        assert(pCounterHelper1.use_count() == 0);
        assert(pCounterHelper2.use_count() == 1);

        pCounterHelper2.reset();
        assert(pCounterHelper1.get() == nullptr);
        assert(pCounterHelper2.get() == nullptr);
        assert(pCounterHelper1.use_count() == 0);
        assert(pCounterHelper2.use_count() == 0);

        assert(sCounter == 0);
    }

    {
        assert(sCounter == 0);

        CounterHelperPtr pCounterHelper1(new CounterHelper(1));
        assert(sCounter == 1);
        assert(pCounterHelper1.use_count() == 1);

        CounterHelperPtr pCounterHelper2(std::move(pCounterHelper1));
        assert(pCounterHelper1.get() == nullptr);
        assert(pCounterHelper2.get() != nullptr);
        assert(pCounterHelper1.use_count() == 0);
        assert(pCounterHelper2.use_count() == 1);
        assert(sCounter == 1);
        assert((*pCounterHelper2).getInstanceNo() == 1);

        pCounterHelper2.reset();
        assert(pCounterHelper1.get() == nullptr);
        assert(pCounterHelper2.get() == nullptr);

        assert(sCounter == 0);
    }

    {
        CounterHelperPtr pCounterHelper(new CounterHelper(1));

        std::vector<CounterHelperPtr> vec1;
        vec1.reserve(222);
        std::vector<CounterHelperPtr> vec2;
        vec2.reserve(333);

        auto future1 = std::async(std::launch::async, [&](){ while (vec1.size() < vec1.capacity()) vec1.push_back(pCounterHelper); });
        auto future2 = std::async(std::launch::async, [&](){ while (vec2.size() < vec2.capacity()) vec2.push_back(pCounterHelper); });

        future1.wait();
        future2.wait();

        assert(pCounterHelper.use_count() == (vec1.size() + vec2.size() + 1));

        vec2.clear();
        assert(pCounterHelper.use_count() == (vec1.size() + 1));

        vec1.clear();
        assert(pCounterHelper.use_count() == 1);
    }

    return 0;
}
