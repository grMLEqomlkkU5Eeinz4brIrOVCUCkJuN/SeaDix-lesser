# Development Notes

## N-API Performance Investigation

This project started as a learning exercise in Node.js C++ addons, but became an investigation into N-API performance overhead. I'm trying to understand when C++ addons are worth the complexity.

### What I've Discovered

1. **N-API overhead is massive** - 7-29x slower than pure C++
2. **C++ implementation is fast** - 1.46M insertions/sec, 17.9M searches/sec
3. **Bulk operations don't help** - Each call still crosses the boundary
4. **String conversion is expensive** - JS string → C++ conversion has cost
5. **Memory management is complex** - Cross-boundary memory handling is tricky

### What I'm Still Figuring Out

- **N-API optimization** - How to reduce JavaScript/C++ boundary overhead
- **Bulk operations** - Whether batching can improve performance (so far: no)
- **Memory patterns** - Best practices for cross-boundary memory management
- **Alternative approaches** - Maybe WebAssembly would be better?
- **Real-world usage** - When is the N-API overhead worth it?

## Architecture Decisions

### Why This Approach?

I wanted to understand Node.js C++ addons and their performance characteristics. A trie seemed like a good test case because:
- It's simple to understand
- String operations are common in JavaScript
- Performance matters for prefix searching
- It would reveal N-API overhead clearly

### Why a Radix Trie?

Regular tries waste space. Radix tries compress common prefixes, which uses memory more efficiently. The implementation does this through:
- Prefix compression in node keys
- Shared common prefixes between words
- Memory usage for sparse datasets

### Memory Management

The implementation uses several strategies:
- `std::unique_ptr` for automatic memory management
- Parent pointers for O(1) cleanup operations
- Smart pointer usage to prevent memory leaks
- Node cleanup that only removes truly orphaned nodes

## Performance Considerations

### Bulk Operations

For bulk insertions, manual loops are recommended:
- `forEach` loops are 25% faster than bulk methods
- No wrapper overhead or complex sorting
- Single C++ call avoids JavaScript/C++ boundary overhead
- Sorting improves cache locality and reduces node splitting
- Pre-allocation of vectors reduces memory allocation overhead

### Memory Usage

Each node stores:
- A string key (compressed prefix)
- A boolean flag for end-of-word
- Parent pointer and character for O(1) cleanup
- A hash map of children

The hash map is efficient for the expected use cases, and the parent pointer optimization significantly improves cleanup performance.

## Future Improvements

### File Streaming

I'm thinking about adding file streaming capabilities:

```javascript
// Load from file
const trie = await SeaDix.fromFile("words.txt");

// Save to file  
await trie.toFile("output.txt");

// Stream processing
const stream = trie.createReadStream();
stream.on("data", (word) => {
// Process each word
});
```

This would be useful for large datasets that don't fit in memory.

### NPM Publishing

If this library actually works and people find it useful, I might publish it to npm. But first I need to:

1. Fix any bugs I haven't found yet
2. Add more comprehensive tests
3. Improve error handling
4. Add better documentation
5. Test on different platforms

### Potential Issues with NPM

- **Build complexity** - Users will need to have C++ build tools installed
- **Platform differences** - Different binaries for different platforms
- **Version compatibility** - Node.js version compatibility is tricky
- **Security** - Native modules have more security implications

## Testing Strategy

I'm using Jest for testing, but I'm not sure if I'm testing the right things. The tests cover:

- Basic functionality
- Error cases
- Performance (sort of)
- Memory usage (basic)

But I might be missing edge cases or performance regressions.

## Build Process

The build process uses:
- `node-gyp` for compiling C++
- `tsc` for TypeScript compilation
- `jest` for testing

I'm not sure if this is the best setup, but it seems to work.

## Debugging

Debugging C++ addons is difficult. I've been using:
- `console.log` statements in C++ (not ideal)
- Node.js debugging tools
- Basic memory checking

I should probably learn proper C++ debugging techniques.

## Code Quality

The C++ code is probably not following best practices. I'm learning as I go, so there are likely:
- Inefficient algorithms
- Poor error handling
- Inconsistent naming
- Missing comments

## Conclusion

This project has been a learning experience. I'm not sure if the code is production-ready, but it seems to work for basic use cases. If you find bugs or know how to improve it, I'd appreciate the feedback.

The main challenges have been:
1. Understanding the N-API
2. Managing memory correctly
3. Getting the build process to work reliably
4. Writing tests that actually test the right things

I'm still learning, so expect bugs and inefficiencies.
