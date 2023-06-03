/// @example eagine/encrypt_decrypt.cpp
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
    main_ctx_object out{"encrypt", ctx};

    memory::buffer encrypted;
    if(ctx.encrypt_shared(
         "do not use this functionality for encryption of predictable data",
         encrypted)) {
        std::string decrypted;
        if(ctx.decrypt_shared(view(encrypted), decrypted)) {
            out.cio_print("decrypted: ${s}").arg("s", decrypted);
        } else {
            out.cio_error("failed to decrypt string");
        }
    } else {
        out.cio_error("failed to encrypt string");
    }

    if(ctx.encrypt_shared_password("test.password", encrypted)) {
        if(ctx.matches_encrypted_shared_password("test.password", encrypted)) {
            out.cio_print("tested password matches");
        } else {
            out.cio_error("failed to match password");
        }
    } else {
        out.cio_error("failed to encrypt password");
    }

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

