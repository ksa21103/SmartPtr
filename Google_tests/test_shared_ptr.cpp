#include <vector>
#include <future>

#include "gtest/gtest.h"

#include "../shared_ptr.h"

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

TEST(SharedPointerTestSuite, CreateEmpty)
{
	EXPECT_EQ(sCounter, 0);

	CounterHelperPtr pCounterHelper;

	EXPECT_EQ(pCounterHelper.use_count(), 0);
	EXPECT_EQ(sCounter, 0);
}

TEST(SharedPointerTestSuite, CreateEmptyCopyThis)
{
	EXPECT_EQ(sCounter, 0);

	CounterHelperPtr pCounterHelper;
	auto pObj = pCounterHelper.get();

	pCounterHelper = pCounterHelper;

	EXPECT_EQ(pCounterHelper.get(), pObj);
	EXPECT_EQ(pCounterHelper.use_count(), 0);
	EXPECT_EQ(sCounter, 0);
}

TEST(SharedPointerTestSuite, CreateEmptyMoveThis)
{
	EXPECT_EQ(sCounter, 0);

	CounterHelperPtr pCounterHelper;
	auto pObj = pCounterHelper.get();

	pCounterHelper = std::move(pCounterHelper);

	EXPECT_EQ(pCounterHelper.get(), pObj);
	EXPECT_EQ(pCounterHelper.use_count(), 0);
	EXPECT_EQ(sCounter, 0);
}

TEST(SharedPointerTestSuite, CreateWithObjAndDestroyOnEmptyAssignmentCopy)
{
	EXPECT_EQ(sCounter, 0);

	CounterHelperPtr pCounterHelper(new CounterHelper(1));

	EXPECT_EQ(sCounter, 1);
	EXPECT_EQ(pCounterHelper.use_count(), 1);

	CounterHelperPtr pCounterHelper2;
	pCounterHelper = pCounterHelper2;

	EXPECT_EQ(sCounter, 0);
	EXPECT_EQ(pCounterHelper.use_count(), 0);
}

TEST(SharedPointerTestSuite, CreateWithObjAndDestroyOnEmptyAssignmentMove)
{
	EXPECT_EQ(sCounter, 0);

	CounterHelperPtr pCounterHelper(new CounterHelper(1));

	EXPECT_EQ(sCounter, 1);
	EXPECT_EQ(pCounterHelper.use_count(), 1);

	pCounterHelper = CounterHelperPtr();

	EXPECT_EQ(sCounter, 0);
	EXPECT_EQ(pCounterHelper.use_count(), 0);
}


TEST(SharedPointerTestSuite, CreateResetWithObjAndResetEmpty)
{
	EXPECT_EQ(sCounter, 0);

	CounterHelperPtr pCounterHelper;
	pCounterHelper.reset(new CounterHelper(1));
	EXPECT_EQ(sCounter, 1);
	EXPECT_EQ(pCounterHelper.use_count(), 1);

	pCounterHelper.reset();
	EXPECT_EQ(pCounterHelper.get(), nullptr);

	EXPECT_EQ(pCounterHelper.use_count(), 0);
}

TEST(SharedPointerTestSuite, CreateWithObjAndResetEmpty)
{
	EXPECT_EQ(sCounter, 0);

	CounterHelperPtr pCounterHelper(new CounterHelper(1));
	EXPECT_EQ(sCounter, 1);
	EXPECT_EQ(pCounterHelper.use_count(), 1);

	pCounterHelper.reset();
	EXPECT_EQ(pCounterHelper.get(), nullptr);

	EXPECT_EQ(pCounterHelper.use_count(), 0);
}

TEST(SharedPointerTestSuite, CreateEmptyAndResetNewObj)
{
	EXPECT_EQ(sCounter, 0);

	CounterHelperPtr pCounterHelper;
	pCounterHelper.reset(new CounterHelper(1));
	EXPECT_EQ(sCounter, 1);
	EXPECT_EQ(pCounterHelper.use_count(), 1);

	pCounterHelper.reset();
	EXPECT_EQ(pCounterHelper.get(), nullptr);

	EXPECT_EQ(pCounterHelper.use_count(), 0);
	EXPECT_EQ(sCounter, 0);
}

