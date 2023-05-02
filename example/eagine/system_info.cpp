/// @example eagine/system_info.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {

auto main(main_ctx& ctx) -> int {
    const std::string na{"N/A"};

    auto& sys = ctx.system();
    main_ctx_object out{"systemInfo", ctx};

    out.cio_print("host id: ${id}")
      .arg("id", "hex", either_or(sys.host_id(), na));

    out.cio_print("hostname: ${name}")
      .arg("name", either_or(sys.hostname(), na));

    out.cio_print("number of CPU cores: ${count}")
      .arg("count", either_or(sys.cpu_concurrent_threads(), na));

    out.cio_print("short time average load: ${load}")
      .arg("load", either_or(sys.short_average_load(), na));
    out.cio_print("long time average load: ${load}")
      .arg("load", either_or(sys.long_average_load(), na));

    out.cio_print("memory page size: ${size}")
      .arg("size", "ByteSize", either_or(sys.memory_page_size(), na));

    out.cio_print("free RAM size: ${size}")
      .arg("size", "ByteSize", either_or(sys.free_ram_size(), na));
    out.cio_print("total RAM size: ${size}")
      .arg("size", "ByteSize", either_or(sys.total_ram_size(), na));

    out.cio_print("free swap size: ${size}")
      .arg("size", "ByteSize", either_or(sys.free_swap_size(), na));
    out.cio_print("total swap size: ${size}")
      .arg("size", "ByteSize", either_or(sys.total_swap_size(), na));

    out.cio_print("temperatures from ${count} sensors: ")
      .arg("count", sys.thermal_sensor_count());

    for(const auto i : integer_range(sys.thermal_sensor_count())) {
        sys.sensor_temperature(i).and_then([&](const auto t_k) {
            const tagged_quantity<float, units::degree_celsius> t_c{t_k};
            const tagged_quantity<float, units::degree_fahrenheit> t_f{t_k};

            out.cio_print("  ${i}: ${degC}[°C], ${degF}[°F], ${kelvin}[K]")
              .arg("kelvin", t_k.value())
              .arg("degC", t_c.value())
              .arg("degF", t_f.value())
              .arg("i", i);
            return noopt{};
        });
    }
    out.cio_print("state of ${count} cooling devices: ")
      .arg("count", sys.cooling_device_count());

    for(const auto i : integer_range(sys.cooling_device_count())) {
        out.cio_print("  ${i}: ${value}")
          .arg("value", "Ratio", either_or(sys.cooling_device_state(i), na))
          .arg("i", i);
    }

    out.cio_print("status of ${count} AC power supplies: ")
      .arg("count", sys.ac_supply_count());

    for(const auto i : integer_range(sys.ac_supply_count())) {
        const auto online = sys.ac_supply_online(i);
        out.cio_print("  ${i}: ${status}")
          .arg(
            "status",
            online       ? string_view{"online"}
            : not online ? string_view{"offline"}
                         : na)
          .arg("i", i);
    }

    out.cio_print("capacity of ${count} batteries: ")
      .arg("count", sys.battery_count());

    for(const auto i : integer_range(sys.battery_count())) {
        out.cio_print("  ${i}: ${value}")
          .arg("value", "Ratio", either_or(sys.battery_capacity(i), na))
          .arg("i", i);
    }

    out.cio_print("used power supply: ${kind}").arg("kind", sys.power_supply());

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

