#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <chrono>

#include "Timer.h"
#include "Trie.h"
#include "Data/Constants.h"

#define PARALLEL
//#define SERIAL

std::string GenerateRandomWord(int length)
{
	std::string word(length, ' ');

	for (int i = 0; i < length; ++i)
		word[i] = ALPHABETS[rand() % (sizeof(ALPHABETS) - 1)];

	return word;
}

void CreateABigFileWithLotsOfWords(const std::string& filename)
{
	srand(time(nullptr));
	std::ofstream outfile(filename);

	for (int i = 0; i < MAX_WORDS; ++i)
		outfile << GenerateRandomWord(WORD_LENGTH) << "\n";

	outfile.close();
}

void ReadDataFromFile(const std::string& filename, std::vector<std::string>& data, std::vector<std::string>& lookup)
{
	std::ifstream file(filename);

	data.reserve(MAX_WORDS);
	lookup.reserve(MAX_WORDS / 2);

	if (file.is_open()) {
		int i = 0;
		std::string line;
		while (std::getline(file, line)) {
			data.push_back(line);
			if (i % 2 == 0) {
				lookup.push_back(line);
			}
			++i;
		}
		file.close();

		printf("Finished reading file!\n");
	}
	else {
		std::cout << "Unable to open file." << std::endl;
	}
}

void TestSerialExecution(const std::vector<std::string>& strings, const std::vector<std::string>& lookupWords)
{
	const Trie trie;
	printf("BEGIN INSERTION\n");
	{
		Timer stopwatch;
		trie.InsertListOfWords(strings);
	}
	printf("END INSERTION\n");

	// WORDS: Serial Deletion -----------------//
	std::cout << "\n-- [Serialized] Deletion Started ----\n" << std::endl;
	{
		Timer stopwatch;
		for (const auto& word : lookupWords)
			bool found = trie.DeleteWord(word);
	}
	std::cout << "\n-- Deletion Done --------\n" << std::endl;

	// SAFETY CHECK: Did we miss any words --//
	printf("LOOKUP STARTED\n");
	{
		Timer stopwatch;
		for (const auto& word : lookupWords)
		{
			const bool found = trie.Lookup(word);
			if (found)
				std::cout << "The word: [" << word << "] was " << "FOUND!" << std::endl;
		}
	}
	printf("LOOKUP DONE\n");
}

void TestParallelExecution(const std::vector<std::string>& strings, const std::vector<std::string>& lookupWords)
{
	const Trie trie;
	printf("BEGIN INSERTION\n");
	{
		Timer timer;
		trie.ParallelInsertListOfWords(strings);
	}
	printf("END INSERTION\n");

	// WORDS: Parallel Deletion -----------------//
	/*{
		Timer stopwatch;
		trie.ParallelLookup(strings);
	}*/

	std::cout << "\n-- Deletion Start ----\n" << std::endl;
	{
		Timer stopwatch;
		trie.ParallelDeleteWords(lookupWords);
	}
	std::cout << "\n-- Deletion Done --------\n" << std::endl;

	{
		Timer stopwatch;
		trie.ParallelLookup(strings);
	}

	/*for (const auto& word : lookupWords)
	{
		const bool found = trie.Lookup(word);
		if (found)
			std::cout << "The word: [" << word << "] was " << "FOUND!" << std::endl;
	}*/
}

int main()
{
	//CreateABigFileWithLotsOfWords(ONE_MILLION_WORDS);

	std::vector<std::string> strings;
	std::vector<std::string> lookupWord;
	ReadDataFromFile(ONE_MILLION_WORDS, strings, lookupWord);

#ifdef PARALLEL
	{
		printf("\n-- Starting Parallel Test ----------\n");
		TestParallelExecution(strings, lookupWord);
		printf("\n-- Ending Parallel Test ------------\n");
	}
#endif

#ifdef SERIAL
	{
		printf("\n-- Starting Serial Test ----------\n");
		TestSerialExecution(strings, lookupWord);
		printf("\n-- Ending Serial Test ------------\n");
	}
#endif

	return 0;
}






























