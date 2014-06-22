// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: localURIQuery.proto

#ifndef PROTOBUF_localURIQuery_2eproto__INCLUDED
#define PROTOBUF_localURIQuery_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_localURIQuery_2eproto();
void protobuf_AssignDesc_localURIQuery_2eproto();
void protobuf_ShutdownFile_localURIQuery_2eproto();

class localURIQuery;
class requiredIntegerCondition;
class requiredDoubleCondition;

enum comparisonOperator {
  LESS_THAN = 0,
  LESS_THAN_OR_EQUAL = 1,
  GREATER_THAN = 2,
  GREATER_THAN_OR_EQUAL = 3,
  EQUAL = 4
};
bool comparisonOperator_IsValid(int value);
const comparisonOperator comparisonOperator_MIN = LESS_THAN;
const comparisonOperator comparisonOperator_MAX = EQUAL;
const int comparisonOperator_ARRAYSIZE = comparisonOperator_MAX + 1;

const ::google::protobuf::EnumDescriptor* comparisonOperator_descriptor();
inline const ::std::string& comparisonOperator_Name(comparisonOperator value) {
  return ::google::protobuf::internal::NameOfEnum(
    comparisonOperator_descriptor(), value);
}
inline bool comparisonOperator_Parse(
    const ::std::string& name, comparisonOperator* value) {
  return ::google::protobuf::internal::ParseNamedEnum<comparisonOperator>(
    comparisonOperator_descriptor(), name, value);
}
// ===================================================================

class localURIQuery : public ::google::protobuf::Message {
 public:
  localURIQuery();
  virtual ~localURIQuery();

  localURIQuery(const localURIQuery& from);

