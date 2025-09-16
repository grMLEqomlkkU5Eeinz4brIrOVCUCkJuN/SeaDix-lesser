# Arena Sizing Guide

> **⚠️ Experimental Feature**: Arena sizing is an experimental optimization feature. This guide is for educational purposes and experimentation only.

## What is Arena Allocation?

Arena allocation is a memory management technique where a large block of memory (the "arena") is pre-allocated and used for multiple smaller allocations. Instead of making individual `malloc()` calls for each node and string, the arena provides a fast, sequential allocation pattern.

### Benefits
- **Reduced allocation overhead**: One large allocation instead of many small ones
- **Better cache locality**: Related data is stored close together
- **Faster deallocation**: Free the entire arena at once
- **Predictable memory usage**: Arena size is known upfront

### Trade-offs
- **Memory waste**: Unused arena space is wasted
- **Fixed size**: Arena cannot grow beyond initial size
- **Manual management**: You must choose appropriate arena size

## How Arena Size Affects Performance

Based on experimental testing on a mobile Intel i5-4210U CPU:

| Arena Size | Insert Performance | Search Performance | Memory Efficiency | Best Use Case |
|------------|-------------------|-------------------|------------------|---------------|
| 32KB       | **644K ops/sec**  | 2.9M ops/sec     | High            | Small datasets (< 1K words) |
| 64KB       | 1.5M ops/sec      | 1.8M ops/sec     | High            | Small-medium datasets |
| 128KB      | 1.5M ops/sec      | 2.8M ops/sec     | High            | Medium datasets (1K-10K words) |
| 256KB      | 823K ops/sec      | 2.7M ops/sec     | Medium          | Medium-large datasets |
| 512KB      | 1.2M ops/sec      | 2.2M ops/sec     | Medium          | Large datasets |
| 1MB        | 1.0M ops/sec      | 2.0M ops/sec     | Medium          | Large datasets (default) |
| 2MB        | 812K ops/sec      | **3.3M ops/sec** | Low             | Search-heavy workloads |
| 4MB        | 883K ops/sec      | **3.2M ops/sec** | Low             | Very large search datasets |
| 8MB        | 1.0M ops/sec      | 2.7M ops/sec     | Low             | Massive datasets |

## Choosing the Right Arena Size

### 1. **Small Datasets (< 1,000 words)**
```javascript
const trie = new SeaDix(32 * 1024); // 32KB arena
```
- **Why**: Small arenas have better cache locality for small datasets
- **Performance**: Best insert performance (644K ops/sec)
- **Memory**: Most efficient memory usage

### 2. **Medium Datasets (1,000 - 10,000 words)**
```javascript
const trie = new SeaDix(128 * 1024); // 128KB arena
```
- **Why**: Balanced performance for both insert and search operations
- **Performance**: Good insert (1.5M ops/sec) and search (2.8M ops/sec)
- **Memory**: Efficient memory usage

### 3. **Large Datasets (10,000 - 100,000 words)**
```javascript
const trie = new SeaDix(1 * 1024 * 1024); // 1MB arena (default)
```
- **Why**: Default size works well for most large datasets
- **Performance**: Balanced insert (1.0M ops/sec) and search (2.0M ops/sec)
- **Memory**: Reasonable memory efficiency

### 4. **Search-Heavy Workloads**
```javascript
const trie = new SeaDix(2 * 1024 * 1024); // 2MB arena
```
- **Why**: Larger arenas optimize for search performance
- **Performance**: Best search performance (3.3M ops/sec)
- **Memory**: Lower efficiency but better search speed

### 5. **Very Large Datasets (> 100,000 words)**
```javascript
const trie = new SeaDix(4 * 1024 * 1024); // 4MB arena
```
- **Why**: Large datasets need larger arenas to avoid fragmentation
- **Performance**: Good search performance (3.2M ops/sec)
- **Memory**: Lower efficiency but necessary for large datasets

### 6. **Massive Datasets (1M+ words)**
```javascript
const trie = new SeaDix(8 * 1024 * 1024); // 8MB arena
```
- **Why**: Very large datasets require substantial arena space
- **Performance**: Balanced performance for massive datasets
- **Memory**: Lower efficiency but necessary for scale

## Dynamic Arena Sizing

You can change the arena size at runtime (experimental):

```javascript
const trie = new SeaDix(64 * 1024); // Start with 64KB

// Insert some data
trie.insert("hello");
trie.insert("world");

// Change arena size (preserves all data)
trie.setArenaSize(2 * 1024 * 1024); // Switch to 2MB

// Continue using the trie
trie.insert("test");
```

**Note**: Changing arena size recreates the entire trie internally, so it's expensive for large datasets.

## Performance Testing

To find the optimal arena size for your specific use case:

```javascript
// Test different arena sizes
const sizes = [32, 64, 128, 256, 512, 1024, 2048, 4096]; // KB
const words = generateTestWords(10000); // Your test data

for (const sizeKB of sizes) {
    const trie = new SeaDix(sizeKB * 1024);
    
    const start = performance.now();
    words.forEach(word => trie.insert(word));
    const insertTime = performance.now() - start;
    
    const searchStart = performance.now();
    words.forEach(word => trie.search(word));
    const searchTime = performance.now() - searchStart;
    
    console.log(`${sizeKB}KB: Insert ${insertTime.toFixed(2)}ms, Search ${searchTime.toFixed(2)}ms`);
}
```

## Memory Considerations

### Arena Size vs Memory Usage
- **Small arenas (32-128KB)**: High memory efficiency, may cause fragmentation
- **Medium arenas (256KB-1MB)**: Balanced efficiency and performance
- **Large arenas (2-8MB)**: Lower efficiency but better performance

### Memory Overhead
All arena sizes show similar memory overhead (~95-98%) due to:
- Node structure overhead
- String storage overhead
- Vector container overhead
- Allocator metadata

## Best Practices

1. **Start with default (1MB)**: Works well for most use cases
2. **Profile your workload**: Test different sizes with your actual data
3. **Consider your hardware**: Mobile CPUs may prefer smaller arenas
4. **Monitor memory usage**: Larger arenas use more memory
5. **Test with realistic data**: Use actual data patterns, not synthetic data

## Limitations

- **Experimental feature**: Not recommended for production use
- **Fixed size**: Arena cannot grow beyond initial allocation
- **Memory waste**: Unused arena space is wasted
- **Platform dependent**: Performance varies significantly across hardware
- **N-API overhead**: Still subject to Node.js/N-API performance limitations

## Troubleshooting

### "Arena size must be greater than 0"
```javascript
// ❌ Wrong
const trie = new SeaDix(0);

// ✅ Correct
const trie = new SeaDix(1024); // At least 1KB
```

### Poor performance with large datasets
```javascript
// ❌ Too small for large dataset
const trie = new SeaDix(32 * 1024);

// ✅ Appropriate size
const trie = new SeaDix(2 * 1024 * 1024);
```

### High memory usage
```javascript
// ❌ Too large for small dataset
const trie = new SeaDix(8 * 1024 * 1024);

// ✅ Appropriate size
const trie = new SeaDix(128 * 1024);
```

## Conclusion

Arena sizing is a powerful optimization technique for experimental projects, but requires careful tuning for your specific use case. Start with the default size and experiment with different sizes based on your performance requirements and memory constraints.

Remember: This is an experimental project for learning and exploration, not production use.
