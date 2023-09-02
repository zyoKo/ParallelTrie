#pragma once

#include <string>
#include <vector>

#include "Types/TrieNode.h"

struct TrieNode;

class Trie
{
public:
    Trie();

    ~Trie();

    void InsertWord(const std::string& word) const;

    void InsertListOfWords(const std::vector<std::string>& wordList) const;

    void ParallelInsertListOfWords(const std::vector<std::string>& wordList) const;

    bool Lookup(const std::string& word) const;

    void ParallelLookup(const std::vector<std::string>& wordList) const;

    bool DeleteWord(const std::string& word) const;

    void ParallelDeleteWords(const std::vector<std::string>& word) const;

    void ClearTrie();

    TrieNode* GetRoot() const;

    int GetSize() const;

private:
    void ParallelInsertListOfWordsAux(const std::vector<std::string>& wordList, int rangeSize, int index) const;

    void ParallelLookupAux(const std::vector<std::string>& wordList, int rangeSize, int index) const;

	int Size(TrieNode* node) const;

    TrieNode* root_;
};
