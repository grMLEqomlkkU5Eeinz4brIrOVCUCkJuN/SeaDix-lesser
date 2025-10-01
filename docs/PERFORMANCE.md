# Performance (Seadix-lesser compact build)

> **⚠️ Experimental Project**: This is a research/experimental project for exploring N-API performance and memory optimization techniques. It is **not recommended for production use**.

## Test Environment
- **CPU**: Intel i7-1255U @ 2.5GHz (2P+8E cores, 12 threads) - **Hybrid Mobile CPU**
- **RAM**: 8GB DDR3
- **Node.js**: v18+
- **Testing Conditions**: IDE running, background processes active
- **Power Management**: Balanced power plan (not high performance)

## Memory Usage

| Dataset | Words | Memory | Bytes/Word | Overhead | Notes |
|---------|-------|--------|------------|----------|-------|
| 3M words| 3,080,821| 335.61MB  | 114.23B       | 98%      | Clean dataset |
| 6.27M words (this build)| 6,273,234| 706.25MB  | 118.05B | ~?      | **Mixed unigram/bigram/unicode** |

*The 6M dataset contains mixed unigram, bigram, and unicode characters, making it particularly challenging for radix trie optimization. This explains the higher memory overhead compared to clean word lists.*

## Memory Overhead Analysis

The high memory overhead (95-98%) in SeaDix is primarily due to the experimental nature of this project and several design choices:

### 1. **Arena-Based Memory Management**
- Uses custom arena allocators for performance optimization
- Pre-allocates large memory blocks to reduce allocation overhead
- Unused arena space contributes to overhead until fully utilized

### 2. **Node Structure Overhead**
- Each trie node contains multiple pointers and metadata
- String storage with length prefixes and null terminators
- Child node arrays with dynamic sizing
- Additional flags and state information per node

### 3. **N-API String Conversion**
- JavaScript strings are converted to C++ std::string objects
- Each conversion creates temporary copies during operations
- String interning and caching mechanisms add overhead

### 4. **Experimental Features**
- Debug information and statistics tracking
- Memory usage monitoring and profiling hooks
- Additional safety checks and validation code

### 5. **Radix Trie Structure**
- Radix tries trade memory for search performance
- Common prefixes are shared, but individual nodes still have overhead
- String splitting and edge storage requires additional metadata

*This is expected for an experimental project focused on N-API performance research rather than production memory efficiency.*

## Enable Word List Performance

| Dataset | Words | Time | Rate | Memory | Bytes/Word | Overhead |
|---------|-------|------|------|--------|------------|----------|
| Enable words| 172,820| 50.03ms | 3.46M words/s | 19.93MB | 120.93B | 98% |

*The enable word list represents a clean, real-world dataset commonly used in text processing applications. This dataset shows excellent performance characteristics with high throughput and reasonable memory usage.*

## Operations/sec (i7-1255U Mobile CPU)

### Core Operations (with system variance)
| Operation | Small Dataset | Medium Dataset | Large Dataset |
|-----------|---------------|----------------|---------------|
| Insert    | 139K-166K     | 124K           | 70K-124K      |
| Search    | ~4.0M (single x1000) | ~3.38M (single x1000) | ~2.66M (single x1000) |
| Remove    | ~3.74M-4.09M  | ~3.92M         | ~4.09M        |

*Performance varies significantly due to mobile CPU thermal throttling and background processes*

### Arena Size Impact
| Arena Size | Insert Performance | Search Performance | Best For |
|------------|-------------------|-------------------|----------|
| 32KB       | **Best** (958K/sec) | Good (4.07M/sec) | Small datasets |
| 128KB      | Good (727K/sec) | Good (1.45M/sec) | Medium datasets |
| 2MB        | Moderate (195K/sec) | **Best** (2.65K/sec) | Search-heavy workloads |
| 4MB        | Slow (783K/sec) | **Best** (1.62K/sec) | Large search datasets |

### Batch vs Individual
| Words | Batch Insert | Individual Insert | Winner |
|-------|--------------|-------------------|--------|
| 100   | 25.2K ops/sec | 25.2K ops/sec    | Tie |
| 1000  | 1.9M ops/sec  | 1.9M ops/sec     | Tie |

### File Loading
| File Size | Time | Rate | Arena Size |
|-----------|------|------|------------|
| 6.27M words | 7.06s | 0.89M words/s | 2MB buffer |

## N-API Overhead Analysis

| Operation | Pure C++ | Node.js | Overhead |
|-----------|----------|---------|----------|
| Insert    | ~2.5M/sec | ~0.20M/sec| ~12.5x slower (compact build config) |
| Search    | ~7.9M/sec | ~4-5.4M/sec| ~1.5-2.0x slower |

*N-API overhead includes string conversion, function marshalling, and memory management. This is expected for experimental N-API projects.*

## System Variance (Hybrid Mobile CPU)

**Moderate variance detected in benchmarks:**
- Insert: 9.1% CV (hybrid core scheduling)
- Search: 19.1% CV (P/E core lottery)
- Batch operations: 7.2-22.7% CV (core assignment variance)

*Hybrid mobile CPU performance varies due to P/E core scheduling lottery and thermal management*

## Arena Size Guide

See [Arena Sizing Guide](ARENA_SIZING.md) for detailed recommendations on choosing the optimal arena size for your use case.

## Recommendations

- **Small datasets (< 1K words)**: Use 32-64KB arena
- **Medium datasets (1K-10K words)**: Use 128KB-1MB arena  
- **Large datasets (> 10K words)**: Use 1-4MB arena
- **Search-heavy workloads**: Use 2-4MB arena
- **Insert-heavy workloads**: Use 32-128KB arena
- **File loading**: Use `insertFromFile()` with appropriate arena size
- **Production use**: **Not recommended** - this is experimental

## Limitations

- **Not production-ready**: This is an experimental project
- **Mobile CPU performance**: Results may vary significantly on different hardware
- **Memory overhead**: High overhead due to experimental memory management