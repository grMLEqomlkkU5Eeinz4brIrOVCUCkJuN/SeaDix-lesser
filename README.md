# SeaDix

A Radix Trie for Node.js built with C++. Optimized for better memory management and prefix operations.

## Usage

```javascript
const SeaDix = require('seadix');

const trie = new SeaDix();

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

### Analytics
- `getMemoryStats()` - Memory usage
- `getHeightStats()` - Trie structure
- `getWordMetrics()` - Word statistics
- `patternSearch(pattern)` - Wildcard search (`*`, `?`)

## Performance

Recent benchmarks on 6M+ words:
- **Memory**: 95-123 bytes per word
- **Insert**: 1M+ ops/sec
- **Search**: 3M+ ops/sec
- **File load**: 2.5s for 6M words

## Options

```javascript
const trie = new SeaDix({
  words: ["initial", "words"], // Pre-populate
  ignoreCase: true             // Case-insensitive
});
```

## Development

```bash
npm run build:all    # Build
npm test            # Test
npm run benchmark:stable  # Benchmark
```

## License

ISC