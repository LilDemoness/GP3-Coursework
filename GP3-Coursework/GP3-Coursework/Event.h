#pragma once

#include <functional>
#include <vector>


template <typename... Arguments>
class Event
{
public:
	typedef std::function<void(Arguments...)> event_signature;

	Event() : callbacks_(std::vector<event_signature>())
	{}
	~Event()
	{
		unsubscribe_all();
	}

	void subscribe(event_signature callback) { callbacks_.emplace_back(callback); }
	void unsubscribe(event_signature callback) { remove_element(callbacks_, callback); }
	void unsubscribe_all() { callbacks_.clear(); }

	void invoke(Arguments... arguments)
	{
		for (unsigned int i = 0; i < callbacks_.size(); ++i)
			callbacks_[i](arguments...);
	}

	inline const bool has_listeners() const { return callbacks_.size() > 0; }


private:
	std::vector<event_signature> callbacks_;


	// Event Vector Helper Functions.
	inline void remove_element(std::vector<std::function<void(Arguments...)>>& vector, std::function<void(Arguments...)>& element)
	{
		for (unsigned int i = 0; i < vector.size(); ++i)
		{
			if (vector[i].target_type() == element.target_type())
			{
				vector.erase(vector.begin() + i);
				return;
			}
		}
	}
};