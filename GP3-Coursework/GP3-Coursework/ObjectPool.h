#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <iostream>

template <typename T>
class ObjectPool
{
public:
	ObjectPool() :
		head_(NULL),
		create_func_(nullptr),
		on_get_func_(nullptr),
		on_release_func_(nullptr),
		on_delete_func_(nullptr)
	{}
	ObjectPool(std::function<std::shared_ptr<T>()> create_func,
			std::function<void(std::shared_ptr<T>)> on_get_func = nullptr,
			std::function<void(std::shared_ptr<T>)> on_release_func = nullptr,
			std::function<void(std::shared_ptr<T>)> on_delete_func = nullptr)
		:
		head_(NULL),
		create_func_(create_func),
		on_get_func_(on_get_func),
		on_release_func_(on_release_func),
		on_delete_func_(on_delete_func)
	{ }

	~ObjectPool()
	{
		// Delete our Pool.
		clear();
	}

	std::shared_ptr<T> get()
	{
		std::shared_ptr<T> element;

		if (head_ != NULL)
		{
			// Return the first object.
			element = head_->element_;

			if (head_->next_ == NULL)	// Only item in pool.
			{
				delete head_;	// Erase previous data.
				head_ = NULL;	// Our pool is now empty.
			}
			else
			{
				ObjectNode* new_head = head_->next_;
				delete head_;
				head_ = new_head;	// Move everything down.
			}
		}
		else
		{
			// Our pool is empty. Create and return a new element.
			element = create_func_();
		}

		if (on_get_func_ != nullptr)
			on_get_func_(element);
		return element;
	}

	void release(std::shared_ptr<T> element)
	{
		// Return the element to the pool.
		ObjectNode* node = new ObjectNode(element);
		if (head_ != NULL)
		{
			// For ease, add the node to the start of our pool.
			node->next_ = head_;
		}
		else
		{
			// The Pool was empty, so this is the only element.
			node->next_ = NULL;
		}
		head_ = node;

		// Reset the element.
		//	Performed after returning to the pool to prevent shared_ptr from disposing it.
		if (on_release_func_ != nullptr)
			on_release_func_(element);
	}
	void release(T* element) { release(std::shared_ptr<Projectile>(element)); }
	

	void clear()
	{
		while (head_ != NULL)
		{
			if (on_delete_func_ != nullptr)
				on_delete_func_(head_->element_);

			ObjectNode* next = head_->next_;
			delete head_;
			head_ = next;
		}
		head_ = NULL;
	}

private:
	struct ObjectNode
	{
		std::shared_ptr<T> element_;
		ObjectNode* next_;

		ObjectNode(std::shared_ptr<T> element) : 
			element_(element),
			next_(NULL)
		{ }
	};
	ObjectNode* head_;

	//std::shared_ptr<T>(&create_func_)() = nullptr;
	std::function<std::shared_ptr<T>()> create_func_;
	std::function<void(std::shared_ptr<T>)> on_get_func_;
	std::function<void(std::shared_ptr<T>)> on_release_func_;
	std::function<void(std::shared_ptr<T>)> on_delete_func_;
};