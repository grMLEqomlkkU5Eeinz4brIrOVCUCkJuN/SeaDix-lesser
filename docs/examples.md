# SeaDix Usage Examples

This is an experimental Radix Trie implementation for learning about N-API performance. The examples below show how to use the library, but keep in mind that N-API overhead makes this primarily useful for understanding JavaScript/C++ interop.

## Basic Operations

### Creating a Trie

```javascript
const { SeaDix } = require("seadix");

// Empty trie
const trie = new SeaDix();

// With initial words
const trieWithWords = new SeaDix({
	words: ["hello", "world", "test"]
});

// Case-insensitive trie
const caseInsensitiveTrie = new SeaDix({
	ignoreCase: true
});
```

### Inserting Words

```javascript
const trie = new SeaDix();

// Single word
trie.insert("hello");

// Multiple words
["world", "test", "node"].forEach(word => trie.insert(word));

// From array
const words = ["apple", "application", "apply"];
words.forEach(word => trie.insert(word));
```

### Searching Words

```javascript
const trie = new SeaDix();
["hello", "world", "help"].forEach(word => trie.insert(word));

// Check if word exists
console.log(trie.search("hello")); // true
console.log(trie.search("hell"));  // false

// Check if any word starts with prefix
console.log(trie.startsWith("he")); // true
console.log(trie.startsWith("xyz")); // false

// Get all words with prefix
console.log(trie.getWordsWithPrefix("he")); // ["hello", "help"]
console.log(trie.getWordsWithPrefix(""));   // ["hello", "world", "help"]
```

### Batch Operations (Recommended)

```javascript
const trie = new SeaDix();

// Batch insert - faster than individual calls for small datasets
const words = ["hello", "world", "test", "batch", "operations"];
const count = trie.insertBatch(words);
console.log(`Inserted ${count} words`); // Inserted 5 words

// Batch search
const searchWords = ["hello", "world", "missing", "test"];
const results = trie.searchBatch(searchWords);
console.log(results); // [true, true, false, true]

// Batch remove
const removeWords = ["hello", "missing", "test"];
const removeResults = trie.removeBatch(removeWords);
console.log(removeResults); // [true, false, true]
```

### Individual Operations

```javascript
const trie = new SeaDix();

// Insert single word
trie.insert("hello");
trie.insert("world");

// Search single word
console.log(trie.search("hello")); // true
console.log(trie.search("hell"));  // false

// Remove single word
const removed = trie.remove("hello");
console.log(removed); // true
console.log(trie.search("hello")); // false
```

## Real-World Examples

### Autocomplete System

```javascript
const { SeaDix } = require("seadix");

class AutocompleteSystem {
	constructor() {
		this.trie = new SeaDix();
	}

	addWords(words) {
		words.forEach(word => this.trie.insert(word));
	}

	getSuggestions(prefix) {
		return this.trie.getWordsWithPrefix(prefix);
	}

	hasWord(word) {
		return this.trie.search(word);
	}
}

// Usage
const autocomplete = new AutocompleteSystem();
autocomplete.addWords([
	"apple", "application", "apply", "applied", "applicant",
	"book", "booking", "booked", "booker", "bookings",
	"car", "card", "care", "careful", "carefully"
]);

console.log(autocomplete.getSuggestions("app")); 
// ["apple", "application", "apply", "applied", "applicant"]

console.log(autocomplete.getSuggestions("book"));
// ["book", "booking", "booked", "booker", "bookings"]
```

### Spell Checker

```javascript
const { SeaDix } = require("seadix");

class SpellChecker {
	constructor(dictionary) {
		this.trie = new SeaDix({ words: dictionary, ignoreCase: true });
	}

	isCorrect(word) {
		return this.trie.search(word);
	}

	getSuggestions(word) {
		// Get words with similar prefixes
		const prefix = word.substring(0, Math.max(1, word.length - 2));
		return this.trie.getWordsWithPrefix(prefix);
	}
}

// Usage
const dictionary = ["hello", "world", "help", "test", "node", "javascript"];
const spellChecker = new SpellChecker(dictionary);

console.log(spellChecker.isCorrect("hello")); // true
console.log(spellChecker.isCorrect("helo"));  // false
console.log(spellChecker.getSuggestions("hel")); // ["hello", "help"]
```

