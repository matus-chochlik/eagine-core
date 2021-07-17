///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "EntryFilterModel.hpp"
#include "LogViewModel.hpp"
//------------------------------------------------------------------------------
EntryFilterModel::EntryFilterModel(LogViewModel& parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(EntFlrtMdl), parent}
  , _parent{parent} {}
//------------------------------------------------------------------------------
