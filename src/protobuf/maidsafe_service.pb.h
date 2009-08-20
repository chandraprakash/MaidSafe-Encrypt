// Generated by the protocol buffer compiler.  DO NOT EDIT!

#ifndef PROTOBUF_maidsafe_5fservice_2eproto__INCLUDED
#define PROTOBUF_maidsafe_5fservice_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2001000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2001000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/service.h>
#include "maidsafe_service_messages.pb.h"

namespace maidsafe {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_maidsafe_5fservice_2eproto();
void protobuf_AssignDesc_maidsafe_5fservice_2eproto();
void protobuf_ShutdownFile_maidsafe_5fservice_2eproto();


// ===================================================================


// ===================================================================

class MaidsafeService_Stub;

class MaidsafeService : public ::google::protobuf::Service {
 protected:
  // This class should be treated as an abstract interface.
  inline MaidsafeService() {};
 public:
  virtual ~MaidsafeService();
  
  typedef MaidsafeService_Stub Stub;
  
  static const ::google::protobuf::ServiceDescriptor* descriptor();
  
  virtual void StoreChunkPrep(::google::protobuf::RpcController* controller,
                       const ::maidsafe::StorePrepRequest* request,
                       ::maidsafe::StorePrepResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void StoreIOU(::google::protobuf::RpcController* controller,
                       const ::maidsafe::StoreIOURequest* request,
                       ::maidsafe::StoreIOUResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void IOUDone(::google::protobuf::RpcController* controller,
                       const ::maidsafe::IOUDoneRequest* request,
                       ::maidsafe::IOUDoneResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void StoreChunk(::google::protobuf::RpcController* controller,
                       const ::maidsafe::StoreRequest* request,
                       ::maidsafe::StoreResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void StoreChunkReference(::google::protobuf::RpcController* controller,
                       const ::maidsafe::StoreReferenceRequest* request,
                       ::maidsafe::StoreReferenceResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void CheckChunk(::google::protobuf::RpcController* controller,
                       const ::maidsafe::CheckChunkRequest* request,
                       ::maidsafe::CheckChunkResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void Get(::google::protobuf::RpcController* controller,
                       const ::maidsafe::GetRequest* request,
                       ::maidsafe::GetResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void Update(::google::protobuf::RpcController* controller,
                       const ::maidsafe::UpdateRequest* request,
                       ::maidsafe::UpdateResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void GetMessages(::google::protobuf::RpcController* controller,
                       const ::maidsafe::GetMessagesRequest* request,
                       ::maidsafe::GetMessagesResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void Delete(::google::protobuf::RpcController* controller,
                       const ::maidsafe::DeleteRequest* request,
                       ::maidsafe::DeleteResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void ValidityCheck(::google::protobuf::RpcController* controller,
                       const ::maidsafe::ValidityCheckRequest* request,
                       ::maidsafe::ValidityCheckResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void SwapChunk(::google::protobuf::RpcController* controller,
                       const ::maidsafe::SwapChunkRequest* request,
                       ::maidsafe::SwapChunkResponse* response,
                       ::google::protobuf::Closure* done);
  
  // implements Service ----------------------------------------------
  
  const ::google::protobuf::ServiceDescriptor* GetDescriptor();
  void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                  ::google::protobuf::RpcController* controller,
                  const ::google::protobuf::Message* request,
                  ::google::protobuf::Message* response,
                  ::google::protobuf::Closure* done);
  const ::google::protobuf::Message& GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const;
  const ::google::protobuf::Message& GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MaidsafeService);
};

class MaidsafeService_Stub : public MaidsafeService {
 public:
  MaidsafeService_Stub(::google::protobuf::RpcChannel* channel);
  MaidsafeService_Stub(::google::protobuf::RpcChannel* channel,
                   ::google::protobuf::Service::ChannelOwnership ownership);
  ~MaidsafeService_Stub();
  
  inline ::google::protobuf::RpcChannel* channel() { return channel_; }
  
  // implements MaidsafeService ------------------------------------------
  
  void StoreChunkPrep(::google::protobuf::RpcController* controller,
                       const ::maidsafe::StorePrepRequest* request,
                       ::maidsafe::StorePrepResponse* response,
                       ::google::protobuf::Closure* done);
  void StoreIOU(::google::protobuf::RpcController* controller,
                       const ::maidsafe::StoreIOURequest* request,
                       ::maidsafe::StoreIOUResponse* response,
                       ::google::protobuf::Closure* done);
  void IOUDone(::google::protobuf::RpcController* controller,
                       const ::maidsafe::IOUDoneRequest* request,
                       ::maidsafe::IOUDoneResponse* response,
                       ::google::protobuf::Closure* done);
  void StoreChunk(::google::protobuf::RpcController* controller,
                       const ::maidsafe::StoreRequest* request,
                       ::maidsafe::StoreResponse* response,
                       ::google::protobuf::Closure* done);
  void StoreChunkReference(::google::protobuf::RpcController* controller,
                       const ::maidsafe::StoreReferenceRequest* request,
                       ::maidsafe::StoreReferenceResponse* response,
                       ::google::protobuf::Closure* done);
  void CheckChunk(::google::protobuf::RpcController* controller,
                       const ::maidsafe::CheckChunkRequest* request,
                       ::maidsafe::CheckChunkResponse* response,
                       ::google::protobuf::Closure* done);
  void Get(::google::protobuf::RpcController* controller,
                       const ::maidsafe::GetRequest* request,
                       ::maidsafe::GetResponse* response,
                       ::google::protobuf::Closure* done);
  void Update(::google::protobuf::RpcController* controller,
                       const ::maidsafe::UpdateRequest* request,
                       ::maidsafe::UpdateResponse* response,
                       ::google::protobuf::Closure* done);
  void GetMessages(::google::protobuf::RpcController* controller,
                       const ::maidsafe::GetMessagesRequest* request,
                       ::maidsafe::GetMessagesResponse* response,
                       ::google::protobuf::Closure* done);
  void Delete(::google::protobuf::RpcController* controller,
                       const ::maidsafe::DeleteRequest* request,
                       ::maidsafe::DeleteResponse* response,
                       ::google::protobuf::Closure* done);
  void ValidityCheck(::google::protobuf::RpcController* controller,
                       const ::maidsafe::ValidityCheckRequest* request,
                       ::maidsafe::ValidityCheckResponse* response,
                       ::google::protobuf::Closure* done);
  void SwapChunk(::google::protobuf::RpcController* controller,
                       const ::maidsafe::SwapChunkRequest* request,
                       ::maidsafe::SwapChunkResponse* response,
                       ::google::protobuf::Closure* done);
 private:
  ::google::protobuf::RpcChannel* channel_;
  bool owns_channel_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MaidsafeService_Stub);
};


// -------------------------------------------------------------------

class VaultRegistration_Stub;

class VaultRegistration : public ::google::protobuf::Service {
 protected:
  // This class should be treated as an abstract interface.
  inline VaultRegistration() {};
 public:
  virtual ~VaultRegistration();
  
  typedef VaultRegistration_Stub Stub;
  
  static const ::google::protobuf::ServiceDescriptor* descriptor();
  
  virtual void OwnVault(::google::protobuf::RpcController* controller,
                       const ::maidsafe::OwnVaultRequest* request,
                       ::maidsafe::OwnVaultResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void IsVaultOwned(::google::protobuf::RpcController* controller,
                       const ::maidsafe::IsOwnedRequest* request,
                       ::maidsafe::IsOwnedResponse* response,
                       ::google::protobuf::Closure* done);
  
  // implements Service ----------------------------------------------
  
  const ::google::protobuf::ServiceDescriptor* GetDescriptor();
  void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                  ::google::protobuf::RpcController* controller,
                  const ::google::protobuf::Message* request,
                  ::google::protobuf::Message* response,
                  ::google::protobuf::Closure* done);
  const ::google::protobuf::Message& GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const;
  const ::google::protobuf::Message& GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(VaultRegistration);
};

class VaultRegistration_Stub : public VaultRegistration {
 public:
  VaultRegistration_Stub(::google::protobuf::RpcChannel* channel);
  VaultRegistration_Stub(::google::protobuf::RpcChannel* channel,
                   ::google::protobuf::Service::ChannelOwnership ownership);
  ~VaultRegistration_Stub();
  
  inline ::google::protobuf::RpcChannel* channel() { return channel_; }
  
  // implements VaultRegistration ------------------------------------------
  
  void OwnVault(::google::protobuf::RpcController* controller,
                       const ::maidsafe::OwnVaultRequest* request,
                       ::maidsafe::OwnVaultResponse* response,
                       ::google::protobuf::Closure* done);
  void IsVaultOwned(::google::protobuf::RpcController* controller,
                       const ::maidsafe::IsOwnedRequest* request,
                       ::maidsafe::IsOwnedResponse* response,
                       ::google::protobuf::Closure* done);
 private:
  ::google::protobuf::RpcChannel* channel_;
  bool owns_channel_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(VaultRegistration_Stub);
};


// ===================================================================


// ===================================================================


}  // namespace maidsafe
#endif  // PROTOBUF_maidsafe_5fservice_2eproto__INCLUDED
