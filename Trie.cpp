#include "Trie.h"

#include "Data/Constants.h"
#include "Types/TrieNode.h"

#include <thread>
#include <atomic>
#include <stack>
#include <shared_mutex>
#include <mutex>

std::shared_mutex trie_shared_mutex;
std::mutex trie_mutex;
int count_of_Created_TrieNodes{ 0 };
std::atomic<int> atomic_count_of_Created_TrieNodes{ 0 };

Trie::Trie()
	:   root_(new TrieNode())
{
}

Trie::~Trie()
{
    printf("Clearing Tree!\n");
    //ClearTrie();
    printf("Tree Successfully cleared!\n");
}

void Trie::InsertWord(const std::string& word) const
{
    TrieNode* current_node = root_;

    for (const char c : word) 
    {
        const int index = c - 'a';
        if (current_node->children[index] == nullptr) 
        {
            current_node->children[index] = new TrieNode();
            //++count_of_Created_TrieNodes;
        }
        current_node = current_node->children[index];
    }

    current_node->is_end_of_word = true;
}

void Trie::InsertListOfWords(const std::vector<std::string>& wordList) const
{
    for (auto& word : wordList)
        InsertWord(word);

    //printf("Count of Created Trie Nodes: %d\n", count_of_Created_TrieNodes);
}

void Trie::ParallelInsertListOfWords(const std::vector<std::string>& wordList) const
{
    std::vector<std::thread> threads;
    threads.reserve(NUM_OF_THREADS);

    int rangeSize = wordList.size() / NUM_OF_THREADS;

    for (unsigned i = 0; i < NUM_OF_THREADS; ++i)
        threads.emplace_back(&Trie::ParallelInsertListOfWordsAux, this, std::ref(wordList), rangeSize, i);

    for (auto& thread : threads)
        thread.join();

    printf("Number of Inserted Words: %d\n", count_of_Created_TrieNodes);

    //printf("Count of Created Trie Nodes: %d\n", atomic_count_of_Created_TrieNodes.load());
}

void Trie::ParallelInsertListOfWordsAux(const std::vector<std::string>& wordList, int rangeSize, int index) const
{
    const int start = rangeSize * index;
    int end = rangeSize * (index + 1);

    if (index == NUM_OF_THREADS - 1)
        end = wordList.size();

    for (int i = start; i < end; ++i)
    {
        TrieNode* current_node = root_;

        for (const char c : wordList[i])
        {
            const int letterIndex = c - 'a';

            std::unique_lock<std::mutex> lock(trie_mutex);
            if (current_node->children[letterIndex] == nullptr)
            {
                current_node->children[letterIndex] = new TrieNode();
                //++count_of_Created_TrieNodes;
            }
            lock.unlock();

            current_node = current_node->children[letterIndex];
        }

        current_node->is_end_of_word = true;
        //++atomic_count_of_Created_TrieNodes;
    }
}

bool Trie::Lookup(const std::string& word) const
{
    const TrieNode* current_node = root_;

    for (const char c : word)
    {
        const int index = c - 'a';
        if (current_node->children[index] == nullptr)
        {
            return false;
        }
        current_node = current_node->children[index];
    }

    return current_node->is_end_of_word;
}

void Trie::ParallelLookup(const std::vector<std::string>& wordList) const
{
    std::vector<std::thread> threads;
    threads.reserve(NUM_OF_THREADS);

    int rangeSize = wordList.size() / NUM_OF_THREADS;

    for (unsigned i = 0; i < NUM_OF_THREADS; ++i)
        threads.emplace_back(&Trie::ParallelLookupAux, this, std::ref(wordList), rangeSize, i);

    for (auto& thread : threads)
        thread.join();
}

void Trie::ParallelLookupAux(const std::vector<std::string>& wordList, int rangeSize, int index) const
{
    const int start = rangeSize * index;
    int end = rangeSize * (index + 1);

    if (index == NUM_OF_THREADS - 1)
        end = wordList.size();

    for (int i = start; i < end; ++i)
    {
        const TrieNode* current_node = root_;
        bool found{ true };

        for (const auto c : wordList[i])
        {
            const int letterIndex = c - 'a';

        	if (current_node->children[letterIndex] == nullptr)
            {
                found = false;
                break;
            }

            current_node = current_node->children[letterIndex];
        }

        //printf("Word %s was %s found!\n", wordList[i].c_str(), found ? "" : "not");
    }
}

