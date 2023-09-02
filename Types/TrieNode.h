#pragma once

#include <mutex>

struct TrieNode
{
	TrieNode* children[26];

	bool is_end_of_word;

	std::mutex mutex;

	TrieNode()
		:	children{ nullptr },
			is_end_of_word{ false }
	{

	}

	bool hasChildren() const
	{
		for (const auto& child : children)
		{
			if (child)
				return true;
		}

		return false;
	}
};

//#include <set>
//
//struct NewTrieNode
//{
//	unsigned children[26];
//
//	bool is_end_of_word;
//
//	NewTrieNode()
//		:	children{ std::numeric_limits<unsigned int>::max() },
//			is_end_of_word{ false }
//	{
//		
//	}
//};
//
//class MyNewTrie
//{
//public:
//	MyNewTrie() = default;
//
//	size_t Insert(const std::string& word)
//	{
//		for (const auto c : word)
//		{
//			int letter = c - 'a';
//
//			if (markers.empty())
//			{
//				root.emplace_back(NewTrieNode());
//				return root.size() - 1;
//			}
//			else
//			{
//				size_t index = *markers.begin();
//				markers.erase(markers.begin());
//				root[index] = NewTrieNode();
//				return index;
//			}
//		}
//
//		return 0;
//	}
//
//private:
//	std::vector<NewTrieNode> root;
//
//	std::set<size_t> markers;
//};