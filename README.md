# SeaDix

An experimental Radix Trie implementation for Node.js built with C++. This is a learning project focused on understanding Node.js addons, N-API overhead, and data structure implementation. All tests were done on an i5-4210u CPU on a dell AIO in school.

You might find a lot of messy code, partly because this is really just for me to learn and also because I tried optimizing some parts and had to revert on numerous occasions.

# NOTICE

If you are conducting benchmarks, you should make use of `stable-benchmarks.ts` this is because I notived a huge amount of variation in runs. Also if you want to convert json to trie or export trie to json (`trie.toJSON()`), make sure to use javascript to handle the json data instead, this is because having to convert JSON via N-API is costly. I might make proper export methods to a dedicated file in the future.

## What This Is

SeaDix is a prefix tree (trie) data structure implementation that I built to experiment with:
- Node.js C++ addons using N-API
- Performance characteristics of JavaScript/C++ interop
- Radix trie data structure implementation
- Memory management across language boundaries

This is primarily a learning project, not a production-ready library.

## Building from Source

For benchmarking, use `stable-benchmarks.ts` for consistent results. For JSON import/export, use JavaScript to handle the data, as N-API conversion is costly.
```bash
npm install
npm run build:all

// Create a new trie
const trie = new SeaDix();

// Insert words
trie.insert("hello");
trie.insert("world");
trie.insert("help");

// Search for words
console.log(trie.search("hello")); // true
console.log(trie.search("hell"));  // false

// Check prefixes
console.log(trie.startsWith("he")); // true

// Get words with prefix
console.log(trie.getWordsWithPrefix("he")); // ["hello", "help"]

// Remove words
trie.remove("hello");
console.log(trie.search("hello")); // false
```

## Constructor Options

```javascript
const trie = new SeaDix({
words: ["initial", "words"], // Pre-populate with words
ignoreCase: true             // Case-insensitive operations
});
```

## API Reference

### Methods

#### `insert(word: string): void`
Adds a word to the trie.

#### `search(word: string): boolean`

// Pattern search (wildcards)
console.log(trie.patternSearch("he*")); // Matches words starting with 'he'
console.log(trie.patternSearch("h?l*")); // Matches words like 'help', 'hello', 'helmet'
Returns true if the word exists in the trie.

#### `startsWith(prefix: string): boolean`
Returns true if any word in the trie starts with the given prefix.

#### `getWordsWithPrefix(prefix: string): string[]`
Returns an array of all words that start with the given prefix.

#### `remove(word: string): boolean`
Removes a word from the trie. Returns true if the word was removed.

#### `isEmpty(): boolean`
Returns true if the trie contains no words.

#### `size(): number`

- `insert(word: string): void`
- `search(word: string): boolean`
- `startsWith(prefix: string): boolean`
- `getWordsWithPrefix(prefix: string): string[]`
- `remove(word: string): boolean`
- `isEmpty(): boolean`
- `size(): number`
- `clear(): void`


### Batch Operations (Recommended)

- `insertBatch(words: string[]): number`
- `searchBatch(words: string[]): boolean[]`
- `removeBatch(words: string[]): boolean[]`
- `removeMany(words: string[]): boolean[]`
// Batch operations - faster for small datasets (5-100 words)
const count = trie.insertBatch(['hello', 'world', 'foo', 'bar']);
console.log(`Inserted ${count} words`);

- `insertFromFile(filePath: string, bufferSize?: number): number` — Insert words from a file, with optional buffer size (default: 1MB).
// Batch search
const results = trie.searchBatch(['hello', 'world', 'missing']);

- `getHeightStats(): { minHeight, maxHeight, averageHeight, modeHeight, allHeights }`
- `getMemoryStats(): { totalBytes, nodeCount, stringBytes, overheadBytes, bytesPerWord }`
- `getWordMetrics(): { minLength, maxLength, averageLength, modeLength, lengthDistribution, totalCharacters }`
- `patternSearch(pattern: string): string[]` — Supports `*` and `?` wildcards
console.log(results); // [true, true, false]


- `getStats(): { wordCount, isEmpty, allWords }`
- `toJSON(): object`
- `fromJSON(json: object): SeaDix`
- `SeaDix.fromWords(words: string[], options?): SeaDix`
// Batch remove
const removed = trie.removeBatch(['hello', 'world']);
console.log(removed); // [true, true]

