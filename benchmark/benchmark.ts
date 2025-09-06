import { SeaDix } from "../lib/index";
import * as fs from "fs";
import * as path from "path";

// Test data sets
const smallDataset: string[] = [
	"hello", "world", "test", "node", "javascript", "typescript", "react", "vue", "angular", "express"
];

const mediumDataset: string[] = [
	"apple", "application", "apply", "applied", "applicant", "applications",
	"book", "booking", "booked", "booker", "bookings", "bookstore",
	"car", "card", "care", "careful", "carefully", "careless", "caretaker",
	"dog", "door", "down", "download", "downward", "downstairs",
	"elephant", "electric", "electronic", "electronics", "elevator",
	"fish", "fire", "first", "firm", "firmly", "firmness",
	"green", "great", "ground", "group", "grow", "growth",
	"house", "home", "hotel", "hospital", "hope", "hopeful",
	"internet", "international", "interview", "interest", "interesting",
	"jump", "just", "justice", "justify", "justification"
];

const largeDataset: string[] = (() => {
	const words: string[] = [];
	const prefixes = ["app", "book", "car", "dog", "ele", "fish", "green", "house", "inter", "jump"];
	const suffixes = ["le", "ing", "ed", "er", "s", "ly", "ful", "ness", "tion", "ment"];
  
	for (let i = 0; i < 1000; i++) {
		const prefix = prefixes[i % prefixes.length];
		const suffix = suffixes[i % suffixes.length];
		const middle = Math.random().toString(36).substring(2, 8);
		words.push(prefix + middle + suffix);
	}
	return words;
})();

interface BenchmarkResult {
  test: string;
  duration: number;
  iterations: number;
  avgPerOp: number;
}

// Benchmark utilities
class Benchmark {
	private name: string;
	private results: BenchmarkResult[] = [];

	constructor(name: string) {
		this.name = name;
	}

	time(fn: () => void, iterations: number = 1): number {
		const start = process.hrtime.bigint();
		for (let i = 0; i < iterations; i++) {
			fn();
		}
		const end = process.hrtime.bigint();
		const duration = Number(end - start) / 1000000; // Convert to milliseconds
		return duration;
	}

	addResult(testName: string, duration: number, iterations: number): void {
		this.results.push({
			test: testName,
			duration: duration,
			iterations: iterations,
			avgPerOp: duration / iterations
		});
	}

	printResults(): void {
		console.log(`\n=== ${this.name} ===`);
		console.log("Test Name".padEnd(30) + "Total (ms)".padEnd(15) + "Avg/Op (ms)".padEnd(15) + "Ops/sec");
		console.log("-".repeat(75));
    
		this.results.forEach(result => {
			const opsPerSec = Math.round(1000 / result.avgPerOp);
			console.log(
				result.test.padEnd(30) + 
        result.duration.toFixed(2).padEnd(15) + 
        result.avgPerOp.toFixed(4).padEnd(15) + 
        opsPerSec.toLocaleString()
			);
		});
	}
}

// Insertion benchmarks
function benchmarkInsertions(): Benchmark {
	const benchmark = new Benchmark("Insertion Performance");
  
	// Test single insert performance with different word types
	// This measures prefix traversal time, not loop overhead
	
	// Short words (minimal traversal)
	const shortWordTime = benchmark.time(() => {
		const trie = new SeaDix();
		trie.insert("hi");
	}, 1000);
	benchmark.addResult("Short words (2 chars)", shortWordTime, 1000);
  
	// Medium words (moderate traversal)
	const mediumWordTime = benchmark.time(() => {
		const trie = new SeaDix();
		trie.insert("hello");
	}, 1000);
	benchmark.addResult("Medium words (5 chars)", mediumWordTime, 1000);
  
	// Long words (deep traversal)
	const longWordTime = benchmark.time(() => {
		const trie = new SeaDix();
		trie.insert("supercalifragilisticexpialidocious");
	}, 1000);
	benchmark.addResult("Long words (34 chars)", longWordTime, 1000);
  
	// Words with common prefixes (tests node splitting)
	const prefixWordTime = benchmark.time(() => {
		const trie = new SeaDix();
		trie.insert("application");
	}, 1000);
	benchmark.addResult("Prefix words (11 chars)", prefixWordTime, 1000);
  
	// Random words (tests general performance)
	const randomWordTime = benchmark.time(() => {
		const trie = new SeaDix();
		trie.insert("qwertyuiop");
	}, 1000);
	benchmark.addResult("Random words (10 chars)", randomWordTime, 1000);
  
	return benchmark;
}

