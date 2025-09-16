# SeaDix

> **⚠️ Experimental Project**: This is a research/experimental project for exploring N-API performance and memory optimization techniques. It is **not recommended for production use**.

A Radix Trie for Node.js built with C++, exploring arena allocation and N-API optimization techniques. This project is for educational purposes and experimentation only.

The reason why I chose to make this was simple. It facinated me how there were many people using pure js solutions which often times resulted in performance penalties. At the same time I have not only never tried C++ (prior to this project), but also never really tried bindings like N-API before.

## Usage

```javascript
const SeaDix = require('seadix');

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

## Performance

**Test Environment**: Intel i5-4210U mobile CPU, IDE running, balanced power plan

Recent benchmarks on mixed unigram/bigram/unicode datasets:
- **Memory**: 114-146 bytes per word (high overhead due to experimental memory management)
- **Insert**: 644K-1.5M ops/sec (varies significantly with arena size)
- **Search**: 2.9M-3.3M ops/sec (varies significantly with arena size)
- **File load**: 1.9s for 3M words, 2.8s for 6M words

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