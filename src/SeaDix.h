#pragma once

#include "RadixTrie.h"
#include <napi.h>
#include <vector>
#include <string>
#include <string_view>

class SeaDix : public Napi::ObjectWrap<SeaDix> {
  public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	explicit SeaDix(const Napi::CallbackInfo &info);
	~SeaDix() = default;

  private:
	static Napi::FunctionReference constructor;
	RadixTrie trie_;
	
	// No caching overhead - direct operations only

	// Methods exposed to JavaScript
	Napi::Value Insert(const Napi::CallbackInfo &info);
	Napi::Value InsertBatch(const Napi::CallbackInfo &info);
	Napi::Value Search(const Napi::CallbackInfo &info);
	Napi::Value SearchBatch(const Napi::CallbackInfo &info);
	Napi::Value StartsWith(const Napi::CallbackInfo &info);
	Napi::Value WordsWithPrefix(const Napi::CallbackInfo &info);
	Napi::Value Remove(const Napi::CallbackInfo &info);
	Napi::Value RemoveBatch(const Napi::CallbackInfo &info);
	Napi::Value Empty(const Napi::CallbackInfo &info);
	Napi::Value Size(const Napi::CallbackInfo &info);
	Napi::Value Clear(const Napi::CallbackInfo &info);
	
	// Utility methods
	std::vector<std::string> extract_string_array(const Napi::Array& array);
};