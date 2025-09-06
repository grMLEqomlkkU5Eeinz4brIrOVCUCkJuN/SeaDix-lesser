# C++ Performance Documentation

This document measures the raw C++ performance of the RadixTrie implementation without N-API overhead. At the same time it adds any working theories that I might have.

## Test Environment

- **CPU**: Intel i5-4210u (2 cores, 4 threads)
- **OS**: Linux 6.16.4-arch1-1
- **Compiler**: clang++ with -O2 optimization (best performance)
- **Memory**: 8GB RAM

## Optimization Level Comparison

### Performance by Optimization Level (1,000 words)

| Optimization | Insert (ops/sec) | Search (ops/sec) | Bulk Insert (ops/sec) |
|--------------|------------------|------------------|----------------------|
| **O0** (no opt) | 321,000 | 611,000 | 556,000 |
| **O2** | 2,778,000 | 10,857,000 | 3,571,000 |
| **O3** | 1,416,000 | 7,837,000 | 3,802,000 |

### Why O2 is Best for This Code

**O2 optimization provides the best overall performance because:**

1. **Search Operations**: O2 achieves 10.9M searches/sec vs O3's 7.8M searches/sec
2. **Insert Operations**: O2 achieves 2.8M inserts/sec vs O3's 1.4M inserts/sec  
3. **Consistent Performance**: O2 provides more predictable performance across different dataset sizes
4. **Compiler Efficiency**: O2 optimizations are well-suited for this radix trie implementation

**O3 Issues:**
- Aggressive optimization can hurt performance in some cases
- May increase code size, affecting cache performance
- Branch prediction optimizations may not work well with this data structure

**O0 Issues:**
- No optimization results in 8-18x slower performance
- Only suitable for debugging and development

## Performance Results (O2 Optimization)

### Insert Operations

| Dataset Size | Operations/sec | Time per Operation |
|--------------|----------------|-------------------|
| 10 words     | 833,333        | 1.2 μs           |
| 58 words     | 1,018,000      | 0.98 μs          |
| 1,000 words  | 2,778,000      | 0.36 μs          |

### Search Operations

| Dataset Size | Operations/sec | Time per Operation |
|--------------|----------------|-------------------|
| 10 words     | 40,161,000     | 0.025 μs         |
| 58 words     | 17,533,000     | 0.057 μs         |
| 1,000 words  | 11,057,000     | 0.090 μs         |

### Bulk Insertion Performance

| Dataset Size | Operations/sec | Time per Operation |
|--------------|----------------|-------------------|
| 10 words     | 556,000        | 1.8 μs           |
| 58 words     | 2,148,000      | 0.47 μs          |
| 1,000 words  | 3,571,000      | 0.28 μs          |

### Memory Usage Test (1,000 words)

| Operation | Time per Operation |
|-----------|-------------------|
| Insert    | 0.256 μs         |
| Search    | 0.100 μs         |

## Prefix Sharing Performance Analysis

The RadixTrie shows interesting behavior with different prefix sharing patterns:

| Pattern Type | Time per Word | Operations/sec |
|--------------|---------------|----------------|
| No common prefixes | 0.2 μs | 5,000,000 |
| Common prefixes | 0.4-0.6 μs | 1,667,000-2,500,000 |
| Deep prefix sharing | 0.17-0.33 μs | 3,000,000-5,882,000 |

**Key Insights:**
- Words with no common prefixes insert fastest (no tree traversal)
- Common prefixes add overhead due to tree navigation
- Deep prefix sharing can be efficient due to path compression

## Memory Usage

### Node Structure
```cpp
struct Node {
    std::string key;                    // Compressed prefix
    bool is_end;                        // End-of-word flag
    char parent_char;                   // Parent character
    Node* parent;                       // Parent pointer
    std::unordered_map<char, std::unique_ptr<Node>> children;
};
```

### Memory Overhead per Node
- **Key string**: Variable length (typically 1-10 characters)
- **is_end flag**: 1 byte
- **parent_char**: 1 byte
- **parent pointer**: 8 bytes (64-bit)
- **children map**: ~24 bytes overhead + 8 bytes per child
- **Total per node**: ~35-50 bytes + key length

### Example Memory Usage
```
Words: ["cat", "car", "card", "care"]
Nodes: 4 (one per word)
Memory: ~200 bytes total
```

## Algorithm Complexity

### Time Complexity
- **Insert**: O(m) where m is word length
- **Search**: O(m) where m is word length
- **Remove**: O(m) where m is word length
- **Prefix Search**: O(m + k) where m is prefix length, k is number of results

