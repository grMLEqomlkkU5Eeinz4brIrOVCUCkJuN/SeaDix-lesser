# Performance

## Test Environment
- **CPU**: Intel i5-4210U @ 2.4GHz (2 cores, 4 threads)
- **RAM**: 8GB DDR3
- **Node.js**: v18+

## Memory Usage

| Dataset | Words | Memory | Bytes/Word | Overhead |
|---------|-------|--------|------------|----------|
| 3M words| 3,080,821| 281MB  | 95B        | 97%      |
| 6M words| 6,273,234| 735MB  | 123B       | 95%      |

*Overhead includes vector storage, allocator metadata, and node structure*

## Operations/sec (i5-4210U)

### Core Operations (varies significantly between runs)
| Operation | Small Dataset | Medium Dataset | Large Dataset |
|-----------|---------------|----------------|---------------|
| Insert    | 177K-642K     | 193K-265K      | 1.3M-1.8M     |
| Search    | 1.8M (1000x)  | 1.7M (1000x)   | 927K (1000x)  |
| Remove    | 566K (1000x)  | 599K (1000x)   | 906K (1000x)  |

*Numbers vary significantly between runs due to system load*

### Batch vs Individual
| Words | Batch Insert | Individual Insert | Winner |
|-------|--------------|-------------------|--------|
| 100   | 18K ops/sec  | 20K ops/sec      | Individual |
| 1000  | 946K ops/sec | 812K ops/sec     | Batch |

### File Loading
| File Size | Time | Rate |
|-----------|------|------|
| 3M words  | 1.6s | 1.9M words/s |
| 6M words  | 2.6s | 2.4M words/s |

## C++ vs Node.js Overhead

| Operation | Pure C++ | Node.js | Overhead |
|-----------|----------|---------|----------|
| Insert    | 2.5M/sec | 750K/sec| 3x slower |
| Search    | 7.9M/sec | 2.8M/sec| 3x slower |

*N-API overhead includes string conversion, function marshalling, and memory management*

## System Variance

**High variance detected in benchmarks:**
- Insert: 20.4% CV
- Search: 27.0% CV  
- Batch operations: 18-20% CV

*Recommendations: Close unnecessary apps, use high-performance power plan, ensure adequate cooling*

## Recommendations

- **Small datasets (5-100 words)**: Use batch operations
- **Large datasets (500+ words)**: Use individual operations  
- **File loading**: Use `insertFromFile()` with default 1MB buffer
- **JSON handling**: Process in JavaScript, not C++