///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "ChartsViewModel.hpp"
#include "EntryLog.hpp"
//------------------------------------------------------------------------------
ChartsViewModel::ChartsViewModel(EntryLog& parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{"ChartsVM", parent}
  , _parent{parent} {}
//------------------------------------------------------------------------------
