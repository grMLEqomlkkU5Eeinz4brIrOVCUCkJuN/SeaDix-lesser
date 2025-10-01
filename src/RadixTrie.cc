#include "RadixTrie.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <numeric>
#include <unordered_map>

RadixTrie::RadixTrie()
	: arena_(1024 * 1024), string_pool_(&arena_), word_count_(0),
	  arena_size_(1024 * 1024) {
	// Initialize string pool
	string_pool_.data.reserve(1024 * 1024);

	// Create root node
	root = std::make_unique<Node>(&arena_);
}

RadixTrie::RadixTrie(size_t arena_size)
	: arena_(arena_size), string_pool_(&arena_), word_count_(0),
	  arena_size_(arena_size) {
	// Initialize string pool
	string_pool_.data.reserve(arena_size / 2);

	// Create root node
	root = std::make_unique<Node>(&arena_);
}

// StringPool implementation
uint32_t RadixTrie::StringPool::intern(std::string_view str) {
	if (str.empty())
		return 0;

	// Create a single pmr::string key and reuse for find/emplace
	std::pmr::string key{str.begin(), str.end(), data.get_allocator().resource()};
	auto it = intern_map.find(key);
	if (it != intern_map.end()) {
		return it->second;
	}

	uint32_t offset = static_cast<uint32_t>(next_offset);
	data.resize(next_offset + str.length());
	memcpy(data.data() + next_offset, str.data(), str.length());
	next_offset += str.length();
	// Store key with move to avoid another allocation/copy
	intern_map.emplace(std::move(key), offset);
	return offset;
}

std::string_view RadixTrie::StringPool::get(uint32_t offset,
												uint16_t length) const {
	if (offset >= data.size()) {
		return {};
	}
	return std::string_view(data.data() + offset, length);
}

// Helper methods for tree traversal
RadixTrie::ChildVec::iterator RadixTrie::find_child(Node *node, char c) {
	// For small child counts, a linear scan is usually faster
	if (node->children.size() <= 8) {
		for (auto it = node->children.begin(); it != node->children.end(); ++it) {
			if (it->first == c) {
				return it;
			}
		}
		return node->children.end();
	}

	// Fall back to binary search for larger child vectors
	auto it = std::lower_bound(
		node->children.begin(), node->children.end(),
		std::make_pair(c, nullptr),
		[](const auto &a, const auto &b) { return a.first < b.first; });
	if (it != node->children.end() && it->first == c) {
		return it;
	}
	return node->children.end();
}

RadixTrie::ChildVec::const_iterator RadixTrie::find_child(const Node *node,
									  char c) {
	// Linear scan for small child counts
	if (node->children.size() <= 8) {
		for (auto it = node->children.begin(); it != node->children.end(); ++it) {
			if (it->first == c) {
				return it;
			}
		}
		return node->children.end();
	}

	// Binary search otherwise
	auto it = std::lower_bound(
		node->children.begin(), node->children.end(),
		std::make_pair(c, nullptr),
		[](const auto &a, const auto &b) { return a.first < b.first; });
	if (it != node->children.end() && it->first == c) {
		return it;
	}
	return node->children.end();
}

size_t RadixTrie::common_prefix_length(std::string_view s1,
										   std::string_view s2) const noexcept {
	size_t i = 0;
	const size_t min_len = std::min(s1.length(), s2.length());
	while (i < min_len && s1[i] == s2[i]) {
		++i;
	}
	return i;
}

RadixTrie::Node *RadixTrie::find_node(std::string_view word) const {
	if (word.empty())
		return nullptr;

	Node *current = root.get();
	size_t pos = 0;

	while (pos < word.length() && current) {
		char first_char = word[pos];
		auto it = find_child(current, first_char);

		if (it == current->children.end()) {
			return nullptr; // Path doesn't exist
		}

		Node *child = it->second.get();
		std::string_view child_key = child->get_key(string_pool_);

		if (pos + child_key.length() > word.length()) {
			return nullptr; // Child key is longer than remaining word
		}

		// Check if the child's key matches the remaining word portion
		if (word.substr(pos, child_key.length()) != child_key) {
			return nullptr; // Keys don't match
		}

		pos += child_key.length();
		current = child;
	}

	return current;
}

