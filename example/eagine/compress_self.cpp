/// @example eagine/compress_self.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;

namespace eagine {
//------------------------------------------------------------------------------
static inline void pack_unpack(
  main_ctx& ctx,
  data_compressor& comp,
  memory::const_block original) {
    ctx.log()
      .info("original size ${original}")
      .arg("original", "ByteSize", original.size());

    memory::buffer buf1{};
    const auto level = data_compression_level::highest;

    if(auto packed{comp.compress(original, buf1, level)}) {

        ctx.log()
          .info("packed size ${packed}")
          .arg("packed", "ByteSize", packed.size());

        memory::buffer buf2{};

        if(auto unpacked{comp.decompress(packed, buf2)}) {

            ctx.log()
              .info("unpacked size ${unpacked}")
              .arg("unpacked", "ByteSize", unpacked.size());

            if(are_equal(original, unpacked)) {
                const auto compr_ratio =
                  float(packed.size()) / float(unpacked.size());
                ctx.log()
                  .info("original and unpacked block are equal")
                  .arg("comprRatio", "Ratio", compr_ratio);
            } else {
                ctx.log().error("original and unpacked block are different");
            }
        } else {
            ctx.log().error("failed to unpack memory block");
        }
    } else {
        ctx.log().error("failed to pack memory block");
    }
}
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {

    if(file_contents self{ctx.exe_path()}) {
        pack_unpack(ctx, ctx.compressor(), self);
    }
    return 0;
}
//------------------------------------------------------------------------------
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

