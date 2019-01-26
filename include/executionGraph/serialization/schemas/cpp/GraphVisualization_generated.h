// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_GRAPHVISUALIZATION_EXECUTIONGRAPH_SERIALIZATION_H_
#define FLATBUFFERS_GENERATED_GRAPHVISUALIZATION_EXECUTIONGRAPH_SERIALIZATION_H_

#include "flatbuffers/flatbuffers.h"

#include "executionGraph/serialization/schemas/cpp/cpp/cpp/cpp/cpp/cpp/cpp/cpp/cpp/cpp/DataTypes_generated.h"
#include "executionGraph/serialization/schemas/cpp/cpp/cpp/cpp/cpp/cpp/cpp/cpp/cpp/cpp/SocketLinkDescription_generated.h"

namespace executionGraph {
namespace serialization {

struct WorkspaceVisualization;

struct NodeVisualization;

struct LinkVisualization;

struct GraphVisualization;

struct WorkspaceVisualization FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_WORKSPACEID = 4,
    VT_BOX = 6
  };
  uint64_t workspaceId() const {
    return GetField<uint64_t>(VT_WORKSPACEID, 0);
  }
  const Box2f *box() const {
    return GetStruct<const Box2f *>(VT_BOX);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint64_t>(verifier, VT_WORKSPACEID) &&
           VerifyFieldRequired<Box2f>(verifier, VT_BOX) &&
           verifier.EndTable();
  }
};

struct WorkspaceVisualizationBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_workspaceId(uint64_t workspaceId) {
    fbb_.AddElement<uint64_t>(WorkspaceVisualization::VT_WORKSPACEID, workspaceId, 0);
  }
  void add_box(const Box2f *box) {
    fbb_.AddStruct(WorkspaceVisualization::VT_BOX, box);
  }
  explicit WorkspaceVisualizationBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  WorkspaceVisualizationBuilder &operator=(const WorkspaceVisualizationBuilder &);
  flatbuffers::Offset<WorkspaceVisualization> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<WorkspaceVisualization>(end);
    fbb_.Required(o, WorkspaceVisualization::VT_BOX);
    return o;
  }
};

inline flatbuffers::Offset<WorkspaceVisualization> CreateWorkspaceVisualization(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint64_t workspaceId = 0,
    const Box2f *box = 0) {
  WorkspaceVisualizationBuilder builder_(_fbb);
  builder_.add_workspaceId(workspaceId);
  builder_.add_box(box);
  return builder_.Finish();
}

struct NodeVisualization FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_NAME = 6,
    VT_POSITION = 8
  };
  uint64_t id() const {
    return GetField<uint64_t>(VT_ID, 0);
  }
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  const Vector2d *position() const {
    return GetStruct<const Vector2d *>(VT_POSITION);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint64_t>(verifier, VT_ID) &&
           VerifyOffsetRequired(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyField<Vector2d>(verifier, VT_POSITION) &&
           verifier.EndTable();
  }
};

struct NodeVisualizationBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(uint64_t id) {
    fbb_.AddElement<uint64_t>(NodeVisualization::VT_ID, id, 0);
  }
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(NodeVisualization::VT_NAME, name);
  }
  void add_position(const Vector2d *position) {
    fbb_.AddStruct(NodeVisualization::VT_POSITION, position);
  }
  explicit NodeVisualizationBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  NodeVisualizationBuilder &operator=(const NodeVisualizationBuilder &);
  flatbuffers::Offset<NodeVisualization> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<NodeVisualization>(end);
    fbb_.Required(o, NodeVisualization::VT_NAME);
    return o;
  }
};

inline flatbuffers::Offset<NodeVisualization> CreateNodeVisualization(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint64_t id = 0,
    flatbuffers::Offset<flatbuffers::String> name = 0,
    const Vector2d *position = 0) {
  NodeVisualizationBuilder builder_(_fbb);
  builder_.add_id(id);
  builder_.add_position(position);
  builder_.add_name(name);
  return builder_.Finish();
}

inline flatbuffers::Offset<NodeVisualization> CreateNodeVisualizationDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint64_t id = 0,
    const char *name = nullptr,
    const Vector2d *position = 0) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  return executionGraph::serialization::CreateNodeVisualization(
      _fbb,
      id,
      name__,
      position);
}

struct LinkVisualization FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAME = 4,
    VT_SOCKET = 6
  };
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  const SocketLinkDescription *socket() const {
    return GetStruct<const SocketLinkDescription *>(VT_SOCKET);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyFieldRequired<SocketLinkDescription>(verifier, VT_SOCKET) &&
           verifier.EndTable();
  }
};

struct LinkVisualizationBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(LinkVisualization::VT_NAME, name);
  }
  void add_socket(const SocketLinkDescription *socket) {
    fbb_.AddStruct(LinkVisualization::VT_SOCKET, socket);
  }
  explicit LinkVisualizationBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  LinkVisualizationBuilder &operator=(const LinkVisualizationBuilder &);
  flatbuffers::Offset<LinkVisualization> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<LinkVisualization>(end);
    fbb_.Required(o, LinkVisualization::VT_SOCKET);
    return o;
  }
};