// Dataset traversal benchmarks - tests how prefix sharing affects performance
function benchmarkDatasetTraversal(): Benchmark {
	const benchmark = new Benchmark("Dataset Traversal Performance");
  
	// Test 1: Words with no common prefixes (worst case for radix trie)
	const noPrefixWords = ["apple", "banana", "cherry", "date", "elderberry"];
	const noPrefixTime = benchmark.time(() => {
		const trie = new SeaDix();
		noPrefixWords.forEach(word => trie.insert(word));
	}, 100);
	benchmark.addResult("No common prefixes (5 words)", noPrefixTime, 100 * noPrefixWords.length);
  
	// Test 2: Words with common prefixes (best case for radix trie)
	const commonPrefixWords = ["application", "apply", "applied", "applicant", "applications"];
	const commonPrefixTime = benchmark.time(() => {
		const trie = new SeaDix();
		commonPrefixWords.forEach(word => trie.insert(word));
	}, 100);
	benchmark.addResult("Common prefixes (5 words)", commonPrefixTime, 100 * commonPrefixWords.length);
  
	// Test 3: Mixed prefix scenario
	const mixedPrefixWords = ["hello", "help", "world", "word", "test", "testing"];
	const mixedPrefixTime = benchmark.time(() => {
		const trie = new SeaDix();
		mixedPrefixWords.forEach(word => trie.insert(word));
	}, 100);
	benchmark.addResult("Mixed prefixes (6 words)", mixedPrefixTime, 100 * mixedPrefixWords.length);
  
	// Test 4: Deep prefix sharing
	const deepPrefixWords = ["a", "ab", "abc", "abcd", "abcde", "abcdef"];
	const deepPrefixTime = benchmark.time(() => {
		const trie = new SeaDix();
		deepPrefixWords.forEach(word => trie.insert(word));
	}, 100);
	benchmark.addResult("Deep prefix sharing (6 words)", deepPrefixTime, 100 * deepPrefixWords.length);
  
	return benchmark;
}

// Search benchmarks
function benchmarkSearches(trie: SeaDix, dataset: string[], datasetName: string): Benchmark {
	const benchmark = new Benchmark(`Search Performance - ${datasetName}`);
  
	// Search existing words
	const searchTime = benchmark.time(() => {
		dataset.forEach(word => trie.search(word));
	}, 10);
	benchmark.addResult("Search Existing (10x)", searchTime, 10);
  
	// Search non-existing words
	const nonExistingWords = dataset.map(word => word + "xyz");
	const searchNonExistingTime = benchmark.time(() => {
		nonExistingWords.forEach(word => trie.search(word));
	}, 10);
	benchmark.addResult("Search Non-existing (10x)", searchNonExistingTime, 10);
  
	// Single search
	const singleSearchTime = benchmark.time(() => {
		trie.search(dataset[0]);
	}, 1000);
	benchmark.addResult("Single Search (1000x)", singleSearchTime, 1000);
  
	return benchmark;
}