// Find node for prefix search - returns the node that represents the longest
// matching prefix
RadixTrie::Node *RadixTrie::find_prefix_node(std::string_view prefix) const {
	if (prefix.empty())
		return root.get();

	Node *current = root.get();
	size_t pos = 0;

	while (pos < prefix.length() && current) {
		char first_char = prefix[pos];
		auto it = find_child(current, first_char);

		if (it == current->children.end()) {
			return nullptr; // Path doesn't exist
		}

		Node *child = it->second.get();
		std::string_view child_key = child->get_key(string_pool_);

		// Check how much of the child key matches the remaining prefix
		size_t match_len = common_prefix_length(prefix.substr(pos), child_key);

		if (match_len == 0) {
			return nullptr; // No common prefix
		}

		// If we've matched the entire child key, move to the child
		if (match_len == child_key.length()) {
			pos += child_key.length();
			current = child;
		} else {
			// Partial match - we can't go further, return nullptr
			// This means the prefix doesn't match any complete path
			return nullptr;
		}
	}

	return current;
}

// Build the full prefix by traversing from the given node back to the root
std::string RadixTrie::build_prefix_from_node(const Node *node) const {
	if (!node || node == root.get()) {
		return "";
	}

	std::string result;
	const Node *current = node;

	// Traverse back to root, collecting keys
	while (current && current != root.get()) {
		std::string_view key = current->get_key(string_pool_);
		result = std::string(key) + result;
		current = current->parent;
	}

	return result;
}

// file streaming, but the user decides the size
size_t RadixTrie::bulk_insert_from_file(const std::string &path,
											size_t buffer_size) {
	std::ifstream file(path, std::ios::in);
	if (!file.is_open()) {
		throw std::runtime_error("File not found: " + path);
	}

	std::string buffer;
	buffer.reserve(buffer_size);

	size_t total_inserted = 0;
	while (std::getline(file, buffer)) {
		if (!buffer.empty()) {
			insert(buffer);
			++total_inserted;
		}
	}

	return total_inserted;
}

void RadixTrie::insert(std::string_view word) {
	if (word.empty())
		return;

	Node *current = root.get();
	size_t pos = 0;

	while (pos < word.length()) {
		char first_char = word[pos];
		auto it = find_child(current, first_char);

		if (it == current->children.end()) {
			// No child with this first character, create new node
			std::string_view remaining = word.substr(pos);
			uint32_t key_offset = string_pool_.intern(remaining);

			auto new_node = std::make_unique<Node>(
				key_offset, static_cast<uint16_t>(remaining.length()), current,
				first_char, &arena_);
			new_node->is_end = true;

			// Insert in sorted order
			auto insert_it = std::lower_bound(
				current->children.begin(), current->children.end(),
				std::make_pair(first_char, nullptr),
				[](const auto &a, const auto &b) { return a.first < b.first; });
			current->children.insert(
				insert_it, std::make_pair(first_char, std::move(new_node)));
			++word_count_;
			return;
		}

		Node *child = it->second.get();
		std::string_view child_key = child->get_key(string_pool_);
		std::string_view remaining = word.substr(pos);

		// Find common prefix
		size_t common_len = common_prefix_length(remaining, child_key);

		if (common_len == 0) {
			// No common prefix, this shouldn't happen with sorted children
			continue;
		}

		if (common_len == child_key.length()) {
			// Child key is fully consumed
			pos += child_key.length();
			current = child;

			if (pos == word.length()) {
				// We've reached the end of the word
				if (!child->is_end) {
					child->is_end = true;
					++word_count_;
				}
				return;
			}
		} else {
			// Need to split the child
			split_node(current, first_char, common_len, child_key, remaining);
			return;
		}
	}
}