inline flatbuffers::Offset<LinkVisualization> CreateLinkVisualization(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> name = 0,
    const SocketLinkDescription *socket = 0) {
  LinkVisualizationBuilder builder_(_fbb);
  builder_.add_socket(socket);
  builder_.add_name(name);
  return builder_.Finish();
}

inline flatbuffers::Offset<LinkVisualization> CreateLinkVisualizationDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr,
    const SocketLinkDescription *socket = 0) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  return executionGraph::serialization::CreateLinkVisualization(
      _fbb,
      name__,
      socket);
}

struct GraphVisualization FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAME = 4,
    VT_WORKSPACEVISUALIZATION = 6,
    VT_NODEVISUALIZATION = 8,
    VT_LINKVISUALIZATION = 10
  };
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  const WorkspaceVisualization *workspaceVisualization() const {
    return GetPointer<const WorkspaceVisualization *>(VT_WORKSPACEVISUALIZATION);
  }
  const flatbuffers::Vector<flatbuffers::Offset<NodeVisualization>> *nodeVisualization() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<NodeVisualization>> *>(VT_NODEVISUALIZATION);
  }
  const flatbuffers::Vector<flatbuffers::Offset<LinkVisualization>> *linkVisualization() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<LinkVisualization>> *>(VT_LINKVISUALIZATION);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffsetRequired(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyOffset(verifier, VT_WORKSPACEVISUALIZATION) &&
           verifier.VerifyTable(workspaceVisualization()) &&
           VerifyOffset(verifier, VT_NODEVISUALIZATION) &&
           verifier.VerifyVector(nodeVisualization()) &&
           verifier.VerifyVectorOfTables(nodeVisualization()) &&
           VerifyOffset(verifier, VT_LINKVISUALIZATION) &&
           verifier.VerifyVector(linkVisualization()) &&
           verifier.VerifyVectorOfTables(linkVisualization()) &&
           verifier.EndTable();
  }
};

struct GraphVisualizationBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(GraphVisualization::VT_NAME, name);
  }
  void add_workspaceVisualization(flatbuffers::Offset<WorkspaceVisualization> workspaceVisualization) {
    fbb_.AddOffset(GraphVisualization::VT_WORKSPACEVISUALIZATION, workspaceVisualization);
  }
  void add_nodeVisualization(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<NodeVisualization>>> nodeVisualization) {
    fbb_.AddOffset(GraphVisualization::VT_NODEVISUALIZATION, nodeVisualization);
  }
  void add_linkVisualization(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<LinkVisualization>>> linkVisualization) {
    fbb_.AddOffset(GraphVisualization::VT_LINKVISUALIZATION, linkVisualization);
  }
  explicit GraphVisualizationBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  GraphVisualizationBuilder &operator=(const GraphVisualizationBuilder &);
  flatbuffers::Offset<GraphVisualization> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<GraphVisualization>(end);
    fbb_.Required(o, GraphVisualization::VT_NAME);
    return o;
  }
};

inline flatbuffers::Offset<GraphVisualization> CreateGraphVisualization(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> name = 0,
    flatbuffers::Offset<WorkspaceVisualization> workspaceVisualization = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<NodeVisualization>>> nodeVisualization = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<LinkVisualization>>> linkVisualization = 0) {
  GraphVisualizationBuilder builder_(_fbb);
  builder_.add_linkVisualization(linkVisualization);
  builder_.add_nodeVisualization(nodeVisualization);
  builder_.add_workspaceVisualization(workspaceVisualization);
  builder_.add_name(name);
  return builder_.Finish();
}

inline flatbuffers::Offset<GraphVisualization> CreateGraphVisualizationDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr,
    flatbuffers::Offset<WorkspaceVisualization> workspaceVisualization = 0,
    const std::vector<flatbuffers::Offset<NodeVisualization>> *nodeVisualization = nullptr,
    const std::vector<flatbuffers::Offset<LinkVisualization>> *linkVisualization = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  auto nodeVisualization__ = nodeVisualization ? _fbb.CreateVector<flatbuffers::Offset<NodeVisualization>>(*nodeVisualization) : 0;
  auto linkVisualization__ = linkVisualization ? _fbb.CreateVector<flatbuffers::Offset<LinkVisualization>>(*linkVisualization) : 0;
  return executionGraph::serialization::CreateGraphVisualization(
      _fbb,
      name__,
      workspaceVisualization,
      nodeVisualization__,
      linkVisualization__);
}

}  // namespace serialization
}  // namespace executionGraph

#endif  // FLATBUFFERS_GENERATED_GRAPHVISUALIZATION_EXECUTIONGRAPH_SERIALIZATION_H_
