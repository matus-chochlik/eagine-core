/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/app_config.hpp>
#include <eagine/file_contents.hpp>
#include <eagine/logging/type/program_args.hpp>
#include <eagine/memory/buffer.hpp>
#include <eagine/memory/span_algo.hpp>

namespace eagine {
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto fetch_resource(
  const string_view description,
  const string_view key,
  const memory::const_block embedded_blk,
  memory::buffer& buf,
  application_config& cfg,
  const logger& log) -> memory::const_block {

    if(const auto res_path{cfg.get<std::string>(key)}) {
        if(const auto contents{file_contents(extract(res_path))}) {
            const auto blk = contents.block();
            buf.resize(blk.size());
            copy(blk, cover(buf));

            log.debug("using ${resource} from file ${path}")
              .arg("resource", description)
              .arg("key", key)
              .arg("path", "FsPath", extract(res_path));
            log.trace("${resource} content:")
              .arg("resource", description)
              .arg("blob", view(buf));
            return view(buf);
        } else {
            log.error("failed to load ${resource} from file ${path}")
              .arg("resource", description)
              .arg("key", key)
              .arg("path", "FsPath", extract(res_path));
        }
    } else if(embedded_blk) {
        log.debug("using embedded ${resource}").arg("resource", description);
        log.trace("${resource} content:")
          .arg("resource", description)
          .arg("blob", embedded_blk);
        return embedded_blk;
    }

    log.warning("could not fetch ${resource}").arg("resource", description);
    return {};
}
//------------------------------------------------------------------------------
} // namespace eagine
