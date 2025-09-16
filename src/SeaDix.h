#pragma once

#include "RadixTrie.h"
#include <napi.h>

class SeaDix : public Napi::ObjectWrap<SeaDix> {
  public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	explicit SeaDix(const Napi::CallbackInfo &info);
	~SeaDix() = default;

  private:
	static Napi::FunctionReference constructor;
	std::unique_ptr<RadixTrie> trie_;

	// Methods exposed to JavaScript
	Napi::Value Insert(const Napi::CallbackInfo &info);
	Napi::Value InsertBatch(const Napi::CallbackInfo &info);
	Napi::Value InsertFromFile(const Napi::CallbackInfo &info);
	Napi::Value Search(const Napi::CallbackInfo &info);
	Napi::Value SearchBatch(const Napi::CallbackInfo &info);
	Napi::Value StartsWith(const Napi::CallbackInfo &info);
	Napi::Value WordsWithPrefix(const Napi::CallbackInfo &info);
	Napi::Value Remove(const Napi::CallbackInfo &info);
	Napi::Value RemoveBatch(const Napi::CallbackInfo &info);
	Napi::Value Empty(const Napi::CallbackInfo &info);
	Napi::Value Size(const Napi::CallbackInfo &info);
	Napi::Value Clear(const Napi::CallbackInfo &info);

	// analytics methods
	Napi::Value GetHeightStats(const Napi::CallbackInfo &info);
	Napi::Value GetMemoryStats(const Napi::CallbackInfo &info);
	Napi::Value GetWordMetrics(const Napi::CallbackInfo &info);
	Napi::Value PatternSearch(const Napi::CallbackInfo &info);

	// arena management
	Napi::Value GetArenaSize(const Napi::CallbackInfo &info);
	Napi::Value SetArenaSize(const Napi::CallbackInfo &info);
};