void RadixTrie::split_node(Node *current, char first_char, size_t common_len,
							 std::string_view child_key,
							 std::string_view remaining) {
	// Find the child to split
	auto it = find_child(current, first_char);
	Node *child = it->second.get();

	// Create intermediate node
	std::string_view common_part = child_key.substr(0, common_len);
	uint32_t common_offset = string_pool_.intern(common_part);

	auto intermediate = std::make_unique<Node>(
		common_offset, static_cast<uint16_t>(common_len), current, first_char,
		&arena_);
	intermediate->is_end = (common_len == remaining.length());

	// Update child to have remaining part
	std::string_view child_remaining = child_key.substr(common_len);
	uint32_t child_offset = string_pool_.intern(child_remaining);

	child->key_offset = child_offset;
	child->key_length = static_cast<uint16_t>(child_remaining.length());
	child->parent = intermediate.get();
	child->parent_char = child_remaining.empty() ? '\0' : child_remaining[0];

	// Move child to intermediate
	intermediate->children.push_back(
		std::make_pair(child_remaining.empty() ? '\0' : child_remaining[0],
					   std::move(it->second)));

	// Create new node for remaining part of the word
	if (common_len < remaining.length()) {
		std::string_view new_remaining = remaining.substr(common_len);
		uint32_t new_offset = string_pool_.intern(new_remaining);

		auto new_node = std::make_unique<Node>(
			new_offset, static_cast<uint16_t>(new_remaining.length()),
			intermediate.get(),
			new_remaining.empty() ? '\0' : new_remaining[0], &arena_);
		new_node->is_end = true;
		intermediate->children.push_back(
			std::make_pair(new_remaining.empty() ? '\0' : new_remaining[0],
						   std::move(new_node)));
	}

	// Replace old child with intermediate
	it->second = std::move(intermediate);
	++word_count_;
}

bool RadixTrie::search(std::string_view word) const {
	Node *node = find_node(word);
	return node != nullptr && node->is_end;
}

bool RadixTrie::starts_with(std::string_view prefix) const {
	if (prefix.empty()) {
		return !empty();
	}

	// Use the same logic as words_with_prefix but just check if any words exist
	std::vector<std::string> result;
	collect_words_with_prefix_recursive(root.get(), "", std::string(prefix),
											result);
	return !result.empty();
}

std::vector<std::string>
RadixTrie::words_with_prefix(std::string_view prefix) const {
	std::vector<std::string> result;

	if (prefix.empty()) {
		collect_words_from_node(root.get(), "", result);
		return result;
	}

	// Use a different approach: collect all words and filter by prefix
	collect_words_with_prefix_recursive(root.get(), "", std::string(prefix),
											result);

	return result;
}

void RadixTrie::collect_words_with_prefix_recursive(
	const Node *node, const std::string &current_word,
	const std::string &prefix, std::vector<std::string> &result) const {
	if (!node)
		return;

	// Check if current word starts with the prefix
	if (current_word.length() >= prefix.length() &&
		current_word.substr(0, prefix.length()) == prefix) {
		if (node->is_end) {
			result.push_back(current_word);
		}
	}

	// Continue searching if current word is still a prefix of the target prefix
	// or if current word already starts with the target prefix
	bool should_continue = false;
	if (current_word.length() < prefix.length()) {
		// Current word is shorter than target prefix, check if it's still a
		// prefix
		should_continue =
			(prefix.substr(0, current_word.length()) == current_word);
	} else {
		// Current word is longer or equal, check if it starts with target
		// prefix
		should_continue = (current_word.substr(0, prefix.length()) == prefix);
	}

	if (should_continue) {
		for (const auto &child_pair : node->children) {
			std::string new_word =
				current_word +
				std::string(child_pair.second->get_key(string_pool_));
			collect_words_with_prefix_recursive(child_pair.second.get(),
													new_word, prefix, result);
		}
	}
}

void RadixTrie::collect_words_from_node(
	const Node *node, const std::string &prefix,
	std::vector<std::string> &result) const {
	if (!node)
		return;

	if (node->is_end) {
		result.push_back(prefix);
	}

	for (const auto &child_pair : node->children) {
		std::string new_prefix =
			prefix + std::string(child_pair.second->get_key(string_pool_));
		collect_words_from_node(child_pair.second.get(), new_prefix, result);
	}
}

