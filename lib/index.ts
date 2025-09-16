/**
 * SeaDix - A Radix Trie implementation for Node.js
 * Built with C++
 */

// Import the native module
const native = require("../build/Release/seadix");

/**
 * Interface for the native SeaDix class
 */
interface NativeSeaDix {
  insert(word: string): void;
  insertBatch(words: string[]): number;
  insertFromFile(path: string, bufferSize?: number): number;  // Added this line
  search(word: string): boolean;
  searchBatch(words: string[]): boolean[];
  startsWith(prefix: string): boolean;
  wordsWithPrefix(prefix: string): string[];
  remove(word: string): boolean;
  removeBatch(words: string[]): boolean[];
  empty(): boolean;
  size(): number;
  clear(): void;

	// Analytics methods
	getHeightStats(): {
		minHeight: number;
		maxHeight: number;
		averageHeight: number;
		modeHeight: number;
		allHeights: number[];
	};
	getMemoryStats(): {
		totalBytes: number;
		nodeCount: number;
		stringBytes: number;
		overheadBytes: number;
		bytesPerWord: number;
	};
	getWordMetrics(): {
		minLength: number;
		maxLength: number;
		averageLength: number;
		modeLength: number;
		lengthDistribution: number[];
		totalCharacters: number;
	};
	patternSearch(pattern: string): string[];
}

/**
 * Options for SeaDix constructor
 */
export interface SeaDixOptions {
  /**
   * Initial words to insert into the trie
   */
  words?: string[];

  /**
   * Whether to ignore case when inserting/searching
   * @default false
   */
  ignoreCase?: boolean;
}

/**
 * Statistics about the trie
 */
export interface TrieStats {
  /** Number of words in the trie */
  wordCount: number;

  /** Whether the trie is empty */
  isEmpty: boolean;

  /** All words in the trie */
  allWords: string[];
}

/**
 * A Radix Trie implementation
 *
 * @example
 * ```typescript
 * import { SeaDix } from 'seadix';
 *
 * const trie = new SeaDix();
 * trie.insert('hello');
 * trie.insert('world');
 *
 * console.log(trie.search('hello')); // true
 * console.log(trie.startsWith('he')); // true
 * console.log(trie.getWordsWithPrefix('h')); // ['hello']
 * ```
 */
export class SeaDix {
	private readonly nativeTrie: NativeSeaDix;
	private readonly ignoreCase: boolean;

	/**
   * Create a new SeaDix instance
   *
   * @param options - Configuration options
   */
	constructor(options: SeaDixOptions = {}) {
		this.nativeTrie = new native.SeaDix();
		this.ignoreCase = options.ignoreCase ?? false;

		// Insert initial words if provided
		if (options.words && options.words.length > 0) {
			for (const word of options.words) {
				this.insert(word);
			}
		}
	}

	/**
	 * Get height statistics for the trie
	 * @returns Object with minHeight, maxHeight, averageHeight, modeHeight, allHeights
	 */
	getHeightStats(): {
		minHeight: number;
		maxHeight: number;
		averageHeight: number;
		modeHeight: number;
		allHeights: number[];
		} {
		return this.nativeTrie.getHeightStats();
	}

	/**
	 * Get memory usage statistics for the trie
	 * @returns Object with totalBytes, nodeCount, stringBytes, overheadBytes, bytesPerWord
	 */
	getMemoryStats(): {
		totalBytes: number;
		nodeCount: number;
		stringBytes: number;
		overheadBytes: number;
		bytesPerWord: number;
		} {
		return this.nativeTrie.getMemoryStats();
	}

	/**
	 * Get word metrics for the trie
	 * @returns Object with minLength, maxLength, averageLength, modeLength, lengthDistribution, totalCharacters
	 */
	getWordMetrics(): {
		minLength: number;
		maxLength: number;
		averageLength: number;
		modeLength: number;
		lengthDistribution: number[];
		totalCharacters: number;
		} {
		return this.nativeTrie.getWordMetrics();
	}

