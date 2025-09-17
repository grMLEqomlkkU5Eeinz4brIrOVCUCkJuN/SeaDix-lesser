#pragma once
#include <functional>
#include <memory>
#include <memory_resource>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

class RadixTrie {
  private:
	// Simple string pool for memory efficiency
	struct StringPool {
		std::vector<char> data;
		size_t next_offset = 0;

		uint32_t intern(std::string_view str);
		std::string_view get(uint32_t offset, uint16_t length) const;
		void clear() {
			data.clear();
			next_offset = 0;
		}
	};

	// Optimized Node structure - much more compact
	struct Node {
		uint32_t key_offset;	 // 4 bytes - offset in string pool
		uint16_t key_length;	 // 2 bytes - length of key
		bool is_end = false;	 // 1 byte
		Node *parent = nullptr;	 // 8 bytes
		char parent_char = '\0'; // 1 byte
		std::vector<std::pair<char, std::unique_ptr<Node>>>
			children; // Sorted vector

		Node() : key_offset(0), key_length(0) {}
		explicit Node(uint32_t offset, uint16_t length)
			: key_offset(offset), key_length(length) {}
		explicit Node(uint32_t offset, uint16_t length, Node *p, char pc)
			: key_offset(offset), key_length(length), parent(p),
			  parent_char(pc) {}

		std::string_view get_key(const StringPool &pool) const {
			return pool.get(key_offset, key_length);
		}
	};

	std::pmr::monotonic_buffer_resource arena_;
	StringPool string_pool_;
	std::unique_ptr<Node> root;
	size_t word_count_;
	size_t arena_size_; // Store arena size for getArenaSize()

	using ChildVec = std::vector<std::pair<char, std::unique_ptr<Node>>>;
	static ChildVec::iterator find_child(Node *node, char c);
	static ChildVec::const_iterator find_child(const Node *node, char c);

	// Helper methods
	size_t common_prefix_length(std::string_view s1,
								std::string_view s2) const noexcept;
	Node *find_node(std::string_view word) const;
	void collect_words_from_node(const Node *node, const std::string &prefix,
								 std::vector<std::string> &result) const;
	void cleanup_orphaned_nodes(std::string_view word);
	void split_node(Node *current, char first_char, size_t common_len,
					std::string_view child_key, std::string_view remaining);

	// New helper methods for analytics
	void calculate_heights_recursive(const Node *node, int current_depth,
									 std::vector<int> &heights) const;
	size_t calculate_memory_recursive(const Node *node) const;
	void collect_word_lengths_recursive(const Node *node, int current_length,
										std::vector<int> &lengths) const;
	void pattern_match_recursive(const Node *node,
								 const std::string &current_word,
								 const std::string &pattern,
								 std::vector<std::string> &results) const;
	bool matches_pattern(const std::string &word,
						 const std::string &pattern) const;

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
	explicit RadixTrie(size_t arena_size);
	~RadixTrie() = default;

	void insert(std::string_view word);
	bool search(std::string_view word) const;
	bool starts_with(std::string_view prefix) const;
	std::vector<std::string> words_with_prefix(std::string_view prefix) const;
	bool remove(std::string_view word);
	bool empty() const noexcept;
	size_t size() const noexcept;
	void clear();

	// Arena management
	size_t getArenaSize() const noexcept;
	bool setArenaSize(size_t new_size);

	// Get all words (for arena resizing)
	std::vector<std::string> get_all_words() const;

	// Updated method with configurable buffer size
	size_t bulk_insert_from_file(const std::string &path,
								 size_t buffer_size = 1024 * 1024);

	// analytics methods
	HeightStats get_height_stats() const;
	MemoryStats get_memory_stats() const;
	WordMetrics get_word_metrics() const;
	std::vector<std::string> pattern_search(const std::string &pattern) const;
};