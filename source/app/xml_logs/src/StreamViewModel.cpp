///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "StreamViewModel.hpp"
#include "EntryLog.hpp"
//------------------------------------------------------------------------------
StreamViewModel::StreamViewModel(EntryLog& parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(StreamVM), parent}
  , _parent{parent}
  , _streamListModel{*this} {}
//------------------------------------------------------------------------------
auto StreamViewModel::backend() const noexcept -> Backend& {
    return entryLog().backend();
}
//------------------------------------------------------------------------------
auto StreamViewModel::getStreamListModel() noexcept -> StreamListModel* {
    return &_streamListModel;
}
//------------------------------------------------------------------------------
void StreamViewModel::onStreamsAdded() {
    _streamListModel.handleStreamsAdded();
}
//------------------------------------------------------------------------------
