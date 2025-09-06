# SeaDix API Documentation

## Class: SeaDix

An experimental Radix Trie implementation for Node.js with C++ backend. This is a learning project focused on understanding N-API performance characteristics.

### Constructor

#### `new SeaDix(options?: SeaDixOptions)`

Creates a new SeaDix instance.

**Parameters:**
- `options` (optional): Configuration object
  - `words?: string[]` - Initial words to insert
  - `ignoreCase?: boolean` - Whether to ignore case (default: false)

**Example:**
```javascript
const trie = new SeaDix();
const trieWithWords = new SeaDix({ words: ["hello", "world"] });
const caseInsensitiveTrie = new SeaDix({ ignoreCase: true });
```

### Instance Methods

#### `insert(word: string): void`

Inserts a word into the trie.

**Parameters:**
- `word: string` - The word to insert

**Throws:**
- `TypeError` - If word is not a string
- `Error` - If word is empty or whitespace only

**Example:**
```javascript
trie.insert("hello");
trie.insert("world");
```

#### Bulk Insertion

For inserting multiple words, use a simple loop for optimal performance:

```javascript
// Recommended approach - 25% faster than bulk methods
["hello", "world", "test"].forEach(word => trie.insert(word));

// Alternative approach
for (const word of words) {
  trie.insert(word);
}
```

#### `search(word: string): boolean`

Searches for a word in the trie.

**Parameters:**
- `word: string` - The word to search for

**Returns:**
- `boolean` - True if the word exists, false otherwise

**Throws:**
- `TypeError` - If word is not a string

**Example:**
```javascript
trie.insert("hello");
console.log(trie.search("hello")); // true
console.log(trie.search("hell"));  // false
```

#### `startsWith(prefix: string): boolean`

Checks if any word in the trie starts with the given prefix.

**Parameters:**
- `prefix: string` - The prefix to check

**Returns:**
- `boolean` - True if any word starts with the prefix

**Throws:**
- `TypeError` - If prefix is not a string

**Example:**
```javascript
trie.insert("hello");
trie.insert("help");
console.log(trie.startsWith("he")); // true
console.log(trie.startsWith("xyz")); // false
```

#### `getWordsWithPrefix(prefix: string): string[]`

Gets all words that start with the given prefix.

**Parameters:**
- `prefix: string` - The prefix to search for

**Returns:**
- `string[]` - Array of words that start with the prefix

**Throws:**
- `TypeError` - If prefix is not a string

**Example:**
```javascript
["hello", "help", "world"].forEach(word => trie.insert(word));
console.log(trie.getWordsWithPrefix("he")); // ["hello", "help"]
console.log(trie.getWordsWithPrefix(""));   // ["hello", "help", "world"]
```

#### `remove(word: string): boolean`

Removes a word from the trie.

**Parameters:**
- `word: string` - The word to remove

**Returns:**
- `boolean` - True if the word was removed, false if not found

**Throws:**
- `TypeError` - If word is not a string

**Example:**
```javascript
trie.insert("hello");
console.log(trie.remove("hello")); // true
console.log(trie.remove("hello")); // false
```

#### `removeMany(words: string[]): boolean[]`

Removes multiple words from the trie.

**Parameters:**
- `words: string[]` - Array of words to remove

**Returns:**
- `boolean[]` - Array indicating which words were successfully removed

**Throws:**
- `TypeError` - If words is not an array

**Example:**
```javascript
["hello", "world", "test"].forEach(word => trie.insert(word));
const results = trie.removeMany(["hello", "xyz", "world"]);
console.log(results); // [true, false, true]
```

#### `isEmpty(): boolean`

Checks if the trie is empty.

**Returns:**
- `boolean` - True if the trie contains no words

**Example:**
```javascript
console.log(trie.isEmpty()); // true
trie.insert("hello");
console.log(trie.isEmpty()); // false
```

#### `size(): number`

Gets the number of words in the trie.

**Returns:**
- `number` - The number of words stored in the trie

**Example:**
```javascript
["hello", "world"].forEach(word => trie.insert(word));
console.log(trie.size()); // 2
```

#### `getAllWords(): string[]`

Gets all words in the trie.

**Returns:**
- `string[]` - Array of all words in the trie

**Example:**
```javascript
["hello", "world"].forEach(word => trie.insert(word));
console.log(trie.getAllWords()); // ["hello", "world"]
```

#### `clear(): void`

Removes all words from the trie.

**Example:**
```javascript
["hello", "world"].forEach(word => trie.insert(word));
trie.clear();
console.log(trie.size()); // 0
```

#### `getStats(): TrieStats`

Gets statistics about the trie.

**Returns:**
- `TrieStats` - Object containing trie statistics
  - `wordCount: number` - Number of words
  - `isEmpty: boolean` - Whether trie is empty
  - `allWords: string[]` - All words in the trie

**Example:**
```javascript
["hello", "world"].forEach(word => trie.insert(word));
const stats = trie.getStats();
console.log(stats.wordCount); // 2
console.log(stats.isEmpty);   // false
```

#### `toJSON(): object`

Converts the trie to a JSON-serializable object.

**Returns:**
- `object` - JSON object with words and options

**Example:**
```javascript
["hello", "world"].forEach(word => trie.insert(word));
const json = trie.toJSON();
console.log(json.words); // ["hello", "world"]
console.log(json.options.ignoreCase); // false
```

### Static Methods

#### `SeaDix.fromWords(words: string[], options?: SeaDixOptions): SeaDix`

Creates a new SeaDix instance from an array of words.

**Parameters:**
- `words: string[]` - Array of words to insert
- `options?: SeaDixOptions` - Configuration options

**Returns:**
- `SeaDix` - New SeaDix instance

**Example:**
```javascript
const trie = SeaDix.fromWords(["hello", "world"]);
console.log(trie.size()); // 2
```

#### `SeaDix.fromJSON(json: object): SeaDix`

Creates a SeaDix instance from a JSON object.

**Parameters:**
- `json: object` - JSON object created by toJSON()

**Returns:**
- `SeaDix` - New SeaDix instance

**Example:**
```javascript
const originalTrie = new SeaDix({ words: ["hello", "world"], ignoreCase: true });
const json = originalTrie.toJSON();
const newTrie = SeaDix.fromJSON(json);
```

## Types

### SeaDixOptions

Configuration options for SeaDix constructor.

```typescript
interface SeaDixOptions {
  words?: string[];      // Initial words to insert
  ignoreCase?: boolean;  // Whether to ignore case (default: false)
}
```

### TrieStats

Statistics about the trie.

```typescript
interface TrieStats {
  wordCount: number;     // Number of words in the trie
  isEmpty: boolean;      // Whether the trie is empty
  allWords: string[];    // All words in the trie
}
```

## Error Handling

All methods that accept string parameters will throw a `TypeError` if the parameter is not a string. Methods that accept arrays will throw a `TypeError` if the parameter is not an array.

The `insert` method will throw an `Error` if the word is empty or contains only whitespace.

## Performance Characteristics

- **Time Complexity:**
  - Insert: O(m) where m is the length of the word
  - Search: O(m) where m is the length of the word
  - Prefix Search: O(m + k) where m is prefix length and k is number of results
  - Remove: O(m) where m is the length of the word

- **Space Complexity:**
  - O(n * m) where n is the number of words and m is the average word length

## Case Sensitivity

By default, SeaDix is case-sensitive. Set `ignoreCase: true` in the constructor options to enable case-insensitive operations.

```javascript
const trie = new SeaDix({ ignoreCase: true });
trie.insert("Hello");
console.log(trie.search("hello")); // true
console.log(trie.search("HELLO")); // true
```
