#pragma once
#include "containers/SkipArray.h"
#include "Logger.h"
#include <vector>
#include <string>

template<size_t size>
void DumpArray(CSECore::SkipArray<int, size>& arr)
{
	auto iter = arr.Begin();
	while (iter != arr.End())
	{
		CSE_LOGI(*iter);
		iter++;
	}
}

/*
* Tests for handling removal cases
* that would result in two separate
* free blocks to be merged.
*/
void TestFreeBlockMerge()
{
	CSECore::SkipArray<int, 32> skipArray;
	skipArray.Add(10);
	int id20 = skipArray.Add(20);
	int id30 = skipArray.Add(30);
	int id40 = skipArray.Add(40);
	skipArray.Add(50);
	skipArray.Add(60);

	CSE_LOGI("\nAdded elements 10, 20, 30, 40, 50, 60.\n");
	DumpArray(skipArray);

	skipArray.RemoveAtIndex(id20);
	skipArray.RemoveAtIndex(id40);

	CSE_LOGI("\nRemoved elements 20 and 40.\nList should now be 10, 30, 50, 60.\n");
	DumpArray(skipArray);

	skipArray.RemoveAtIndex(id30);

	CSE_LOGI("\nRemoved element 30.\nList should now be 10, 50, 60.\n");
	DumpArray(skipArray);

	int id1000 = skipArray.Add(1000);
	
	CSE_LOGI("\nAdded element 1000.\nList should now be 10, 1000, 50, 60.\n");
	DumpArray(skipArray);

	int id2000 = skipArray.Add(2000);
	int id3000 = skipArray.Add(3000);

	CSE_LOGI("\nAdded elements 2000 and 3000.\nList should now be 10, 1000, 2000, 3000, 50, 60.\n");
	DumpArray(skipArray);

	skipArray.Add(4000);

	CSE_LOGI("\nAdded element 4000.\nList should now be 10, 1000, 2000, 3000, 50, 60, 4000.\n");
	DumpArray(skipArray);

	skipArray.RemoveAtIndex(id3000);
	skipArray.RemoveAtIndex(id1000);

	CSE_LOGI("\nRemoved elements 3000 and 1000.\nList should now be 10, 2000, 50, 60, 4000.\n");
	DumpArray(skipArray);

	skipArray.RemoveAtIndex(id2000);
	skipArray.Add(1234);
	skipArray.Add(5678);

	CSE_LOGI("\nRemoved element 2000 and added elements 1234 and 5678.\nList should now be 10, 1234, 5678, 50, 60, 4000.\n");
	DumpArray(skipArray);

	skipArray.Add(1111);
	skipArray.Add(2222);
	skipArray.Add(3333);

	CSE_LOGI("\nAdded elements 1111, 2222, and 3333.\nList should now be 10, 1234, 5678, 1111, 50, 60, 4000, 2222, 3333.\n");
	DumpArray(skipArray);
}

/*
* Tests for handling removal cases
* that would result in existing
* free blocks to be extended/appended to
*/
void TestFreeBlockAppend()
{
	CSECore::SkipArray<int, 32> skipArray;
	skipArray.Add(10);
	int id20 = skipArray.Add(20);
	int id30 = skipArray.Add(30);
	int id40 = skipArray.Add(40);
	skipArray.Add(50);
	skipArray.Add(60);

	CSE_LOGI("\nAdded elements 10, 20, 30, 40, 50, 60.\n");
	DumpArray(skipArray);

	skipArray.RemoveAtIndex(id20);

	CSE_LOGI("\nRemoved element 20.\nList should now be 10, 30, 40, 50, 60.");
	DumpArray(skipArray);

	skipArray.RemoveAtIndex(id30);

	CSE_LOGI("\nRemoved element 30.\nList should now be 10, 40, 50, 60.");
	DumpArray(skipArray);

	skipArray.RemoveAtIndex(id40);

	CSE_LOGI("\nRemoved element 40.\nList should now be 10, 50, 60.");
	DumpArray(skipArray);

	skipArray.Add(2000);
	skipArray.Add(3000);
	skipArray.Add(4000);
	skipArray.Add(1234);

	CSE_LOGI("\nAdded elements 2000, 3000, 4000, and 1234.\nList should now be 10, 2000, 3000, 4000, 50, 60, 1234.");
	DumpArray(skipArray);
}

