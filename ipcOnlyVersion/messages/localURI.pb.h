// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: localURI.proto

#ifndef PROTOBUF_localURI_2eproto__INCLUDED
#define PROTOBUF_localURI_2eproto__INCLUDED

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
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_localURI_2eproto();
void protobuf_AssignDesc_localURI_2eproto();
void protobuf_ShutdownFile_localURI_2eproto();

class localURI;
class integerKeyValuePair;
class doubleKeyValuePair;

// ===================================================================

class localURI : public ::google::protobuf::Message {
 public:
  localURI();
  virtual ~localURI();

  localURI(const localURI& from);

  inline localURI& operator=(const localURI& from) {
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
  static const localURI& default_instance();

  void Swap(localURI* other);

  // implements Message ----------------------------------------------

  localURI* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const localURI& from);
  void MergeFrom(const localURI& from);
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

  // required string resouceName = 1;
  inline bool has_resoucename() const;
  inline void clear_resoucename();
  static const int kResouceNameFieldNumber = 1;
  inline const ::std::string& resoucename() const;
  inline void set_resoucename(const ::std::string& value);
  inline void set_resoucename(const char* value);
  inline void set_resoucename(const char* value, size_t size);
  inline ::std::string* mutable_resoucename();
  inline ::std::string* release_resoucename();
  inline void set_allocated_resoucename(::std::string* resoucename);

  // required string IPCPath = 2;
  inline bool has_ipcpath() const;
  inline void clear_ipcpath();
  static const int kIPCPathFieldNumber = 2;
  inline const ::std::string& ipcpath() const;
  inline void set_ipcpath(const ::std::string& value);
  inline void set_ipcpath(const char* value);
  inline void set_ipcpath(const char* value, size_t size);
  inline ::std::string* mutable_ipcpath();
  inline ::std::string* release_ipcpath();
  inline void set_allocated_ipcpath(::std::string* ipcpath);

  // optional uint64 originatingProcessID = 3;
  inline bool has_originatingprocessid() const;
  inline void clear_originatingprocessid();
  static const int kOriginatingProcessIDFieldNumber = 3;
  inline ::google::protobuf::uint64 originatingprocessid() const;
  inline void set_originatingprocessid(::google::protobuf::uint64 value);

  // repeated string tags = 4;
  inline int tags_size() const;
  inline void clear_tags();
  static const int kTagsFieldNumber = 4;
  inline const ::std::string& tags(int index) const;
  inline ::std::string* mutable_tags(int index);
  inline void set_tags(int index, const ::std::string& value);
  inline void set_tags(int index, const char* value);
  inline void set_tags(int index, const char* value, size_t size);
  inline ::std::string* add_tags();
  inline void add_tags(const ::std::string& value);
  inline void add_tags(const char* value);
  inline void add_tags(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& tags() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_tags();

  // repeated .integerKeyValuePair integerPairs = 5;
  inline int integerpairs_size() const;
  inline void clear_integerpairs();
  static const int kIntegerPairsFieldNumber = 5;
  inline const ::integerKeyValuePair& integerpairs(int index) const;
  inline ::integerKeyValuePair* mutable_integerpairs(int index);
  inline ::integerKeyValuePair* add_integerpairs();
  inline const ::google::protobuf::RepeatedPtrField< ::integerKeyValuePair >&
      integerpairs() const;
  inline ::google::protobuf::RepeatedPtrField< ::integerKeyValuePair >*
      mutable_integerpairs();

  // repeated .doubleKeyValuePair doublePairs = 6;
  inline int doublepairs_size() const;
  inline void clear_doublepairs();
  static const int kDoublePairsFieldNumber = 6;
  inline const ::doubleKeyValuePair& doublepairs(int index) const;
  inline ::doubleKeyValuePair* mutable_doublepairs(int index);
  inline ::doubleKeyValuePair* add_doublepairs();
  inline const ::google::protobuf::RepeatedPtrField< ::doubleKeyValuePair >&
      doublepairs() const;
  inline ::google::protobuf::RepeatedPtrField< ::doubleKeyValuePair >*
      mutable_doublepairs();

  // @@protoc_insertion_point(class_scope:localURI)
 private:
  inline void set_has_resoucename();
  inline void clear_has_resoucename();
  inline void set_has_ipcpath();
  inline void clear_has_ipcpath();
  inline void set_has_originatingprocessid();
  inline void clear_has_originatingprocessid();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* resoucename_;
  ::std::string* ipcpath_;
  ::google::protobuf::uint64 originatingprocessid_;
  ::google::protobuf::RepeatedPtrField< ::std::string> tags_;
  ::google::protobuf::RepeatedPtrField< ::integerKeyValuePair > integerpairs_;
  ::google::protobuf::RepeatedPtrField< ::doubleKeyValuePair > doublepairs_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(6 + 31) / 32];

