#pragma once
#include "refcount/RefCounted.h"
#include "internal/DescriptorPool_Vulkan.h"
#include "../GPUDataLayout.h"
#include "containers/SkipArray.h"
#include <unordered_map>

namespace CSERenderer
{
class GPUPipelineDataRegistry;

enum GPUPipelineDataType
{
	DATA_TYPE_NULL,
	DATA_TYPE_STATIC,
	DATA_TYPE_DYNAMIC
};

enum GPUPipelineDataAccess
{
	DATA_ACCESS_NULL,
	DATA_ACCESS_READ_ONLY,
	DATA_ACCESS_READ_AND_WRITE
};

class GPUPipelineData : public CSECore::RefCounted
{
public:
	GPUPipelineData();
	GPUPipelineData(GPUPipelineDataRegistry* source, 
		CSECore::Ref<SSBODescriptor> descriptor,
		CSECore::Ref<GPUDataLayout> layout, 
		GPUPipelineDataType type,
		GPUPipelineDataAccess access);
	~GPUPipelineData();

	const SSBODescriptor* GetDescriptor();
	const GPUDataLayout* GetLayout();
	GPUPipelineDataType GetType();
	GPUPipelineDataAccess GetAccess();

private:
	friend struct GPUPipelineDataDeleter;

	GPUPipelineDataRegistry* _source;
	CSECore::Ref<SSBODescriptor> _descriptor;
	CSECore::Ref<GPUDataLayout> _layout;
	GPUPipelineDataType _type;
	GPUPipelineDataAccess _access;
};

struct GPUPipelineDataDeleter
{
	void operator()(GPUPipelineData* data);
};

class GPUPipelineDataRegistry
{
public:
	GPUPipelineDataRegistry();
	~GPUPipelineDataRegistry();

	CSECore::Ref<GPUPipelineData> CreatePipelineData(CSECore::Ref<GPUDataLayout> layout, GPUPipelineDataType type, GPUPipelineDataAccess access);
	CSECore::Ref<GPUPipelineData> GetStaticPipelineData(uint32_t nameHash);
	void ReleasePipelineData(GPUPipelineData* data);

private:
	/*
	* Maybe we could organize the read-only data by their
	* assigned values to promote sharing any existing read-only
	* pipeline data.
	* 
	* this would be really useful if a particular set of read
	* only data gets frequently used across multiple different
	* pipeline inputs, saving on descriptor space.
	* 
	* this is more of a long-term design question, but wouldn't
	* it be easier to just reference EVERYTHING via handles that
	* represent the state of whatever object needs to be accessed?
	* that way, we wouldnt be passing around the resources themselves
	* via pointer. itd also make things a bit easier on the render graph
	* since the explicit instance of a given resource can be known
	* immediately from the handle itself.
	* 
	* only downside is that we'd need a pool and table for each resource
	* we access and we'd have to readapt any access to the resource's values
	* to be done through said table.
	*/

	CSECore::SkipArray<GPUPipelineData, 256> _data;
	std::unordered_map<uint32_t, uint32_t> _hashToStaticDataIndex;
};

}