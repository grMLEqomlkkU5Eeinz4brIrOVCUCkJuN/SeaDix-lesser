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
		 InstanceMethod("insertFromFile",
						&SeaDix::InsertFromFile), // New method
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

// Search method - uses string_view to avoid copies
Napi::Value SeaDix::Search(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String argument expected")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Use string_view to avoid unnecessary string copy
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

// Batch Insert method - reduces N-API overhead
Napi::Value SeaDix::InsertBatch(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsArray()) {
		Napi::TypeError::New(env, "Array argument expected")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	Napi::Array words = info[0].As<Napi::Array>();
	uint32_t count = 0;

	// Process all words in a single C++ call
	for (uint32_t i = 0; i < words.Length(); ++i) {
		if (words.Get(i).IsString()) {
			std::string word = words.Get(i).As<Napi::String>().Utf8Value();
			if (!word.empty()) {
				std::string_view word_view(word);
				trie_.insert(word_view);
				count++;
			}
		}
	}

	return Napi::Number::New(env, count);
}

// Batch Search method - returns array of results
Napi::Value SeaDix::SearchBatch(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsArray()) {
		Napi::TypeError::New(env, "Array argument expected")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	Napi::Array words = info[0].As<Napi::Array>();
	Napi::Array results = Napi::Array::New(env, words.Length());

	// Process all searches in a single C++ call
	for (uint32_t i = 0; i < words.Length(); ++i) {
		if (words.Get(i).IsString()) {
			std::string word = words.Get(i).As<Napi::String>().Utf8Value();
			std::string_view word_view(word);
			bool found = trie_.search(word_view);
			results.Set(i, Napi::Boolean::New(env, found));
		} else {
			results.Set(i, Napi::Boolean::New(env, false));
		}
	}

	return results;
}

// Batch Remove method - returns array of success flags
Napi::Value SeaDix::RemoveBatch(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsArray()) {
		Napi::TypeError::New(env, "Array argument expected")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	Napi::Array words = info[0].As<Napi::Array>();
	Napi::Array results = Napi::Array::New(env, words.Length());

	// Process all removals in a single C++ call
	for (uint32_t i = 0; i < words.Length(); ++i) {
		if (words.Get(i).IsString()) {
			std::string word = words.Get(i).As<Napi::String>().Utf8Value();
			std::string_view word_view(word);
			bool removed = trie_.remove(word_view);
			results.Set(i, Napi::Boolean::New(env, removed));
		} else {
			results.Set(i, Napi::Boolean::New(env, false));
		}
	}

	return results;
}

// Module initialization
Napi::Object Init(Napi::Env env, Napi::Object exports) {
	return SeaDix::Init(env, exports);
}

// New InsertFromFile method
Napi::Value SeaDix::InsertFromFile(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	// Validate arguments
	if (info.Length() < 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "File path string argument expected")
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	std::string file_path = info[0].As<Napi::String>().Utf8Value();

	// Default buffer size of 1MB, but allow override
	size_t buffer_size = 1024 * 1024; // 1MB default

	// Check for optional buffer size parameter
	if (info.Length() >= 2 && info[1].IsNumber()) {
		double buffer_size_double = info[1].As<Napi::Number>().DoubleValue();

		// Validate buffer size
		if (buffer_size_double <= 0 || buffer_size_double > SIZE_MAX) {
			Napi::RangeError::New(
				env, "Buffer size must be positive and within valid range")
				.ThrowAsJavaScriptException();
			return env.Undefined();
		}

		buffer_size = static_cast<size_t>(buffer_size_double);

		// Ensure minimum buffer size of 1KB for efficiency
		if (buffer_size < 1024) {
			buffer_size = 1024;
		}
	}

	try {
		size_t words_inserted =
			trie_.bulk_insert_from_file(file_path, buffer_size);
		return Napi::Number::New(env, static_cast<double>(words_inserted));
	} catch (const std::exception &e) {
		Napi::Error::New(env,
						 std::string("Failed to insert from file: ") + e.what())
			.ThrowAsJavaScriptException();
		return env.Undefined();
	}
}

// Register the module
NODE_API_MODULE(seadix, Init)