// Delete benchmarks
function benchmarkDeletes(trie: SeaDix, dataset: string[], datasetName: string): Benchmark {
	const benchmark = new Benchmark(`Delete Performance - ${datasetName}`);
  
	// Delete existing words (create a copy to avoid destroying the original trie)
	const deleteTime = benchmark.time(() => {
		dataset.forEach(word => trie.remove(word));
	}, 1);
	benchmark.addResult("Delete Existing", deleteTime, dataset.length);
	
	// Re-insert words for non-existing delete test
	dataset.forEach(word => trie.insert(word));
  
	// Delete non-existing words
	const nonExistingWords = dataset.map(word => word + "xyz");
	const deleteNonExistingTime = benchmark.time(() => {
		nonExistingWords.forEach(word => trie.remove(word));
	}, 1);
	benchmark.addResult("Delete Non-existing", deleteNonExistingTime, dataset.length);
  
	// Single delete
	const singleDeleteTime = benchmark.time(() => {
		trie.remove("testword");
	}, 1000);
	benchmark.addResult("Single Delete (1000x)", singleDeleteTime, 1000);
  
	return benchmark;
}

// Loop performance benchmarks
function benchmarkLoopPerformance(trie: SeaDix, dataset: string[], datasetName: string): Benchmark {
	const benchmark = new Benchmark(`Loop Performance - ${datasetName}`);
	
	// Test forEach loop performance (fresh trie each iteration)
	const forEachTime = benchmark.time(() => {
		const freshTrie = new SeaDix();
		dataset.forEach(word => freshTrie.insert(word));
	}, 10);
	benchmark.addResult("forEach loop (10x)", forEachTime, dataset.length * 10);
  
	// Test for...of loop performance (fresh trie each iteration)
	const forOfTime = benchmark.time(() => {
		const freshTrie = new SeaDix();
		for (const word of dataset) {
			freshTrie.insert(word);
		}
	}, 10);
	benchmark.addResult("for...of loop (10x)", forOfTime, dataset.length * 10);
  
	// Test traditional for loop performance (fresh trie each iteration)
	const forLoopTime = benchmark.time(() => {
		const freshTrie = new SeaDix();
		for (let i = 0; i < dataset.length; i++) {
			freshTrie.insert(dataset[i]);
		}
	}, 10);
	benchmark.addResult("for loop (10x)", forLoopTime, dataset.length * 10);
  
	return benchmark;
}

// Utility operations benchmarks
function benchmarkUtilityOperations(trie: SeaDix, dataset: string[], datasetName: string): Benchmark {
	const benchmark = new Benchmark(`Utility Operations - ${datasetName}`);
  
	// size() calls
	const sizeTime = benchmark.time(() => {
		trie.size();
	}, 10000);
	benchmark.addResult("size() (10000x)", sizeTime, 10000);
  
	// isEmpty() calls
	const isEmptyTime = benchmark.time(() => {
		trie.isEmpty();
	}, 10000);
	benchmark.addResult("isEmpty() (10000x)", isEmptyTime, 10000);
  
	// getWordsWithPrefix("") calls (equivalent to getAllWords)
	const getAllWordsTime = benchmark.time(() => {
		trie.getWordsWithPrefix("");
	}, 100);
	benchmark.addResult("getWordsWithPrefix('') (100x)", getAllWordsTime, 100);
  
	// getStats() calls
	const getStatsTime = benchmark.time(() => {
		trie.getStats();
	}, 1000);
	benchmark.addResult("getStats() (1000x)", getStatsTime, 1000);
  
	return benchmark;
}

// Prefix benchmarks
function benchmarkPrefixes(trie: SeaDix, dataset: string[], datasetName: string): Benchmark {
	const benchmark = new Benchmark(`Prefix Performance - ${datasetName}`);
  
	// Test various prefixes
	const prefixes = ["a", "ap", "app", "b", "bo", "boo", "c", "ca", "car"];
  
	// startsWith
	const startsWithTime = benchmark.time(() => {
		prefixes.forEach(prefix => trie.startsWith(prefix));
	}, 100);
	benchmark.addResult("startsWith (100x)", startsWithTime, prefixes.length * 100);
  
	// getWordsWithPrefix
	const getWordsTime = benchmark.time(() => {
		prefixes.forEach(prefix => trie.getWordsWithPrefix(prefix));
	}, 10);
	benchmark.addResult("getWordsWithPrefix (10x)", getWordsTime, prefixes.length * 10);
  
	return benchmark;
}

