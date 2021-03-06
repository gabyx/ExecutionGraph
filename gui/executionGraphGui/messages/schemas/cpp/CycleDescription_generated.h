// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_CYCLEDESCRIPTION_EXECUTIONGRAPHGUI_SERIALIZATION_H_
#define FLATBUFFERS_GENERATED_CYCLEDESCRIPTION_EXECUTIONGRAPHGUI_SERIALIZATION_H_

#include "flatbuffers/flatbuffers.h"

#include "executionGraph/serialization/schemas/cpp/SocketLinkDescription_generated.h"

namespace executionGraphGui {
namespace serialization {

struct CycleDescription;

struct CycleDescription FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_CYCLEPATH = 4
  };
  const flatbuffers::Vector<const executionGraph::serialization::SocketLinkDescription *> *cyclePath() const {
    return GetPointer<const flatbuffers::Vector<const executionGraph::serialization::SocketLinkDescription *> *>(VT_CYCLEPATH);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffsetRequired(verifier, VT_CYCLEPATH) &&
           verifier.VerifyVector(cyclePath()) &&
           verifier.EndTable();
  }
};

struct CycleDescriptionBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_cyclePath(flatbuffers::Offset<flatbuffers::Vector<const executionGraph::serialization::SocketLinkDescription *>> cyclePath) {
    fbb_.AddOffset(CycleDescription::VT_CYCLEPATH, cyclePath);
  }
  explicit CycleDescriptionBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  CycleDescriptionBuilder &operator=(const CycleDescriptionBuilder &);
  flatbuffers::Offset<CycleDescription> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<CycleDescription>(end);
    fbb_.Required(o, CycleDescription::VT_CYCLEPATH);
    return o;
  }
};

inline flatbuffers::Offset<CycleDescription> CreateCycleDescription(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<const executionGraph::serialization::SocketLinkDescription *>> cyclePath = 0) {
  CycleDescriptionBuilder builder_(_fbb);
  builder_.add_cyclePath(cyclePath);
  return builder_.Finish();
}

inline flatbuffers::Offset<CycleDescription> CreateCycleDescriptionDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<executionGraph::serialization::SocketLinkDescription> *cyclePath = nullptr) {
  auto cyclePath__ = cyclePath ? _fbb.CreateVectorOfStructs<executionGraph::serialization::SocketLinkDescription>(*cyclePath) : 0;
  return executionGraphGui::serialization::CreateCycleDescription(
      _fbb,
      cyclePath__);
}

}  // namespace serialization
}  // namespace executionGraphGui

#endif  // FLATBUFFERS_GENERATED_CYCLEDESCRIPTION_EXECUTIONGRAPHGUI_SERIALIZATION_H_