  friend void  protobuf_AddDesc_localURI_2eproto();
  friend void protobuf_AssignDesc_localURI_2eproto();
  friend void protobuf_ShutdownFile_localURI_2eproto();

  void InitAsDefaultInstance();
  static localURI* default_instance_;
};
// -------------------------------------------------------------------

class integerKeyValuePair : public ::google::protobuf::Message {
 public:
  integerKeyValuePair();
  virtual ~integerKeyValuePair();

  integerKeyValuePair(const integerKeyValuePair& from);

  inline integerKeyValuePair& operator=(const integerKeyValuePair& from) {
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
  static const integerKeyValuePair& default_instance();

  void Swap(integerKeyValuePair* other);

  // implements Message ----------------------------------------------

  integerKeyValuePair* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const integerKeyValuePair& from);
  void MergeFrom(const integerKeyValuePair& from);
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

  // required string key = 1;
  inline bool has_key() const;
  inline void clear_key();
  static const int kKeyFieldNumber = 1;
  inline const ::std::string& key() const;
  inline void set_key(const ::std::string& value);
  inline void set_key(const char* value);
  inline void set_key(const char* value, size_t size);
  inline ::std::string* mutable_key();
  inline ::std::string* release_key();
  inline void set_allocated_key(::std::string* key);

  // required int32 value = 2;
  inline bool has_value() const;
  inline void clear_value();
  static const int kValueFieldNumber = 2;
  inline ::google::protobuf::int32 value() const;
  inline void set_value(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:integerKeyValuePair)
 private:
  inline void set_has_key();
  inline void clear_has_key();
  inline void set_has_value();
  inline void clear_has_value();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* key_;
  ::google::protobuf::int32 value_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_localURI_2eproto();
  friend void protobuf_AssignDesc_localURI_2eproto();
  friend void protobuf_ShutdownFile_localURI_2eproto();

  void InitAsDefaultInstance();
  static integerKeyValuePair* default_instance_;
};
// -------------------------------------------------------------------

class doubleKeyValuePair : public ::google::protobuf::Message {
 public:
  doubleKeyValuePair();
  virtual ~doubleKeyValuePair();

  doubleKeyValuePair(const doubleKeyValuePair& from);

  inline doubleKeyValuePair& operator=(const doubleKeyValuePair& from) {
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
  static const doubleKeyValuePair& default_instance();

  void Swap(doubleKeyValuePair* other);

  // implements Message ----------------------------------------------

  doubleKeyValuePair* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const doubleKeyValuePair& from);
  void MergeFrom(const doubleKeyValuePair& from);
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

  // required string key = 1;
  inline bool has_key() const;
  inline void clear_key();
  static const int kKeyFieldNumber = 1;
  inline const ::std::string& key() const;
  inline void set_key(const ::std::string& value);
  inline void set_key(const char* value);
  inline void set_key(const char* value, size_t size);
  inline ::std::string* mutable_key();
  inline ::std::string* release_key();
  inline void set_allocated_key(::std::string* key);