/*
* Tests for handling addition
* and removal of elements from
* the beginning and end of the
* block's full size
*/
void TestFreeBlockRemoveEdge()
{
	CSECore::SkipArray<int, 4> skipArray;
	int begin = skipArray.Add(1);
	skipArray.Add(2);
	skipArray.Add(3);
	int end  = skipArray.Add(4);

	CSE_LOGI("\nAdded elements 1, 2, 3, 4");
	DumpArray(skipArray);

	skipArray.RemoveAtIndex(begin);
	skipArray.RemoveAtIndex(end);

	CSE_LOGI("\nRemoved elements 1 and 4.\nList should now be 2, 3.");
	DumpArray(skipArray);

	skipArray.Add(5);
	skipArray.Add(6);

	CSE_LOGI("\nAdded elements 5 and 6.\nList should now be 6, 2, 3, 5.");
	DumpArray(skipArray);
}

/*
* Tests for removing constant
* values from the list rather
* than entries at specific addresses
*/
void TestRemoveConstants()
{
	CSECore::SkipArray<int, 32> skipArray;
	skipArray.Add(1);
	skipArray.Add(2);
	skipArray.Add(3);
	skipArray.Add(4);

	CSE_LOGI("\nAdded elements 1, 2, 3, and 4.\nList should now be 1, 2, 3, 4.");
	DumpArray(skipArray);

	skipArray.Remove(2);
	skipArray.Remove(3);

	CSE_LOGI("\nRemoved elements 2 and 3.\nList should now be 1, 4.");
	DumpArray(skipArray);

	skipArray.Remove(5);

	CSE_LOGI("\nAttempted to remove element 5.\nList should still be 1, 4.");
	DumpArray(skipArray);

	skipArray.Add(4);
	skipArray.Add(4);
	skipArray.Add(4);

	CSE_LOGI("\nAdded element 4 three times.\nList should now be 1, 4, 4, 4, 4.");
	DumpArray(skipArray);

	skipArray.Remove(4);
	skipArray.Remove(4);

	CSE_LOGI("\nRemoved element 4 two times.\nList should now be 1, 4, 4.");
	DumpArray(skipArray);

	skipArray.Add(2);

	CSE_LOGI("\nAdded element 2.\nList should now be 1, 2, 4, 4.");
	DumpArray(skipArray);
}


class TestNonMoveClass
{
public:
	TestNonMoveClass()
		: _intVec(), _str()
	{
		CSE_LOGI("TestNonMoveClass default ctor called.");
	}

	TestNonMoveClass(const std::vector<int>& intVec, const std::string& str)
		: _intVec(intVec), _str(str)
	{
		CSE_LOGI("TestNonMoveClass param ctor called.");
	}

	TestNonMoveClass(const TestNonMoveClass& other)
		: _intVec(other._intVec), _str(other._str)
	{
		CSE_LOGI("TestNonMoveClass copy ctor called.");
	}

	~TestNonMoveClass()
	{
		CSE_LOGI("TestNonMoveClass dtor called.");
	}

	void operator=(const TestNonMoveClass& other)
	{
		CSE_LOGI("TestNonMoveClass copy assignment operator called.");
		_intVec = other._intVec;
		_str = other._str;
	}

	const std::vector<int>& GetIntVector()
	{
		return _intVec;
	}

	const std::string& GetString()
	{
		return _str;
	}

private:
	std::vector<int> _intVec;
	std::string _str;
};

void DumpTestNonMoveSkipArray(CSECore::SkipArray<TestNonMoveClass, 10>& arr)
{
	auto iter = arr.Begin();
	while (iter != arr.End())
	{
		std::string intVecStr;
		for (int i = 0; i < iter->GetIntVector().size(); i++)
		{
			intVecStr += std::to_string(iter->GetIntVector()[i]);
			
			if (i + 1 < iter->GetIntVector().size())
			{
				intVecStr += ", ";
			}
		}

		CSE_LOGI("{ { " << intVecStr << " }, " << iter->GetString() << " }");
		iter++;
	}
}

