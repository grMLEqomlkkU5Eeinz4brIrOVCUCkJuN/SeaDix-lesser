import { SeaDix } from "../lib/index";

interface BenchmarkResult {
	arenaSize: number;
	insertTime: number;
	searchTime: number;
	memoryUsage: number;
	operationsPerSecond: number;
}

function benchmarkArenaSize(arenaSize: number, wordCount: number = 10000): BenchmarkResult {
	const trie = new SeaDix(arenaSize);
	const words = Array.from({ length: wordCount }, (_, i) => `word${i.toString().padStart(5, '0')}`);

	// Benchmark insertions
	const insertStart = process.hrtime.bigint();
	words.forEach(word => trie.insert(word));
	const insertEnd = process.hrtime.bigint();
	const insertTime = Number(insertEnd - insertStart) / 1000000; // Convert to milliseconds

	// Benchmark searches
	const searchStart = process.hrtime.bigint();
	words.forEach(word => trie.search(word));
	const searchEnd = process.hrtime.bigint();
	const searchTime = Number(searchEnd - searchStart) / 1000000; // Convert to milliseconds

	// Get memory stats
	const memStats = trie.getMemoryStats();
	const operationsPerSecond = Math.round((wordCount * 1000) / insertTime);

	return {
		arenaSize,
		insertTime,
		searchTime,
		memoryUsage: memStats.totalBytes,
		operationsPerSecond
	};
}

function runArenaBenchmark() {
	console.log("SeaDix Arena Size Performance Benchmark");
	console.log("=====================================");
	console.log("Testing different arena sizes with 10,000 words");
	console.log();

	const arenaSizes = [
		64 * 1024,    // 64KB
		128 * 1024,   // 128KB
		256 * 1024,   // 256KB
		512 * 1024,   // 512KB
		1024 * 1024,  // 1MB (default)
		2 * 1024 * 1024,  // 2MB
		4 * 1024 * 1024,  // 4MB
		8 * 1024 * 1024,  // 8MB
	];

	const results: BenchmarkResult[] = [];

	console.log("Arena Size    Insert (ms)  Search (ms)  Memory (MB)  Ops/sec");
	console.log("------------------------------------------------------------");

	for (const arenaSize of arenaSizes) {
		const result = benchmarkArenaSize(arenaSize);
		results.push(result);

		const arenaSizeKB = Math.round(arenaSize / 1024);
		const memoryMB = (result.memoryUsage / (1024 * 1024)).toFixed(2);
		
		console.log(
			`${arenaSizeKB.toString().padStart(8)}KB  ` +
			`${result.insertTime.toFixed(2).padStart(10)}  ` +
			`${result.searchTime.toFixed(2).padStart(10)}  ` +
			`${memoryMB.padStart(9)}  ` +
			`${result.operationsPerSecond.toLocaleString().padStart(8)}`
		);
	}

	console.log();
	console.log("Analysis:");
	console.log("=========");

	// Find best performing arena size
	const bestInsert = results.reduce((best, current) => 
		current.insertTime < best.insertTime ? current : best
	);
	const bestSearch = results.reduce((best, current) => 
		current.searchTime < best.searchTime ? current : best
	);
	const bestMemory = results.reduce((best, current) => 
		current.memoryUsage < best.memoryUsage ? current : best
	);

	console.log(`Best Insert Performance: ${Math.round(bestInsert.arenaSize / 1024)}KB (${bestInsert.insertTime.toFixed(2)}ms)`);
	console.log(`Best Search Performance: ${Math.round(bestSearch.arenaSize / 1024)}KB (${bestSearch.searchTime.toFixed(2)}ms)`);
	console.log(`Best Memory Usage: ${Math.round(bestMemory.arenaSize / 1024)}KB (${(bestMemory.memoryUsage / (1024 * 1024)).toFixed(2)}MB)`);

	// Calculate performance ratios
	const defaultResult = results.find(r => r.arenaSize === 1024 * 1024)!;
	console.log();
	console.log("Performance vs Default (1MB):");
	console.log("==============================");

	results.forEach(result => {
		const arenaSizeKB = Math.round(result.arenaSize / 1024);
		const insertRatio = (result.insertTime / defaultResult.insertTime).toFixed(2);
		const searchRatio = (result.searchTime / defaultResult.searchTime).toFixed(2);
		const memoryRatio = (result.memoryUsage / defaultResult.memoryUsage).toFixed(2);
		
		console.log(`${arenaSizeKB.toString().padStart(8)}KB: Insert ${insertRatio}x, Search ${searchRatio}x, Memory ${memoryRatio}x`);
	});

	// Recommendations
	console.log();
	console.log("Recommendations:");
	console.log("===============");
	
	const smallArena = results.find(r => r.arenaSize === 64 * 1024);
	const largeArena = results.find(r => r.arenaSize === 8 * 1024 * 1024);
	
	if (smallArena && largeArena) {
		const memoryDiff = ((largeArena.memoryUsage - smallArena.memoryUsage) / (1024 * 1024)).toFixed(2);
		const insertDiff = (largeArena.insertTime - smallArena.insertTime).toFixed(2);
		
		console.log(`• Small datasets (< 1K words): Use 64KB arena`);
		console.log(`• Medium datasets (1K-10K words): Use 256KB-1MB arena`);
		console.log(`• Large datasets (> 10K words): Use 1MB+ arena`);
		console.log(`• Memory difference: ${memoryDiff}MB between smallest and largest`);
		console.log(`• Performance difference: ${insertDiff}ms insert time`);
	}
}

// Run the benchmark
runArenaBenchmark();
