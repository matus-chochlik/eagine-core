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
#include <iomanip>
#include <iostream>

namespace eagine {

auto main(main_ctx& ctx) -> int {
    const std::string na{"N/A"};

    auto& sys = ctx.system();

    std::cout << "host id: " << std::hex << either_or(sys.host_id(), na)
              << std::dec << std::endl;
    std::cout << "hostname: " << either_or(sys.hostname(), na) << std::endl;
    std::cout << "number of CPU cores: "
              << either_or(sys.cpu_concurrent_threads(), na) << std::endl;

    std::cout << "short time average load: "
              << either_or(sys.short_average_load(), na) << std::endl;
    std::cout << "long time average load: "
              << either_or(sys.long_average_load(), na) << std::endl;

    std::cout << "memory page size: " << either_or(sys.memory_page_size(), na)
              << std::endl;

    std::cout << "free RAM size: " << either_or(sys.free_ram_size(), na)
              << std::endl;
    std::cout << "total RAM size: " << either_or(sys.total_ram_size(), na)
              << std::endl;

    std::cout << "free swap size: " << either_or(sys.free_swap_size(), na)
              << std::endl;
    std::cout << "total swap size: " << either_or(sys.total_swap_size(), na)
              << std::endl;

    std::cout << "temperatures from " << sys.thermal_sensor_count()
              << " sensors: " << std::endl;

    const auto print_temperature = [](auto opt_t_kelvin) {
        if(opt_t_kelvin) {
            const auto t_k{extract(opt_t_kelvin)};

            tagged_quantity<float, units::degree_celsius> t_c{t_k};
            std::cout << t_c.value() << "[°C], ";

            tagged_quantity<float, units::degree_fahrenheit> t_f{t_k};
            std::cout << t_f.value() << "[°F], ";

            std::cout << t_k.value() << "[K]";
        } else {
            std::cout << "N/A";
        }
    };

    for(const auto i : integer_range(sys.thermal_sensor_count())) {
        std::cout << "  " << i << ": ";
        print_temperature(sys.sensor_temperature(i));
        std::cout << std::endl;
    }

    const auto [opt_t_k_min, opt_t_k_max] = sys.temperature_min_max();
    std::cout << "minimum / maximum temperature: ";
    print_temperature(opt_t_k_min);
    std::cout << " / ";
    print_temperature(opt_t_k_max);
    std::cout << std::endl;

    std::cout << "state of " << sys.cooling_device_count()
              << " cooling devices: " << std::endl;

    for(const auto i : integer_range(sys.cooling_device_count())) {
        std::cout << "  " << i << ": ";
        if(const auto opt_val{sys.cooling_device_state(i)}) {
            std::cout << extract(opt_val) * 100.F << "%";
        } else {
            std::cout << "N/A";
        }
        std::cout << std::endl;
    }

    std::cout << "status of " << sys.ac_supply_count()
              << " AC power supplies: " << std::endl;

    for(const auto i : integer_range(sys.ac_supply_count())) {
        std::cout << "  " << i << ": ";
        const auto online = sys.ac_supply_online(i);
        if(online) {
            std::cout << "online";
        } else if(!online) {
            std::cout << "offline";
        } else {
            std::cout << "N/A";
        }
        std::cout << std::endl;
    }

    std::cout << "capacity of " << sys.battery_count()
              << " batteries: " << std::endl;

    for(const auto i : integer_range(sys.battery_count())) {
        std::cout << "  " << i << ": ";
        if(const auto opt_val{sys.battery_capacity(i)}) {

            std::cout << extract(opt_val) * 100.F << "%";
        } else {
            std::cout << "N/A";
        }
        std::cout << std::endl;
    }

    std::cout << "used power supply: " << enumerator_name(sys.power_supply())
              << std::endl;

    return 0;
}

} // namespace eagine