//#include <algorithm>
//#include <iostream>
//#include <vector>
//#include <thread>
//#include <string>
//#include <fstream>
//#include <mutex>
//#include <functional>
//#include <atomic>
//
//static constexpr unsigned ALPHABET_SIZE = 26;
//
//class TrieNode
//{
//public:
//    TrieNode* children[ALPHABET_SIZE];
//    std::atomic<bool> isEndOfWord;
//
//	std::mutex nodeMutex;
//
//    TrieNode()
//	{
//        for (auto& child : children)
//	        child = nullptr;
//
//        isEndOfWord.store(false);
//    }
//};
//
//class ParallelTrie
//{
//    TrieNode* root;
//
//public:
//    ParallelTrie() {
//        root = new TrieNode();
//    }
//
//    void Insert(const std::string& word) const
//	{
//        TrieNode* current = root;
//
//        for (const char i : word)
//        {
//            current->nodeMutex.lock();
//            const int index = i - 'a';
//            if (!current->children[index])
//            {
//                current->children[index] = new TrieNode();
//				current->nodeMutex.unlock();
//            }
//            else
//            {
//				current->nodeMutex.unlock();
//            }
//
//            current = current->children[index];
//        }
//
//        // mark last node as leaf
//        current->isEndOfWord.store(true);
//    }
//
//    bool search(const std::string& word) const
//    {
//        TrieNode* pCrawl = root;
//
//        for (const char i : word)
//        {
//            const int index = i - 'a';
//            if (!pCrawl->children[index])
//                return false;
//
//            pCrawl = pCrawl->children[index];
//        }
//
//        return pCrawl != nullptr && pCrawl->isEndOfWord.load();
//    }
//
//    bool isEmpty(TrieNode* root)
//    {
//        for (const auto& child : root->children)
//        {
//	        if (child)
//		        return false;
//        }
//        return true;
//    }
//
//    // Recursive function to delete a key from given Trie
//    TrieNode* remove(TrieNode* root, std::string key, int depth = 0)
//    {
//        // If tree is empty
//        if (!root)
//            return nullptr;
//
//        // If last character of key is being processed
//        if (depth == key.size()) 
//        {
//            // This node is no more end of word after
//            // removal of given key
//            if (root->isEndOfWord)
//                root->isEndOfWord = false;
//
//            // If given is not prefix of any other word
//            if (isEmpty(root)) {
//                delete root;
//                root = nullptr;
//            }
//
//            return root;
//        }
//
//        // If not last character, recur for the child
//        // obtained using ASCII value
//        int index = key[depth] - 'a';
//        root->children[index] = remove(root->children[index], key, depth + 1);
//
//        // If root does not have any child (its only child got
//        // deleted), and it is not end of another word.
//        if (isEmpty(root) && root->isEndOfWord == false) 
//        {
//            delete root;
//            root = nullptr;
//        }
//
//        return root;
//    }
//};
//
//void insertWords(ParallelTrie& trie, std::vector<std::string>& words)
//{
//    for (std::string& word : words)
//        trie.InsertListOfWords(word);
//}
//
//void searchWords(ParallelTrie& trie, std::vector<std::string>& words)
//{
//    int count = 0;
//    for (const std::string& word : words)
//    {
//        if (trie.search(word))
//        {
//			++count;
//        }
//    }
//
//    std::cout << "\nTotal Count: " << count << std::endl;
//}
//
//class Timer
//{
//    std::chrono::time_point<std::chrono::steady_clock> start;
//
//public:
//    void Start()
//    {
//        start = std::chrono::steady_clock::now();
//    }
//
//    void Stop(std::string str = "")
//    {
//        auto end = std::chrono::steady_clock::now();
//        const auto diff = end - start;
//        std::cout << "Time taken for " << str << ": " << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;
//    }
//};
//
//int main()
//{
//	std::ifstream file("./src/words_alpha.txt"); // Open the file for reading
//	std::vector<std::string> strings; // Create an empty vector to store the strings
//
//    if (file.is_open()) { // Check if the file is open
//	    std::string line;
//        while (std::getline(file, line)) { // Read each line of the file
//            strings.push_back(line); // Insert the line into the vector
//        }
//        file.close(); // Close the file
//    }
//    else {
//	    std::cout << "Unable to open file." << std::endl; // Display an error message if the file can't be opened
//    }
//
//    ParallelTrie trie;
//    Timer timer;
//
//	std::vector<std::thread> threads;
//
//    timer.Start();
//    for (int i = 0; i < 2; ++i)
//        threads.push_back(std::thread(insertWords, std::ref(trie), std::ref(strings)));
//
//    for (auto& thread : threads)
//        thread.join();
//    timer.Stop("Parallel");
//
//    searchWords(trie, strings);
//
//    /*timer.Start();
//    insertWords(trie, strings);
//    timer.Stop("Serialized");*/
//
//    threads.clear();
//
//    return 0;
//}