TEST(SharedPointerTestSuite, CreateObjCreateEmptyAssignReset)
{
	EXPECT_EQ(sCounter, 0);

	CounterHelperPtr pCounterHelper1(new CounterHelper(1));
	EXPECT_EQ(sCounter, 1);
	EXPECT_EQ(pCounterHelper1.use_count(), 1);

	assert((*pCounterHelper1).getInstanceNo() == 1);

	CounterHelperPtr pCounterHelper2;
	pCounterHelper2 = pCounterHelper1;
	EXPECT_EQ(pCounterHelper1.get(), pCounterHelper2.get());
	EXPECT_EQ(pCounterHelper1.use_count(), 2);
	EXPECT_EQ(pCounterHelper2.use_count(), 2);
	EXPECT_EQ(sCounter, 1);
	EXPECT_EQ((*pCounterHelper2).getInstanceNo(), 1);

	EXPECT_EQ(sCounter, 1);

	pCounterHelper1.reset();
	EXPECT_EQ(pCounterHelper1.get(), nullptr);
	EXPECT_NE(pCounterHelper2.get(), nullptr);
	EXPECT_EQ(pCounterHelper1.use_count(), 0);
	EXPECT_EQ(pCounterHelper2.use_count(), 1);

	pCounterHelper2.reset();
	EXPECT_EQ(pCounterHelper1.get(), nullptr);
	EXPECT_EQ(pCounterHelper2.get(), nullptr);
	EXPECT_EQ(pCounterHelper1.use_count(), 0);
	EXPECT_EQ(pCounterHelper2.use_count(), 0);

	EXPECT_EQ(sCounter, 0);
}

TEST(SharedPointerTestSuite, CreateObjCreateWithCopyConstructorReset)
{
	EXPECT_EQ(sCounter, 0);

	CounterHelperPtr pCounterHelper1(new CounterHelper(1));
	EXPECT_EQ(sCounter, 1);
	EXPECT_EQ(pCounterHelper1.use_count(), 1);

	CounterHelperPtr pCounterHelper2(pCounterHelper1);
	EXPECT_EQ(pCounterHelper1.get(), pCounterHelper2.get());
	EXPECT_EQ(pCounterHelper1.use_count(), 2);
	EXPECT_EQ(pCounterHelper2.use_count(), 2);
	EXPECT_EQ(sCounter, 1);

	EXPECT_EQ((*pCounterHelper1).getInstanceNo(), 1);
	EXPECT_EQ((*pCounterHelper2).getInstanceNo(), 1);

	pCounterHelper1.reset();
	EXPECT_EQ(pCounterHelper1.get(), nullptr);
	assert(pCounterHelper2.get() != nullptr);
	EXPECT_EQ(pCounterHelper1.use_count(), 0);
	EXPECT_EQ(pCounterHelper2.use_count(), 1);

	pCounterHelper2.reset();
	EXPECT_EQ(pCounterHelper1.get(), nullptr);
	EXPECT_EQ(pCounterHelper2.get(), nullptr);
	EXPECT_EQ(pCounterHelper1.use_count(), 0);
	EXPECT_EQ(pCounterHelper2.use_count(), 0);

	EXPECT_EQ(sCounter, 0);
}

TEST(SharedPointerTestSuite, CreateWithObjCreateWithMoveConstructorReset)
{
	EXPECT_EQ(sCounter, 0);

	CounterHelperPtr pCounterHelper1(new CounterHelper(1));
	EXPECT_EQ(sCounter, 1);
	EXPECT_EQ(pCounterHelper1.use_count(), 1);

	CounterHelperPtr pCounterHelper2(std::move(pCounterHelper1));
	EXPECT_EQ(pCounterHelper1.get(), nullptr);
	EXPECT_NE(pCounterHelper2.get(), nullptr);
	EXPECT_EQ(pCounterHelper1.use_count(), 0);
	EXPECT_EQ(pCounterHelper2.use_count(), 1);
	EXPECT_EQ(sCounter, 1);
	EXPECT_EQ((*pCounterHelper2).getInstanceNo(), 1);

	pCounterHelper2.reset();
	EXPECT_EQ(pCounterHelper1.get(), nullptr);
	EXPECT_EQ(pCounterHelper2.get(), nullptr);

	EXPECT_EQ(sCounter, 0);
}

