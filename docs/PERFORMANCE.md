# N-API Performance Investigation

## Overview

This document explores the performance characteristics of Node.js C++ addons and the overhead introduced by the N-API boundary. This is an experimental investigation into JavaScript/C++ interop performance.

## N-API Overhead Analysis

### The Performance Gap

| Operation | Pure C++ | Node.js (N-API) | Overhead Factor |
|-----------|----------|-----------------|-----------------|
| **Insertion** | 1.46M ops/sec | 208K ops/sec | **7x slower** |
| **Search** | 17.9M ops/sec | 625K ops/sec | **29x slower** |
| **Size Check** | - | 1.4M ops/sec | - |

### Why N-API is Slow

Each JavaScript function call to C++ involves:

1. **Parameter Marshalling**: Convert JS values to C++ types
2. **String Conversion**: JS string → C++ string_view (expensive)
3. **Function Call Overhead**: N-API wrapper execution
4. **Memory Management**: Cross-boundary memory handling
5. **Return Value Conversion**: C++ result → JS value

### Time Complexity (Algorithm Only)

| Operation | Time Complexity | Notes |
|-----------|----------------|-------|
| `insert(word)` | O(m) | m = word length |
| `search(word)` | O(m) | m = word length |
| `startsWith(prefix)` | O(m) | m = prefix length |
| `wordsWithPrefix(prefix)` | O(m + k) | m = prefix length, k = number of results |
| `remove(word)` | O(m) | m = word length |

### Space Complexity

- **Memory Usage**: O(total characters in all words)
- **Node Overhead**: Each node stores key, end flag, parent pointer, and children map
- **Compression**: Radix trie compresses common prefixes, reducing memory usage

## What I'm Trying to Figure Out

### 1. Batch Operations (Implemented!)

I've implemented batch operations that reduce N-API overhead:

```typescript
// Individual calls - each has N-API overhead
for (const word of words) {
    trie.insert(word);  // 7x slower than pure C++
}

// Batch operations - single N-API call
trie.insertBatch(words);  // 36% faster than individual calls!
```

**Results:**
- **36% performance improvement** for batch operations
- Single N-API call instead of N calls
- Reduced string conversion overhead
- Better memory locality

### 2. Memory Management Across Boundaries

Managing memory between JavaScript and C++ is complex:

```cpp
// C++ side - automatic cleanup
std::unique_ptr<Node> node = std::make_unique<Node>();
// Memory automatically freed when out of scope
```

```javascript
// JavaScript side - N-API handles conversion
const result = trie.search('word');  // C++ bool → JS boolean
```

**Challenges:**
- String conversion overhead
- Memory ownership across boundaries
- Garbage collection interaction

## Experimental Findings

### 1. Batch Operations Work!

I implemented batch operations that reduce N-API overhead:

```typescript
// Individual calls - each crosses N-API boundary
words.forEach(word => trie.insert(word));           // 2.28ms for 1000 words
for (const word of words) trie.insert(word);        // 2.28ms for 1000 words

// Batch operations - single N-API call
trie.insertBatch(words);                            // 1.44ms for 1000 words (36% faster!)
```

**Why batch operations work:**
- Single N-API call instead of N calls
- Reduced string conversion overhead
- Better memory locality in C++
- Less JavaScript/C++ boundary crossing

### 2. The C++ Implementation is Fast

Pure C++ performance is good:

```cpp
// Pure C++ - no N-API overhead
trie.insert("word");  // 1.46M ops/sec
trie.search("word");  // 17.9M ops/sec
```

**The bottleneck is in the N-API layer, not the algorithm.**

## Testing Performance

### Pure C++ Performance Test

Run the C++ performance test to see the underlying algorithm speed:

```bash
make -f Makefile.test run
```

This shows the true performance without N-API overhead.

### Node.js Performance Test

Run the Node.js benchmarks to see N-API overhead:

```bash
npm run benchmark:full
```

### Performance Comparison

| Test Type | Pure C++ | Node.js | Overhead |
|-----------|----------|---------|----------|
| **Insertion** | 1.46M ops/sec | 208K ops/sec | 7x slower |
| **Search** | 17.9M ops/sec | 625K ops/sec | 29x slower |
| **Bulk Insert** | 1.45M ops/sec | 208K ops/sec | 7x slower |

## Memory Usage

### Node Structure

Each node in the trie contains:
- `std::string key` - Compressed prefix (typically small)
- `bool is_end` - End-of-word flag (1 byte)
- `Node* parent` - Parent pointer (8 bytes)
- `char parent_char` - Parent character (1 byte)
- `std::unordered_map<char, std::unique_ptr<Node>> children` - Children map

### Memory Efficiency

The radix trie uses memory efficiently because:
- Common prefixes are shared between words
- No redundant storage of common substrings
- Compressed representation reduces memory footprint

### Example Memory Usage

For 1000 words with average length 10:
- **Naive storage**: ~10,000 characters
- **Radix trie**: ~3,000-5,000 characters (depending on prefix sharing)

## What I'm Still Figuring Out

### 1. Can N-API Overhead Be Reduced?

Current approaches I've tried:
- Bulk operations (didn't help)
- String pre-allocation (minimal improvement)
- Batch operations (worse performance)

**Still exploring:**
- WebAssembly as alternative
- Different N-API patterns
- Memory management optimizations

### 2. When Is N-API Worth It?

The 7-29x overhead makes me question when C++ addons are beneficial:

**Maybe worth it:**
- Complex algorithms that are hard to implement in JS
- CPU-intensive operations
- When the C++ speed gain > N-API overhead

**Probably not worth it:**
- Simple operations called frequently
- String-heavy operations
- When N-API overhead dominates

### 3. Alternative Approaches

**WebAssembly might be better:**
- Lower overhead than N-API
- Better tooling and debugging
- More portable across platforms

**Pure JavaScript might be sufficient:**
- Modern JS engines are good
- No cross-boundary overhead
- Easier to maintain and debug
