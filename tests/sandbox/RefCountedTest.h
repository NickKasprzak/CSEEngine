#pragma once
#include "refcount/RefCounted.h"
#include "refcount/Ref.h"
#include "Logger.h"

class RefCountedResource : public CSECore::RefCounted
{
public:
	int num;

	RefCountedResource()
		: num(0)
	{
		CSE_LOGI("Constructed reference counted resource.");
	}

	~RefCountedResource()
	{
		CSE_LOGI("Destructed reference counted resource.");
	}
};

CSECore::Ref<RefCountedResource> OwningTest()
{
	CSECore::Ref<RefCountedResource> ref1(new RefCountedResource());
	CSE_LOGI("Created resource. Ref count: " << ref1->GetRefCount());

	{
		CSECore::Ref<RefCountedResource> ref2(ref1);
		ref2->num++;
		CSE_LOGI("Created new resource ref and incremented resource value. Ref count: " << ref1->GetRefCount() << ". Resource value: " << ref1->num);

		CSECore::Ref<RefCountedResource> ref3(ref2);
		ref3->num++;
		CSE_LOGI("Created new resource ref and incremented resource value. Ref count: " << ref1->GetRefCount() << ". Resource value: " << ref1->num);
	}

	CSE_LOGI("Removed previous resource refs. Ref count: " << ref1->GetRefCount() << ". Resource value: " << ref1->num);
	return ref1;
}

void NonOwningTest(CSECore::Ref<RefCountedResource> ref)
{
	CSE_LOGI("Got non-owned resource ref. Resource ref count: " << ref->GetRefCount() << ". Resource value: " << ref->num);

	CSECore::Ref<RefCountedResource> ref2(ref);
	ref2->num = ref2->num + 100;
	CSECore::Ref<RefCountedResource> ref3(ref);
	ref3->num += ref3->num + 100;
	CSE_LOGI("Created two new resource refs and increased value by 100 twice. Resource ref count: " << ref->GetRefCount() << ". Resource value: " << ref->num);
}

int main()
{
	{
		CSECore::Ref<RefCountedResource> refVal = OwningTest();
		CSE_LOGI("Got ref counted resource from OwningTest. Ref count: " << refVal->GetRefCount() << ". Resource value: " << refVal->num);
	}

	CSE_LOGI("\n");

	RefCountedResource resource;
	NonOwningTest(CSECore::Ref<RefCountedResource>(resource));
	CSE_LOGI("NonOwningTest completed. Resource ref count: " << resource.GetRefCount() << ". Resource value: " << resource.num);
}