// Case sensitivity benchmark
function benchmarkCaseSensitivity(): Benchmark {
	const benchmark = new Benchmark("Case Sensitivity Performance");
  
	// Case sensitive trie
	const caseSensitiveTrie = new SeaDix({ ignoreCase: false });
	const caseSensitiveTime = benchmark.time(() => {
		mediumDataset.forEach(word => caseSensitiveTrie.insert(word));
	});
	benchmark.addResult("Case Sensitive Insert", caseSensitiveTime, mediumDataset.length);
  
	// Case insensitive trie
	const caseInsensitiveTrie = new SeaDix({ ignoreCase: true });
	const caseInsensitiveTime = benchmark.time(() => {
		mediumDataset.forEach(word => caseInsensitiveTrie.insert(word));
	});
	benchmark.addResult("Case Insensitive Insert", caseInsensitiveTime, mediumDataset.length);
  
	// Search performance comparison
	const searchSensitiveTime = benchmark.time(() => {
		mediumDataset.forEach(word => caseSensitiveTrie.search(word));
	}, 10);
	benchmark.addResult("Case Sensitive Search (10x)", searchSensitiveTime, mediumDataset.length * 10);
  
	const searchInsensitiveTime = benchmark.time(() => {
		mediumDataset.forEach(word => caseInsensitiveTrie.search(word));
	}, 10);
	benchmark.addResult("Case Insensitive Search (10x)", searchInsensitiveTime, mediumDataset.length * 10);
  
	return benchmark;
}

// JSON serialization benchmark
function benchmarkSerialization(): Benchmark {
	const benchmark = new Benchmark("Serialization Performance");
  
	const trie = new SeaDix({ words: mediumDataset, ignoreCase: true });
  
	// toJSON performance
	const toJsonTime = benchmark.time(() => {
		trie.toJSON();
	}, 1000);
	benchmark.addResult("toJSON (1000x)", toJsonTime, 1000);
  
	// fromJSON performance
	const json = trie.toJSON();
	const fromJsonTime = benchmark.time(() => {
		SeaDix.fromJSON(json);
	}, 100);
	benchmark.addResult("fromJSON (100x)", fromJsonTime, 100);
  
	return benchmark;
}

// Memory usage benchmark
function benchmarkMemory(): Benchmark {
	const benchmark = new Benchmark("Memory Usage");
  
	// Force garbage collection if available
	if (global.gc) {
		global.gc();
	}
  
	const initialMemory = process.memoryUsage();
  
	// Create a single trie with large dataset
	const trie = new SeaDix();
	const insertTime = benchmark.time(() => {
		largeDataset.forEach(word => trie.insert(word));
	});
	benchmark.addResult("Insert 1000 words", insertTime, 1);
  
	const afterInsertMemory = process.memoryUsage();
	const memoryPerWord = (afterInsertMemory.heapUsed - initialMemory.heapUsed) / largeDataset.length;
	benchmark.addResult("Memory per word (bytes)", memoryPerWord, 1);
  
	// Test memory usage with different operations
	const searchTime = benchmark.time(() => {
		largeDataset.forEach(word => trie.search(word));
	});
	benchmark.addResult("Search 1000 words", searchTime, 1);
  
	const prefixTime = benchmark.time(() => {
		trie.getWordsWithPrefix("a");
		trie.getWordsWithPrefix("b");
		trie.getWordsWithPrefix("c");
	});
	benchmark.addResult("Prefix operations", prefixTime, 1);
  
	// Test memory after deletions
	const deleteTime = benchmark.time(() => {
		largeDataset.slice(0, 500).forEach(word => trie.remove(word));
	});
	benchmark.addResult("Delete 500 words", deleteTime, 1);
  
	const afterDeleteMemory = process.memoryUsage();
	const memoryAfterDelete = (afterDeleteMemory.heapUsed - initialMemory.heapUsed) / 1024 / 1024;
	benchmark.addResult("Memory after cleanup (MB)", memoryAfterDelete, 1);
  
	return benchmark;
}

