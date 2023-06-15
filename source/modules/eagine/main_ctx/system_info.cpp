/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.main_ctx:system_info;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.reflection;
import eagine.core.units;
import eagine.core.valid_if;
import :parent;
import :object;

namespace eagine {

/// @brief System power supply kind.
/// @ingroup msgbus
/// @see system_info
export enum class power_supply_kind : std::uint8_t {
    /// @brief Unknown power supply.
    unknown,
    /// @brief Battery.
    battery,
    /// @brief AC power supply.
    ac_supply
};

export template <typename Selector>
constexpr auto enumerator_mapping(
  const std::type_identity<power_supply_kind>,
  const Selector) noexcept {
    return enumerator_map_type<power_supply_kind, 3>{
      {{"unknown", power_supply_kind::unknown},
       {"battery", power_supply_kind::battery},
       {"ac_supply", power_supply_kind::ac_supply}}};
}

class system_info_impl;

/// @brief Class providing basic system information.
/// @ingroup main_context
export class system_info : public main_ctx_object {
public:
    system_info(main_ctx_parent);

    /// @brief Alias for host id type.
    using host_id_type = std::uint32_t;

    /// @brief Do potentially expensive pre-initialization and caching.
    auto preinitialize() noexcept -> system_info&;

    /// @brief Returns a unique numeric id of this host computer.
    [[nodiscard]] auto host_id() noexcept -> valid_if_positive<host_id_type>;

    /// @brief Returns the hostname.
    [[nodiscard]] auto hostname() noexcept -> valid_if_not_empty<std::string>;

    /// @brief Paths to the system's configuration directory.
    [[nodiscard]] auto config_dir_path() noexcept
      -> valid_if_not_empty<string_view>;

    /// @brief Returns the system uptime.
    [[nodiscard]] auto uptime() noexcept -> std::chrono::duration<float>;

    /// @brief Returns the number of threads that can run concurrently.
    [[nodiscard]] auto cpu_concurrent_threads() noexcept
      -> valid_if_positive<span_size_t> {
        return {span_size(std::thread::hardware_concurrency())};
    }

    /// @brief Returns the number of running processes.
    [[nodiscard]] auto current_processes() noexcept
      -> valid_if_positive<span_size_t>;

    /// @brief Returns the short-term average system load in range (0.0, 1.0).
    [[nodiscard]] auto short_average_load() noexcept
      -> valid_if_nonnegative<float>;

    /// @brief Returns the long-term average system load in range (0.0, 1.0).
    [[nodiscard]] auto long_average_load() noexcept
      -> valid_if_nonnegative<float>;

    /// @brief Returns the memory page size on this system.
    [[nodiscard]] auto memory_page_size() noexcept
      -> valid_if_positive<span_size_t>;

    /// @brief Returns free RAM size.
    [[nodiscard]] auto free_ram_size() noexcept
      -> valid_if_positive<span_size_t>;

    /// @brief Returns total RAM size.
    [[nodiscard]] auto total_ram_size() noexcept
      -> valid_if_positive<span_size_t>;

    /// @brief Returns free swap size.
    [[nodiscard]] auto free_swap_size() noexcept
      -> valid_if_nonnegative<span_size_t>;

    /// @brief Returns total swap size.
    [[nodiscard]] auto total_swap_size() noexcept
      -> valid_if_nonnegative<span_size_t>;

    /// @brief Returns the number of system thermal sensors.
    [[nodiscard]] auto thermal_sensor_count() noexcept -> span_size_t;

    /// @brief Returns the temperature on the i-th thermal sensor.
    [[nodiscard]] auto sensor_temperature(span_size_t) noexcept
      -> valid_if_positive<kelvins_t<float>>;

    /// @brief Returns the minimum and maximum temperature on the thermal sensors.
    [[nodiscard]] auto temperature_min_max() noexcept -> std::tuple<
      valid_if_positive<kelvins_t<float>>,
      valid_if_positive<kelvins_t<float>>>;

    /// @brief Returns the temperature on the CPU thermal sensor if present.
    [[nodiscard]] auto cpu_temperature() noexcept
      -> valid_if_positive<kelvins_t<float>>;

    /// @brief Returns the temperature on the GPU thermal sensor if present.
    [[nodiscard]] auto gpu_temperature() noexcept
      -> valid_if_positive<kelvins_t<float>>;

    /// @brief Returns the number of cooling devices.
    [[nodiscard]] auto cooling_device_count() noexcept -> span_size_t;

    /// @brief Returns the state of the i-th cooling device.
    [[nodiscard]] auto cooling_device_state(span_size_t) noexcept
      -> valid_if_between_0_1<float>;

    /// @brief Returns the count of batteries in the system.
    [[nodiscard]] auto battery_count() noexcept -> span_size_t;

    /// @brief Returns the capacity of the i-th battery.
    [[nodiscard]] auto battery_capacity(span_size_t) noexcept
      -> valid_if_between_0_1<float>;

    /// @brief Returns the count of AC power supplies in the system.
    [[nodiscard]] auto ac_supply_count() noexcept -> span_size_t;

    /// @brief Indicates whether the i-th power supply is online
    [[nodiscard]] auto ac_supply_online(span_size_t) noexcept -> tribool;

    /// @brief Returns the currently user power supply kind.
    [[nodiscard]] auto power_supply() noexcept -> power_supply_kind {
        for(const auto i : integer_range(ac_supply_count())) {
            if(ac_supply_online(i)) {
                return power_supply_kind::ac_supply;
            }
        }
        for(const auto i : integer_range(battery_count())) {
            if(battery_capacity(i) > 0.F) {
                return power_supply_kind::battery;
            }
        }
        return power_supply_kind::unknown;
    }

private:
    shared_holder<system_info_impl> _pimpl;
    auto _impl() noexcept -> optional_reference<system_info_impl>;
};

} // namespace eagine

