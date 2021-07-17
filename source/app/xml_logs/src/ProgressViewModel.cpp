///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "ProgressViewModel.hpp"
#include "EntryLog.hpp"
//------------------------------------------------------------------------------
ProgressViewModel::ProgressViewModel(EntryLog& parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(ProgressVM), parent}
  , _parent{parent} {}
//------------------------------------------------------------------------------
