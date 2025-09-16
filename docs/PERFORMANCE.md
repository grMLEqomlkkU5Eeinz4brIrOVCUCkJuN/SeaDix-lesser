# Performance

> **⚠️ Experimental Project**: This is a research/experimental project for exploring N-API performance and memory optimization techniques. It is **not recommended for production use**.

## Test Environment
- **CPU**: Intel i5-4210U @ 2.4GHz (2 cores, 4 threads) - **Mobile CPU**
- **RAM**: 8GB DDR3
- **Node.js**: v18+
- **Testing Conditions**: IDE running, background processes active
- **Power Management**: Balanced power plan (not high performance)

## Memory Usage

| Dataset | Words | Memory | Bytes/Word | Overhead | Notes |
|---------|-------|--------|------------|----------|-------|
| 3M words| 3,080,821| 335MB  | 114B       | 98%      | Clean dataset |
| 6M words| 6,273,234| 874MB  | 146B       | 95%      | **Mixed unigram/bigram/unicode** |

*The 6M dataset contains mixed unigram, bigram, and unicode characters, making it particularly challenging for radix trie optimization. This explains the higher memory overhead compared to clean word lists.*

## Operations/sec (i5-4210U Mobile CPU)

### Core Operations (with system variance)
| Operation | Small Dataset | Medium Dataset | Large Dataset |
|-----------|---------------|----------------|---------------|
| Insert    | 644K ops/sec  | 193K-265K      | 1.3M-1.8M     |
| Search    | 2.9M ops/sec  | 1.7M ops/sec   | 927K ops/sec  |
| Remove    | 566K ops/sec  | 599K ops/sec   | 906K ops/sec  |

*Performance varies significantly due to mobile CPU thermal throttling and background processes*

### Arena Size Impact
| Arena Size | Insert Performance | Search Performance | Best For |
|------------|-------------------|-------------------|----------|
| 32KB       | **Best** (644K/sec) | Good (2.9M/sec) | Small datasets |
| 128KB      | Good (513K/sec) | Good (2.8M/sec) | Medium datasets |
| 2MB        | Moderate (278K/sec) | **Best** (3.3M/sec) | Search-heavy workloads |
| 4MB        | Slow (191K/sec) | **Best** (3.2M/sec) | Large search datasets |

### Batch vs Individual
| Words | Batch Insert | Individual Insert | Winner |
|-------|--------------|-------------------|--------|
| 100   | 12.6K ops/sec | 17.2K ops/sec    | Individual |
| 1000  | 946K ops/sec  | 812K ops/sec     | Batch |

### File Loading
| File Size | Time | Rate | Arena Size |
|-----------|------|------|------------|
| 3M words  | 1.9s | 1.6M words/s | 4MB |
| 6M words  | 2.8s | 2.2M words/s | 8MB |

## N-API Overhead Analysis

| Operation | Pure C++ | Node.js | Overhead |
|-----------|----------|---------|----------|
| Insert    | ~2.5M/sec | 644K/sec| **4x slower** |
| Search    | ~7.9M/sec | 2.9M/sec| **3x slower** |

*N-API overhead includes string conversion, function marshalling, and memory management. This is expected for experimental N-API projects.*

## System Variance (Mobile CPU)

**High variance detected in benchmarks:**
- Insert: 23.4% CV (thermal throttling)
- Search: 26.9% CV (background processes)
- Batch operations: 21-35% CV (system load)

*Mobile CPU performance varies significantly due to thermal management and power saving features*

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
- **N-API overhead**: Significant performance penalty compared to pure C++
- **System sensitivity**: Performance varies greatly with system load and thermal conditions