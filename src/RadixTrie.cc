#include "RadixTrie.h"
#include <algorithm>

RadixTrie::RadixTrie() : root(std::make_unique<Node>()), word_count_(0) {}

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
	if (!root || word.empty())
		return nullptr;

	Node *current = root.get();
	size_t pos = 0;

	while (pos < word.length()) {
		char first_char = word[pos];
		auto it = current->children.find(first_char);

		if (it == current->children.end()) {
			return nullptr; // Path doesn't exist
		}

		Node *child = it->second.get();
		const std::string &child_key = child->key;

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

void RadixTrie::collect_words_from_node(
	const Node *node, const std::string &prefix,
	std::vector<std::string> &result) const {
	if (!node)
		return;

	// Use more efficient string concatenation
	std::string full_word;
	full_word.reserve(prefix.length() + node->key.length());
	full_word = prefix;
	full_word += node->key;

	if (node->is_end) {
		result.push_back(full_word);
	}

	// Pre-calculate size for better performance
	if (!node->children.empty()) {
		for (const auto &[ch, child] : node->children) {
			collect_words_from_node(child.get(), full_word, result);
		}
	}
}

void RadixTrie::insert(std::string_view word) {
	if (word.empty())
		return;

	Node *current = root.get();
	size_t pos = 0;

	while (pos < word.length()) {
		char first_char = word[pos];
		auto it = current->children.find(first_char);

		if (it == current->children.end()) {
			// No child with this first character, create new node
			auto new_node = std::make_unique<Node>(
				std::string(word.data() + pos, word.length() - pos), current,
				first_char);
			new_node->is_end = true;
			current->children[first_char] = std::move(new_node);
			++word_count_;
			return;
		}

		Node *child = it->second.get();
		const std::string &child_key = child->key;
		std::string_view remaining(word.data() + pos, word.length() - pos);

		size_t common_len = common_prefix_length(child_key, remaining);

		if (common_len == child_key.length()) {
			// The child's key is a complete prefix of the remaining word
			pos += common_len;
			current = child;

			if (pos == word.length()) {
				// Word ends here
				if (!child->is_end) {
					child->is_end = true;
					++word_count_;
				}
				return;
			}
		} else {
			// Need to split the child node - use helper method
			split_node(current, first_char, common_len, child_key, remaining);
			pos += common_len;
			current = current->children[first_char].get();

			if (pos == word.length()) {
				// Word ends at the intermediate node
				if (!current->is_end) {
					current->is_end = true;
					++word_count_;
				}
				return;
			}
		}
	}
}

bool RadixTrie::search(std::string_view word) const {
	Node *node = find_node(word);
	return node != nullptr && node->is_end;
}

bool RadixTrie::starts_with(std::string_view prefix) const {
	if (prefix.empty()) {
		return !empty();
	}

	Node *current = root.get();
	size_t pos = 0;

	while (pos < prefix.length() && current) {
		char first_char = prefix[pos];
		auto it = current->children.find(first_char);

		if (it == current->children.end()) {
			return false; // Path doesn't exist
		}

		Node *child = it->second.get();
		const std::string &child_key = child->key;

		if (pos + child_key.length() > prefix.length()) {
			// Child key is longer than remaining prefix
			// Check if the prefix matches the beginning of the child key
			return prefix.substr(pos) ==
				   child_key.substr(0, prefix.length() - pos);
		}

		// Check if the child's key matches the remaining prefix portion
		if (prefix.substr(pos, child_key.length()) != child_key) {
			return false; // Keys don't match
		}

		pos += child_key.length();
		current = child;
	}

	// If we've consumed the entire prefix, it's a valid prefix
	return pos == prefix.length();
}

std::vector<std::string>
RadixTrie::words_with_prefix(std::string_view prefix) const {
	std::vector<std::string> result;

	if (prefix.empty()) {
		// Return all words in the trie
		collect_words_from_node(root.get(), "", result);
		return result;
	}

	// Find the node that represents the prefix
	Node *current = root.get();
	size_t pos = 0;

	while (pos < prefix.length() && current) {
		char first_char = prefix[pos];
		auto it = current->children.find(first_char);

		if (it == current->children.end()) {
			return result; // Prefix not found
		}

		Node *child = it->second.get();
		const std::string &child_key = child->key;

		if (pos + child_key.length() > prefix.length()) {
			// Child key is longer than remaining prefix
			if (prefix.substr(pos) ==
				child_key.substr(0, prefix.length() - pos)) {
				// The prefix ends in the middle of this child's key
				// Collect all words from this child with the correct prefix
				collect_words_from_node(
					child, std::string(prefix.substr(0, pos)), result);
			}
			return result;
		}

		// Check if the child's key matches the remaining prefix portion
		if (prefix.substr(pos, child_key.length()) != child_key) {
			return result; // Keys don't match
		}

		pos += child_key.length();
		current = child;
	}

	// If we've consumed the entire prefix, collect from the current node
	if (pos == prefix.length() && current) {
		// Calculate the prefix up to (but not including) the current node
		std::string base_prefix(prefix);
		if (!current->key.empty() &&
			base_prefix.length() >= current->key.length()) {
			base_prefix.resize(base_prefix.length() - current->key.length());
		}
		collect_words_from_node(current, base_prefix, result);
	}

	return result;
}

void RadixTrie::cleanup_orphaned_nodes(std::string_view word) {
	if (word.empty() || !root)
		return;

	Node *current = root.get();
	size_t pos = 0;

	// Find the node to delete
	while (pos < word.length() && current) {
		char first_char = word[pos];
		auto it = current->children.find(first_char);

		if (it == current->children.end()) {
			return; // Path doesn't exist
		}

		Node *child = it->second.get();
		const std::string &child_key = child->key;

		if (pos + child_key.length() > word.length()) {
			return; // Child key is longer than remaining word
		}

		// Check if the child's key matches the remaining word portion
		if (word.substr(pos, child_key.length()) != child_key) {
			return; // Keys don't match
		}

		pos += child_key.length();
		current = child;
	}

	// Only clean up if we found the exact node and it's marked as end
	if (current && current->is_end) {
		// Clean up from the current node back to the root using parent pointers
		while (current && current->parent) {
			Node *parent = current->parent;
			char char_to_remove = current->parent_char;

			// If the current node has no children and is not an end node, it
			// can be removed
			if (current->children.empty() && !current->is_end) {
				parent->children.erase(char_to_remove);
				current = parent; // Move up to parent
			} else {
				// If this node has children or is an end node, stop cleanup
				break;
			}
		}
	}
}

bool RadixTrie::remove(std::string_view word) {
	if (word.empty() || !root)
		return false;

	Node *node = find_node(word);
	if (node && node->is_end) {
		node->is_end = false;
		--word_count_; // Decrement counter

		// Clean up orphaned nodes
		cleanup_orphaned_nodes(word);
		return true;
	}
	return false;
}

bool RadixTrie::empty() const noexcept { return word_count_ == 0; }

size_t RadixTrie::size() const noexcept { return word_count_; }

void RadixTrie::clear() {
	root = std::make_unique<Node>();
	word_count_ = 0; // Reset counter
}

void RadixTrie::split_node(Node *current, char first_char, size_t common_len,
						   const std::string &child_key,
						   std::string_view /* remaining */) {
	// Get the old child before moving it
	auto old_child = std::move(current->children[first_char]);
	
	// Create intermediate node with common prefix (use move semantics)
	std::string common_prefix(child_key.data(), common_len);
	auto intermediate = std::make_unique<Node>(std::move(common_prefix), current, first_char);

	// Update child's key to remaining part using more efficient string operations
	std::string remaining_key(child_key.data() + common_len, child_key.length() - common_len);
	old_child->key = std::move(remaining_key);
	
	// Update parent pointers for the old child
	char old_first_char = old_child->key[0];
	old_child->parent = intermediate.get();
	old_child->parent_char = old_first_char;
	
	// Move the old child under the intermediate node
	intermediate->children[old_first_char] = std::move(old_child);

	// Replace the old child with the intermediate node
	current->children[first_char] = std::move(intermediate);
}