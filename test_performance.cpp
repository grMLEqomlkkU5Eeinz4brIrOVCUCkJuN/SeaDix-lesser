#include "src/RadixTrie.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace std::chrono;

// Test data sets - matching benchmark.ts
vector<string> getSmallDataset() {
	return {"hello",	  "world", "test", "node",	  "javascript",
			"typescript", "react", "vue",  "angular", "express"};
}

vector<string> getMediumDataset() {
	return {"apple",	   "application",	"apply",	 "applied",
			"applicant",   "applications",	"book",		 "booking",
			"booked",	   "booker",		"bookings",	 "bookstore",
			"car",		   "card",			"care",		 "careful",
			"carefully",   "careless",		"caretaker", "dog",
			"door",		   "down",			"download",	 "downward",
			"downstairs",  "elephant",		"electric",	 "electronic",
			"electronics", "elevator",		"fish",		 "fire",
			"first",	   "firm",			"firmly",	 "firmness",
			"green",	   "great",			"ground",	 "group",
			"grow",		   "growth",		"house",	 "home",
			"hotel",	   "hospital",		"hope",		 "hopeful",
			"internet",	   "international", "interview", "interest",
			"interesting", "jump",			"just",		 "justice",
			"justify",	   "justification"};
}

vector<string> getPrefixWords() {
	return {"application", "apply", "applied", "applicant", "applications"};
}

vector<string> getNoPrefixWords() {
	return {"apple", "banana", "cherry", "date", "elderberry"};
}

vector<string> getDeepPrefixWords() {
	return {"a", "ab", "abc", "abcd", "abcde", "abcdef"};
}

vector<string> getLargeDataset() {
	vector<string> words;
	words.reserve(1000);

	vector<string> prefixes = {"app",  "book",	"car",	 "dog",	  "ele",
							   "fish", "green", "house", "inter", "jump"};
	vector<string> suffixes = {"le", "ing", "ed",	"er",	"s",
							   "ly", "ful", "ness", "tion", "ment"};

	for (int i = 0; i < 1000; i++) {
		string prefix = prefixes[i % prefixes.size()];
		string suffix = suffixes[i % suffixes.size()];

		// Generate random middle part (similar to JavaScript
		// Math.random().toString(36).substring(2, 8))
		string middle;
		for (int j = 0; j < 6; j++) {
			middle += static_cast<char>('a' + (rand() % 26));
		}

		words.push_back(prefix + middle + suffix);
	}

	return words;
}

void testInsertionPerformance() {
	cout << "=== Insertion Performance Test ===" << endl;

	// Test small dataset
	auto smallWords = getSmallDataset();
	RadixTrie smallTrie;

	auto start = high_resolution_clock::now();
	for (const auto &word : smallWords) {
		smallTrie.insert(word);
	}
	auto end = high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(end - start);
	double avgTime = static_cast<double>(duration.count()) / smallWords.size();

	cout << "Small dataset (" << smallWords.size() << " words): " << avgTime
		 << " μs per word" << endl;
	cout << "Operations per second: " << (1000000.0 / avgTime) << endl;

	// Test medium dataset
	auto mediumWords = getMediumDataset();
	RadixTrie mediumTrie;

	start = high_resolution_clock::now();
	for (const auto &word : mediumWords) {
		mediumTrie.insert(word);
	}
	end = high_resolution_clock::now();

	duration = duration_cast<microseconds>(end - start);
	avgTime = static_cast<double>(duration.count()) / mediumWords.size();

	cout << "Medium dataset (" << mediumWords.size() << " words): " << avgTime
		 << " μs per word" << endl;
	cout << "Operations per second: " << (1000000.0 / avgTime) << endl;

	// Test large dataset
	auto largeWords = getLargeDataset();
	RadixTrie largeTrie;

	start = high_resolution_clock::now();
	for (const auto &word : largeWords) {
		largeTrie.insert(word);
	}
	end = high_resolution_clock::now();

	duration = duration_cast<microseconds>(end - start);
	avgTime = static_cast<double>(duration.count()) / largeWords.size();

	cout << "Large dataset (" << largeWords.size() << " words): " << avgTime
		 << " μs per word" << endl;
	cout << "Operations per second: " << (1000000.0 / avgTime) << endl;
	cout << endl;
}

void testPrefixSharingPerformance() {
	cout << "=== Prefix Sharing Performance Test ===" << endl;

	// Test 1: No common prefixes
	auto noPrefixWords = getNoPrefixWords();
	RadixTrie trie1;

	auto start1 = high_resolution_clock::now();
	for (const auto &word : noPrefixWords) {
		trie1.insert(word);
	}
	auto end1 = high_resolution_clock::now();

	auto duration1 = duration_cast<microseconds>(end1 - start1);
	double avgTime1 =
		static_cast<double>(duration1.count()) / noPrefixWords.size();

	cout << "No common prefixes (" << noPrefixWords.size()
		 << " words): " << avgTime1 << " μs per word" << endl;

	// Test 2: Common prefixes
	auto prefixWords = getPrefixWords();
	RadixTrie trie2;

	auto start2 = high_resolution_clock::now();
	for (const auto &word : prefixWords) {
		trie2.insert(word);
	}
	auto end2 = high_resolution_clock::now();

	auto duration2 = duration_cast<microseconds>(end2 - start2);
	double avgTime2 =
		static_cast<double>(duration2.count()) / prefixWords.size();

	cout << "Common prefixes (" << prefixWords.size() << " words): " << avgTime2
		 << " μs per word" << endl;

	// Test 3: Deep prefix sharing
	auto deepPrefixWords = getDeepPrefixWords();
	RadixTrie trie3;

	auto start3 = high_resolution_clock::now();
	for (const auto &word : deepPrefixWords) {
		trie3.insert(word);
	}
	auto end3 = high_resolution_clock::now();

	auto duration3 = duration_cast<microseconds>(end3 - start3);
	double avgTime3 =
		static_cast<double>(duration3.count()) / deepPrefixWords.size();

	cout << "Deep prefix sharing (" << deepPrefixWords.size()
		 << " words): " << avgTime3 << " μs per word" << endl;

	cout << endl;
}

