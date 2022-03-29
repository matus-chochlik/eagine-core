/// @example eagine/system_info.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/integer_range.hpp>
#include <eagine/main.hpp>
#include <eagine/reflect/enumerators.hpp>
#include <eagine/system_info.hpp>

namespace eagine {

auto main(main_ctx& ctx) -> int {
    const std::string na{"N/A"};

    auto& sys = ctx.system();

    sys.cio_print("host id: ${id}")
      .arg(EAGINE_ID(id), EAGINE_ID(hex), either_or(sys.host_id(), na));

    sys.cio_print("hostname: ${name}")
      .arg(EAGINE_ID(name), either_or(sys.hostname(), na));

    sys.cio_print("number of CPU cores: ${count}")
      .arg(EAGINE_ID(count), either_or(sys.cpu_concurrent_threads(), na));

    sys.cio_print("short time average load: ${load}")
      .arg(EAGINE_ID(load), either_or(sys.short_average_load(), na));
    sys.cio_print("long time average load: ${load}")
      .arg(EAGINE_ID(load), either_or(sys.long_average_load(), na));

    sys.cio_print("memory page size: ${size}")
      .arg(
        EAGINE_ID(size),
        EAGINE_ID(ByteSize),
        either_or(sys.memory_page_size(), na));

    sys.cio_print("free RAM size: ${size}")
      .arg(
        EAGINE_ID(size),
        EAGINE_ID(ByteSize),
        either_or(sys.free_ram_size(), na));
    sys.cio_print("total RAM size: ${size}")
      .arg(
        EAGINE_ID(size),
        EAGINE_ID(ByteSize),
        either_or(sys.total_ram_size(), na));

    sys.cio_print("free swap size: ${size}")
      .arg(
        EAGINE_ID(size),
        EAGINE_ID(ByteSize),
        either_or(sys.free_swap_size(), na));
    sys.cio_print("total swap size: ${size}")
      .arg(
        EAGINE_ID(size),
        EAGINE_ID(ByteSize),
        either_or(sys.total_swap_size(), na));

    sys.cio_print("temperatures from ${count} sensors: ")
      .arg(EAGINE_ID(count), sys.thermal_sensor_count());

    for(const auto i : integer_range(sys.thermal_sensor_count())) {
        const auto t_k{extract(sys.sensor_temperature(i))};
        const tagged_quantity<float, units::degree_celsius> t_c{t_k};
        const tagged_quantity<float, units::degree_fahrenheit> t_f{t_k};

        sys.cio_print("  ${i}: ${degC}[°C], ${degF}[°F], ${kelvin}[K]")
          .arg(EAGINE_ID(kelvin), t_k.value())
          .arg(EAGINE_ID(degC), t_c.value())
          .arg(EAGINE_ID(degF), t_f.value())
          .arg(EAGINE_ID(i), i);
    }
    sys.cio_print("state of ${count} cooling devices: ")
      .arg(EAGINE_ID(count), sys.cooling_device_count());

    for(const auto i : integer_range(sys.cooling_device_count())) {
        sys.cio_print("  ${i}: ${value}")
          .arg(
            EAGINE_ID(value),
            EAGINE_ID(Ratio),
            either_or(sys.cooling_device_state(i), na))
          .arg(EAGINE_ID(i), i);
    }

    sys.cio_print("status of ${count} AC power supplies: ")
      .arg(EAGINE_ID(count), sys.thermal_sensor_count());

    for(const auto i : integer_range(sys.ac_supply_count())) {
        const auto online = sys.ac_supply_online(i);
        sys.cio_print("  ${i}: ${status}")
          .arg(
            EAGINE_ID(status),
            online    ? string_view{"online"}
            : !online ? string_view{"offline"}
                      : na)
          .arg(EAGINE_ID(i), i);
    }

    sys.cio_print("capacity of ${count} batteries: ")
      .arg(EAGINE_ID(count), sys.battery_count());

    for(const auto i : integer_range(sys.battery_count())) {
        sys.cio_print("  ${i}: ${value}")
          .arg(
            EAGINE_ID(value),
            EAGINE_ID(Ratio),
            either_or(sys.battery_capacity(i), na))
          .arg(EAGINE_ID(i), i);
    }

    sys.cio_print("used power supply: ${kind}")
      .arg(EAGINE_ID(kind), sys.power_supply());

    return 0;
}

} // namespace eagine