### Individual Operations

For single operations or large datasets (500+ words):

```typescript
trie.insert('hello');
trie.search('world');
trie.remove('test');

// For large datasets, individual operations are faster
for (const word of largeWordList) {
    trie.insert(word);
}
```

#### `insertBatch(words: string[]): number`
Inserts multiple words in a single batch operation. Returns number of words successfully inserted.

#### `searchBatch(words: string[]): boolean[]`
Searches for multiple words in a single batch operation. Returns array of boolean results.

#### `removeBatch(words: string[]): boolean[]`
Removes multiple words in a single batch operation. Returns array of boolean results indicating success.

#### `removeMany(words: string[]): boolean[]`
Removes multiple words. Returns array of booleans indicating success.

### Utility Methods

#### `getStats(): TrieStats`

- For large datasets, use file streaming and batch operations for best performance.
- Pattern search supports `*` (zero or more characters) and `?` (single character).
- Analytics methods provide insights into trie structure and memory usage.
Returns statistics about the trie.

#### `toJSON(): object`
Creates a new trie from a JSON object.

### Static Methods

#### `SeaDix.fromWords(words: string[], options?): SeaDix`
Creates a new trie with initial words.

## N-API Overhead Investigation

This project is primarily about understanding the performance characteristics of Node.js C++ addons. Here's what I've discovered:

### File Streaming Insertion
#### `insertFromFile(filePath: string, bufferSize?: number): number`
Allows file streaming, a buffer can be provided, but the default is 1 MB (note that the user needs to multiply by 1024 for size.).

### The N-API Boundary Problem

Every call from JavaScript to C++ has significant overhead:
- **Pure C++**: 2.7M insertions/sec,  4.01606e+07 searches/sec
- **Node.js (N-API)**: 208K insertions/sec, 625K searches/sec
- **Overhead**: 7x slower for insertions, 29x slower for searches

### Why This Happens

Each JavaScript function call to C++ requires:
1. **String conversion**: JavaScript string → C++ string_view
2. **Function marshalling**: N-API parameter conversion
3. **Memory management**: Cross-boundary memory handling
4. **Return value conversion**: C++ result → JavaScript value

### Performance Characteristics

- **Insertion**: O(m) where m is the length of the word
- **Search**: O(m) where m is the length of the word
- **Prefix Search**: O(m + k) where m is prefix length and k is number of results
- **Node Cleanup**: O(1) per node using parent pointer optimization

## Benchmarking

Run performance tests:

```bash
npm run benchmark        # Simple benchmark
npm run benchmark:full  # Comprehensive benchmark
npm run benchmark:stable  # Comprehensive benchmark
```

## Development

### Building

```bash
npm run build          # Build C++ addon
npm run build:ts       # Compile TypeScript
npm run build:all      # Build everything
```

### Testing

```bash
npm test               # Run tests
npm run test:watch     # Watch mode
```

### Formatting

```bash
npm run format         # Format C++ code
```

## Experimental Findings

Through this project, I've discovered several interesting things:

- **C++ Implementation is Fast**: The underlying radix trie runs at 1.46M ops/sec
- **N-API is the Bottleneck**: JavaScript/C++ boundary adds 7-29x overhead
- **Bulk Operations Don't Help**: Creating fresh tries per iteration amplifies overhead
- **String Conversion is Expensive**: Each JS string → C++ conversion has cost
- **Memory Management Matters**: Cross-boundary memory handling affects performance

## Known Issues

- Cross-platform compatibility needs more testing
- Large datasets (>2^53 words) may have precision issues with size reporting
- Bulk inserts have no optimization and to be honest it is because no matter what I did (e.g. pre-sorting, counting post insertion) single inserts were just faster.

## What I'm Still Figuring Out

1. **Publishing to npm** - If this actually works and people find it useful
2. **File streaming** - Adding methods to load/save large datasets from files
3. **Better error handling** - Making the C++ side more robust
4. **More data structures** - Maybe other tree structures if this goes well
5. **Multi-threading** - I am honestly doubtful about this, but parallel searching would be nice

## Contributing

If you find bugs or know how to make this better, please let me know. I'm still learning C++ and Node.js addons, so any help would be appreciated.

## License

ISC