TEST(SharedPointerTestSuite, CreateWithObjCreateWithMoveAssignmentReset)
{
	EXPECT_EQ(sCounter, 0);

	CounterHelperPtr pCounterHelper1(new CounterHelper(1));
	EXPECT_EQ(sCounter, 1);
	EXPECT_EQ(pCounterHelper1.use_count(), 1);

	CounterHelperPtr pCounterHelper2;
	pCounterHelper2 = (std::move(pCounterHelper1));
	EXPECT_EQ(pCounterHelper1.get(), nullptr);
	EXPECT_NE(pCounterHelper2.get(), nullptr);
	EXPECT_EQ(pCounterHelper1.use_count(), 0);
	EXPECT_EQ(pCounterHelper2.use_count(), 1);
	EXPECT_EQ(sCounter, 1);
	EXPECT_EQ((*pCounterHelper2).getInstanceNo(), 1);

	pCounterHelper2.reset();
	EXPECT_EQ(pCounterHelper1.get(), nullptr);
	EXPECT_EQ(pCounterHelper2.get(), nullptr);

	EXPECT_EQ(sCounter, 0);
}

TEST(SharedPointerTestSuite, CreateWithObjMoveThisToThis)
{
	EXPECT_EQ(sCounter, 0);

	CounterHelperPtr pCounterHelper1(new CounterHelper(1));
	EXPECT_EQ(sCounter, 1);
	EXPECT_EQ(pCounterHelper1.use_count(), 1);

	auto pObj = pCounterHelper1.get();

	pCounterHelper1 = std::move(pCounterHelper1);

	EXPECT_EQ(pCounterHelper1.get(), pObj);

	EXPECT_EQ(pCounterHelper1.use_count(), 1);
	EXPECT_EQ(sCounter, 1);

	pCounterHelper1.reset();

	EXPECT_EQ(sCounter, 0);
}

TEST(SharedPointerTestSuite, CreateWithObjAndStdSwap)
{
	EXPECT_EQ(sCounter, 0);

	{
		CounterHelperPtr pCounterHelper1(new CounterHelper(1));
		CounterHelperPtr pCounterHelper2(new CounterHelper(2));

		EXPECT_EQ((*pCounterHelper1).getInstanceNo(), 1);
		EXPECT_EQ((*pCounterHelper2).getInstanceNo(), 2);

		EXPECT_EQ(sCounter, 2);
		EXPECT_EQ(pCounterHelper1.use_count(), 1);
		EXPECT_EQ(pCounterHelper2.use_count(), 1);

		std::swap(pCounterHelper1, pCounterHelper2);

		EXPECT_EQ(sCounter, 2);
		EXPECT_EQ(pCounterHelper1.use_count(), 1);
		EXPECT_EQ(pCounterHelper2.use_count(), 1);

		EXPECT_EQ((*pCounterHelper1).getInstanceNo(), 2);
		EXPECT_EQ((*pCounterHelper2).getInstanceNo(), 1);
	}

	EXPECT_EQ(sCounter, 0);
}

TEST(SharedPointerTestSuite, CreateWithObjParallelsCopy)
{
	CounterHelperPtr pCounterHelper(new CounterHelper(1));

	std::vector<CounterHelperPtr> vec1;
	vec1.reserve(222);
	std::vector<CounterHelperPtr> vec2;
	vec2.reserve(333);

	auto future1 = std::async(std::launch::async,
							  [&]()
							  {
                                  while (vec1.size() < vec1.capacity())
                                      vec1.push_back(pCounterHelper);
							  });
	auto future2 = std::async(std::launch::async,
			                  [&]()
			                  {
		                          while (vec2.size() < vec2.capacity())
                                      vec2.push_back(pCounterHelper);
			                  });

	future1.wait();
	future2.wait();

	EXPECT_EQ(pCounterHelper.use_count(), vec1.size() + vec2.size() + 1);

	vec2.clear();
	EXPECT_EQ(pCounterHelper.use_count(), vec1.size() + 1);

	vec1.clear();
	EXPECT_EQ(pCounterHelper.use_count(), 1);
}
