///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "LogViewModel.hpp"
#include "EntryLog.hpp"
//------------------------------------------------------------------------------
LogViewModel::LogViewModel(EntryLog& parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(LogVM), parent}
  , _parent{parent}
  , _entryFilterModel{*this}
  , _entryListModel{*this} {}
//------------------------------------------------------------------------------
