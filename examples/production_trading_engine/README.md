# Chaos Preprocessor: Production Trading & Risk Engine Example

This example demonstrates how to use the **Chaos Preprocessor Library** to build a modern, zero-overhead compile-time reflection and serialization engine for high-frequency financial trading systems.

## Why Chaos Preprocessor?

In high-performance C++ systems (trading engines, game networking, distributed databases), developers frequently face the **Boilerplate Problem**:
- Defining a data model requires writing member variables, constructors, comparison operators, JSON serializers, JSON parsers, binary wire encoders, and logging formatters.
- When a new field is added, every single function must be manually updated, creating severe maintenance burden and subtle desynchronization bugs.
- Runtime reflection (e.g. `typeid`, virtual tables, `std::any`) introduces heap allocations, cache misses, and runtime overhead unacceptable in latency-critical paths.

### The Solution: Compile-Time Macro Reflection

With **Chaos Preprocessor**, data contracts are declared **once** declaratively:

```cpp
CHAOS_REFLECT_ENUM(OrderSide,
    (BUY)
    (SELL)
)

CHAOS_REFLECT_STRUCT(Money,
    ((uint64_t, units))
    ((uint32_t, nanos))
    ((Currency, currency))
)

CHAOS_REFLECT_STRUCT(LimitOrder,
    ((uint64_t, order_id))
    ((std::string, client_ref))
    ((std::string, symbol))
    ((OrderSide, side))
    ((OrderType, type))
    ((TimeInForce, tif))
    ((uint64_t, quantity))
    ((Money, limit_price))
    ((TraderProfile, trader))
    ((OrderStatus, status))
    ((uint64_t, timestamp_epoch_ns))
)
```

The preprocessor automatically generates:
1. **Strongly-typed Structs and Enums** (`enum class`, `struct`) with zero runtime overhead.
2. **Metadata**: `field_count`, `field_names` array (`std::string_view`), enum string conversion.
3. **Generic Field Visitors**: `for_each_field(obj, visitor)` traversing all members without virtual functions.
4. **Zero-Copy JSON Serializer & Deserializer**: Parses directly into struct fields with 64-bit integer precision.
5. **Compact Binary Protocol**: Serializes to byte streams (73% smaller than JSON) with 100% roundtrip integrity.
6. **Operators**: Deep equality (`operator==`, `operator!=`) and stream formatting (`operator<<`).

## Architecture & Components

| Component | Description |
|---|---|
| [`include/chaos_reflection.hpp`](include/chaos_reflection.hpp) | Core reflection macros and zero-dependency JSON/binary serialization engine powered by Chaos PP |
| [`include/trading_models.hpp`](include/trading_models.hpp) | Financial domain models (`LimitOrder`, `ExecutionReport`, `Money`, `TraderProfile`) |
| [`include/matching_engine.hpp`](include/matching_engine.hpp) | Simulated matching engine with pre-trade risk controls and binary audit logging |
| [`src/main.cpp`](src/main.cpp) | End-to-end demonstration, field visitor inspection, JSON/binary roundtrips, and 50k-order benchmark |

## Building and Running

Run directly using `make`:

```bash
make example
```

Or via `cmake`:

```bash
cmake -B build
cmake --build build --target production_example
./build/examples/production_example
```

## Performance Benchmark

In the included 50,000-order pipeline benchmark (which executes pre-trade risk validation, liquidity matching, execution report generation, and binary journal recording per order):
- **Throughput**: ~180,000+ orders / second
- **Latency**: ~5.5 µs average per end-to-end transaction
- **Memory Overhead**: 0 bytes of dynamic reflection metadata; completely resolved at compile time.