	/**
	 * Search for words matching a pattern (supports * and ? wildcards)
	 * @param pattern - Pattern string with wildcards
	 * @returns Array of matching words
	 */
	patternSearch(pattern: string): string[] {
		if (typeof pattern !== "string") {
			throw new TypeError("Pattern must be a string");
		}
		return this.nativeTrie.patternSearch(pattern);
	}

	/**
   * Normalize a word based on case sensitivity settings
   */
	private normalizeWord(word: string): string {
		if (typeof word !== "string") {
			throw new TypeError("Word must be a string");
		}
		return this.ignoreCase ? word.toLowerCase() : word;
	}

	/**
   * Validate that a word is not empty
   */
	private validateWord(word: string): void {
		if (!word || word.trim().length === 0) {
			throw new Error("Word cannot be empty or whitespace only");
		}
	}

	/**
   * Insert a word into the trie
   *
   * @param word - The word to insert
   * @throws {TypeError} If word is not a string
   * @throws {Error} If word is empty or whitespace only
   *
   * @example
   * ```typescript
   * trie.insert('hello');
   * trie.insert('world');
   * ```
   */
	insert(word: string): void {
		this.validateWord(word);
		const normalizedWord = this.normalizeWord(word);
		this.nativeTrie.insert(normalizedWord);
	}

	/**
	 * Insert multiple words in a single batch operation
	 * This reduces N-API overhead by making only one call to C++
	 *
	 * @param words - Array of words to insert
	 * @returns Number of words successfully inserted
	 * @throws {TypeError} If words is not an array
	 *
	 * @example
	 * ```typescript
	 * const count = trie.insertBatch(['hello', 'world', 'test']);
	 * console.log(`Inserted ${count} words`);
	 * ```
	 */
	insertBatch(words: string[]): number {
		if (!Array.isArray(words)) {
			throw new TypeError("Words must be an array");
		}

		// Normalize all words before batch insert
		const normalizedWords = words
			.filter(word => word && typeof word === "string" && word.trim().length > 0)
			.map(word => this.normalizeWord(word));

		return this.nativeTrie.insertBatch(normalizedWords);
	}

	/**
	 * Insert words from a text file using streaming
	 * This is highly efficient for large files as it processes them in chunks
	 *
	 * @param filePath - Path to the text file containing words (one per line)
	 * @param bufferSize - Buffer size in bytes for file streaming (default: 1MB)
	 * @returns Number of words successfully inserted
	 * @throws {TypeError} If filePath is not a string
	 * @throws {Error} If file cannot be read or buffer size is invalid
	 *
	 * @example
	 * ```typescript
	 * // Insert from file with default 1MB buffer
	 * const count = trie.insertFromFile('./words.txt');
	 *
	 * // Insert from file with custom 1KB buffer
	 * const count2 = trie.insertFromFile('./words.txt', 1024);
	 * console.log(`Inserted ${count2} words`);
	 * ```
	 */
	insertFromFile(filePath: string, bufferSize?: number): number {
		if (typeof filePath !== "string") {
			throw new TypeError("File path must be a string");
		}

		if (bufferSize !== undefined) {
			if (typeof bufferSize !== "number" || bufferSize <= 0) {
				throw new Error("Buffer size must be a positive number");
			}
		}

		try {
			return this.nativeTrie.insertFromFile(filePath, bufferSize);
		} catch (error: any) {
			throw new Error(`Failed to insert from file: ${error.message}`);
		}
	}

	/**
   * Search for a word in the trie
   *
   * @param word - The word to search for
   * @returns True if the word exists, false otherwise
   * @throws {TypeError} If word is not a string
   *
   * @example
   * ```typescript
   * trie.insert('hello');
   * console.log(trie.search('hello')); // true
   * console.log(trie.search('hell'));  // false
   * ```
   */
	search(word: string): boolean {
		if (typeof word !== "string") {
			throw new TypeError("Word must be a string");
		}

		if (word.trim().length === 0) {
			return false;
		}

		const normalizedWord = this.normalizeWord(word);
		return this.nativeTrie.search(normalizedWord);
	}

