#include "SeaDix.h"
#include <limits>

// Static member definition
Napi::FunctionReference SeaDix::constructor;

// Constructor
SeaDix::SeaDix(const Napi::CallbackInfo &info)
	: Napi::ObjectWrap<SeaDix>(info) {
	// Constructor can be empty since RadixTrie has a default constructor
}

// Static method to initialize and export the class
Napi::Object SeaDix::Init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(
		env, "SeaDix",
		{InstanceMethod("insert", &SeaDix::Insert),
		 InstanceMethod("insertBatch", &SeaDix::InsertBatch),
		 InstanceMethod("search", &SeaDix::Search),
		 InstanceMethod("searchBatch", &SeaDix::SearchBatch),
		 InstanceMethod("startsWith", &SeaDix::StartsWith),
		 InstanceMethod("wordsWithPrefix", &SeaDix::WordsWithPrefix),
		 InstanceMethod("remove", &SeaDix::Remove),
		 InstanceMethod("removeBatch", &SeaDix::RemoveBatch),
		 InstanceMethod("empty", &SeaDix::Empty),
		 InstanceMethod("size", &SeaDix::Size),
		 InstanceMethod("clear", &SeaDix::Clear)});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("SeaDix", func);
	return exports;
}

// Insert method - uses string_view to avoid copies
Napi::Value SeaDix::Insert(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String argument expected")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Use string_view to avoid unnecessary string copy
	std::string word = info[0].As<Napi::String>().Utf8Value();
	std::string_view word_view(word);
	trie_.insert(word_view);

	return env.Undefined();
}

// Fast Search method - direct operation, no overhead
Napi::Value SeaDix::Search(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String argument expected")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Direct search - fastest path
	std::string word = info[0].As<Napi::String>().Utf8Value();
	std::string_view word_view(word);
	bool found = trie_.search(word_view);

	return Napi::Boolean::New(env, found);
}

// StartsWith method
Napi::Value SeaDix::StartsWith(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String argument expected")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	std::string prefix = info[0].As<Napi::String>().Utf8Value();
	bool hasPrefix = trie_.starts_with(prefix);

	return Napi::Boolean::New(env, hasPrefix);
}

// WordsWithPrefix method
Napi::Value SeaDix::WordsWithPrefix(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String argument expected")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	std::string prefix = info[0].As<Napi::String>().Utf8Value();
	std::vector<std::string> words = trie_.words_with_prefix(prefix);

	// Pre-allocate array for batch string creation
	Napi::Array result = Napi::Array::New(env, words.size());
	for (size_t i = 0; i < words.size(); ++i) {
		result[i] = Napi::String::New(env, words[i].c_str(), words[i].length());
	}

	return result;
}

// Remove method
Napi::Value SeaDix::Remove(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String argument expected")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	std::string word = info[0].As<Napi::String>().Utf8Value();
	bool removed = trie_.remove(word);

	return Napi::Boolean::New(env, removed);
}

// Empty method
Napi::Value SeaDix::Empty(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	bool isEmpty = trie_.empty();
	return Napi::Boolean::New(env, isEmpty);
}

// Size method
Napi::Value SeaDix::Size(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	size_t size = trie_.size();
	// Use safe conversion for large numbers
	// Check if size can be safely converted to double (use a reasonable upper
	// bound)
	const size_t MAX_SAFE_SIZE =
		1ULL << 53; // 2^53 is the max safe integer in JavaScript
	if (size > MAX_SAFE_SIZE) {
		Napi::TypeError::New(env,
							 "Size too large to represent as JavaScript number")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}
	return Napi::Number::New(env, static_cast<double>(size));
}

// Clear method
Napi::Value SeaDix::Clear(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	trie_.clear();
	return env.Undefined();
}

// Fast Batch Insert method - simple and direct
Napi::Value SeaDix::InsertBatch(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsArray()) {
		Napi::TypeError::New(env, "Array argument expected")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	Napi::Array js_words = info[0].As<Napi::Array>();
	
	// Extract all strings in one pass to minimize N-API calls
	std::vector<std::string> words = extract_string_array(js_words);
	uint32_t count = 0;

	// Simple, fast processing - no overhead
	for (const auto& word : words) {
		if (!word.empty()) {
			std::string_view word_view(word);
			trie_.insert(word_view);
			count++;
		}
	}

	return Napi::Number::New(env, count);
}

// Fast Batch Search method - direct operations
Napi::Value SeaDix::SearchBatch(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsArray()) {
		Napi::TypeError::New(env, "Array argument expected")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	Napi::Array js_words = info[0].As<Napi::Array>();
	
	// Extract all strings in one pass
	std::vector<std::string> words = extract_string_array(js_words);
	
	// Pre-allocate result array
	Napi::Array results = Napi::Array::New(env, words.size());

	// Direct searches - no caching overhead
	for (size_t i = 0; i < words.size(); ++i) {
		const std::string& word = words[i];
		bool found = false;
		
		if (!word.empty()) {
			std::string_view word_view(word);
			found = trie_.search(word_view);
		}
		
		results.Set(static_cast<uint32_t>(i), Napi::Boolean::New(env, found));
	}

	return results;
}

// Fast Batch Remove method - direct operations
Napi::Value SeaDix::RemoveBatch(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsArray()) {
		Napi::TypeError::New(env, "Array argument expected")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	Napi::Array js_words = info[0].As<Napi::Array>();
	
	// Extract all strings in one pass
	std::vector<std::string> words = extract_string_array(js_words);
	
	// Pre-allocate result array
	Napi::Array results = Napi::Array::New(env, words.size());

	// Direct removals - no overhead
	for (size_t i = 0; i < words.size(); ++i) {
		const std::string& word = words[i];
		bool removed = false;
		if (!word.empty()) {
			std::string_view word_view(word);
			removed = trie_.remove(word_view);
		}
		results.Set(static_cast<uint32_t>(i), Napi::Boolean::New(env, removed));
	}

	return results;
}


// Utility method: Extract string array efficiently
std::vector<std::string> SeaDix::extract_string_array(const Napi::Array& array) {
	std::vector<std::string> result;
	result.reserve(array.Length());
	
	for (uint32_t i = 0; i < array.Length(); ++i) {
		Napi::Value value = array.Get(i);
		if (value.IsString()) {
			result.emplace_back(value.As<Napi::String>().Utf8Value());
		} else {
			result.emplace_back(); // Empty string for non-string values
		}
	}
	
	return result;
}

// Module initialization
Napi::Object Init(Napi::Env env, Napi::Object exports) {
	return SeaDix::Init(env, exports);
}

// Register the module
NODE_API_MODULE(seadix, Init)