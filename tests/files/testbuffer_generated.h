// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_TESTBUFFER_TEST_H_
#define FLATBUFFERS_GENERATED_TESTBUFFER_TEST_H_

#include "flatbuffers/flatbuffers.h"

namespace test {

struct Vec3;

struct Test;

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) Vec3 FLATBUFFERS_FINAL_CLASS {
 private:
  float x_;
  float y_;
  float z_;

 public:
  Vec3() {
    memset(this, 0, sizeof(Vec3));
  }
  Vec3(float _x, float _y, float _z)
      : x_(flatbuffers::EndianScalar(_x)),
        y_(flatbuffers::EndianScalar(_y)),
        z_(flatbuffers::EndianScalar(_z)) {
  }
  float x() const {
    return flatbuffers::EndianScalar(x_);
  }
  float y() const {
    return flatbuffers::EndianScalar(y_);
  }
  float z() const {
    return flatbuffers::EndianScalar(z_);
  }
};
FLATBUFFERS_STRUCT_END(Vec3, 12);

struct Test FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_POS = 4,
    VT_POS2 = 6
  };
  const flatbuffers::Vector<const Vec3 *> *pos() const {
    return GetPointer<const flatbuffers::Vector<const Vec3 *> *>(VT_POS);
  }
  const flatbuffers::Vector<const Vec3 *> *pos2() const {
    return GetPointer<const flatbuffers::Vector<const Vec3 *> *>(VT_POS2);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyVector(pos()) &&
           VerifyOffset(verifier, VT_POS2) &&
           verifier.VerifyVector(pos2()) &&
           verifier.EndTable();
  }
};

struct TestBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_pos(flatbuffers::Offset<flatbuffers::Vector<const Vec3 *>> pos) {
    fbb_.AddOffset(Test::VT_POS, pos);
  }
  void add_pos2(flatbuffers::Offset<flatbuffers::Vector<const Vec3 *>> pos2) {
    fbb_.AddOffset(Test::VT_POS2, pos2);
  }
  explicit TestBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  TestBuilder &operator=(const TestBuilder &);
  flatbuffers::Offset<Test> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Test>(end);
    return o;
  }
};

inline flatbuffers::Offset<Test> CreateTest(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<const Vec3 *>> pos = 0,
    flatbuffers::Offset<flatbuffers::Vector<const Vec3 *>> pos2 = 0) {
  TestBuilder builder_(_fbb);
  builder_.add_pos2(pos2);
  builder_.add_pos(pos);
  return builder_.Finish();
}

inline flatbuffers::Offset<Test> CreateTestDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<Vec3> *pos = nullptr,
    const std::vector<Vec3> *pos2 = nullptr) {
  auto pos__ = pos ? _fbb.CreateVectorOfStructs<Vec3>(*pos) : 0;
  auto pos2__ = pos2 ? _fbb.CreateVectorOfStructs<Vec3>(*pos2) : 0;
  return test::CreateTest(
      _fbb,
      pos__,
      pos2__);
}

inline const test::Test *GetTest(const void *buf) {
  return flatbuffers::GetRoot<test::Test>(buf);
}

inline const test::Test *GetSizePrefixedTest(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<test::Test>(buf);
}

inline bool VerifyTestBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<test::Test>(nullptr);
}

inline bool VerifySizePrefixedTestBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<test::Test>(nullptr);
}

inline void FinishTestBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<test::Test> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedTestBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<test::Test> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace test

#endif  // FLATBUFFERS_GENERATED_TESTBUFFER_TEST_H_
