// Copyright 2020, Nathan Blane

#pragma once

#include "Debugging/Assertion.hpp"
#include "Debugging/DebugOutput.hpp"
#include "BasicTypes/Uncopyable.hpp"
#include "Containers/DynamicArray.hpp"
#include "Utilities/MacroHelpers.hpp"
#include "Math/MathFunctions.hpp"
#include "Utilities/Array.hpp"

struct UnitStats
{
	int testCount = 0;
	int testPass = 0;
	int testFail = 0;
	int indvAsserts = 0;
};
struct UnitData
{
	const char *pMemberName = nullptr;
	char *pSourceFilePath = nullptr;
	int sourceLineNumber = 0;
	bool result = false;
};

struct UnitTest
{
	UnitTest(const char* pTestName);

	virtual void run(UnitData &, UnitStats &) const = 0;
	virtual void teardown() const {};

	const char * name;
};

class UnitTestRegistry : private Uncopyable
{
public:
	static void AddTest(UnitTest& test)
	{
		UnitTestRegistry *pRegistry = UnitTestRegistry::privGetInstance();

		pRegistry->tests.Add(&test);
	}
	static void RunTests()
	{
		//UnitTrace::out("\nTestRegistry:RunTests()\n");
		Debug::Print("\n");
		Debug::Print("---- Testing ----\n");
		UnitTestRegistry *pRegistry = UnitTestRegistry::privGetInstance();

		UnitData unitData;
		UnitStats unitStats;

		for(const auto& test : pRegistry->tests)
		{
			unitStats.testCount++;

			// run the test
			unitData.result = true;
			test->run(unitData, unitStats);
		
			if (unitData.result == true)
			{
				unitStats.testPass++;
			}
			else
			{
				unitStats.testFail++;
			}
		}

		Debug::Print("\n");
		Debug::Printf(" testCount: {}\n", unitStats.testCount);
		Debug::Printf("  testPass: {}\n", unitStats.testPass);
		Debug::Printf("  testFail: {}\n", unitStats.testFail);
		Debug::Printf("indvChecks: {}\n", unitStats.indvAsserts);
		Debug::Print("\n");
		Debug::Print("-----------------\n");
	}

private:
	UnitTestRegistry() = default;

	static UnitTestRegistry* privGetInstance()
	{
		static UnitTestRegistry tRegistry;
		return &tRegistry;
	}

	DynamicArray<UnitTest*> tests;
};

// Allow conditional expressions that are constant.
#pragma warning( disable : 4127 )

// Increments total check count
// Creates a clickable format in the output window for failure
// Abort test on first fail
#define CHECK( condition ) \
{ \
	_UnitStats.indvAsserts++;\
	if( !( condition ) ) \
	{ \
		_UnitData.result = false;  \
		_UnitData.pMemberName = this->name; \
		_UnitData.pSourceFilePath = __FILE__; \
		_UnitData.sourceLineNumber = __LINE__; \
		Debug::Printf("{}({}): {} \n", _UnitData.pSourceFilePath, _UnitData.sourceLineNumber, _UnitData.pMemberName ); \
		this->teardown(); \
		return; \
	} \
	else\
	{\
	}\
}

#define VA_ARGS(...) , ## __VA_ARGS__

#define CHECK_TRUE(value) CHECK(value == true);
#define CHECK_FALSE(value) CHECK(value == false);
#define CHECK_ZERO(value) CHECK(value == 0)

#define CHECK_PTR(value) CHECK(value != nullptr);
#define CHECK_REF(value) CHECK(&value != nullptr);
#define CHECK_NULL(value) CHECK(value == nullptr);

#define CHECK_EQ(value1, value2) CHECK(value1 == value2)
#define CHECK_NE(value1, value2) CHECK(value1 != value2)
#define CHECK_LT(value1, value2) CHECK(value1 < value2)
#define CHECK_LE(value1, value2) CHECK(value1 <= value2)
#define CHECK_GT(value1, value2) CHECK(value1 > value2)
#define CHECK_GE(value1, value2) CHECK(value1 >= value2)


#define CHECK_FLTEQ( value1, value2, ...) CHECK(Math::IsEqual(value1, value2 VA_ARGS(__VA_ARGS__)))

#define TEST(TestName, GroupName)													\
class TestName##GroupName##_Test : public UnitTest									\
{																					\
	public:																			\
		TestName##GroupName##_Test():												\
		UnitTest(STRING(TestName##GroupName##_Test))								\
		{																			\
		};																			\
																					\
	virtual void run(UnitData &_UnitData, UnitStats &_UnitStats) const override;	\
} TestName##GroupName##_instance;													\
																					\
void TestName##GroupName##_Test::run(UnitData& _UnitData, UnitStats& _UnitStats) const


#define TEST_WITH_TEARDOWN(TestName, GroupName)												\
class TestName##GroupName##_Test : public UnitTest											\
{																							\
	public:																					\
		TestName##GroupName##_Test():														\
		UnitTest(STRING(TestName##GroupName##_Test))										\
		{																					\
		};																					\
																							\
	virtual void run(UnitData &_UnitData, UnitStats &_UnitStats) const override;			\
	virtual void teardown() override;														\
} TestName##GroupName##_instance;															\
																							\
void TestName##GroupName##_Test::run(UnitData& _UnitData, UnitStats& _UnitStats) const 



#define TEST_TEARDOWN(TestName, GroupName)	\
void TestName##GroupName##_Test::teardown() const


