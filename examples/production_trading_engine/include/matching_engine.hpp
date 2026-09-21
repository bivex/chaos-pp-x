#ifndef MATCHING_ENGINE_HPP
#define MATCHING_ENGINE_HPP

#include "trading_models.hpp"
#include <chrono>
#include <algorithm>
#include <vector>

namespace trading {

class RiskAndMatchingEngine {
    uint64_t next_exec_id = 100000;
    uint64_t next_fill_id = 500000;

    std::vector<uint8_t> binary_audit_journal;
    uint64_t total_volume = 0;
    uint64_t orders_ingested = 0;
    uint64_t orders_executed = 0;
    uint64_t orders_rejected = 0;

public:
    ExecutionReport process_order(LimitOrder& order) {
        auto start = std::chrono::steady_clock::now();
        ++orders_ingested;

        ExecutionReport report;
        report.report_id = ++next_exec_id;
        report.order_id = order.order_id;
        report.symbol = order.symbol;

        // 1. Pre-Trade Risk Check
        uint64_t notional = order.quantity * order.limit_price.units;
        if (!order.trader.is_market_maker && notional > order.trader.max_credit_units) {
            order.status = OrderStatus::REJECTED;
            report.final_status = OrderStatus::REJECTED;
            report.cumulative_qty = 0;
            report.leaves_qty = order.quantity;
            report.message = "Risk rejection: order notional exceeds trader credit limit";
            ++orders_rejected;
            
            auto end = std::chrono::steady_clock::now();
            report.latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            record_audit(order, report);
            return report;
        }

        if (order.quantity == 0) {
            order.status = OrderStatus::REJECTED;
            report.final_status = OrderStatus::REJECTED;
            report.cumulative_qty = 0;
            report.leaves_qty = 0;
            report.message = "Risk rejection: quantity must be positive";
            ++orders_rejected;

            auto end = std::chrono::steady_clock::now();
            report.latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            record_audit(order, report);
            return report;
        }

        // 2. Matching Engine Fill Allocation
        order.status = OrderStatus::RISK_PASSED;

        if (order.quantity >= 2) {
            // Multi-fill simulation: 60% first fill, 40% second fill
            uint64_t qty1 = (order.quantity * 6) / 10;
            uint64_t qty2 = order.quantity - qty1;

            FillEvent f1;
            f1.fill_id = ++next_fill_id;
            f1.order_id = order.order_id;
            f1.filled_qty = qty1;
            f1.match_price = order.limit_price;
            f1.fill_epoch_ns = order.timestamp_epoch_ns + 120;
            report.fills.push_back(f1);

            FillEvent f2;
            f2.fill_id = ++next_fill_id;
            f2.order_id = order.order_id;
            f2.filled_qty = qty2;
            f2.match_price = order.limit_price;
            f2.fill_epoch_ns = order.timestamp_epoch_ns + 350;
            report.fills.push_back(f2);
        } else {
            FillEvent f1;
            f1.fill_id = ++next_fill_id;
            f1.order_id = order.order_id;
            f1.filled_qty = order.quantity;
            f1.match_price = order.limit_price;
            f1.fill_epoch_ns = order.timestamp_epoch_ns + 100;
            report.fills.push_back(f1);
        }

        order.status = OrderStatus::FILLED;
        report.final_status = OrderStatus::FILLED;
        report.cumulative_qty = order.quantity;
        report.leaves_qty = 0;
        report.message = "Order fully executed across book liquidity";
        total_volume += order.quantity;
        ++orders_executed;

        auto end = std::chrono::steady_clock::now();
        report.latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // 3. Low-Latency Binary Audit Record
        record_audit(order, report);

        return report;
    }

    void record_audit(const LimitOrder& order, const ExecutionReport& report) {
        chaos_reflection::write_binary(binary_audit_journal, order);
        chaos_reflection::write_binary(binary_audit_journal, report);
    }

    size_t audit_journal_size() const {
        return binary_audit_journal.size();
    }

    EngineMetrics compute_metrics(uint64_t elapsed_ns) const {
        EngineMetrics m;
        m.total_orders_ingested = orders_ingested;
        m.total_orders_executed = orders_executed;
        m.total_orders_rejected = orders_rejected;
        m.total_volume_traded = total_volume;
        m.total_pipeline_time_ns = elapsed_ns;
        
        if (orders_ingested > 0 && elapsed_ns > 0) {
            double total_us = static_cast<double>(elapsed_ns) / 1000.0;
            m.avg_latency_us = total_us / static_cast<double>(orders_ingested);
            m.throughput_ops_per_sec = (static_cast<double>(orders_ingested) / static_cast<double>(elapsed_ns)) * 1e9;
        }
        return m;
    }
};

} // namespace trading

#endif // MATCHING_ENGINE_HPP
