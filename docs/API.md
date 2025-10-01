# API Reference (Seadix-lesser)

> **⚠️ Experimental Project**: This is a research/experimental project for exploring N-API performance and memory optimization techniques. It is **not recommended for production use**.

## Constructor

### `new SeaDix(optionsOrArenaSize?)`

Creates a new Radix Trie instance.

**Parameters:**
- `optionsOrArenaSize` (optional): Either a configuration object or arena size in bytes
  - **As number**: Arena size in bytes (experimental feature)
  - **As object**: Configuration object
    - `words?: string[]` - Pre-populate with words
    - `ignoreCase?: boolean` - Case-insensitive operations (default: false)

**Examples:**
```javascript
// Default constructor (1MB arena)
const trie = new SeaDix();

// With configuration options
const trieWithWords = new SeaDix({ words: ['hello', 'world'] });
const caseInsensitive = new SeaDix({ ignoreCase: true });

// With custom arena size (experimental)
const smallTrie = new SeaDix(64 * 1024); // 64KB arena
const largeTrie = new SeaDix(4 * 1024 * 1024); // 4MB arena
```

**Arena Size Guidelines:**
- **Small datasets (< 1K words)**: 32-64KB
- **Medium datasets (1K-10K words)**: 128KB-1MB  
- **Large datasets (> 10K words)**: 1-4MB
- **Search-heavy workloads**: 2-4MB
- **Insert-heavy workloads**: 32-128KB

See [Arena Sizing Guide](ARENA_SIZING.md) for detailed recommendations.

## Core Operations

### `insert(word: string): void`

Adds a word to the trie.

**Parameters:**
- `word` - The word to insert

**Example:**
```javascript
trie.insert("hello");
trie.insert("world");
```

### `search(word: string): boolean`

Checks if a word exists in the trie.

**Parameters:**
- `word` - The word to search for

**Returns:** `true` if the word exists, `false` otherwise

**Example:**
```javascript
console.log(trie.search("hello")); // true
console.log(trie.search("hell"));  // false
```

### `remove(word: string): boolean`

Removes a word from the trie.

**Parameters:**
- `word` - The word to remove

**Returns:** `true` if the word was removed, `false` if it didn't exist

**Example:**
```javascript
const removed = trie.remove("hello");
console.log(removed); // true
```

### `startsWith(prefix: string): boolean`

Checks if any word in the trie starts with the given prefix.

**Parameters:**
- `prefix` - The prefix to check

**Returns:** `true` if any word starts with the prefix

**Example:**
```javascript
console.log(trie.startsWith("he")); // true
console.log(trie.startsWith("xyz")); // false
```

### `wordsWithPrefix(prefix: string): string[]`

Returns all words that start with the given prefix.

**Parameters:**
- `prefix` - The prefix to search for

**Returns:** Array of words starting with the prefix

**Example:**
```javascript
trie.insert("hello");
trie.insert("help");
trie.insert("world");

console.log(trie.wordsWithPrefix("he")); // ["hello", "help"]
console.log(trie.wordsWithPrefix(""));   // ["hello", "help", "world"]
```

## Batch Operations

### `insertBatch(words: string[]): number`

Inserts multiple words in a single operation.

**Parameters:**
- `words` - Array of words to insert

**Returns:** Number of words successfully inserted

**Example:**
```javascript
const count = trie.insertBatch(['hello', 'world', 'foo', 'bar']);
console.log(`Inserted ${count} words`);
```

### `searchBatch(words: string[]): boolean[]`

Searches for multiple words in a single operation.

**Parameters:**
- `words` - Array of words to search for

**Returns:** Array of boolean results

**Example:**
```javascript
const results = trie.searchBatch(['hello', 'world', 'missing']);
console.log(results); // [true, true, false]
```

### `removeBatch(words: string[]): boolean[]`

Removes multiple words in a single operation.

**Parameters:**
- `words` - Array of words to remove

**Returns:** Array of boolean results indicating success

**Example:**
```javascript
const removed = trie.removeBatch(['hello', 'world']);
console.log(removed); // [true, true]
```

## File Operations

### `insertFromFile(filePath: string, bufferSize?: number): number`

Loads words from a file, one word per line.

**Parameters:**
- `filePath` - Path to the file
- `bufferSize` - Buffer size in bytes (default: 1MB)

**Returns:** Number of words loaded

**Example:**
```javascript
const count = trie.insertFromFile('./words.txt');
console.log(`Loaded ${count} words`);

// With custom buffer size
const count2 = trie.insertFromFile('./words.txt', 2 * 1024 * 1024); // 2MB
```

## Arena Management (Experimental)

### `getArenaSize(): number`

Gets the current arena size in bytes.

**Returns:** Arena size in bytes

**Example:**
```javascript
const arenaSize = trie.getArenaSize();
console.log(`Current arena size: ${arenaSize} bytes`);
```