void testSearchPerformance() {
	cout << "=== Search Performance Test ===" << endl;

	// Test small dataset
	auto smallWords = getSmallDataset();
	RadixTrie smallTrie;
	for (const auto &word : smallWords) {
		smallTrie.insert(word);
	}

	auto start = high_resolution_clock::now();
	for (int i = 0; i < 1000; i++) {
		for (const auto &word : smallWords) {
			smallTrie.search(word);
		}
	}
	auto end = high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(end - start);
	double totalSearches = 1000.0 * smallWords.size();
	double avgTime = static_cast<double>(duration.count()) / totalSearches;

	cout << "Small dataset (" << smallWords.size() << " words): " << avgTime
		 << " μs per search" << endl;
	cout << "Searches per second: " << (1000000.0 / avgTime) << endl;

	// Test medium dataset
	auto mediumWords = getMediumDataset();
	RadixTrie mediumTrie;
	for (const auto &word : mediumWords) {
		mediumTrie.insert(word);
	}

	start = high_resolution_clock::now();
	for (int i = 0; i < 1000; i++) {
		for (const auto &word : mediumWords) {
			mediumTrie.search(word);
		}
	}
	end = high_resolution_clock::now();

	duration = duration_cast<microseconds>(end - start);
	totalSearches = 1000.0 * mediumWords.size();
	avgTime = static_cast<double>(duration.count()) / totalSearches;

	cout << "Medium dataset (" << mediumWords.size() << " words): " << avgTime
		 << " μs per search" << endl;
	cout << "Searches per second: " << (1000000.0 / avgTime) << endl;

	// Test large dataset
	auto largeWords = getLargeDataset();
	RadixTrie largeTrie;
	for (const auto &word : largeWords) {
		largeTrie.insert(word);
	}

	start = high_resolution_clock::now();
	for (int i = 0; i < 1000; i++) {
		for (const auto &word : largeWords) {
			largeTrie.search(word);
		}
	}
	end = high_resolution_clock::now();

	duration = duration_cast<microseconds>(end - start);
	totalSearches = 1000.0 * largeWords.size();
	avgTime = static_cast<double>(duration.count()) / totalSearches;

	cout << "Large dataset (" << largeWords.size() << " words): " << avgTime
		 << " μs per search" << endl;
	cout << "Searches per second: " << (1000000.0 / avgTime) << endl;
	cout << endl;
}

void testBulkInsertionPerformance() {
	cout << "=== Bulk Insertion Performance Test ===" << endl;

	// Test with the same datasets as benchmark.ts
	auto smallWords = getSmallDataset();
	auto mediumWords = getMediumDataset();
	auto largeWords = getLargeDataset();

	vector<pair<string, vector<string>>> datasets = {
		{"Small", smallWords}, {"Medium", mediumWords}, {"Large", largeWords}};

	for (const auto &dataset : datasets) {
		RadixTrie trie;

		auto start = high_resolution_clock::now();
		for (const auto &word : dataset.second) {
			trie.insert(word);
		}
		auto end = high_resolution_clock::now();

		auto duration = duration_cast<microseconds>(end - start);
		double avgTime =
			static_cast<double>(duration.count()) / dataset.second.size();

		cout << dataset.first << " dataset (" << dataset.second.size()
			 << " words): " << avgTime << " μs per word" << endl;
		cout << "Operations per second: " << (1000000.0 / avgTime) << endl;
		cout << "Trie size: " << trie.size() << endl;
		cout << endl;
	}
}

void testMemoryUsage() {
	cout << "=== Memory Usage Test ===" << endl;

	auto words = getLargeDataset();
	RadixTrie trie;

	// Insert words and measure time
	auto start = high_resolution_clock::now();
	for (const auto &word : words) {
		trie.insert(word);
	}
	auto end = high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(end - start);
	double avgTime = static_cast<double>(duration.count()) / words.size();

	cout << "Inserted " << words.size() << " words in " << duration.count()
		 << " μs" << endl;
	cout << "Average per word: " << avgTime << " μs" << endl;
	cout << "Trie size: " << trie.size() << endl;

	// Test search performance
	auto searchStart = high_resolution_clock::now();
	for (int i = 0; i < 100; i++) {
		for (const auto &word : words) {
			trie.search(word);
		}
	}
	auto searchEnd = high_resolution_clock::now();

	auto searchDuration = duration_cast<microseconds>(searchEnd - searchStart);
	double totalSearches = 100.0 * words.size();
	double avgSearchTime =
		static_cast<double>(searchDuration.count()) / totalSearches;

	cout << "Performed " << totalSearches << " searches in "
		 << searchDuration.count() << " μs" << endl;
	cout << "Average per search: " << avgSearchTime << " μs" << endl;
	cout << endl;
}

int main() {
	// Seed random number generator for consistent results
	srand(42);

	cout << "RadixTrie C++ Performance Test" << endl;
	cout << "==============================" << endl;
	cout << endl;

	testInsertionPerformance();
	testPrefixSharingPerformance();
	testSearchPerformance();
	testBulkInsertionPerformance();
	testMemoryUsage();

	cout << "Performance test completed!" << endl;

	return 0;
}