  // required double value = 2;
  inline bool has_value() const;
  inline void clear_value();
  static const int kValueFieldNumber = 2;
  inline double value() const;
  inline void set_value(double value);

  // @@protoc_insertion_point(class_scope:doubleKeyValuePair)
 private:
  inline void set_has_key();
  inline void clear_has_key();
  inline void set_has_value();
  inline void clear_has_value();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* key_;
  double value_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_localURI_2eproto();
  friend void protobuf_AssignDesc_localURI_2eproto();
  friend void protobuf_ShutdownFile_localURI_2eproto();

  void InitAsDefaultInstance();
  static doubleKeyValuePair* default_instance_;
};
// ===================================================================


// ===================================================================

// localURI

// required string resouceName = 1;
inline bool localURI::has_resoucename() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void localURI::set_has_resoucename() {
  _has_bits_[0] |= 0x00000001u;
}
inline void localURI::clear_has_resoucename() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void localURI::clear_resoucename() {
  if (resoucename_ != &::google::protobuf::internal::kEmptyString) {
    resoucename_->clear();
  }
  clear_has_resoucename();
}
inline const ::std::string& localURI::resoucename() const {
  return *resoucename_;
}
inline void localURI::set_resoucename(const ::std::string& value) {
  set_has_resoucename();
  if (resoucename_ == &::google::protobuf::internal::kEmptyString) {
    resoucename_ = new ::std::string;
  }
  resoucename_->assign(value);
}
inline void localURI::set_resoucename(const char* value) {
  set_has_resoucename();
  if (resoucename_ == &::google::protobuf::internal::kEmptyString) {
    resoucename_ = new ::std::string;
  }
  resoucename_->assign(value);
}
inline void localURI::set_resoucename(const char* value, size_t size) {
  set_has_resoucename();
  if (resoucename_ == &::google::protobuf::internal::kEmptyString) {
    resoucename_ = new ::std::string;
  }
  resoucename_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* localURI::mutable_resoucename() {
  set_has_resoucename();
  if (resoucename_ == &::google::protobuf::internal::kEmptyString) {
    resoucename_ = new ::std::string;
  }
  return resoucename_;
}
inline ::std::string* localURI::release_resoucename() {
  clear_has_resoucename();
  if (resoucename_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = resoucename_;
    resoucename_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void localURI::set_allocated_resoucename(::std::string* resoucename) {
  if (resoucename_ != &::google::protobuf::internal::kEmptyString) {
    delete resoucename_;
  }
  if (resoucename) {
    set_has_resoucename();
    resoucename_ = resoucename;
  } else {
    clear_has_resoucename();
    resoucename_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required string IPCPath = 2;
inline bool localURI::has_ipcpath() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void localURI::set_has_ipcpath() {
  _has_bits_[0] |= 0x00000002u;
}
inline void localURI::clear_has_ipcpath() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void localURI::clear_ipcpath() {
  if (ipcpath_ != &::google::protobuf::internal::kEmptyString) {
    ipcpath_->clear();
  }
  clear_has_ipcpath();
}
inline const ::std::string& localURI::ipcpath() const {
  return *ipcpath_;
}
inline void localURI::set_ipcpath(const ::std::string& value) {
  set_has_ipcpath();
  if (ipcpath_ == &::google::protobuf::internal::kEmptyString) {
    ipcpath_ = new ::std::string;
  }
  ipcpath_->assign(value);
}
inline void localURI::set_ipcpath(const char* value) {
  set_has_ipcpath();
  if (ipcpath_ == &::google::protobuf::internal::kEmptyString) {
    ipcpath_ = new ::std::string;
  }
  ipcpath_->assign(value);
}
inline void localURI::set_ipcpath(const char* value, size_t size) {
  set_has_ipcpath();
  if (ipcpath_ == &::google::protobuf::internal::kEmptyString) {
    ipcpath_ = new ::std::string;
  }
  ipcpath_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* localURI::mutable_ipcpath() {
  set_has_ipcpath();
  if (ipcpath_ == &::google::protobuf::internal::kEmptyString) {
    ipcpath_ = new ::std::string;
  }
  return ipcpath_;
}
inline ::std::string* localURI::release_ipcpath() {
  clear_has_ipcpath();
  if (ipcpath_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = ipcpath_;
    ipcpath_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void localURI::set_allocated_ipcpath(::std::string* ipcpath) {
  if (ipcpath_ != &::google::protobuf::internal::kEmptyString) {
    delete ipcpath_;
  }
  if (ipcpath) {
    set_has_ipcpath();
    ipcpath_ = ipcpath;
  } else {
    clear_has_ipcpath();
    ipcpath_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional uint64 originatingProcessID = 3;
inline bool localURI::has_originatingprocessid() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void localURI::set_has_originatingprocessid() {
  _has_bits_[0] |= 0x00000004u;
}
inline void localURI::clear_has_originatingprocessid() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void localURI::clear_originatingprocessid() {
  originatingprocessid_ = GOOGLE_ULONGLONG(0);
  clear_has_originatingprocessid();
}
inline ::google::protobuf::uint64 localURI::originatingprocessid() const {
  return originatingprocessid_;
}
inline void localURI::set_originatingprocessid(::google::protobuf::uint64 value) {
  set_has_originatingprocessid();
  originatingprocessid_ = value;
}

// repeated string tags = 4;
inline int localURI::tags_size() const {
  return tags_.size();
}
inline void localURI::clear_tags() {
  tags_.Clear();
}
inline const ::std::string& localURI::tags(int index) const {
  return tags_.Get(index);
}
inline ::std::string* localURI::mutable_tags(int index) {
  return tags_.Mutable(index);
}
inline void localURI::set_tags(int index, const ::std::string& value) {
  tags_.Mutable(index)->assign(value);
}
inline void localURI::set_tags(int index, const char* value) {
  tags_.Mutable(index)->assign(value);
}
inline void localURI::set_tags(int index, const char* value, size_t size) {
  tags_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* localURI::add_tags() {
  return tags_.Add();
}
inline void localURI::add_tags(const ::std::string& value) {
  tags_.Add()->assign(value);
}
inline void localURI::add_tags(const char* value) {
  tags_.Add()->assign(value);
}
inline void localURI::add_tags(const char* value, size_t size) {
  tags_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
localURI::tags() const {
  return tags_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
localURI::mutable_tags() {
  return &tags_;
}

// repeated .integerKeyValuePair integerPairs = 5;
inline int localURI::integerpairs_size() const {
  return integerpairs_.size();
}
inline void localURI::clear_integerpairs() {
  integerpairs_.Clear();
}
inline const ::integerKeyValuePair& localURI::integerpairs(int index) const {
  return integerpairs_.Get(index);
}
inline ::integerKeyValuePair* localURI::mutable_integerpairs(int index) {
  return integerpairs_.Mutable(index);
}
inline ::integerKeyValuePair* localURI::add_integerpairs() {
  return integerpairs_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::integerKeyValuePair >&
localURI::integerpairs() const {
  return integerpairs_;
}
inline ::google::protobuf::RepeatedPtrField< ::integerKeyValuePair >*
localURI::mutable_integerpairs() {
  return &integerpairs_;
}

// repeated .doubleKeyValuePair doublePairs = 6;
inline int localURI::doublepairs_size() const {
  return doublepairs_.size();
}
inline void localURI::clear_doublepairs() {
  doublepairs_.Clear();
}
inline const ::doubleKeyValuePair& localURI::doublepairs(int index) const {
  return doublepairs_.Get(index);
}
inline ::doubleKeyValuePair* localURI::mutable_doublepairs(int index) {
  return doublepairs_.Mutable(index);
}
inline ::doubleKeyValuePair* localURI::add_doublepairs() {
  return doublepairs_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::doubleKeyValuePair >&
localURI::doublepairs() const {
  return doublepairs_;
}
inline ::google::protobuf::RepeatedPtrField< ::doubleKeyValuePair >*
localURI::mutable_doublepairs() {
  return &doublepairs_;
}

// -------------------------------------------------------------------

// integerKeyValuePair

// required string key = 1;
inline bool integerKeyValuePair::has_key() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void integerKeyValuePair::set_has_key() {
  _has_bits_[0] |= 0x00000001u;
}
inline void integerKeyValuePair::clear_has_key() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void integerKeyValuePair::clear_key() {
  if (key_ != &::google::protobuf::internal::kEmptyString) {
    key_->clear();
  }
  clear_has_key();
}
inline const ::std::string& integerKeyValuePair::key() const {
  return *key_;
}
inline void integerKeyValuePair::set_key(const ::std::string& value) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  key_->assign(value);
}
inline void integerKeyValuePair::set_key(const char* value) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  key_->assign(value);
}
inline void integerKeyValuePair::set_key(const char* value, size_t size) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  key_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* integerKeyValuePair::mutable_key() {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  return key_;
}
inline ::std::string* integerKeyValuePair::release_key() {
  clear_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = key_;
    key_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void integerKeyValuePair::set_allocated_key(::std::string* key) {
  if (key_ != &::google::protobuf::internal::kEmptyString) {
    delete key_;
  }
  if (key) {
    set_has_key();
    key_ = key;
  } else {
    clear_has_key();
    key_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required int32 value = 2;
inline bool integerKeyValuePair::has_value() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void integerKeyValuePair::set_has_value() {
  _has_bits_[0] |= 0x00000002u;
}
inline void integerKeyValuePair::clear_has_value() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void integerKeyValuePair::clear_value() {
  value_ = 0;
  clear_has_value();
}
inline ::google::protobuf::int32 integerKeyValuePair::value() const {
  return value_;
}
inline void integerKeyValuePair::set_value(::google::protobuf::int32 value) {
  set_has_value();
  value_ = value;
}

// -------------------------------------------------------------------

// doubleKeyValuePair

// required string key = 1;
inline bool doubleKeyValuePair::has_key() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void doubleKeyValuePair::set_has_key() {
  _has_bits_[0] |= 0x00000001u;
}
inline void doubleKeyValuePair::clear_has_key() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void doubleKeyValuePair::clear_key() {
  if (key_ != &::google::protobuf::internal::kEmptyString) {
    key_->clear();
  }
  clear_has_key();
}
inline const ::std::string& doubleKeyValuePair::key() const {
  return *key_;
}
inline void doubleKeyValuePair::set_key(const ::std::string& value) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  key_->assign(value);
}
inline void doubleKeyValuePair::set_key(const char* value) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  key_->assign(value);
}
inline void doubleKeyValuePair::set_key(const char* value, size_t size) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  key_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* doubleKeyValuePair::mutable_key() {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  return key_;
}
inline ::std::string* doubleKeyValuePair::release_key() {
  clear_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = key_;
    key_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void doubleKeyValuePair::set_allocated_key(::std::string* key) {
  if (key_ != &::google::protobuf::internal::kEmptyString) {
    delete key_;
  }
  if (key) {
    set_has_key();
    key_ = key;
  } else {
    clear_has_key();
    key_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required double value = 2;
inline bool doubleKeyValuePair::has_value() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void doubleKeyValuePair::set_has_value() {
  _has_bits_[0] |= 0x00000002u;
}
inline void doubleKeyValuePair::clear_has_value() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void doubleKeyValuePair::clear_value() {
  value_ = 0;
  clear_has_value();
}
inline double doubleKeyValuePair::value() const {
  return value_;
}
inline void doubleKeyValuePair::set_value(double value) {
  set_has_value();
  value_ = value;
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_localURI_2eproto__INCLUDED
