/// @example eagine/serialize_fragmented.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {
//------------------------------------------------------------------------------
template <typename T>
void use_data(std::vector<T>& data) {
    std::size_t n = data.size();
    std::cout << "element count: " << n;

    bool data_ok = true;
    for(const auto& v : data) {
        data_ok &= are_equal(v, T(--n));
    }

    if(data_ok) {
        std::cout << ", data ok" << std::endl;
    }
}
//------------------------------------------------------------------------------
template <typename T>
struct deserial_data_state {
    deserialization_errors errors{};
    fast_deserializer_backend backend{};
    fragment_deserialize_wrapper<span<T>> defragmenter{};
    std::vector<T> data{};
    span_size_t count{0};
    bool first{true};
    bool failed{false};
};
//------------------------------------------------------------------------------
template <typename T>
void deserialize_data(
  const memory::const_block fragment,
  deserial_data_state<T>& state) {

    if(not state.failed) {
        block_data_source source(fragment);
        state.backend.set_source(source);
        if(state.first) {
            state.errors |= state.backend.begin();
            if(state.errors) {
                state.failed = true;
            } else {
                std::size_t count{0};
                state.errors |= deserializer<std::size_t>().read(
                  cover_one(count), state.backend);
                if(state.errors) {
                    state.failed = true;
                } else {
                    state.data.resize(count);
                    state.defragmenter.set_target(cover(state.data));
                }
            }
        }

        using error_code = deserialization_error_code;
        using defragmenter_type = fragment_deserialize_wrapper<span<T>>;

        state.errors = {};

        if(not state.defragmenter.is_done()) {
            state.errors |= deserializer<defragmenter_type>().read(
              state.defragmenter, state.backend);

            if(not state.errors.has(error_code::not_enough_data)) {
                if(state.errors.has_at_most(error_code::incomplete_read)) {
                    if(state.defragmenter.is_done()) {
                        std::cout << "deserialized " << state.count
                                  << " fragments " << std::endl;
                        use_data(state.data);
                    }
                } else if(not state.first) {
                    state.failed = true;
                }
            }
        }
        state.first = false;
        state.count++;
    }
}
//------------------------------------------------------------------------------
template <typename T>
void serialize_data(const std::vector<T>& instance) {

    deserial_data_state<T> state{};
    std::array<byte, 256> chunk{};

    block_data_sink sink(cover(chunk));
    fast_serializer_backend backend{sink};
    serialization_errors errors = backend.begin();

    if(not errors) {
        errors |= serializer<std::size_t>().write(instance.size(), backend);

        using error_code = serialization_error_code;
        using fragmenter_type = fragment_serialize_wrapper<span<const T>>;
        fragmenter_type fragmenter(view(instance));

        while(not fragmenter.is_done()) {
            errors = {};
            if(not sink.free()) {
                sink = block_data_sink{cover(chunk)};
                backend.set_sink(sink);
            }
            errors |= serializer<fragmenter_type>().write(fragmenter, backend);
            if(errors.has(error_code::too_much_data)) {
                sink = block_data_sink{cover(chunk)};
                backend.set_sink(sink);
                continue;
            }
            if(not errors.has_at_most(error_code::incomplete_write)) {
                break;
            }
            deserialize_data(sink.done(), state);
        }
        errors |= backend.finish();
    }
}
//------------------------------------------------------------------------------
template <typename T>
auto generate_data(std::size_t n) -> std::vector<T> {

    std::vector<T> data(n);
    for(auto& v : data) {
        v = T(--n);
    }

    return data;
}
//------------------------------------------------------------------------------
} // namespace eagine

auto main() -> int {
    using namespace eagine;

    serialize_data(generate_data<int>(std_size(8 * 1024)));
    serialize_data(generate_data<short>(std_size(4 * 1024)));
    serialize_data(generate_data<char>(std_size(2 * 1024)));

    return 0;
}