bool RadixTrie::remove(std::string_view word) {
	Node *node = find_node(word);
	if (!node || !node->is_end) {
		return false;
	}

	node->is_end = false;
	--word_count_;

	// Clean up orphaned nodes
	cleanup_orphaned_nodes(node);
	return true;
}

void RadixTrie::cleanup_orphaned_nodes(Node *node) {
	if (!node || node->is_end || !node->children.empty()) {
		return; // Node is not orphaned
	}

	Node *current = node;
	// Clean up from the current node back to the root using parent pointers
	while (current != root.get()) {
		Node *parent = current->parent;
		if (!parent)
			break;

		// Find and remove this child from parent
		char char_to_remove = current->parent_char;
		auto it = find_child(parent, char_to_remove);
		if (it != parent->children.end()) {
			parent->children.erase(it);
		}

		// Move to parent and check if it can be removed
		current = parent;

		// If the current node has no children and is not an end node, it
		// can be removed, otherwise stop
		if (current->children.empty() && !current->is_end) {
			continue; // Continue cleaning up
		} else {
			break; // Stop cleaning up
		}
	}
}

bool RadixTrie::empty() const noexcept { return word_count_ == 0; }

size_t RadixTrie::size() const noexcept { return word_count_; }

void RadixTrie::clear() {
	root = std::make_unique<Node>(&arena_);
	word_count_ = 0;
	string_pool_.clear();
}

// Arena management
size_t RadixTrie::getArenaSize() const noexcept { return arena_size_; }

bool RadixTrie::setArenaSize(size_t new_size) {
	if (new_size < 1024)
		return false; // Minimum size

	arena_size_ = new_size;
	return true;
}

// Height statistics
RadixTrie::HeightStats RadixTrie::get_height_stats() const {
	HeightStats stats;
	std::vector<int> heights;

	calculate_heights_recursive(root.get(), 0, heights);

	if (heights.empty()) {
		stats.min_height = 0;
		stats.max_height = 0;
		stats.average_height = 0.0;
		stats.mode_height = 0;
		return stats;
	}

	stats.min_height = *std::min_element(heights.begin(), heights.end());
	stats.max_height = *std::max_element(heights.begin(), heights.end());
	stats.average_height =
		std::accumulate(heights.begin(), heights.end(), 0.0) / heights.size();
	stats.all_heights = heights;

	// Calculate mode
	std::unordered_map<int, int> height_counts;
	for (int h : heights) {
		height_counts[h]++;
	}

	int max_count = 0;
	for (const auto &pair : height_counts) {
		if (pair.second > max_count) {
			max_count = pair.second;
			stats.mode_height = pair.first;
		}
	}

	return stats;
}

void RadixTrie::calculate_heights_recursive(const Node *node, int current_depth,
												 std::vector<int> &heights) const {
	if (!node)
		return;

	if (node->is_end) {
		heights.push_back(current_depth);
	}

	for (const auto &child_pair : node->children) {
		calculate_heights_recursive(child_pair.second.get(), current_depth + 1,
										heights);
	}
}

// Memory statistics
RadixTrie::MemoryStats RadixTrie::get_memory_stats() const {
	MemoryStats stats;

	if (empty()) {
		stats.total_bytes = sizeof(RadixTrie) + sizeof(Node);
		stats.node_count = 1;
		stats.string_bytes = 0;
		stats.overhead_bytes = stats.total_bytes;
		stats.bytes_per_word = 0.0;
		return stats;
	}

	// Count nodes and calculate memory
	size_t node_count = calculate_memory_recursive(root.get());
	size_t string_bytes = string_pool_.next_offset;

	stats.node_count = node_count;
	stats.string_bytes = string_bytes;

	// Calculate actual memory usage more accurately
	// Only count the actual data, not the class overhead
	stats.total_bytes = node_count * sizeof(Node) + string_bytes +
						node_count * 24; // Approximate vector overhead per node
	stats.overhead_bytes = stats.total_bytes - string_bytes;
	stats.bytes_per_word = static_cast<double>(stats.total_bytes) / word_count_;

	return stats;
}

