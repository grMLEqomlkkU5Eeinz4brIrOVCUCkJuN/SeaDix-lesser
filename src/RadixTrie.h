#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <regex>
#include <functional>

class RadixTrie {
  private:
	struct Node {
		std::string key;
		bool is_end = false;
		Node *parent = nullptr;
		char parent_char = '\0';
		std::unordered_map<char, std::unique_ptr<Node>> children;

		Node() = default;
		explicit Node(std::string k) : key(std::move(k)) {}
		explicit Node(std::string k, Node *p, char pc)
			: key(std::move(k)), parent(p), parent_char(pc) {}
	};

	std::unique_ptr<Node> root;
	size_t word_count_; // Add this line to track word count

	// Helper methods
	size_t common_prefix_length(std::string_view s1,
								std::string_view s2) const noexcept;
	Node *find_node(std::string_view word) const;
	void collect_words_from_node(const Node *node, const std::string &prefix,
								 std::vector<std::string> &result) const;
	void cleanup_orphaned_nodes(std::string_view word);
	void split_node(Node *current, char first_char, size_t common_len,
					const std::string &child_key, std::string_view remaining);

	// New helper methods for analytics
	void calculate_heights_recursive(const Node* node, int current_depth,
									std::vector<int>& heights) const;
	size_t calculate_memory_recursive(const Node* node) const;
	void collect_word_lengths_recursive(const Node* node, int current_length,
										std::vector<int>& lengths) const;
	void pattern_match_recursive(const Node* node, const std::string& current_word,
								const std::string& pattern, std::vector<std::string>& results) const;
	bool matches_pattern(const std::string& word, const std::string& pattern) const;

  public:
	// Height statistics structure
	struct HeightStats {
		int min_height;
		int max_height;
		double average_height;
		int mode_height;
		std::vector<int> all_heights;
	};

	// Memory usage structure
	struct MemoryStats {
		size_t total_bytes;
		size_t node_count;
		size_t string_bytes;
		size_t overhead_bytes;
		double bytes_per_word;
	};

	// Word metrics structure
	struct WordMetrics {
		int min_length;
		int max_length;
		double average_length;
		int mode_length;
		std::vector<int> length_distribution;
		size_t total_characters;
	};

	RadixTrie();

	void insert(std::string_view word);
	bool search(std::string_view word) const;
	bool starts_with(std::string_view prefix) const;
	std::vector<std::string> words_with_prefix(std::string_view prefix) const;
	bool remove(std::string_view word);
	bool empty() const noexcept;
	size_t size() const noexcept;
	void clear();

	// Updated method with configurable buffer size
	size_t bulk_insert_from_file(const std::string &path,
								 size_t buffer_size = 1024 * 1024);

	// analytics methods
	HeightStats get_height_stats() const;
	MemoryStats get_memory_stats() const;
	WordMetrics get_word_metrics() const;
	std::vector<std::string> pattern_search(const std::string& pattern) const;
};