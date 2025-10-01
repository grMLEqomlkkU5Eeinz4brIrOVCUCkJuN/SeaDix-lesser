# Seadix-lesser (compact build)

> **⚠️ Experimental Project**: This is a research/experimental project for exploring N-API performance and memory optimization techniques. It is **not recommended for production use**.

Currently: I am working on 2 proper builds to publish as NPM libraries. 1 being a more CPU throughput focused one and another being more memory optimized one.

Seadix-lesser is a compact-memory build/profile of the SeaDix Radix Trie for Node.js, built with C++, exploring arena allocation and N-API optimization techniques. This project is for educational purposes and experimentation only.

The reason why I chose to make this was simple. It facinated me how there were many people using pure js solutions which often times resulted in performance penalties. At the same time I have not only never tried C++ (prior to this project), but also never really tried bindings like N-API before.

## Usage

```javascript
const SeaDix = require('seadix-lesser');

// Default constructor (1MB arena)
const trie = new SeaDix();

// With custom arena size (experimental)
const smallTrie = new SeaDix(64 * 1024); // 64KB arena
const largeTrie = new SeaDix(4 * 1024 * 1024); // 4MB arena

// Basic operations
trie.insert("hello");
trie.insert("world");
trie.insert("help");

console.log(trie.search("hello")); // true
console.log(trie.startsWith("he")); // true
console.log(trie.wordsWithPrefix("he")); // ["hello", "help"]

// Batch operations (faster for multiple words)
trie.insertBatch(['foo', 'bar', 'baz']);
const results = trie.searchBatch(['foo', 'bar', 'missing']); // [true, true, false]

// File loading
const count = trie.insertFromFile('./words.txt');
console.log(`Loaded ${count} words`);

// Arena management (experimental)
console.log(`Arena size: ${trie.getArenaSize()} bytes`);
trie.setArenaSize(2 * 1024 * 1024); // Change to 2MB arena
```

## API

### Core Methods
- `insert(word)` - Add word
- `search(word)` - Check if exists
- `remove(word)` - Remove word
- `startsWith(prefix)` - Check prefix
- `wordsWithPrefix(prefix)` - Get words with prefix

### Batch Methods
- `insertBatch(words[])` - Insert multiple
- `searchBatch(words[])` - Search multiple
- `removeBatch(words[])` - Remove multiple

### File Methods
- `insertFromFile(path, bufferSize?)` - Load from file

### Arena Management (Experimental)
- `getArenaSize()` - Get current arena size
- `setArenaSize(size)` - Change arena size

### Analytics
- `getMemoryStats()` - Memory usage
- `getHeightStats()` - Trie structure
- `getWordMetrics()` - Word statistics
- `patternSearch(pattern)` - Wildcard search (`*`, `?`)

## Performance (compact build)

**Test Environment**: Intel i5-4210U mobile CPU, IDE running, balanced power plan

Recent benchmarks (this build) on mixed unigram/bigram/unicode datasets:
- **Memory**: ~118 bytes per word on 6.27M words (file-stream), total ~706MB
- **Insert (micro)**: 139K–166K ops/sec (short/prefix words), 70K–124K on longer words
- **Search (micro)**: up to ~4.0M ops/sec (single search x1000)
- **Delete (micro)**: up to ~4.1M ops/sec (single delete x1000)
- **Stable suite (stats)**: Insert ~195K ops/sec mean; Search hit ~5.41M ops/sec mean
- **File load (6.27M words)**: 7.06s; export JSON 8.61s; import JSON 10.65s

**Note**: Performance varies significantly due to mobile CPU thermal throttling and experimental memory management techniques.

## Options

```javascript
// Configuration options
const trie = new SeaDix({
  words: ["initial", "words"], // Pre-populate
  ignoreCase: true             // Case-insensitive
});

// Arena size (experimental)
const trie = new SeaDix(128 * 1024); // 128KB arena
```

## Arena Sizing

See [Arena Sizing Guide](docs/ARENA_SIZING.md) for detailed recommendations on choosing the optimal arena size for your use case.

## Documentation

- [API Reference](docs/API.md) - Complete API documentation
- [Performance Analysis](docs/PERFORMANCE.md) - Detailed performance metrics
- [Arena Sizing Guide](docs/ARENA_SIZING.md) - How to choose arena sizes

## Development

```bash
npm run build:all    # Build
npm test            # Test
npm run benchmark:stable  # Benchmark
npm run benchmark:arena   # Arena size benchmark
```

## Limitations

- **Not production-ready**: This is an experimental project
- **High memory overhead**: 95-98% overhead due to experimental memory management
- **N-API overhead**: 3-4x slower than pure C++ due to Node.js/N-API overhead (but this is still better than pure js)

## License

ISC