size_t RadixTrie::calculate_memory_recursive(const Node *node) const {
	if (!node)
		return 0;

	size_t count = 1; // Count this node
	for (const auto &child_pair : node->children) {
		count += calculate_memory_recursive(child_pair.second.get());
	}
	return count;
}

// Word metrics
RadixTrie::WordMetrics RadixTrie::get_word_metrics() const {
	WordMetrics metrics;
	std::vector<int> lengths;

	collect_word_lengths_recursive(root.get(), 0, lengths);

	if (lengths.empty()) {
		metrics.min_length = 0;
		metrics.max_length = 0;
		metrics.average_length = 0.0;
		metrics.mode_length = 0;
		metrics.total_characters = 0;
		return metrics;
	}

	metrics.min_length = *std::min_element(lengths.begin(), lengths.end());
	metrics.max_length = *std::max_element(lengths.begin(), lengths.end());
	metrics.average_length =
		std::accumulate(lengths.begin(), lengths.end(), 0.0) / lengths.size();
	metrics.total_characters =
		std::accumulate(lengths.begin(), lengths.end(), 0);
	metrics.length_distribution = lengths;

	// Calculate mode
	std::unordered_map<int, int> length_counts;
	for (int len : lengths) {
		length_counts[len]++;
	}

	int max_count = 0;
	for (const auto &pair : length_counts) {
		if (pair.second > max_count) {
			max_count = pair.second;
			metrics.mode_length = pair.first;
		}
	}

	return metrics;
}

void RadixTrie::collect_word_lengths_recursive(
	const Node *node, int current_length, std::vector<int> &lengths) const {
	if (!node)
		return;

	if (node->is_end) {
		lengths.push_back(current_length);
	}

	for (const auto &child_pair : node->children) {
		int new_length =
			current_length + static_cast<int>(child_pair.second->key_length);
		collect_word_lengths_recursive(child_pair.second.get(), new_length,
									   lengths);
	}
}

// Pattern matching
bool RadixTrie::matches_pattern(const std::string &word,
								  const std::string &pattern) const {
	size_t word_idx = 0, pattern_idx = 0;
	size_t word_len = word.length(), pattern_len = pattern.length();

	while (word_idx < word_len && pattern_idx < pattern_len) {
		if (pattern[pattern_idx] == '*') {
			// Handle '*'
			pattern_idx++;
			if (pattern_idx == pattern_len) {
				return true; // '*' at end matches everything
			}

			// Find next character in pattern
			char next_char = pattern[pattern_idx];
			while (word_idx < word_len && word[word_idx] != next_char) {
				word_idx++;
			}
		} else if (pattern[pattern_idx] == '?' ||
				   word[word_idx] == pattern[pattern_idx]) {
			word_idx++;
			pattern_idx++;
		} else {
			return false;
		}
	}

	// Handle trailing '*'
	while (pattern_idx < pattern_len && pattern[pattern_idx] == '*') {
		pattern_idx++;
	}

	return word_idx == word_len && pattern_idx == pattern_len;
}

// Recursive pattern matching
void RadixTrie::pattern_match_recursive(
	const Node *node, const std::string &current_word,
	const std::string &pattern, std::vector<std::string> &results) const {
	if (!node)
		return;

	std::string full_word =
		current_word + std::string(node->get_key(string_pool_));

	if (node->is_end && matches_pattern(full_word, pattern)) {
		results.push_back(full_word);
	}

	for (const auto &child_pair : node->children) {
		pattern_match_recursive(child_pair.second.get(), full_word, pattern,
								results);
	}
}

// Pattern search with wildcards (* and ?)
std::vector<std::string>
RadixTrie::pattern_search(const std::string &pattern) const {
	std::vector<std::string> results;

	if (pattern.empty() || empty()) {
		return results;
	}

	pattern_match_recursive(root.get(), "", pattern, results);

	// Sort results for consistent output
	std::sort(results.begin(), results.end());

	return results;
}

// Get all words (for arena resizing)
std::vector<std::string> RadixTrie::get_all_words() const {
	std::vector<std::string> result;
	collect_words_from_node(root.get(), "", result);
	return result;
}