// Batch operations benchmark
function benchmarkBatchOperations(): Benchmark {
	const benchmark = new Benchmark("Batch Operations Performance");
  
	// Test batch insert vs individual insert
	const words = Array.from({ length: 1000 }, (_, i) => `word${i}`);
	
	// Individual insertions
	const individualTrie = new SeaDix();
	const individualTime = benchmark.time(() => {
		words.forEach(word => individualTrie.insert(word));
	});
	benchmark.addResult("Individual Insert (1000 words)", individualTime, words.length);
  
	// Batch insertions
	const batchTrie = new SeaDix();
	const batchTime = benchmark.time(() => {
		batchTrie.insertBatch(words);
	});
	benchmark.addResult("Batch Insert (1000 words)", batchTime, words.length);
  
	// Calculate improvement
	const improvement = ((individualTime - batchTime) / individualTime * 100).toFixed(1);
	console.log(`\nBatch Insert Improvement: ${improvement}% faster`);
  
	// Test batch search vs individual search
	const searchWords = words.slice(0, 500); // Search half the words
	
	// Individual searches
	const individualSearchTime = benchmark.time(() => {
		searchWords.forEach(word => individualTrie.search(word));
	});
	benchmark.addResult("Individual Search (500 words)", individualSearchTime, searchWords.length);
  
	// Batch searches
	const batchSearchTime = benchmark.time(() => {
		batchTrie.searchBatch(searchWords);
	});
	benchmark.addResult("Batch Search (500 words)", batchSearchTime, searchWords.length);
  
	// Test batch remove vs individual remove
	const removeWords = words.slice(0, 200); // Remove 200 words
	
	// Individual removes
	const individualRemoveTime = benchmark.time(() => {
		removeWords.forEach(word => individualTrie.remove(word));
	});
	benchmark.addResult("Individual Remove (200 words)", individualRemoveTime, removeWords.length);
  
	// Batch removes
	const batchRemoveTime = benchmark.time(() => {
		batchTrie.removeBatch(removeWords);
	});
	benchmark.addResult("Batch Remove (200 words)", batchRemoveTime, removeWords.length);
  
	return benchmark;
}

// Batch vs Individual comparison benchmark
function benchmarkBatchComparison(): Benchmark {
	const benchmark = new Benchmark("Batch vs Individual Comparison");
  
	const testSizes = [10, 50, 100, 500, 1000];
	
	for (const size of testSizes) {
		const words = Array.from({ length: size }, (_, i) => `test${i}`);
		
		// Individual operations
		const individualTrie = new SeaDix();
		const individualTime = benchmark.time(() => {
			words.forEach(word => individualTrie.insert(word));
		});
		benchmark.addResult(`Individual Insert (${size} words)`, individualTime, size);
		
		// Batch operations
		const batchTrie = new SeaDix();
		const batchTime = benchmark.time(() => {
			batchTrie.insertBatch(words);
		});
		benchmark.addResult(`Batch Insert (${size} words)`, batchTime, size);
		
		// Calculate speedup
		const speedup = (individualTime / batchTime).toFixed(2);
		console.log(`  Speedup: ${speedup}x`);
	}
  
	return benchmark;
}