  inline localURIQuery& operator=(const localURIQuery& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const localURIQuery& default_instance();

  void Swap(localURIQuery* other);

  // implements Message ----------------------------------------------

  localURIQuery* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const localURIQuery& from);
  void MergeFrom(const localURIQuery& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated string requiredTags = 1;
  inline int requiredtags_size() const;
  inline void clear_requiredtags();
  static const int kRequiredTagsFieldNumber = 1;
  inline const ::std::string& requiredtags(int index) const;
  inline ::std::string* mutable_requiredtags(int index);
  inline void set_requiredtags(int index, const ::std::string& value);
  inline void set_requiredtags(int index, const char* value);
  inline void set_requiredtags(int index, const char* value, size_t size);
  inline ::std::string* add_requiredtags();
  inline void add_requiredtags(const ::std::string& value);
  inline void add_requiredtags(const char* value);
  inline void add_requiredtags(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& requiredtags() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_requiredtags();

  // repeated string forbiddenTags = 2;
  inline int forbiddentags_size() const;
  inline void clear_forbiddentags();
  static const int kForbiddenTagsFieldNumber = 2;
  inline const ::std::string& forbiddentags(int index) const;
  inline ::std::string* mutable_forbiddentags(int index);
  inline void set_forbiddentags(int index, const ::std::string& value);
  inline void set_forbiddentags(int index, const char* value);
  inline void set_forbiddentags(int index, const char* value, size_t size);
  inline ::std::string* add_forbiddentags();
  inline void add_forbiddentags(const ::std::string& value);
  inline void add_forbiddentags(const char* value);
  inline void add_forbiddentags(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& forbiddentags() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_forbiddentags();

  // repeated .requiredIntegerCondition integerConditions = 3;
  inline int integerconditions_size() const;
  inline void clear_integerconditions();
  static const int kIntegerConditionsFieldNumber = 3;
  inline const ::requiredIntegerCondition& integerconditions(int index) const;
  inline ::requiredIntegerCondition* mutable_integerconditions(int index);
  inline ::requiredIntegerCondition* add_integerconditions();
  inline const ::google::protobuf::RepeatedPtrField< ::requiredIntegerCondition >&
      integerconditions() const;
  inline ::google::protobuf::RepeatedPtrField< ::requiredIntegerCondition >*
      mutable_integerconditions();

  // repeated .requiredDoubleCondition doubleConditions = 4;
  inline int doubleconditions_size() const;
  inline void clear_doubleconditions();
  static const int kDoubleConditionsFieldNumber = 4;
  inline const ::requiredDoubleCondition& doubleconditions(int index) const;
  inline ::requiredDoubleCondition* mutable_doubleconditions(int index);
  inline ::requiredDoubleCondition* add_doubleconditions();
  inline const ::google::protobuf::RepeatedPtrField< ::requiredDoubleCondition >&
      doubleconditions() const;
  inline ::google::protobuf::RepeatedPtrField< ::requiredDoubleCondition >*
      mutable_doubleconditions();

  // @@protoc_insertion_point(class_scope:localURIQuery)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::std::string> requiredtags_;
  ::google::protobuf::RepeatedPtrField< ::std::string> forbiddentags_;
  ::google::protobuf::RepeatedPtrField< ::requiredIntegerCondition > integerconditions_;
  ::google::protobuf::RepeatedPtrField< ::requiredDoubleCondition > doubleconditions_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];

  friend void  protobuf_AddDesc_localURIQuery_2eproto();
  friend void protobuf_AssignDesc_localURIQuery_2eproto();
  friend void protobuf_ShutdownFile_localURIQuery_2eproto();

  void InitAsDefaultInstance();
  static localURIQuery* default_instance_;
};
// -------------------------------------------------------------------

class requiredIntegerCondition : public ::google::protobuf::Message {
 public:
  requiredIntegerCondition();
  virtual ~requiredIntegerCondition();

  requiredIntegerCondition(const requiredIntegerCondition& from);

  inline requiredIntegerCondition& operator=(const requiredIntegerCondition& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const requiredIntegerCondition& default_instance();

  void Swap(requiredIntegerCondition* other);

  // implements Message ----------------------------------------------

  requiredIntegerCondition* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const requiredIntegerCondition& from);
  void MergeFrom(const requiredIntegerCondition& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string field = 1;
  inline bool has_field() const;
  inline void clear_field();
  static const int kFieldFieldNumber = 1;
  inline const ::std::string& field() const;
  inline void set_field(const ::std::string& value);
  inline void set_field(const char* value);
  inline void set_field(const char* value, size_t size);
  inline ::std::string* mutable_field();
  inline ::std::string* release_field();
  inline void set_allocated_field(::std::string* field);

  // required .comparisonOperator comparison = 2;
  inline bool has_comparison() const;
  inline void clear_comparison();
  static const int kComparisonFieldNumber = 2;
  inline ::comparisonOperator comparison() const;
  inline void set_comparison(::comparisonOperator value);

  // required int32 value = 3;
  inline bool has_value() const;
  inline void clear_value();
  static const int kValueFieldNumber = 3;
  inline ::google::protobuf::int32 value() const;
  inline void set_value(::google::protobuf::int32 value);

  // optional bool comparisonResultOnNULL = 4 [default = false];
  inline bool has_comparisonresultonnull() const;
  inline void clear_comparisonresultonnull();
  static const int kComparisonResultOnNULLFieldNumber = 4;
  inline bool comparisonresultonnull() const;
  inline void set_comparisonresultonnull(bool value);

  // @@protoc_insertion_point(class_scope:requiredIntegerCondition)
 private:
  inline void set_has_field();
  inline void clear_has_field();
  inline void set_has_comparison();
  inline void clear_has_comparison();
  inline void set_has_value();
  inline void clear_has_value();
  inline void set_has_comparisonresultonnull();
  inline void clear_has_comparisonresultonnull();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* field_;
  int comparison_;
  ::google::protobuf::int32 value_;
  bool comparisonresultonnull_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];

  friend void  protobuf_AddDesc_localURIQuery_2eproto();
  friend void protobuf_AssignDesc_localURIQuery_2eproto();
  friend void protobuf_ShutdownFile_localURIQuery_2eproto();

  void InitAsDefaultInstance();
  static requiredIntegerCondition* default_instance_;
};
// -------------------------------------------------------------------

class requiredDoubleCondition : public ::google::protobuf::Message {
 public:
  requiredDoubleCondition();
  virtual ~requiredDoubleCondition();

  requiredDoubleCondition(const requiredDoubleCondition& from);

  inline requiredDoubleCondition& operator=(const requiredDoubleCondition& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const requiredDoubleCondition& default_instance();

  void Swap(requiredDoubleCondition* other);

  // implements Message ----------------------------------------------

  requiredDoubleCondition* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const requiredDoubleCondition& from);
  void MergeFrom(const requiredDoubleCondition& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string field = 1;
  inline bool has_field() const;
  inline void clear_field();
  static const int kFieldFieldNumber = 1;
  inline const ::std::string& field() const;
  inline void set_field(const ::std::string& value);
  inline void set_field(const char* value);
  inline void set_field(const char* value, size_t size);
  inline ::std::string* mutable_field();
  inline ::std::string* release_field();
  inline void set_allocated_field(::std::string* field);

  // required .comparisonOperator comparison = 2;
  inline bool has_comparison() const;
  inline void clear_comparison();
  static const int kComparisonFieldNumber = 2;
  inline ::comparisonOperator comparison() const;
  inline void set_comparison(::comparisonOperator value);

  // required double value = 3;
  inline bool has_value() const;
  inline void clear_value();
  static const int kValueFieldNumber = 3;
  inline double value() const;
  inline void set_value(double value);

  // optional bool comparisonResultOnNULL = 4 [default = false];
  inline bool has_comparisonresultonnull() const;
  inline void clear_comparisonresultonnull();
  static const int kComparisonResultOnNULLFieldNumber = 4;
  inline bool comparisonresultonnull() const;
  inline void set_comparisonresultonnull(bool value);

  // @@protoc_insertion_point(class_scope:requiredDoubleCondition)
 private:
  inline void set_has_field();
  inline void clear_has_field();
  inline void set_has_comparison();
  inline void clear_has_comparison();
  inline void set_has_value();
  inline void clear_has_value();
  inline void set_has_comparisonresultonnull();
  inline void clear_has_comparisonresultonnull();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* field_;
  double value_;
  int comparison_;
  bool comparisonresultonnull_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];

  friend void  protobuf_AddDesc_localURIQuery_2eproto();
  friend void protobuf_AssignDesc_localURIQuery_2eproto();
  friend void protobuf_ShutdownFile_localURIQuery_2eproto();

  void InitAsDefaultInstance();
  static requiredDoubleCondition* default_instance_;
};
// ===================================================================


// ===================================================================

// localURIQuery

// repeated string requiredTags = 1;
inline int localURIQuery::requiredtags_size() const {
  return requiredtags_.size();
}
inline void localURIQuery::clear_requiredtags() {
  requiredtags_.Clear();
}
inline const ::std::string& localURIQuery::requiredtags(int index) const {
  return requiredtags_.Get(index);
}
inline ::std::string* localURIQuery::mutable_requiredtags(int index) {
  return requiredtags_.Mutable(index);
}
inline void localURIQuery::set_requiredtags(int index, const ::std::string& value) {
  requiredtags_.Mutable(index)->assign(value);
}
inline void localURIQuery::set_requiredtags(int index, const char* value) {
  requiredtags_.Mutable(index)->assign(value);
}
inline void localURIQuery::set_requiredtags(int index, const char* value, size_t size) {
  requiredtags_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* localURIQuery::add_requiredtags() {
  return requiredtags_.Add();
}
inline void localURIQuery::add_requiredtags(const ::std::string& value) {
  requiredtags_.Add()->assign(value);
}
inline void localURIQuery::add_requiredtags(const char* value) {
  requiredtags_.Add()->assign(value);
}
inline void localURIQuery::add_requiredtags(const char* value, size_t size) {
  requiredtags_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
localURIQuery::requiredtags() const {
  return requiredtags_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
localURIQuery::mutable_requiredtags() {
  return &requiredtags_;
}

// repeated string forbiddenTags = 2;
inline int localURIQuery::forbiddentags_size() const {
  return forbiddentags_.size();
}
inline void localURIQuery::clear_forbiddentags() {
  forbiddentags_.Clear();
}
inline const ::std::string& localURIQuery::forbiddentags(int index) const {
  return forbiddentags_.Get(index);
}
inline ::std::string* localURIQuery::mutable_forbiddentags(int index) {
  return forbiddentags_.Mutable(index);
}
inline void localURIQuery::set_forbiddentags(int index, const ::std::string& value) {
  forbiddentags_.Mutable(index)->assign(value);
}
inline void localURIQuery::set_forbiddentags(int index, const char* value) {
  forbiddentags_.Mutable(index)->assign(value);
}
inline void localURIQuery::set_forbiddentags(int index, const char* value, size_t size) {
  forbiddentags_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* localURIQuery::add_forbiddentags() {
  return forbiddentags_.Add();
}
inline void localURIQuery::add_forbiddentags(const ::std::string& value) {
  forbiddentags_.Add()->assign(value);
}
inline void localURIQuery::add_forbiddentags(const char* value) {
  forbiddentags_.Add()->assign(value);
}
inline void localURIQuery::add_forbiddentags(const char* value, size_t size) {
  forbiddentags_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
localURIQuery::forbiddentags() const {
  return forbiddentags_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
localURIQuery::mutable_forbiddentags() {
  return &forbiddentags_;
}

// repeated .requiredIntegerCondition integerConditions = 3;
inline int localURIQuery::integerconditions_size() const {
  return integerconditions_.size();
}
inline void localURIQuery::clear_integerconditions() {
  integerconditions_.Clear();
}
inline const ::requiredIntegerCondition& localURIQuery::integerconditions(int index) const {
  return integerconditions_.Get(index);
}
inline ::requiredIntegerCondition* localURIQuery::mutable_integerconditions(int index) {
  return integerconditions_.Mutable(index);
}
inline ::requiredIntegerCondition* localURIQuery::add_integerconditions() {
  return integerconditions_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::requiredIntegerCondition >&
localURIQuery::integerconditions() const {
  return integerconditions_;
}
inline ::google::protobuf::RepeatedPtrField< ::requiredIntegerCondition >*
localURIQuery::mutable_integerconditions() {
  return &integerconditions_;
}

// repeated .requiredDoubleCondition doubleConditions = 4;
inline int localURIQuery::doubleconditions_size() const {
  return doubleconditions_.size();
}
inline void localURIQuery::clear_doubleconditions() {
  doubleconditions_.Clear();
}
inline const ::requiredDoubleCondition& localURIQuery::doubleconditions(int index) const {
  return doubleconditions_.Get(index);
}
inline ::requiredDoubleCondition* localURIQuery::mutable_doubleconditions(int index) {
  return doubleconditions_.Mutable(index);
}
inline ::requiredDoubleCondition* localURIQuery::add_doubleconditions() {
  return doubleconditions_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::requiredDoubleCondition >&
localURIQuery::doubleconditions() const {
  return doubleconditions_;
}
inline ::google::protobuf::RepeatedPtrField< ::requiredDoubleCondition >*
localURIQuery::mutable_doubleconditions() {
  return &doubleconditions_;
}

// -------------------------------------------------------------------

// requiredIntegerCondition

// required string field = 1;
inline bool requiredIntegerCondition::has_field() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void requiredIntegerCondition::set_has_field() {
  _has_bits_[0] |= 0x00000001u;
}
inline void requiredIntegerCondition::clear_has_field() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void requiredIntegerCondition::clear_field() {
  if (field_ != &::google::protobuf::internal::kEmptyString) {
    field_->clear();
  }
  clear_has_field();
}
inline const ::std::string& requiredIntegerCondition::field() const {
  return *field_;
}
inline void requiredIntegerCondition::set_field(const ::std::string& value) {
  set_has_field();
  if (field_ == &::google::protobuf::internal::kEmptyString) {
    field_ = new ::std::string;
  }
  field_->assign(value);
}
inline void requiredIntegerCondition::set_field(const char* value) {
  set_has_field();
  if (field_ == &::google::protobuf::internal::kEmptyString) {
    field_ = new ::std::string;
  }
  field_->assign(value);
}
inline void requiredIntegerCondition::set_field(const char* value, size_t size) {
  set_has_field();
  if (field_ == &::google::protobuf::internal::kEmptyString) {
    field_ = new ::std::string;
  }
  field_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* requiredIntegerCondition::mutable_field() {
  set_has_field();
  if (field_ == &::google::protobuf::internal::kEmptyString) {
    field_ = new ::std::string;
  }
  return field_;
}
inline ::std::string* requiredIntegerCondition::release_field() {
  clear_has_field();
  if (field_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = field_;
    field_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void requiredIntegerCondition::set_allocated_field(::std::string* field) {
  if (field_ != &::google::protobuf::internal::kEmptyString) {
    delete field_;
  }
  if (field) {
    set_has_field();
    field_ = field;
  } else {
    clear_has_field();
    field_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required .comparisonOperator comparison = 2;
inline bool requiredIntegerCondition::has_comparison() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void requiredIntegerCondition::set_has_comparison() {
  _has_bits_[0] |= 0x00000002u;
}
inline void requiredIntegerCondition::clear_has_comparison() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void requiredIntegerCondition::clear_comparison() {
  comparison_ = 0;
  clear_has_comparison();
}
inline ::comparisonOperator requiredIntegerCondition::comparison() const {
  return static_cast< ::comparisonOperator >(comparison_);
}
inline void requiredIntegerCondition::set_comparison(::comparisonOperator value) {
  assert(::comparisonOperator_IsValid(value));
  set_has_comparison();
  comparison_ = value;
}

// required int32 value = 3;
inline bool requiredIntegerCondition::has_value() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void requiredIntegerCondition::set_has_value() {
  _has_bits_[0] |= 0x00000004u;
}
inline void requiredIntegerCondition::clear_has_value() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void requiredIntegerCondition::clear_value() {
  value_ = 0;
  clear_has_value();
}
inline ::google::protobuf::int32 requiredIntegerCondition::value() const {
  return value_;
}
inline void requiredIntegerCondition::set_value(::google::protobuf::int32 value) {
  set_has_value();
  value_ = value;
}

// optional bool comparisonResultOnNULL = 4 [default = false];
inline bool requiredIntegerCondition::has_comparisonresultonnull() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void requiredIntegerCondition::set_has_comparisonresultonnull() {
  _has_bits_[0] |= 0x00000008u;
}
inline void requiredIntegerCondition::clear_has_comparisonresultonnull() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void requiredIntegerCondition::clear_comparisonresultonnull() {
  comparisonresultonnull_ = false;
  clear_has_comparisonresultonnull();
}
inline bool requiredIntegerCondition::comparisonresultonnull() const {
  return comparisonresultonnull_;
}
inline void requiredIntegerCondition::set_comparisonresultonnull(bool value) {
  set_has_comparisonresultonnull();
  comparisonresultonnull_ = value;
}

// -------------------------------------------------------------------

// requiredDoubleCondition

// required string field = 1;
inline bool requiredDoubleCondition::has_field() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void requiredDoubleCondition::set_has_field() {
  _has_bits_[0] |= 0x00000001u;
}
inline void requiredDoubleCondition::clear_has_field() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void requiredDoubleCondition::clear_field() {
  if (field_ != &::google::protobuf::internal::kEmptyString) {
    field_->clear();
  }
  clear_has_field();
}
inline const ::std::string& requiredDoubleCondition::field() const {
  return *field_;
}
inline void requiredDoubleCondition::set_field(const ::std::string& value) {
  set_has_field();
  if (field_ == &::google::protobuf::internal::kEmptyString) {
    field_ = new ::std::string;
  }
  field_->assign(value);
}
inline void requiredDoubleCondition::set_field(const char* value) {
  set_has_field();
  if (field_ == &::google::protobuf::internal::kEmptyString) {
    field_ = new ::std::string;
  }
  field_->assign(value);
}
inline void requiredDoubleCondition::set_field(const char* value, size_t size) {
  set_has_field();
  if (field_ == &::google::protobuf::internal::kEmptyString) {
    field_ = new ::std::string;
  }
  field_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* requiredDoubleCondition::mutable_field() {
  set_has_field();
  if (field_ == &::google::protobuf::internal::kEmptyString) {
    field_ = new ::std::string;
  }
  return field_;
}
inline ::std::string* requiredDoubleCondition::release_field() {
  clear_has_field();
  if (field_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = field_;
    field_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void requiredDoubleCondition::set_allocated_field(::std::string* field) {
  if (field_ != &::google::protobuf::internal::kEmptyString) {
    delete field_;
  }
  if (field) {
    set_has_field();
    field_ = field;
  } else {
    clear_has_field();
    field_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required .comparisonOperator comparison = 2;
inline bool requiredDoubleCondition::has_comparison() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void requiredDoubleCondition::set_has_comparison() {
  _has_bits_[0] |= 0x00000002u;
}
inline void requiredDoubleCondition::clear_has_comparison() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void requiredDoubleCondition::clear_comparison() {
  comparison_ = 0;
  clear_has_comparison();
}
inline ::comparisonOperator requiredDoubleCondition::comparison() const {
  return static_cast< ::comparisonOperator >(comparison_);
}
inline void requiredDoubleCondition::set_comparison(::comparisonOperator value) {
  assert(::comparisonOperator_IsValid(value));
  set_has_comparison();
  comparison_ = value;
}

// required double value = 3;
inline bool requiredDoubleCondition::has_value() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void requiredDoubleCondition::set_has_value() {
  _has_bits_[0] |= 0x00000004u;
}
inline void requiredDoubleCondition::clear_has_value() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void requiredDoubleCondition::clear_value() {
  value_ = 0;
  clear_has_value();
}
inline double requiredDoubleCondition::value() const {
  return value_;
}
inline void requiredDoubleCondition::set_value(double value) {
  set_has_value();
  value_ = value;
}

// optional bool comparisonResultOnNULL = 4 [default = false];
inline bool requiredDoubleCondition::has_comparisonresultonnull() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void requiredDoubleCondition::set_has_comparisonresultonnull() {
  _has_bits_[0] |= 0x00000008u;
}
inline void requiredDoubleCondition::clear_has_comparisonresultonnull() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void requiredDoubleCondition::clear_comparisonresultonnull() {
  comparisonresultonnull_ = false;
  clear_has_comparisonresultonnull();
}
inline bool requiredDoubleCondition::comparisonresultonnull() const {
  return comparisonresultonnull_;
}
inline void requiredDoubleCondition::set_comparisonresultonnull(bool value) {
  set_has_comparisonresultonnull();
  comparisonresultonnull_ = value;
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::comparisonOperator>() {
  return ::comparisonOperator_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_localURIQuery_2eproto__INCLUDED