	/**
	 * Search for multiple words in a single batch operation
	 * This reduces N-API overhead by making only one call to C++
	 *
	 * @param words - Array of words to search for
	 * @returns Array of boolean results for each word
	 * @throws {TypeError} If words is not an array
	 *
	 * @example
	 * ```typescript
	 * const results = trie.searchBatch(['hello', 'world', 'test']);
	 * console.log(results); // [true, true, false]
	 * ```
	 */
	searchBatch(words: string[]): boolean[] {
		if (!Array.isArray(words)) {
			throw new TypeError("Words must be an array");
		}

		// Normalize all words before batch search
		const normalizedWords = words.map(word =>
			word && typeof word === "string" ? this.normalizeWord(word) : ""
		);

		return this.nativeTrie.searchBatch(normalizedWords);
	}

	/**
   * Check if any word in the trie starts with the given prefix
   *
   * @param prefix - The prefix to check
   * @returns True if any word starts with the prefix, false otherwise
   * @throws {TypeError} If prefix is not a string
   *
   * @example
   * ```typescript
   * trie.insert('hello');
   * console.log(trie.startsWith('he'));   // true
   * console.log(trie.startsWith('world')); // false
   * ```
   */
	startsWith(prefix: string): boolean {
		if (typeof prefix !== "string") {
			throw new TypeError("Prefix must be a string");
		}

		if (prefix.length === 0) {
			return this.size() > 0;
		}

		const normalizedPrefix = this.normalizeWord(prefix);
		return this.nativeTrie.startsWith(normalizedPrefix);
	}

	/**
   * Get all words that start with the given prefix
   *
   * @param prefix - The prefix to search for
   * @returns Array of words that start with the prefix
   * @throws {TypeError} If prefix is not a string
   *
   * @example
   * ```typescript
   * trie.insertMany(['hello', 'help', 'world']);
   * console.log(trie.getWordsWithPrefix('he')); // ['hello', 'help']
   * ```
   */
	getWordsWithPrefix(prefix: string): string[] {
		if (typeof prefix !== "string") {
			throw new TypeError("Prefix must be a string");
		}

		const normalizedPrefix = this.normalizeWord(prefix);
		const results = this.nativeTrie.wordsWithPrefix(normalizedPrefix);

		// If case insensitive, we might want to return original casing
		// For now, we return the normalized results
		return results;
	}

	/**
   * Remove a word from the trie
   *
   * @param word - The word to remove
   * @returns True if the word was removed, false if it wasn't found
   * @throws {TypeError} If word is not a string
   *
   * @example
   * ```typescript
   * trie.insert('hello');
   * console.log(trie.remove('hello')); // true
   * console.log(trie.remove('hello')); // false (already removed)
   * ```
   */
	remove(word: string): boolean {
		if (typeof word !== "string") {
			throw new TypeError("Word must be a string");
		}

		if (word.trim().length === 0) {
			return false;
		}

		const normalizedWord = this.normalizeWord(word);
		return this.nativeTrie.remove(normalizedWord);
	}

	/**
	 * Remove multiple words in a single batch operation
	 * This reduces N-API overhead by making only one call to C++
	 *
	 * @param words - Array of words to remove
	 * @returns Array of boolean results indicating success for each word
	 * @throws {TypeError} If words is not an array
	 *
	 * @example
	 * ```typescript
	 * const results = trie.removeBatch(['hello', 'world', 'test']);
	 * console.log(results); // [true, true, false]
	 * ```
	 */
	removeBatch(words: string[]): boolean[] {
		if (!Array.isArray(words)) {
			throw new TypeError("Words must be an array");
		}

		// Normalize all words before batch remove
		const normalizedWords = words.map(word =>
			word && typeof word === "string" ? this.normalizeWord(word) : ""
		);

		return this.nativeTrie.removeBatch(normalizedWords);
	}

