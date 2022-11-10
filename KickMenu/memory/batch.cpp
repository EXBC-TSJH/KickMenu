#include "../common.hpp"
#include "batch.hpp"
#include "range.hpp"

namespace memory
{
	void batch::add(std::string name, pattern pattern, std::function<void(handle)> callback)
	{
		m_entries.emplace_back(std::move(name), std::move(pattern), std::move(callback));
	}

	void batch::run(range region)
	{
		bool all_found = true;
		for (auto& entry : m_entries)
		{
			if (auto result = region.scan(entry.m_pattern))
			{
				if (entry.m_callback)
				{
					std::invoke(std::move(entry.m_callback), result);
				}
				else
				{
					all_found = false;
					MessageBoxA(NULL, entry.m_name.c_str(), "Failed to find", MB_OK);
				}
			}
			else
			{
				all_found = false;
				MessageBoxA(NULL, entry.m_name.c_str(), "Failed to find", MB_OK);
			}
		}

		m_entries.clear();
		if (!all_found)
		{
			throw std::runtime_error("Failed to find some patterns.");
		}
	}
}
