#pragma once

#include <cstddef>
#include <cassert>
#include <vector>

// Source: 'https://github.com/alecthomas/entityx/blob/master/entityx/help/Pool.h'.
// Provides a resizable, semi-contiguous pool of memory for constructing objects in.
// Pointers into the pool will be invalidated only when the pool is destroyed.
//
// The semi-contiguous nature aims to provide cache-friendly integration.
//
// Lookups are O(1)
// Appends are amortized O(1) (O(1) on average).
class Pool
{
public:
	explicit Pool(std::size_t element_size, std::size_t chunk_size = 8192) :
		element_size_(element_size),
		chunk_size_(chunk_size),
		capacity_(0)
	{}
	~Pool()
	{
		for (char* ptr : blocks_)
		{
			delete[] ptr;
		}
	}

	std::size_t get_size() const { return size_; }
	std::size_t get_capacity() const { return capacity_; }
	std::size_t get_chunks() const { return blocks_.size(); }


	// Ensure that at least 'n' elements will fit in the pool.
	inline void expand(std::size_t n)
	{
		if (n >= size_)
		{
			if (n >= capacity_)
				reserve(n);
			size_ = n;
		}
	}

	inline void reserve(std::size_t n)
	{
		while (capacity_ < n)
		{
			char* chunk = new char[element_size_ * chunk_size_];
			blocks_.push_back(chunk);
			capacity_ += chunk_size_;
		}
	}


	inline void* get(std::size_t n)
	{
		assert(n < size_);
		return blocks_[n / chunk_size_] + (n % chunk_size_) * element_size_;
	}
	inline const void* get(std::size_t n) const
	{
		assert(n < size_);
		return blocks_[n / chunk_size_] + (n % chunk_size_) * element_size_;
	}

	virtual void destroy(std::size_t n) = 0;

private:
	std::vector<char*> blocks_;
	std::size_t element_size_;
	std::size_t chunk_size_;
	std::size_t size_ = 0;
	std::size_t capacity_;
};