// Run all benchmarks
function runAllBenchmarks(): void {
	console.log("SeaDix Performance Benchmarks");
	console.log("=============================");
  
	// Insertion benchmarks
	const insertBenchmark = benchmarkInsertions();
	insertBenchmark.printResults();
  
	// Dataset traversal benchmarks
	const traversalBenchmark = benchmarkDatasetTraversal();
	traversalBenchmark.printResults();
  
	// Search benchmarks for different dataset sizes
	const smallTrie = new SeaDix();
	smallDataset.forEach(word => smallTrie.insert(word));
	const smallSearchBenchmark = benchmarkSearches(smallTrie, smallDataset, "Small Dataset");
	smallSearchBenchmark.printResults();
  
	const mediumTrie = new SeaDix();
	mediumDataset.forEach(word => mediumTrie.insert(word));
	const mediumSearchBenchmark = benchmarkSearches(mediumTrie, mediumDataset, "Medium Dataset");
	mediumSearchBenchmark.printResults();
  
	const largeTrie = new SeaDix();
	largeDataset.forEach(word => largeTrie.insert(word));
	const largeSearchBenchmark = benchmarkSearches(largeTrie, largeDataset, "Large Dataset");
	largeSearchBenchmark.printResults();
  
	// Delete benchmarks
	const smallDeleteTrie = new SeaDix();
	smallDataset.forEach(word => smallDeleteTrie.insert(word));
	const smallDeleteBenchmark = benchmarkDeletes(smallDeleteTrie, smallDataset, "Small Dataset");
	smallDeleteBenchmark.printResults();
  
	const mediumDeleteTrie = new SeaDix();
	mediumDataset.forEach(word => mediumDeleteTrie.insert(word));
	const mediumDeleteBenchmark = benchmarkDeletes(mediumDeleteTrie, mediumDataset, "Medium Dataset");
	mediumDeleteBenchmark.printResults();
  
	const largeDeleteTrie = new SeaDix();
	largeDataset.forEach(word => largeDeleteTrie.insert(word));
	const largeDeleteBenchmark = benchmarkDeletes(largeDeleteTrie, largeDataset, "Large Dataset");
	largeDeleteBenchmark.printResults();
  
	// Loop performance benchmarks
	const loopTrie = new SeaDix();
	const loopBenchmark = benchmarkLoopPerformance(loopTrie, mediumDataset, "Medium Dataset");
	loopBenchmark.printResults();
  
	// Utility operations benchmarks
	const utilityTrie = new SeaDix();
	mediumDataset.forEach(word => utilityTrie.insert(word));
	const utilityBenchmark = benchmarkUtilityOperations(utilityTrie, mediumDataset, "Medium Dataset");
	utilityBenchmark.printResults();
  
	// Prefix benchmarks
	const smallPrefixBenchmark = benchmarkPrefixes(smallTrie, smallDataset, "Small Dataset");
	smallPrefixBenchmark.printResults();
  
	const mediumPrefixBenchmark = benchmarkPrefixes(mediumTrie, mediumDataset, "Medium Dataset");
	mediumPrefixBenchmark.printResults();
  
	const largePrefixBenchmark = benchmarkPrefixes(largeTrie, largeDataset, "Large Dataset");
	largePrefixBenchmark.printResults();
  
	// Case sensitivity benchmark
	const caseSensitivityBenchmark = benchmarkCaseSensitivity();
	caseSensitivityBenchmark.printResults();
  
	// Serialization benchmark
	const serializationBenchmark = benchmarkSerialization();
	serializationBenchmark.printResults();
  
	// Memory benchmark
	const memoryBenchmark = benchmarkMemory();
	memoryBenchmark.printResults();
  
	// Batch operations benchmarks
	const batchBenchmark = benchmarkBatchOperations();
	batchBenchmark.printResults();
  
	const batchComparisonBenchmark = benchmarkBatchComparison();
	batchComparisonBenchmark.printResults();
  
	console.log("\nBenchmark completed!");
}

// Run benchmarks if this file is executed directly
if (require.main === module) {
	runAllBenchmarks();
}

export {
	Benchmark,
	benchmarkInsertions,
	benchmarkDatasetTraversal,
	benchmarkSearches,
	benchmarkDeletes,
	benchmarkLoopPerformance,
	benchmarkUtilityOperations,
	benchmarkPrefixes,
	benchmarkCaseSensitivity,
	benchmarkSerialization,
	benchmarkMemory,
	benchmarkBatchOperations,
	benchmarkBatchComparison,
	runAllBenchmarks
};
