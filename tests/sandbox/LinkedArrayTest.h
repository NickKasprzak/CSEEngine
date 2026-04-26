#pragma once
#include "containers/LinkedArray.h"
#include "containers/SkipArray.h"
#include "Logger.h"

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
	int* ptr20 = skipArray.Add(20);
	int* ptr30 = skipArray.Add(30);
	int* ptr40 = skipArray.Add(40);
	skipArray.Add(50);
	skipArray.Add(60);

	CSE_LOGI("\nAdded elements 10, 20, 30, 40, 50, 60.\n");
	DumpArray(skipArray);

	skipArray.Remove(ptr20);
	skipArray.Remove(ptr40);

	CSE_LOGI("\nRemoved elements 20 and 40.\nList should now be 10, 30, 50, 60.\n");
	DumpArray(skipArray);

	skipArray.Remove(ptr30);

	CSE_LOGI("\nRemoved element 30.\nList should now be 10, 50, 60.\n");
	DumpArray(skipArray);

	int* ptr1000 = skipArray.Add(1000);
	
	CSE_LOGI("\nAdded element 1000.\nList should now be 10, 1000, 50, 60.\n");
	DumpArray(skipArray);

	int* ptr2000 = skipArray.Add(2000);
	int* ptr3000 = skipArray.Add(3000);

	CSE_LOGI("\nAdded elements 2000 and 3000.\nList should now be 10, 1000, 2000, 3000, 50, 60.\n");
	DumpArray(skipArray);

	skipArray.Add(4000);

	CSE_LOGI("\nAdded element 4000.\nList should now be 10, 1000, 2000, 3000, 50, 60, 4000.\n");
	DumpArray(skipArray);

	skipArray.Remove(ptr3000);
	skipArray.Remove(ptr1000);

	CSE_LOGI("\nRemoved elements 3000 and 1000.\nList should now be 10, 2000, 50, 60, 4000.\n");
	DumpArray(skipArray);

	skipArray.Remove(ptr2000);
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
	int* ptr20 = skipArray.Add(20);
	int* ptr30 = skipArray.Add(30);
	int* ptr40 = skipArray.Add(40);
	skipArray.Add(50);
	skipArray.Add(60);

	CSE_LOGI("\nAdded elements 10, 20, 30, 40, 50, 60.\n");
	DumpArray(skipArray);

	skipArray.Remove(ptr20);

	CSE_LOGI("\nRemoved element 20.\nList should now be 10, 30, 40, 50, 60.");
	DumpArray(skipArray);

	skipArray.Remove(ptr30);

	CSE_LOGI("\nRemoved element 30.\nList should now be 10, 40, 50, 60.");
	DumpArray(skipArray);

	skipArray.Remove(ptr40);

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
	int* begin = skipArray.Add(1);
	skipArray.Add(2);
	skipArray.Add(3);
	int* end  = skipArray.Add(4);

	CSE_LOGI("\nAdded elements 1, 2, 3, 4");
	DumpArray(skipArray);

	skipArray.Remove(begin);
	skipArray.Remove(end);

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

int main()
{
	//TestFreeBlockMerge();
	//TestFreeBlockAppend();
	//TestFreeBlockRemoveEdge();
	//TestRemoveConstants();
}