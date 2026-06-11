#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace CSECore
{

namespace CSECore_Private
{

static const uint32_t FNV_OFFSET_BASIS_32 = 2166136261;
static const uint32_t FNV_PRIME_32 = 16777619;

static uint32_t FNVHash1A_32(const char* data, size_t dataSize)
{
	uint32_t hash = FNV_OFFSET_BASIS_32;
	for (int i = 0; i < dataSize; i++)
	{
		hash ^= static_cast<uint32_t>(*data);
		hash *= FNV_PRIME_32;
	}

	return hash;
}

static uint32_t FNVHash1A_32(const char* data, size_t dataSize, uint32_t prevHash)
{
	uint32_t hash = prevHash;
	for (int i = 0; i < dataSize; i++)
	{
		hash ^= static_cast<uint32_t>(*data);
		hash *= FNV_PRIME_32;
	}

	return hash;
}

}

template<typename Type>
static uint32_t FNVHash(const Type& typeRef)
{
	return CSECore_Private::FNVHash1A_32(reinterpret_cast<const char*>(&typeRef), sizeof(Type));
}

template<typename Type>
static uint32_t FNVHash(const std::vector<Type>& typeRef)
{
	uint32_t hash = 0;
	for (int i = 0; i < typeRef.size(); i++)
	{
		hash = hash ^ FNVHash(typeRef[i]);
	}
	return hash;
}

template<>
static uint32_t FNVHash<std::string>(const std::string& typeRef)
{
	return CSECore_Private::FNVHash1A_32(typeRef.data(), typeRef.size());
}

static uint32_t FNVHash(const char* typeRef)
{
	return CSECore_Private::FNVHash1A_32(typeRef, strlen(typeRef));
}

static uint32_t FNVHash(const char* typeRef, size_t size)
{
	return CSECore_Private::FNVHash1A_32(typeRef, size);
}

}