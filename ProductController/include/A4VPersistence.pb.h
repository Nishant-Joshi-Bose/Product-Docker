// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: A4VPersistence.proto

#ifndef PROTOBUF_A4VPersistence_2eproto__INCLUDED
#define PROTOBUF_A4VPersistence_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
#include "RebroadcastLatencyMode.pb.h"
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_A4VPersistence_2eproto();
void protobuf_AssignDesc_A4VPersistence_2eproto();
void protobuf_ShutdownFile_A4VPersistence_2eproto();

class a4vpersistence;

// ===================================================================

class a4vpersistence : public ::google::protobuf::Message {
 public:
  a4vpersistence();
  virtual ~a4vpersistence();
  
  a4vpersistence(const a4vpersistence& from);
  
  inline a4vpersistence& operator=(const a4vpersistence& from) {
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
  static const a4vpersistence& default_instance();
  
  void Swap(a4vpersistence* other);
  
  // implements Message ----------------------------------------------
  
  a4vpersistence* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const a4vpersistence& from);
  void MergeFrom(const a4vpersistence& from);
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
  
  // optional .rebroadcastlatencymode rebroadcastLatencyMode = 1;
  inline bool has_rebroadcastlatencymode() const;
  inline void clear_rebroadcastlatencymode();
  static const int kRebroadcastLatencyModeFieldNumber = 1;
  inline const ::rebroadcastlatencymode& rebroadcastlatencymode() const;
  inline ::rebroadcastlatencymode* mutable_rebroadcastlatencymode();
  inline ::rebroadcastlatencymode* release_rebroadcastlatencymode();
  
  // @@protoc_insertion_point(class_scope:a4vpersistence)
 private:
  inline void set_has_rebroadcastlatencymode();
  inline void clear_has_rebroadcastlatencymode();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::rebroadcastlatencymode* rebroadcastlatencymode_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_A4VPersistence_2eproto();
  friend void protobuf_AssignDesc_A4VPersistence_2eproto();
  friend void protobuf_ShutdownFile_A4VPersistence_2eproto();
  
  void InitAsDefaultInstance();
  static a4vpersistence* default_instance_;
};
// ===================================================================


// ===================================================================

// a4vpersistence

// optional .rebroadcastlatencymode rebroadcastLatencyMode = 1;
inline bool a4vpersistence::has_rebroadcastlatencymode() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void a4vpersistence::set_has_rebroadcastlatencymode() {
  _has_bits_[0] |= 0x00000001u;
}
inline void a4vpersistence::clear_has_rebroadcastlatencymode() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void a4vpersistence::clear_rebroadcastlatencymode() {
  if (rebroadcastlatencymode_ != NULL) rebroadcastlatencymode_->::rebroadcastlatencymode::Clear();
  clear_has_rebroadcastlatencymode();
}
inline const ::rebroadcastlatencymode& a4vpersistence::rebroadcastlatencymode() const {
  return rebroadcastlatencymode_ != NULL ? *rebroadcastlatencymode_ : *default_instance_->rebroadcastlatencymode_;
}
inline ::rebroadcastlatencymode* a4vpersistence::mutable_rebroadcastlatencymode() {
  set_has_rebroadcastlatencymode();
  if (rebroadcastlatencymode_ == NULL) rebroadcastlatencymode_ = new ::rebroadcastlatencymode;
  return rebroadcastlatencymode_;
}
inline ::rebroadcastlatencymode* a4vpersistence::release_rebroadcastlatencymode() {
  clear_has_rebroadcastlatencymode();
  ::rebroadcastlatencymode* temp = rebroadcastlatencymode_;
  rebroadcastlatencymode_ = NULL;
  return temp;
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_A4VPersistence_2eproto__INCLUDED
