#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

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

  public:
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
};