void TestClasses()
{
	CSECore::SkipArray<TestNonMoveClass, 10> arr;
	arr.Add(TestNonMoveClass(std::vector<int>({ 10, 20, 30 }), std::string("test string")));
	int removeMe = arr.Add(TestNonMoveClass(std::vector<int>({ 40, 50, 60 }), std::string("another one")));
	arr.Add(TestNonMoveClass(std::vector<int>({ 70, 80, 90 }), std::string("the third")));

	CSE_LOGI("Added three elements. Array should be { { 10, 20, 30 }, \"test string\" }, { { 40, 50, 60 }, \"another one\" }, { { 70, 80, 90 }, \"the third\" }");
	DumpTestNonMoveSkipArray(arr);

	arr.RemoveAtIndex(removeMe);
	arr.Add(TestNonMoveClass(std::vector<int>({ 1000, 2000, 3000 }), std::string("replacement.")));

	CSE_LOGI("Removed and added element. Array should be { { 10, 20, 30 }, \"test string\" }, { { 1000, 2000, 3000 }, \"replacement\" }, { { 70, 80, 90 }, \"the third\" }");
	DumpTestNonMoveSkipArray(arr);
}

class TestMoveClass
{
public:
	TestMoveClass()
		: _intVec(), _str()
	{
		CSE_LOGI("TestMoveClass default ctor called.");
	}

	TestMoveClass(const std::vector<int>& intVec, const std::string& str)
		: _intVec(intVec), _str(str)
	{
		CSE_LOGI("TestMoveClass param ctor called.");
	}

	TestMoveClass(TestMoveClass&& other) noexcept
		: _intVec(std::move(other._intVec)), _str(std::move(other._str))
	{
		CSE_LOGI("TestMoveClass move ctor called.");
	}

	~TestMoveClass()
	{
		CSE_LOGI("TestNonMoveClass dtor called.");
	}

	void operator=(TestMoveClass&& other) noexcept
	{
		CSE_LOGI("TestMoveClass move assignment operator called.");
		_intVec = std::move(other._intVec);
		_str = std::move(other._str);
	}

	const std::vector<int>& GetIntVector()
	{
		return _intVec;
	}

	const std::string& GetString()
	{
		return _str;
	}

private:
	std::vector<int> _intVec;
	std::string _str;
};

void DumpTestMoveSkipArray(CSECore::SkipArray<TestMoveClass, 10>& arr)
{
	auto iter = arr.Begin();
	while (iter != arr.End())
	{
		std::string intVecStr;
		for (int i = 0; i < iter->GetIntVector().size(); i++)
		{
			intVecStr += std::to_string(iter->GetIntVector()[i]);

			if (i + 1 < iter->GetIntVector().size())
			{
				intVecStr += ", ";
			}
		}

		CSE_LOGI("{ { " << intVecStr << " }, " << iter->GetString() << " }");
		iter++;
	}
}

void TestMoveAdd()
{
	CSECore::SkipArray<TestMoveClass, 10> arr;
	arr.Add(TestMoveClass(std::vector<int>({ 10, 20, 30 }), std::string("test string")));
	int removeMe = arr.Add(TestMoveClass(std::vector<int>({ 40, 50, 60 }), std::string("another one")));
	arr.Add(TestMoveClass(std::vector<int>({ 70, 80, 90 }), std::string("the third")));

	CSE_LOGI("Added three elements. Array should be { { 10, 20, 30 }, \"test string\" }, { { 40, 50, 60 }, \"another one\" }, { { 70, 80, 90 }, \"the third\" }");
	DumpTestMoveSkipArray(arr);

	arr.RemoveAtIndex(removeMe);
	arr.Add(TestMoveClass(std::vector<int>({ 1000, 2000, 3000 }), std::string("replacement.")));

	CSE_LOGI("Removed and added element. Array should be { { 10, 20, 30 }, \"test string\" }, { { 1000, 2000, 3000 }, \"replacement\" }, { { 70, 80, 90 }, \"the third\" }");
	DumpTestMoveSkipArray(arr);

	arr.Clear();
	TestMoveClass stacked(std::vector<int>({ 1, 2, 3, 4 }), std::string("i need to move."));
	arr.Add(std::move(stacked));
	//arr.Add(stacked); // Should always result in a static assert.
	
	CSE_LOGI("Cleared all and added an element. Array should be { { 1, 2, 3, 4 }, \"i need to move\" }");
	DumpTestMoveSkipArray(arr);
}

int main()
{
	//TestFreeBlockMerge();
	//TestFreeBlockAppend();
	//TestFreeBlockRemoveEdge();
	//TestRemoveConstants();
	TestClasses();
	TestMoveAdd();
}