import { SeaDix } from "../lib/index";

console.log("SeaDix Simple Performance Test");
console.log("==============================");

// Test data
const testWords: string[] = [
	"hello", "world", "test", "node", "javascript", "typescript", "react", "vue", "angular", "express",
	"apple", "application", "apply", "applied", "applicant", "applications",
	"book", "booking", "booked", "booker", "bookings", "bookstore",
	"car", "card", "care", "careful", "carefully", "careless", "caretaker",
	"dog", "door", "down", "download", "downward", "downstairs"
];

// Create trie
const trie = new SeaDix();

// Test insertion
console.log("\n1. Insertion Test");
console.log("-----------------");
const insertStart = process.hrtime.bigint();
testWords.forEach(word => trie.insert(word));
const insertEnd = process.hrtime.bigint();
const insertTime = Number(insertEnd - insertStart) / 1000000;
console.log(`Inserted ${testWords.length} words in ${insertTime.toFixed(2)}ms`);
console.log(`Average: ${(insertTime / testWords.length).toFixed(4)}ms per word`);

// Test search
console.log("\n2. Search Test");
console.log("--------------");
const searchStart = process.hrtime.bigint();
testWords.forEach(word => trie.search(word));
const searchEnd = process.hrtime.bigint();
const searchTime = Number(searchEnd - searchStart) / 1000000;
console.log(`Searched ${testWords.length} words in ${searchTime.toFixed(2)}ms`);
console.log(`Average: ${(searchTime / testWords.length).toFixed(4)}ms per search`);

// Test prefix search
console.log("\n3. Prefix Search Test");
console.log("---------------------");
const prefixes: string[] = ["a", "ap", "app", "b", "bo", "boo", "c", "ca", "car", "h", "he", "hel"];
const prefixStart = process.hrtime.bigint();
prefixes.forEach(prefix => {
	const words = trie.getWordsWithPrefix(prefix);
	console.log(`Prefix "${prefix}": ${words.length} words found`);
});
const prefixEnd = process.hrtime.bigint();
const prefixTime = Number(prefixEnd - prefixStart) / 1000000;
console.log(`Prefix searches completed in ${prefixTime.toFixed(2)}ms`);

// Test startsWith
console.log("\n4. startsWith Test");
console.log("------------------");
const startsWithStart = process.hrtime.bigint();
prefixes.forEach(prefix => trie.startsWith(prefix));
const startsWithEnd = process.hrtime.bigint();
const startsWithTime = Number(startsWithEnd - startsWithStart) / 1000000;
console.log(`startsWith checks completed in ${startsWithTime.toFixed(2)}ms`);

// Test delete operations
console.log("\n5. Delete Operations Test");
console.log("-------------------------");
const deleteStart = process.hrtime.bigint();
const wordsToDelete = testWords.slice(0, 10); // Delete first 10 words
wordsToDelete.forEach(word => trie.remove(word));
const deleteEnd = process.hrtime.bigint();
const deleteTime = Number(deleteEnd - deleteStart) / 1000000;
console.log(`Deleted ${wordsToDelete.length} words in ${deleteTime.toFixed(2)}ms`);
console.log(`Average: ${(deleteTime / wordsToDelete.length).toFixed(4)}ms per delete`);

// Test individual operations (not bulk)
console.log("\n6. Individual Operations Test");
console.log("------------------------------");
const individualInsertStart = process.hrtime.bigint();
["test1", "test2", "test3", "test4", "test5"].forEach(word => trie.insert(word));
const individualInsertEnd = process.hrtime.bigint();
const individualInsertTime = Number(individualInsertEnd - individualInsertStart) / 1000000;
console.log(`Individual inserts completed in ${individualInsertTime.toFixed(2)}ms`);

const individualRemoveStart2 = process.hrtime.bigint();
const removeResults2 = trie.removeMany(["test1", "test2", "test3", "test4", "test5"]);
const individualRemoveEnd2 = process.hrtime.bigint();
const individualRemoveTime2 = Number(individualRemoveEnd2 - individualRemoveStart2) / 1000000;
console.log(`Individual removes completed in ${individualRemoveTime2.toFixed(2)}ms`);
console.log(`Removed: ${removeResults2.filter(r => r).length}/${removeResults2.length} words`);

// Test utility operations individually
console.log("\n7. Utility Operations Test");
console.log("--------------------------");

// Test size() performance
const sizeStart = process.hrtime.bigint();
for (let i = 0; i < 1000; i++) {
	trie.size();
}
const sizeEnd = process.hrtime.bigint();
const sizeTime = Number(sizeEnd - sizeStart) / 1000000;
console.log(`size() (1000x): ${sizeTime.toFixed(2)}ms`);

// Test isEmpty() performance
const isEmptyStart = process.hrtime.bigint();
for (let i = 0; i < 1000; i++) {
	trie.isEmpty();
}
const isEmptyEnd = process.hrtime.bigint();
const isEmptyTime = Number(isEmptyEnd - isEmptyStart) / 1000000;
console.log(`isEmpty() (1000x): ${isEmptyTime.toFixed(2)}ms`);

// Test getWordsWithPrefix performance
const prefixTestStart = process.hrtime.bigint();
for (let i = 0; i < 100; i++) {
	trie.getWordsWithPrefix("");
}
const prefixTestEnd = process.hrtime.bigint();
const prefixTestTime = Number(prefixTestEnd - prefixTestStart) / 1000000;
console.log(`getWordsWithPrefix('') (100x): ${prefixTestTime.toFixed(2)}ms`);