### `setArenaSize(size: number): boolean`

Changes the arena size and recreates the trie with existing data.

**Parameters:**
- `size` - New arena size in bytes (must be > 0)

**Returns:** `true` if successful, `false` otherwise

**Throws:**
- `TypeError` if size is not a number
- `Error` if size is not greater than 0

**Example:**
```javascript
// Start with small arena
const trie = new SeaDix(64 * 1024);
trie.insert("hello");
trie.insert("world");

// Change to larger arena (preserves data)
const success = trie.setArenaSize(2 * 1024 * 1024);
if (success) {
    console.log("Arena size changed successfully");
    // Data is preserved
    console.log(trie.search("hello")); // true
}
```

**Performance Note:** Changing arena size is expensive as it recreates the entire trie internally. Avoid frequent changes with large datasets.

## Analytics

### `getMemoryStats(): MemoryStats`

Returns detailed memory usage statistics.

**Returns:** Object with memory information
- `totalBytes` - Total memory usage in bytes
- `nodeCount` - Number of nodes
- `stringBytes` - Memory used by strings
- `overheadBytes` - Memory overhead
- `bytesPerWord` - Average bytes per word

**Example:**
```javascript
const stats = trie.getMemoryStats();
console.log(`Memory usage: ${stats.totalBytes} bytes`);
console.log(`Bytes per word: ${stats.bytesPerWord}`);
```

### `getHeightStats(): HeightStats`

Returns trie height statistics.

**Returns:** Object with height information
- `minHeight` - Minimum height
- `maxHeight` - Maximum height
- `averageHeight` - Average height
- `modeHeight` - Most common height
- `allHeights` - Array of all heights

**Example:**
```javascript
const stats = trie.getHeightStats();
console.log(`Height range: ${stats.minHeight} - ${stats.maxHeight}`);
```

### `getWordMetrics(): WordMetrics`

Returns word length statistics.

**Returns:** Object with word metrics
- `minLength` - Minimum word length
- `maxLength` - Maximum word length
- `averageLength` - Average word length
- `modeLength` - Most common word length
- `totalCharacters` - Total character count
- `lengthDistribution` - Array of word counts by length

**Example:**
```javascript
const metrics = trie.getWordMetrics();
console.log(`Average word length: ${metrics.averageLength}`);
```

### `patternSearch(pattern: string): string[]`

Searches for words matching a wildcard pattern.

**Parameters:**
- `pattern` - Pattern with `*` (zero or more chars) and `?` (single char)

**Returns:** Array of matching words

**Example:**
```javascript
trie.insert("hello");
trie.insert("help");
trie.insert("world");

console.log(trie.patternSearch("he*"));  // ["hello", "help"]
console.log(trie.patternSearch("h?l*")); // ["help"]
console.log(trie.patternSearch("*o*"));  // ["hello", "world"]
```

## Utility Methods

### `size(): number`

Returns the number of words in the trie.

**Returns:** Number of words

**Example:**
```javascript
console.log(trie.size()); // 3
```

### `empty(): boolean`

Checks if the trie is empty.

**Returns:** `true` if empty, `false` otherwise

**Example:**
```javascript
console.log(trie.empty()); // false
```

### `clear(): void`

Removes all words from the trie.

**Example:**
```javascript
trie.clear();
console.log(trie.empty()); // true
```

### `getStats(): TrieStats`

Returns basic trie statistics.

**Returns:** Object with basic stats
- `wordCount` - Number of words
- `isEmpty` - Whether trie is empty
- `allWords` - Array of all words

**Example:**
```javascript
const stats = trie.getStats();
console.log(`Word count: ${stats.wordCount}`);
```

## Serialization

### `toJSON(): object`

Exports the trie to a JSON object.

**Returns:** JSON representation of the trie

**Example:**
```javascript
const json = trie.toJSON();
// Use JavaScript to handle JSON data for better performance
```

### `fromJSON(json: object): SeaDix`

Creates a trie from a JSON object.

**Parameters:**
- `json` - JSON object from `toJSON()`

**Returns:** New SeaDix instance

**Example:**
```javascript
const json = trie.toJSON();
const newTrie = trie.fromJSON(json);
```

## Static Methods

### `SeaDix.fromWords(words: string[], options?): SeaDix`

Creates a new trie with initial words.

**Parameters:**
- `words` - Array of words
- `options` - Optional configuration (same as constructor)

**Returns:** New SeaDix instance

**Example:**
```javascript
const trie = SeaDix.fromWords(['hello', 'world', 'test']);
```

## Performance Notes

- **Batch operations** are recommended for 5-100 words
- **Individual operations** are faster for large datasets (500+ words)
- **File streaming** is most efficient for loading large datasets
- **JSON operations** should be handled in JavaScript for better performance
- **Pattern search** supports `*` and `?` wildcards
