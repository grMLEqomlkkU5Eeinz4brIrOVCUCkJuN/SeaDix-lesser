# Development (Seadix-lesser)

## Build

```bash
npm run build:all    # Build everything
npm run build        # C++ only
npm run build:ts     # TypeScript only
```

## Test

```bash
npm test             # Run tests
npm run test:watch   # Watch mode
```

## Benchmark

```bash
npm run benchmark:stable  # Stable benchmark (compact build numbers)
npm run benchmark:file3   # File benchmark
npm run cpp:test:o2       # C++ test
```

## Code Style

- C++: Use `const`, `std::string_view`, `noexcept`
- TypeScript: Strict types, JSDoc comments
- Format: `npm run format` (C++), `npm run lint:fix` (TS)

## Structure

```
src/           # C++ source
├── RadixTrie.h/cc  # Core trie
└── SeaDix.h/cc     # Node.js wrapper

lib/           # TypeScript
└── index.ts   # Main file

tests/         # Test files
benchmark/     # Benchmarks
```

## Memory Management

- Use `std::unique_ptr` for ownership
- Use `std::string_view` to avoid copies
- Implement RAII in constructors/destructors
- Use move semantics

## Debugging

```bash
# C++ debugging
npm run build:debug
gdb node

# Node.js debugging  
node --inspect test_performance.js
```

## Contributing

1. Fork repository
2. Create feature branch
3. Add tests
4. Run benchmarks
5. Submit PR