///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "StreamListModel.hpp"
#include "Backend.hpp"
#include "EntryLog.hpp"
#include "StreamViewModel.hpp"
#include "Utility.hpp"
#include <eagine/extract.hpp>
#include <eagine/is_within_limits.hpp>
//------------------------------------------------------------------------------
StreamListModel::StreamListModel(StreamViewModel& parent)
  : QAbstractItemModel{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(StmListMdl), parent}
  , _parent{parent} {}
//------------------------------------------------------------------------------
auto StreamListModel::backend() const noexcept -> Backend& {
    return _parent.backend();
}
//------------------------------------------------------------------------------
auto StreamListModel::roleNames() const -> QHash<int, QByteArray> {
    QHash<int, QByteArray> result;
    result.insert(Qt::DisplayRole, "display");
    result.insert(streamLogIdentity, "logIdentity");
    result.insert(streamOSName, "osName");
    result.insert(streamOSCodeName, "osCodeName");
    result.insert(streamGitBranch, "gitBranch");
    result.insert(streamGitHashId, "gitHashId");
    result.insert(streamGitVersion, "gitVersion");
    result.insert(streamGitDescribe, "gitDescribe");
    result.insert(streamArchitecture, "architecture");
    result.insert(streamCompilerName, "compilerName");
    result.insert(streamCompilerVersionMajor, "compilerVersionMajor");
    result.insert(streamCompilerVersionMinor, "compilerVersionMinor");
    result.insert(streamCompilerVersionPatch, "compilerVersionPatch");
    return result;
}
//------------------------------------------------------------------------------
auto StreamListModel::columnCount(const QModelIndex&) const -> int {
    return 1;
}
//------------------------------------------------------------------------------
auto StreamListModel::parent(const QModelIndex&) const -> QModelIndex {
    return {};
}
//------------------------------------------------------------------------------
auto StreamListModel::index(int row, int column, const QModelIndex&) const
  -> QModelIndex {
    return QAbstractItemModel::createIndex(
      row, column, _parent.entryLog().getStreamInfo(row));
}
//------------------------------------------------------------------------------
auto StreamListModel::rowCount(const QModelIndex& i) const -> int {
    return i.isValid() ? 0 : _parent.entryLog().getStreamCount();
}
//------------------------------------------------------------------------------
auto StreamListModel::getStreamCount() const -> int {
    return _parent.entryLog().getStreamCount();
}
//------------------------------------------------------------------------------
auto StreamListModel::getStreamLogIdentity(const LogStreamInfo& info) const
  -> QString {
    return toQString(info.logIdentity);
}
//------------------------------------------------------------------------------
auto StreamListModel::getOSName(const LogStreamInfo& info) const -> QVariant {
    if(!info.osName.empty()) {
        return toQString(info.osName);
    }
    return {};
}
//------------------------------------------------------------------------------
auto StreamListModel::getOSCodeName(const LogStreamInfo& info) const
  -> QVariant {
    if(!info.osCodeName.empty()) {
        return toQString(info.osCodeName);
    }
    return {};
}
//------------------------------------------------------------------------------
auto StreamListModel::getGitBranch(const LogStreamInfo& info) const
  -> QVariant {
    if(!info.gitBranch.empty()) {
        return toQString(info.gitBranch);
    }
    return {};
}
//------------------------------------------------------------------------------
auto StreamListModel::getGitHashId(const LogStreamInfo& info) const
  -> QVariant {
    if(!info.gitHashId.empty()) {
        return toQString(info.gitHashId);
    }
    return {};
}
//------------------------------------------------------------------------------
auto StreamListModel::getGitVersion(const LogStreamInfo& info) const
  -> QVariant {
    if(!info.gitVersion.empty()) {
        return toQString(info.gitVersion);
    }
    return {};
}
//------------------------------------------------------------------------------
auto StreamListModel::getGitDescribe(const LogStreamInfo& info) const
  -> QVariant {
    if(!info.gitDescribe.empty()) {
        return toQString(info.gitDescribe);
    }
    return {};
}
//------------------------------------------------------------------------------
auto StreamListModel::getArchitecture(const LogStreamInfo& info) const
  -> QVariant {
    if(!info.architecture.empty()) {
        return toQString(info.architecture);
    }
    return {};
}
//------------------------------------------------------------------------------
auto StreamListModel::getCompilerName(const LogStreamInfo& info) const
  -> QVariant {
    if(!info.compilerName.empty()) {
        return toQString(info.compilerName);
    }
    return {};
}
//------------------------------------------------------------------------------
auto StreamListModel::getCompilerVersionMajor(const LogStreamInfo& info) const
  -> QVariant {
    if(info.compilerVersionMajor >= 0) {
        return {info.compilerVersionMajor};
    }
    return {};
}
//------------------------------------------------------------------------------
auto StreamListModel::getCompilerVersionMinor(const LogStreamInfo& info) const
  -> QVariant {
    if(info.compilerVersionMinor >= 0) {
        return {info.compilerVersionMinor};
    }
    return {};
}
//------------------------------------------------------------------------------
auto StreamListModel::getCompilerVersionPatch(const LogStreamInfo& info) const
  -> QVariant {
    if(info.compilerVersionPatch >= 0) {
        return {info.compilerVersionPatch};
    }
    return {};
}
//------------------------------------------------------------------------------
auto StreamListModel::data(const QModelIndex& index, int role) const
  -> QVariant {
    if(auto optStream{static_cast<LogStreamInfo*>(index.internalPointer())}) {
        auto& stream{eagine::extract(optStream)};
        switch(role) {
            case streamLogIdentity:
                return {getStreamLogIdentity(stream)};
            case streamOSName:
                return getOSName(stream);
            case streamOSCodeName:
                return getOSCodeName(stream);
            case streamGitBranch:
                return getGitBranch(stream);
            case streamGitHashId:
                return getGitHashId(stream);
            case streamGitVersion:
                return getGitVersion(stream);
            case streamGitDescribe:
                return getGitDescribe(stream);
            case streamArchitecture:
                return getArchitecture(stream);
            case streamCompilerName:
                return getCompilerName(stream);
            case streamCompilerVersionMajor:
                return getCompilerVersionMajor(stream);
            case streamCompilerVersionMinor:
                return getCompilerVersionMinor(stream);
            case streamCompilerVersionPatch:
                return getCompilerVersionPatch(stream);
            default:
                break;
        }
    }
    return {};
}
//------------------------------------------------------------------------------
void StreamListModel::handleStreamsAdded() {
    emit modelReset({});
    emit streamCountChanged();
}
//------------------------------------------------------------------------------