bool Trie::DeleteWord(const std::string& word) const
{
    TrieNode* current = root_;

    std::vector<TrieNode*> trieNodes;
    for (const char c : word) 
    {
        const int index = c - 'a';
        if (!current->children[index]) 
            return false;
        
        trieNodes.push_back(current);
        current = current->children[index];
    }

    if (!current->is_end_of_word)
    {
        printf("Word: %s is not part of this database!\n", word.c_str());
        return false;
    }

    // Set the current to false
    current->is_end_of_word = false;

    // if the node has children, no need to do delete operation
    if (current->hasChildren())
        return true;

    // traverse the trie backwards and keep deleting nodes until you find either of one
    // 1. it has children
    // 2. or if it is an end of any other word
    for (int i = static_cast<int>(trieNodes.size()) - 1; i >= 0; --i) 
    {
        TrieNode* parentNode = trieNodes[i];
        const char c = word[i];

        const int index = c - 'a';
    	const TrieNode* child = parentNode->children[index];

        if (child->hasChildren() || child->is_end_of_word)
            return true;

        parentNode->children[index] = nullptr;
        delete child;
    }

    return true;
}

void Trie::ParallelDeleteWords(const std::vector<std::string>& words) const
{
    // Split the words vector into chunks
    std::vector<std::vector<std::string>> wordChunks(NUM_OF_THREADS);
    const int chunkSize = words.size() / NUM_OF_THREADS;
    int remainder = words.size() % NUM_OF_THREADS;
    int start_index = 0;

    for (unsigned i = 0; i < NUM_OF_THREADS; ++i)
    {
        int end_index = start_index + chunkSize;
    	if (i < remainder)
            end_index++;
        wordChunks[i] = std::vector<std::string>(words.begin() + start_index, words.begin() + end_index);
    	start_index = end_index;
    }

    // Spawn threads to process chunks
    std::vector<std::thread> threads(NUM_OF_THREADS);
    for (unsigned i = 0; i < NUM_OF_THREADS; ++i)
    {
        threads[i] = std::thread([&wordChunks, this, i] {
            for (const std::string& word : wordChunks[i])
            {
                // Lock before deleting word
                std::unique_lock<std::mutex> lock(root_->mutex);

                TrieNode* current = root_;

                std::vector<TrieNode*> trieNodes;
                for (const char c : word)
                {
                    const int index = c - 'a';
                    if (!current->children[index])
                        break;

                    trieNodes.push_back(current);
                    current = current->children[index];
                }

                if (!current->is_end_of_word)
                    continue;

                // Set the current to false
                current->is_end_of_word = false;

                // if the node has children, no need to do delete operation
                if (current->hasChildren())
                    continue;

                // traverse the trie backwards and keep deleting nodes until you find either of one
                // 1. it has children
                // 2. or if it is an end of any other word
                for (int j = static_cast<int>(trieNodes.size()) - 1; j >= 0; --j)
                {
                    TrieNode* parentNode = trieNodes[j];
                    const char c = word[j];

                    const int index = c - 'a';
                    TrieNode* child = parentNode->children[index];

                    if (child->hasChildren() || child->is_end_of_word)
                        break;

                    // Lock before deleting node
                    std::unique_lock<std::mutex> lock2(child->mutex);

                    parentNode->children[index] = nullptr;
                    delete child;
                }
            }
        });
    }

    for (auto& thread : threads)
        thread.join();
}

void Trie::ClearTrie()
{
    std::stack<TrieNode*> allTrieNodes;
    allTrieNodes.push(root_);

    while (!allTrieNodes.empty())
    {
        TrieNode* current = allTrieNodes.top();
        allTrieNodes.pop();

        for (TrieNode* child : current->children)
        {
            if (child)
                allTrieNodes.push(child);
        }

        delete current;
    }

    root_ = new TrieNode();
}

TrieNode* Trie::GetRoot() const
{
    return root_;
}

int Trie::GetSize() const
{
    return Size(root_);
}

int Trie::Size(TrieNode* node) const
{
	if (node == nullptr) 
    {
		return 0;
	}

	int count = 1;

    for (TrieNode* child : node->children)
    {
        count += Size(child);
    }

    return count;
}