### Word Filter

```javascript
const { SeaDix } = require("seadix");

class WordFilter {
	constructor(bannedWords) {
		this.trie = new SeaDix({ words: bannedWords, ignoreCase: true });
	}

	containsBannedWord(text) {
		const words = text.toLowerCase().split(/\s+/);
		return words.some(word => this.trie.search(word));
	}

	getBannedWords(text) {
		const words = text.toLowerCase().split(/\s+/);
		return words.filter(word => this.trie.search(word));
	}
}

// Usage
const bannedWords = ["spam", "scam", "fake", "fraud"];
const filter = new WordFilter(bannedWords);

const text1 = "This is a legitimate offer";
const text2 = "This is a spam message";

console.log(filter.containsBannedWord(text1)); // false
console.log(filter.containsBannedWord(text2)); // true
console.log(filter.getBannedWords(text2)); // ["spam"]
```

### Prefix Counter

```javascript
const { SeaDix } = require("seadix");

class PrefixCounter {
	constructor() {
		this.trie = new SeaDix();
	}

	addWord(word) {
		this.trie.insert(word);
	}

	getPrefixCount(prefix) {
		return this.trie.getWordsWithPrefix(prefix).length;
	}

	getPrefixWords(prefix) {
		return this.trie.getWordsWithPrefix(prefix);
	}

	getStats() {
		return this.trie.getStats();
	}
}

// Usage
const counter = new PrefixCounter();
counter.addWord("apple");
counter.addWord("application");
counter.addWord("apply");
counter.addWord("book");
counter.addWord("booking");

console.log(counter.getPrefixCount("app")); // 3
console.log(counter.getPrefixWords("app")); // ["apple", "application", "apply"]
console.log(counter.getStats().wordCount); // 5
```

### Trie Serialization

```javascript
const { SeaDix } = require("seadix");

// Create and populate trie
const trie1 = new SeaDix({ 
	words: ["hello", "world", "test"], 
	ignoreCase: true 
});

// Serialize to JSON
const json = trie1.toJSON();
console.log(json);
// { words: ["hello", "world", "test"], options: { ignoreCase: true } }

// Save to file
const fs = require("fs");
fs.writeFileSync("trie.json", JSON.stringify(json, null, 2));

// Load from file
const loadedJson = JSON.parse(fs.readFileSync("trie.json", "utf8"));
const trie2 = SeaDix.fromJSON(loadedJson);

console.log(trie2.size()); // 3
console.log(trie2.search("HELLO")); // true (case insensitive)
```

### Performance Testing

```javascript
const { SeaDix } = require("seadix");

function performanceTest() {
	const trie = new SeaDix();
	const words = [];
	
	// Generate test words
	for (let i = 0; i < 10000; i++) {
		words.push(`word${i}`);
	}

	// Test insertion
	console.time("Insertion");
	words.forEach(word => trie.insert(word));
	console.timeEnd("Insertion");

	// Test search
	console.time("Search");
	words.forEach(word => trie.search(word));
	console.timeEnd("Search");

	// Test prefix search
	console.time("Prefix Search");
	for (let i = 0; i < 1000; i++) {
		trie.getWordsWithPrefix("word");
	}
	console.timeEnd("Prefix Search");

	console.log(`Total words: ${trie.size()}`);
}

performanceTest();
```

## Error Handling

```javascript
const { SeaDix } = require("seadix");
const trie = new SeaDix();

try {
	// These will throw errors
	trie.insert("");           // Error: Word cannot be empty
	trie.insert(null);         // TypeError: Word must be a string
	// No insertMany method - use forEach instead
	// ["word1", "word2"].forEach(word => trie.insert(word));
} catch (error) {
	console.error("Error:", error.message);
}

// Safe operations
try {
	trie.insert("valid-word");
	console.log(trie.search("valid-word")); // true
} catch (error) {
	console.error("Unexpected error:", error.message);
}
```
