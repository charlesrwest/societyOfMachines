// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: localURI.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "localURI.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* localURI_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  localURI_reflection_ = NULL;
const ::google::protobuf::Descriptor* integerKeyValuePair_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  integerKeyValuePair_reflection_ = NULL;
const ::google::protobuf::Descriptor* doubleKeyValuePair_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  doubleKeyValuePair_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_localURI_2eproto() {
  protobuf_AddDesc_localURI_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "localURI.proto");
  GOOGLE_CHECK(file != NULL);
  localURI_descriptor_ = file->message_type(0);
  static const int localURI_offsets_[6] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(localURI, resourcename_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(localURI, ipcpath_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(localURI, originatingprocessid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(localURI, tags_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(localURI, integerpairs_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(localURI, doublepairs_),
  };
  localURI_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      localURI_descriptor_,
      localURI::default_instance_,
      localURI_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(localURI, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(localURI, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(localURI));
  integerKeyValuePair_descriptor_ = file->message_type(1);
  static const int integerKeyValuePair_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(integerKeyValuePair, key_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(integerKeyValuePair, value_),
  };
  integerKeyValuePair_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      integerKeyValuePair_descriptor_,
      integerKeyValuePair::default_instance_,
      integerKeyValuePair_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(integerKeyValuePair, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(integerKeyValuePair, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(integerKeyValuePair));
  doubleKeyValuePair_descriptor_ = file->message_type(2);
  static const int doubleKeyValuePair_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(doubleKeyValuePair, key_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(doubleKeyValuePair, value_),
  };
  doubleKeyValuePair_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      doubleKeyValuePair_descriptor_,
      doubleKeyValuePair::default_instance_,
      doubleKeyValuePair_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(doubleKeyValuePair, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(doubleKeyValuePair, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(doubleKeyValuePair));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_localURI_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    localURI_descriptor_, &localURI::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    integerKeyValuePair_descriptor_, &integerKeyValuePair::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    doubleKeyValuePair_descriptor_, &doubleKeyValuePair::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_localURI_2eproto() {
  delete localURI::default_instance_;
  delete localURI_reflection_;
  delete integerKeyValuePair::default_instance_;
  delete integerKeyValuePair_reflection_;
  delete doubleKeyValuePair::default_instance_;
  delete doubleKeyValuePair_reflection_;
}

void protobuf_AddDesc_localURI_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\016localURI.proto\"\263\001\n\010localURI\022\024\n\014resourc"
    "eName\030\001 \002(\t\022\017\n\007IPCPath\030\002 \002(\t\022\034\n\024originat"
    "ingProcessID\030\003 \001(\004\022\014\n\004tags\030\004 \003(\t\022*\n\014inte"
    "gerPairs\030\005 \003(\0132\024.integerKeyValuePair\022(\n\013"
    "doublePairs\030\006 \003(\0132\023.doubleKeyValuePair\"1"
    "\n\023integerKeyValuePair\022\013\n\003key\030\001 \002(\t\022\r\n\005va"
    "lue\030\002 \002(\005\"0\n\022doubleKeyValuePair\022\013\n\003key\030\001"
    " \002(\t\022\r\n\005value\030\002 \002(\001", 299);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "localURI.proto", &protobuf_RegisterTypes);
  localURI::default_instance_ = new localURI();
  integerKeyValuePair::default_instance_ = new integerKeyValuePair();
  doubleKeyValuePair::default_instance_ = new doubleKeyValuePair();
  localURI::default_instance_->InitAsDefaultInstance();
  integerKeyValuePair::default_instance_->InitAsDefaultInstance();
  doubleKeyValuePair::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_localURI_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_localURI_2eproto {
  StaticDescriptorInitializer_localURI_2eproto() {
    protobuf_AddDesc_localURI_2eproto();
  }
} static_descriptor_initializer_localURI_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int localURI::kResourceNameFieldNumber;
const int localURI::kIPCPathFieldNumber;
const int localURI::kOriginatingProcessIDFieldNumber;
const int localURI::kTagsFieldNumber;
const int localURI::kIntegerPairsFieldNumber;
const int localURI::kDoublePairsFieldNumber;
#endif  // !_MSC_VER

localURI::localURI()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void localURI::InitAsDefaultInstance() {
}

localURI::localURI(const localURI& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void localURI::SharedCtor() {
  _cached_size_ = 0;
  resourcename_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ipcpath_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  originatingprocessid_ = GOOGLE_ULONGLONG(0);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

localURI::~localURI() {
  SharedDtor();
}

void localURI::SharedDtor() {
  if (resourcename_ != &::google::protobuf::internal::kEmptyString) {
    delete resourcename_;
  }
  if (ipcpath_ != &::google::protobuf::internal::kEmptyString) {
    delete ipcpath_;
  }
  if (this != default_instance_) {
  }
}

void localURI::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* localURI::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return localURI_descriptor_;
}

const localURI& localURI::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_localURI_2eproto();
  return *default_instance_;
}

localURI* localURI::default_instance_ = NULL;

localURI* localURI::New() const {
  return new localURI;
}

void localURI::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_resourcename()) {
      if (resourcename_ != &::google::protobuf::internal::kEmptyString) {
        resourcename_->clear();
      }
    }
    if (has_ipcpath()) {
      if (ipcpath_ != &::google::protobuf::internal::kEmptyString) {
        ipcpath_->clear();
      }
    }
    originatingprocessid_ = GOOGLE_ULONGLONG(0);
  }
  tags_.Clear();
  integerpairs_.Clear();
  doublepairs_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool localURI::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string resourceName = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_resourcename()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->resourcename().data(), this->resourcename().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_IPCPath;
        break;
      }

      // required string IPCPath = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_IPCPath:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_ipcpath()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->ipcpath().data(), this->ipcpath().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_originatingProcessID;
        break;
      }

      // optional uint64 originatingProcessID = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_originatingProcessID:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &originatingprocessid_)));
          set_has_originatingprocessid();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_tags;
        break;
      }

      // repeated string tags = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_tags:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_tags()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->tags(this->tags_size() - 1).data(),
            this->tags(this->tags_size() - 1).length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_tags;
        if (input->ExpectTag(42)) goto parse_integerPairs;
        break;
      }

      // repeated .integerKeyValuePair integerPairs = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_integerPairs:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_integerpairs()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(42)) goto parse_integerPairs;
        if (input->ExpectTag(50)) goto parse_doublePairs;
        break;
      }

      // repeated .doubleKeyValuePair doublePairs = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_doublePairs:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_doublepairs()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(50)) goto parse_doublePairs;
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void localURI::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string resourceName = 1;
  if (has_resourcename()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->resourcename().data(), this->resourcename().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->resourcename(), output);
  }

  // required string IPCPath = 2;
  if (has_ipcpath()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->ipcpath().data(), this->ipcpath().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->ipcpath(), output);
  }

  // optional uint64 originatingProcessID = 3;
  if (has_originatingprocessid()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(3, this->originatingprocessid(), output);
  }

  // repeated string tags = 4;
  for (int i = 0; i < this->tags_size(); i++) {
  ::google::protobuf::internal::WireFormat::VerifyUTF8String(
    this->tags(i).data(), this->tags(i).length(),
    ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      4, this->tags(i), output);
  }

  // repeated .integerKeyValuePair integerPairs = 5;
  for (int i = 0; i < this->integerpairs_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      5, this->integerpairs(i), output);
  }

  // repeated .doubleKeyValuePair doublePairs = 6;
  for (int i = 0; i < this->doublepairs_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      6, this->doublepairs(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* localURI::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string resourceName = 1;
  if (has_resourcename()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->resourcename().data(), this->resourcename().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->resourcename(), target);
  }

  // required string IPCPath = 2;
  if (has_ipcpath()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->ipcpath().data(), this->ipcpath().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->ipcpath(), target);
  }

  // optional uint64 originatingProcessID = 3;
  if (has_originatingprocessid()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(3, this->originatingprocessid(), target);
  }

  // repeated string tags = 4;
  for (int i = 0; i < this->tags_size(); i++) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->tags(i).data(), this->tags(i).length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(4, this->tags(i), target);
  }

  // repeated .integerKeyValuePair integerPairs = 5;
  for (int i = 0; i < this->integerpairs_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        5, this->integerpairs(i), target);
  }

  // repeated .doubleKeyValuePair doublePairs = 6;
  for (int i = 0; i < this->doublepairs_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        6, this->doublepairs(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int localURI::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string resourceName = 1;
    if (has_resourcename()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->resourcename());
    }

    // required string IPCPath = 2;
    if (has_ipcpath()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->ipcpath());
    }

    // optional uint64 originatingProcessID = 3;
    if (has_originatingprocessid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->originatingprocessid());
    }

  }
  // repeated string tags = 4;
  total_size += 1 * this->tags_size();
  for (int i = 0; i < this->tags_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->tags(i));
  }

  // repeated .integerKeyValuePair integerPairs = 5;
  total_size += 1 * this->integerpairs_size();
  for (int i = 0; i < this->integerpairs_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->integerpairs(i));
  }

  // repeated .doubleKeyValuePair doublePairs = 6;
  total_size += 1 * this->doublepairs_size();
  for (int i = 0; i < this->doublepairs_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->doublepairs(i));
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void localURI::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const localURI* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const localURI*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void localURI::MergeFrom(const localURI& from) {
  GOOGLE_CHECK_NE(&from, this);
  tags_.MergeFrom(from.tags_);
  integerpairs_.MergeFrom(from.integerpairs_);
  doublepairs_.MergeFrom(from.doublepairs_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_resourcename()) {
      set_resourcename(from.resourcename());
    }
    if (from.has_ipcpath()) {
      set_ipcpath(from.ipcpath());
    }
    if (from.has_originatingprocessid()) {
      set_originatingprocessid(from.originatingprocessid());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void localURI::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void localURI::CopyFrom(const localURI& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool localURI::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;

  for (int i = 0; i < integerpairs_size(); i++) {
    if (!this->integerpairs(i).IsInitialized()) return false;
  }
  for (int i = 0; i < doublepairs_size(); i++) {
    if (!this->doublepairs(i).IsInitialized()) return false;
  }
  return true;
}

void localURI::Swap(localURI* other) {
  if (other != this) {
    std::swap(resourcename_, other->resourcename_);
    std::swap(ipcpath_, other->ipcpath_);
    std::swap(originatingprocessid_, other->originatingprocessid_);
    tags_.Swap(&other->tags_);
    integerpairs_.Swap(&other->integerpairs_);
    doublepairs_.Swap(&other->doublepairs_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata localURI::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = localURI_descriptor_;
  metadata.reflection = localURI_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int integerKeyValuePair::kKeyFieldNumber;
const int integerKeyValuePair::kValueFieldNumber;
#endif  // !_MSC_VER

integerKeyValuePair::integerKeyValuePair()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void integerKeyValuePair::InitAsDefaultInstance() {
}

integerKeyValuePair::integerKeyValuePair(const integerKeyValuePair& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void integerKeyValuePair::SharedCtor() {
  _cached_size_ = 0;
  key_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  value_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

integerKeyValuePair::~integerKeyValuePair() {
  SharedDtor();
}

void integerKeyValuePair::SharedDtor() {
  if (key_ != &::google::protobuf::internal::kEmptyString) {
    delete key_;
  }
  if (this != default_instance_) {
  }
}

void integerKeyValuePair::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* integerKeyValuePair::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return integerKeyValuePair_descriptor_;
}

const integerKeyValuePair& integerKeyValuePair::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_localURI_2eproto();
  return *default_instance_;
}

integerKeyValuePair* integerKeyValuePair::default_instance_ = NULL;

integerKeyValuePair* integerKeyValuePair::New() const {
  return new integerKeyValuePair;
}

void integerKeyValuePair::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_key()) {
      if (key_ != &::google::protobuf::internal::kEmptyString) {
        key_->clear();
      }
    }
    value_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool integerKeyValuePair::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string key = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_key()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->key().data(), this->key().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_value;
        break;
      }

      // required int32 value = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_value:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &value_)));
          set_has_value();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void integerKeyValuePair::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string key = 1;
  if (has_key()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->key().data(), this->key().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->key(), output);
  }

  // required int32 value = 2;
  if (has_value()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->value(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* integerKeyValuePair::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string key = 1;
  if (has_key()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->key().data(), this->key().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->key(), target);
  }

  // required int32 value = 2;
  if (has_value()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->value(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int integerKeyValuePair::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string key = 1;
    if (has_key()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->key());
    }

    // required int32 value = 2;
    if (has_value()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->value());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void integerKeyValuePair::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const integerKeyValuePair* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const integerKeyValuePair*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void integerKeyValuePair::MergeFrom(const integerKeyValuePair& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_key()) {
      set_key(from.key());
    }
    if (from.has_value()) {
      set_value(from.value());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void integerKeyValuePair::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void integerKeyValuePair::CopyFrom(const integerKeyValuePair& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool integerKeyValuePair::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;

  return true;
}

void integerKeyValuePair::Swap(integerKeyValuePair* other) {
  if (other != this) {
    std::swap(key_, other->key_);
    std::swap(value_, other->value_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata integerKeyValuePair::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = integerKeyValuePair_descriptor_;
  metadata.reflection = integerKeyValuePair_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int doubleKeyValuePair::kKeyFieldNumber;
const int doubleKeyValuePair::kValueFieldNumber;
#endif  // !_MSC_VER

doubleKeyValuePair::doubleKeyValuePair()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void doubleKeyValuePair::InitAsDefaultInstance() {
}

doubleKeyValuePair::doubleKeyValuePair(const doubleKeyValuePair& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void doubleKeyValuePair::SharedCtor() {
  _cached_size_ = 0;
  key_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  value_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

doubleKeyValuePair::~doubleKeyValuePair() {
  SharedDtor();
}

void doubleKeyValuePair::SharedDtor() {
  if (key_ != &::google::protobuf::internal::kEmptyString) {
    delete key_;
  }
  if (this != default_instance_) {
  }
}

void doubleKeyValuePair::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* doubleKeyValuePair::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return doubleKeyValuePair_descriptor_;
}

const doubleKeyValuePair& doubleKeyValuePair::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_localURI_2eproto();
  return *default_instance_;
}

doubleKeyValuePair* doubleKeyValuePair::default_instance_ = NULL;

doubleKeyValuePair* doubleKeyValuePair::New() const {
  return new doubleKeyValuePair;
}

void doubleKeyValuePair::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_key()) {
      if (key_ != &::google::protobuf::internal::kEmptyString) {
        key_->clear();
      }
    }
    value_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool doubleKeyValuePair::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string key = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_key()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->key().data(), this->key().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(17)) goto parse_value;
        break;
      }

      // required double value = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
         parse_value:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   double, ::google::protobuf::internal::WireFormatLite::TYPE_DOUBLE>(
                 input, &value_)));
          set_has_value();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void doubleKeyValuePair::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string key = 1;
  if (has_key()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->key().data(), this->key().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->key(), output);
  }

  // required double value = 2;
  if (has_value()) {
    ::google::protobuf::internal::WireFormatLite::WriteDouble(2, this->value(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* doubleKeyValuePair::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string key = 1;
  if (has_key()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->key().data(), this->key().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->key(), target);
  }

  // required double value = 2;
  if (has_value()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteDoubleToArray(2, this->value(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int doubleKeyValuePair::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string key = 1;
    if (has_key()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->key());
    }

    // required double value = 2;
    if (has_value()) {
      total_size += 1 + 8;
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void doubleKeyValuePair::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const doubleKeyValuePair* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const doubleKeyValuePair*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void doubleKeyValuePair::MergeFrom(const doubleKeyValuePair& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_key()) {
      set_key(from.key());
    }
    if (from.has_value()) {
      set_value(from.value());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void doubleKeyValuePair::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void doubleKeyValuePair::CopyFrom(const doubleKeyValuePair& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool doubleKeyValuePair::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;

  return true;
}

void doubleKeyValuePair::Swap(doubleKeyValuePair* other) {
  if (other != this) {
    std::swap(key_, other->key_);
    std::swap(value_, other->value_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata doubleKeyValuePair::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = doubleKeyValuePair_descriptor_;
  metadata.reflection = doubleKeyValuePair_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
