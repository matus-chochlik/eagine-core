///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_STREAM_LIST_MODEL
#define EAGINE_XML_LOG_STREAM_LIST_MODEL

#include <eagine/main_ctx_object.hpp>
#include <QAbstractItemModel>

class Backend;
class StreamViewModel;
struct LogStreamInfo;
//------------------------------------------------------------------------------
class StreamListModel final
  : public QAbstractItemModel
  , public eagine::main_ctx_object {
    Q_OBJECT

    Q_PROPERTY(int streamCount READ getStreamCount NOTIFY streamCountChanged)
public:
    StreamListModel(StreamViewModel& parent);

    auto backend() const noexcept -> Backend&;

    auto roleNames() const -> QHash<int, QByteArray> final;
    auto index(int row, int column, const QModelIndex& parent = {}) const
      -> QModelIndex final;
    auto parent(const QModelIndex& child) const -> QModelIndex final;
    auto columnCount(const QModelIndex& parent) const -> int final;
    auto rowCount(const QModelIndex& parent) const -> int final;
    auto data(const QModelIndex& index, int role) const -> QVariant final;
    auto getStreamCount() const -> int;

    void handleStreamsAdded();
signals:
    void streamCountChanged();
public slots:

private:
    enum StreamAttributes : int {
        streamLogIdentity,
        streamOSName,
        streamOSCodeName,
        streamGitBranch,
        streamGitHashId,
        streamGitVersion,
        streamGitDescribe,
        streamArchitecture,
        streamCompilerName,
        streamCompilerVersionMajor,
        streamCompilerVersionMinor,
        streamCompilerVersionPatch
    };

    auto getStreamLogIdentity(const LogStreamInfo&) const -> QString;
    auto getOSName(const LogStreamInfo&) const -> QVariant;
    auto getOSCodeName(const LogStreamInfo&) const -> QVariant;
    auto getGitBranch(const LogStreamInfo&) const -> QVariant;
    auto getGitHashId(const LogStreamInfo&) const -> QVariant;
    auto getGitVersion(const LogStreamInfo&) const -> QVariant;
    auto getGitDescribe(const LogStreamInfo&) const -> QVariant;
    auto getArchitecture(const LogStreamInfo&) const -> QVariant;
    auto getCompilerName(const LogStreamInfo&) const -> QVariant;
    auto getCompilerVersionMajor(const LogStreamInfo&) const -> QVariant;
    auto getCompilerVersionMinor(const LogStreamInfo&) const -> QVariant;
    auto getCompilerVersionPatch(const LogStreamInfo&) const -> QVariant;

    StreamViewModel& _parent;
};
//------------------------------------------------------------------------------
#endif