### Space Complexity
- **Total Memory**: O(total characters in all words)
- **Compression Ratio**: Typically 30-70% of naive trie storage

## Performance Characteristics

### What Affects Performance

1. **Word Length**: Longer words take more time
2. **Prefix Sharing**: More shared prefixes = mixed performance impact
3. **Tree Depth**: Deeper trees have more traversal overhead
4. **Memory Locality**: Better cache performance with fewer allocations

### Performance Patterns

1. **Insert Performance**: Improves with dataset size (2.8M ops/sec for large datasets)
2. **Search Performance**: Decreases slightly with dataset size but remains fast (11M+ ops/sec)
3. **Bulk Insert Performance**: Best with large datasets (3.6M ops/sec)
4. **Memory Performance**: Consistent ~0.1 μs per search operation

## Comparison with JavaScript (N-API Overhead)

### Insert Operations
| Dataset Size | C++ (ops/sec) | JavaScript (ops/sec) | Overhead Factor |
|--------------|---------------|---------------------|-----------------|
| 10 words     | 833,333       | 109,120            | 7.6x            |
| 58 words     | 1,018,000     | 537,161            | 1.9x            |
| 1,000 words  | 2,778,000     | 782,043            | 3.6x            |

### Search Operations
| Dataset Size | C++ (ops/sec) | JavaScript (ops/sec) | Overhead Factor |
|--------------|---------------|---------------------|-----------------|
| 10 words     | 40,161,000    | 527,783            | 76.1x           |
| 58 words     | 17,533,000    | 375,120            | 46.8x           |
| 1,000 words  | 11,057,000    | 1,298,528          | 8.5x            |

### Key Findings
- **Small datasets**: N-API overhead is highest (8-76x slower)
- **Large datasets**: N-API overhead is lower (4-9x slower)
- **Search operations**: Higher overhead than insert operations
- **Batch operations**: Can reduce overhead for small datasets
- **O2 optimization**: Provides best performance for this radix trie implementation

### When C++ Addons Make Sense
- Complex algorithms that are hard to implement in JavaScript
- Operations that benefit from C++ optimizations
- Memory-intensive operations
- When the algorithm complexity justifies the N-API overhead

### When Pure JavaScript Might Be Better
- Simple operations called frequently
- When N-API overhead dominates algorithm time
- When maintainability is more important than performance
- For operations that don't benefit from C++ optimizations

## Available Optimization Commands

### npm Scripts
```bash
npm run cpp:test:o0      # No optimization (debugging)
npm run cpp:test:o2      # O2 optimization (recommended)
npm run cpp:test:build   # O3 optimization (default)
npm run cpp:test:debug   # Debug build with symbols
npm run cpp:test:release # Release build
npm run cpp:test:help    # Show all available options
```

### Make Commands
```bash
make -f Makefile.test o0      # No optimization
make -f Makefile.test o2      # O2 optimization  
make -f Makefile.test         # O3 optimization (default)
make -f Makefile.test debug   # Debug build
make -f Makefile.test release # Release build
make -f Makefile.test help    # Show help
```

## Benchmarking Methodology

### Test Setup
```cpp
// Compile with: clang++ -O2 -std=c++17 test_performance.cpp src/RadixTrie.cc
// Run with: ./test_performance

#include "RadixTrie.h"
#include <chrono>
#include <vector>
#include <string>

int main() {
    RadixTrie trie;
    std::vector<std::string> words = generateTestWords(1000);
    
    // Measure insert performance
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& word : words) {
        trie.insert(word);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    // Calculate operations per second
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double ops_per_sec = (words.size() * 1000000.0) / duration.count();
    
    return 0;
}
```

### Measurement Accuracy
- Each test runs multiple iterations for accuracy
- Results are averaged across multiple runs
- System is idle during testing
- High-resolution timing used for precision

## Conclusion

The C++ implementation shows excellent performance for a radix trie:
- Insert operations scale well, reaching 2.8 million ops/sec for large datasets
- Search operations are extremely fast, achieving over 11 million ops/sec
- Memory usage is efficient due to prefix compression
- Performance scales predictably with dataset size

The main bottleneck in the Node.js version is the N-API overhead, not the algorithm itself. The C++ implementation demonstrates that the radix trie algorithm is highly efficient, with O2 optimization providing the best balance of performance across all operation types.

**Key Takeaways:**
- O2 optimization is optimal for this radix trie implementation
- Large datasets show the best performance characteristics
- Search operations are significantly faster than insert operations
- The algorithm scales well with increasing dataset sizes