// Test getStats performance
const statsStart = process.hrtime.bigint();
for (let i = 0; i < 100; i++) {
	trie.getStats();
}
const statsEnd = process.hrtime.bigint();
const statsTime = Number(statsEnd - statsStart) / 1000000;
console.log(`getStats() (100x): ${statsTime.toFixed(2)}ms`);

const size = trie.size();
const isEmpty = trie.isEmpty();
const allWords = trie.getWordsWithPrefix("");
const stats = trie.getStats();

// Analytics methods output
console.log("\n10. Analytics Methods Output");
console.log("----------------------------");
console.log("Height Stats:", JSON.stringify(trie.getHeightStats(), null, 2));
console.log("Memory Stats:", JSON.stringify(trie.getMemoryStats(), null, 2));
console.log("Word Metrics:", JSON.stringify(trie.getWordMetrics(), null, 2));
console.log("Pattern Search ('*a*'):", trie.patternSearch("*a*"));

// Memory usage (trie-specific)
console.log("\n8. Trie Memory Usage");
console.log("--------------------");
const memUsage = process.memoryUsage();
console.log(`Node.js Heap Used: ${(memUsage.heapUsed / 1024 / 1024).toFixed(2)} MB`);
console.log(`Node.js Heap Total: ${(memUsage.heapTotal / 1024 / 1024).toFixed(2)} MB`);
console.log("Note: This includes all Node.js overhead, not just trie memory");

// Trie statistics
console.log("\n9. Trie Statistics");
console.log("------------------");
console.log(`Total words: ${trie.size()}`);
console.log(`Is empty: ${trie.isEmpty()}`);
console.log(`All words: ${trie.getWordsWithPrefix("").length}`);
console.log(`Stats: ${JSON.stringify(stats, null, 2)}`);

// Test batch operations
console.log("\n8. Batch Operations Test");
console.log("-------------------------");

// Create fresh trie for batch tests
const batchTrie = new SeaDix();
const batchWords = Array.from({ length: 100 }, (_, i) => `batch${i}`);

// Test batch insert vs individual insert
console.log("\nBatch Insert vs Individual Insert:");
const individualBatchStart = process.hrtime.bigint();
batchWords.forEach(word => batchTrie.insert(word));
const individualBatchEnd = process.hrtime.bigint();
const individualBatchTime = Number(individualBatchEnd - individualBatchStart) / 1000000;

// Clear and test batch insert
batchTrie.clear();
const batchInsertStart = process.hrtime.bigint();
const insertedCount = batchTrie.insertBatch(batchWords);
const batchInsertEnd = process.hrtime.bigint();
const batchInsertTime = Number(batchInsertEnd - batchInsertStart) / 1000000;

console.log(`Individual insert (100 words): ${individualBatchTime.toFixed(2)}ms`);
console.log(`Batch insert (100 words): ${batchInsertTime.toFixed(2)}ms`);
console.log(`Improvement: ${((individualBatchTime - batchInsertTime) / individualBatchTime * 100).toFixed(1)}% faster`);

// Test batch search vs individual search
console.log("\nBatch Search vs Individual Search:");
const searchWords = batchWords.slice(0, 50); // Search half the words

const individualSearchStart = process.hrtime.bigint();
searchWords.forEach(word => batchTrie.search(word));
const individualSearchEnd = process.hrtime.bigint();
const individualSearchTime = Number(individualSearchEnd - individualSearchStart) / 1000000;

const batchSearchStart = process.hrtime.bigint();
const searchResults = batchTrie.searchBatch(searchWords);
const batchSearchEnd = process.hrtime.bigint();
const batchSearchTime = Number(batchSearchEnd - batchSearchStart) / 1000000;

console.log(`Individual search (50 words): ${individualSearchTime.toFixed(2)}ms`);
console.log(`Batch search (50 words): ${batchSearchTime.toFixed(2)}ms`);
console.log(`Found: ${searchResults.filter(r => r).length}/${searchResults.length} words`);

// Test batch remove vs individual remove
console.log("\nBatch Remove vs Individual Remove:");
const removeWords = batchWords.slice(0, 30); // Remove 30 words

const individualRemoveStart3 = process.hrtime.bigint();
removeWords.forEach(word => batchTrie.remove(word));
const individualRemoveEnd3 = process.hrtime.bigint();
const individualRemoveTime3 = Number(individualRemoveEnd3 - individualRemoveStart3) / 1000000;

// Re-insert words for batch remove test
removeWords.forEach(word => batchTrie.insert(word));
const batchRemoveStart = process.hrtime.bigint();
const removeResults3 = batchTrie.removeBatch(removeWords);
const batchRemoveEnd = process.hrtime.bigint();
const batchRemoveTime = Number(batchRemoveEnd - batchRemoveStart) / 1000000;

console.log(`Individual remove (30 words): ${individualRemoveTime3.toFixed(2)}ms`);
console.log(`Batch remove (30 words): ${batchRemoveTime.toFixed(2)}ms`);
console.log(`Removed: ${removeResults3.filter(r => r).length}/${removeResults3.length} words`);

console.log("\n------------------------");
const clearStart = process.hrtime.bigint();
trie.clear();
const clearEnd = process.hrtime.bigint();
const clearTime = Number(clearEnd - clearStart) / 1000000;
console.log(`Clear operation completed in ${clearTime.toFixed(2)}ms`);
console.log(`After clear - Size: ${trie.size()}, Is empty: ${trie.isEmpty()}`);

console.log("\nTest completed!");
