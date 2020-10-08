#include <iostream>
#include <list>
using namespace std;

class AllocatorBlock
{
	struct AllocHeader
	{
		uint8_t* pointer;
		size_t size;
		AllocHeader* next;
		AllocHeader* prev;
	};

public:
	explicit AllocatorBlock();
	void* mem_alloc(size_t size);
	void* mem_realloc(void* addr, size_t size);
	void mem_free(void* addr);
	void mem_dump();

private:
	AllocHeader* FirstBlock;
	AllocHeader* LastBlock;
	list<AllocHeader*>* Free;
	list<AllocHeader*>* Used;
	size_t StandSize(size_t size);
	AllocHeader* SerchBlock(size_t size);
	AllocHeader* MemoryAllocation(size_t size);
	void BlockAddition(AllocHeader* AllocHeader);
	AllocHeader* UnionBlocks(AllocHeader* left, AllocHeader* right);
	bool CheckBlock(const std::list<AllocHeader*>* list, AllocHeader* AllocHeader);
};
#pragma once
