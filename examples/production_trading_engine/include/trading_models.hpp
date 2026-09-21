#ifndef TRADING_MODELS_HPP
#define TRADING_MODELS_HPP

#include "chaos_reflection.hpp"

namespace trading {

// ============================================================================
// Enums with reflection, string conversions, and parsing
// ============================================================================

CHAOS_REFLECT_ENUM(OrderSide,
    (BUY)
    (SELL)
)

CHAOS_REFLECT_ENUM(OrderType,
    (MARKET)
    (LIMIT)
    (STOP_LOSS)
)

CHAOS_REFLECT_ENUM(TimeInForce,
    (DAY)
    (GTC)
    (IOC)
    (FOK)
)

CHAOS_REFLECT_ENUM(OrderStatus,
    (NEW)
    (RISK_PASSED)
    (PARTIALLY_FILLED)
    (FILLED)
    (REJECTED)
    (CANCELLED)
)

CHAOS_REFLECT_ENUM(Currency,
    (USD)
    (EUR)
    (GBP)
    (BTC)
    (ETH)
)

// ============================================================================
// Data Structs with zero-boilerplate reflection
// ============================================================================

CHAOS_REFLECT_STRUCT(Money,
    ((uint64_t, units))
    ((uint32_t, nanos))
    ((Currency, currency))
)

CHAOS_REFLECT_STRUCT(TraderProfile,
    ((std::string, trader_id))
    ((std::string, firm_id))
    ((std::string, desk_id))
    ((bool, is_market_maker))
    ((uint64_t, max_credit_units))
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

CHAOS_REFLECT_STRUCT(FillEvent,
    ((uint64_t, fill_id))
    ((uint64_t, order_id))
    ((uint64_t, filled_qty))
    ((Money, match_price))
    ((uint64_t, fill_epoch_ns))
)

CHAOS_REFLECT_STRUCT(ExecutionReport,
    ((uint64_t, report_id))
    ((uint64_t, order_id))
    ((std::string, symbol))
    ((OrderStatus, final_status))
    ((uint64_t, cumulative_qty))
    ((uint64_t, leaves_qty))
    ((std::vector<FillEvent>, fills))
    ((std::string, message))
    ((uint64_t, latency_ns))
)

CHAOS_REFLECT_STRUCT(EngineMetrics,
    ((uint64_t, total_orders_ingested))
    ((uint64_t, total_orders_executed))
    ((uint64_t, total_orders_rejected))
    ((uint64_t, total_volume_traded))
    ((uint64_t, total_pipeline_time_ns))
    ((double, avg_latency_us))
    ((double, throughput_ops_per_sec))
)

} // namespace trading

#endif // TRADING_MODELS_HPP