	/**
   * Remove multiple words from the trie
   *
   * @param words - Array of words to remove
   * @returns Array of booleans indicating which words were successfully removed
   * @throws {TypeError} If words is not an array
   *
   * @example
   * ```typescript
   * trie.insertMany(['hello', 'world']);
   * const results = trie.removeMany(['hello', 'foo']); // [true, false]
   * ```
   */
	removeMany(words: string[]): boolean[] {
		if (!Array.isArray(words)) {
			throw new TypeError("Words must be an array");
		}

		return words.map(word => this.remove(word));
	}

	/**
   * Check if the trie is empty
   *
   * @returns True if the trie contains no words, false otherwise
   *
   * @example
   * ```typescript
   * const trie = new SeaDix();
   * console.log(trie.isEmpty()); // true
   * trie.insert('hello');
   * console.log(trie.isEmpty()); // false
   * ```
   */
	isEmpty(): boolean {
		return this.nativeTrie.empty();
	}


	/**
   * Get the number of words in the trie
   *
   * @returns The number of words stored in the trie
   *
   * @example
   * ```typescript
   * trie.insertMany(['hello', 'world']);
   * console.log(trie.size()); // 2
   * ```
   */
	size(): number {
		return this.nativeTrie.size();
	}

	/**
   * Clear all words from the trie
   *
   * @example
   * ```typescript
   * trie.insertMany(['hello', 'world']);
   * trie.clear();
   * console.log(trie.size()); // 0
   * ```
   */
	clear(): void {
		this.nativeTrie.clear();
	}

	/**
   * Get statistics about the trie
   *
   * @returns Object containing trie statistics
   *
   * @example
   * ```typescript
   * const stats = trie.getStats();
   * console.log(stats.wordCount); // Number of words
   * console.log(stats.isEmpty);   // Whether trie is empty
   * ```
   */
	getStats(): TrieStats {
		return {
			wordCount: this.size(),
			isEmpty: this.isEmpty(),
			allWords: this.getWordsWithPrefix("")
		};
	}

	/**
   * Create a new SeaDix instance from an array of words
   *
   * @param words - Array of words to insert
   * @param options - Configuration options
   * @returns New SeaDix instance
   *
   * @example
   * ```typescript
   * const trie = SeaDix.fromWords(['hello', 'world']);
   * console.log(trie.size()); // 2
   * ```
   */
	static fromWords(words: string[], options: Omit<SeaDixOptions, "words"> = {}): SeaDix {
		const trie = new SeaDix(options);
		for (const word of words) {
			trie.insert(word);
		}
		return trie;
	}

	/**
   * Convert the trie to a JSON-serializable object
   *
   * @returns Object that can be JSON.stringify'd
   *
   * @example
   * ```typescript
   * const json = JSON.stringify(trie.toJSON());
   * ```
   */
	toJSON(): { words: string[]; options: { ignoreCase: boolean } } {
		return {
			words: this.getWordsWithPrefix(""),
			options: {
				ignoreCase: this.ignoreCase
			}
		};
	}

	/**
   * Create a SeaDix instance from a JSON object
   *
   * @param json - JSON object created by toJSON()
   * @returns New SeaDix instance
   *
   * @example
   * ```typescript
   * const trie1 = new SeaDix();
   * trie1.insert('hello');
   * const json = trie1.toJSON();
   * const trie2 = SeaDix.fromJSON(json);
   * ```
   */
	static fromJSON(json: { words: string[]; options?: { ignoreCase?: boolean } }): SeaDix {
		const trie = new SeaDix({
			ignoreCase: json.options?.ignoreCase ?? false
		});
		for (const word of json.words) {
			trie.insert(word);
		}
		return trie;
	}
}

// Export the class as default
export default SeaDix;

// Also export the native module for direct access
export { native as nativeModule };