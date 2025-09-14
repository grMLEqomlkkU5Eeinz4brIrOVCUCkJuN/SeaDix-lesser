import { SeaDix } from "../lib/index";
import { performance } from "perf_hooks";

console.log("SeaDix File-Streaming Benchmark");
console.log("==============================");

const trie = new SeaDix();
const filePath = "./enable1.txt";
const bufferSize = 1024 * 1024; // 1MB buffer

// Start timing
const start = performance.now();

// Insert words from file
const wordsInserted = trie.insertFromFile(filePath, bufferSize);

// End timing
const end = performance.now();

// Output results
console.log(`Words inserted: ${wordsInserted}`);
console.log(`Time taken: ${(end - start).toFixed(2)} ms`);
