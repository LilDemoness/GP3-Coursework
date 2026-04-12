#pragma once

#include <functional>
#include <vector>

template <typename TReturnType, typename... Arguments>
class Event
{
public:
	typedef std::function<TReturnType(Arguments...)> event_signature;

	void subscribe(event_signature callback) { callbacks_.emplace(callbacks_.end(), callback); }
	void unsubscribe(event_signature callback) { remove_element(callbacks_, callback); }
	void unsubscribe_all() { callbacks_.clear(); }

	void invoke(Arguments... arguments)
	{
		for (unsigned int i = 0; i < callbacks_.size(); ++i)
			callbacks_[i](arguments...);
	}


private:
	std::vector<event_signature> callbacks_;


	// Event Vector Helper Functions.
	inline void remove_element(std::vector<std::function<TReturnType(Arguments...)>>& vector, std::function<TReturnType(Arguments...)>& element)
	{
		for (unsigned int i = 0; i < vector.size(); ++i)
		{
			if (&vector[i] == &element)
			{
				vector.erase(vector.begin() + i);
				return;
			}
		}
	}
};