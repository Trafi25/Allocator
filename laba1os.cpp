#include <iostream>
#include "Windows.h"
#include "allocgead.h"

using namespace std;

int main() {
	auto allocator = new AllocatorBlock();
	auto mem = allocator->mem_alloc(sizeof(long));
	auto mem2 = allocator->mem_alloc(5);
	allocator->mem_dump();
	auto mem_real = allocator->mem_realloc(mem, 23);
	allocator->mem_dump();
	allocator->mem_free(mem2);
	allocator->mem_dump();

	return 0;
}

AllocatorBlock::AllocatorBlock()
{
	Used = new list<AllocHeader*>();
	Free = new list<AllocHeader*>();
	FirstBlock = nullptr;
	LastBlock = nullptr;
}

void* AllocatorBlock::mem_alloc(size_t size)
{
	size = StandSize(size);
	AllocHeader* AllocHeader = SerchBlock(size);

	if (AllocHeader == nullptr)
	{
		AllocHeader = MemoryAllocation(size);

		if (AllocHeader == nullptr)
		{
			return nullptr;
		}
	}

	Used->push_back(AllocHeader);
	return AllocHeader->pointer;
}

size_t AllocatorBlock::StandSize(size_t size)
{
	return (size + sizeof(int) - 1) & ~(sizeof(int) - 1);
}

AllocatorBlock::AllocHeader* AllocatorBlock::SerchBlock(size_t size) {
	for (AllocHeader* AllocHeader : *Free)
	{
		if (AllocHeader->size < size)
			continue;
		{
			Free->remove(AllocHeader);
			return AllocHeader;
		}
	}

	return nullptr;
}

AllocatorBlock::AllocHeader* AllocatorBlock::MemoryAllocation(size_t size)
{
	const auto mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);

	if (mem == nullptr)
	{
		return nullptr;
	}

	const auto block = new AllocHeader();
	block->next = nullptr;
	block->prev = nullptr;
	block->pointer = static_cast<uint8_t*>(mem);
	block->size = size;

	if ((Free->size() == 0) && (Used->size() == 0)) FirstBlock = block;


	if (LastBlock != nullptr)
	{
		LastBlock->next = block;
		block->prev = LastBlock;
	}

	LastBlock = block;

	return block;
}

void* AllocatorBlock::mem_realloc(void* addr, size_t size)
{
	mem_free(addr);
	return mem_alloc(size);
}

void AllocatorBlock::mem_free(void* addr)
{
	const auto pointer = reinterpret_cast<uint8_t*>(addr);
	for (auto AllocHeader : *Used)
	{
		if (AllocHeader->pointer != pointer)
			continue;
		Used->remove(AllocHeader);
		Free->push_back(AllocHeader);
		BlockAddition(AllocHeader);
		return;
	}
}

void AllocatorBlock::BlockAddition(AllocHeader* AllocHeader)
{
	if (AllocHeader->prev != nullptr && CheckBlock(Free, AllocHeader->prev))
	{
		AllocHeader = UnionBlocks(AllocHeader->prev, AllocHeader);
	}

	if (AllocHeader->next != nullptr && CheckBlock(Free, AllocHeader->next))
	{
		AllocHeader = UnionBlocks(AllocHeader, AllocHeader->next);
	}
}

AllocatorBlock::AllocHeader* AllocatorBlock::UnionBlocks(AllocHeader* left, AllocHeader* right)
{
	left->size += right->size;
	left->next = right->next;
	return left;
}

void AllocatorBlock::mem_dump()
{
	auto СurrentB = FirstBlock;
	cout << endl << "--- Condition ---" << endl;
	while (true) {

		cout << "Memory allocated: " << СurrentB->size << "\n";
		cout << "State of the current block: ";
		if (CheckBlock(Free, СurrentB))
			cout << "Free" << endl;
		else if (CheckBlock(Used, СurrentB))
			cout << "Used" << endl;

		if (СurrentB->next == nullptr)
			break;

		СurrentB = СurrentB->next;
	}
}
bool AllocatorBlock::CheckBlock(const std::list<AllocHeader*>* list, AllocHeader* AllocHeader)
{


	for (auto list_block : *list)
	{
		if (list_block->pointer == AllocHeader->pointer)
			return true;
	}
	return false;
}