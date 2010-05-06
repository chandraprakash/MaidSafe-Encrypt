/*
* ============================================================================
*
* Copyright [2009] maidsafe.net limited
*
* Description:  RPCs used by vault
* Version:      1.0
* Created:      2009-02-22-00.18.57
* Revision:     none
* Compiler:     gcc
* Author:       Fraser Hutchison (fh), fraser.hutchison@maidsafe.net
* Company:      maidsafe.net limited
*
* The following source code is property of maidsafe.net limited and is not
* meant for external use.  The use of this code is governed by the license
* file LICENSE.TXT found in the root of this directory and also on
* www.maidsafe.net.
*
* You are not free to copy, amend or otherwise use this source code without
* the explicit written permission of the board of directors of maidsafe.net.
*
* ============================================================================
*/

#include "maidsafe/vault/vaultrpc.h"

namespace maidsafe_vault {

void VaultRpcs::StoreChunk(const std::string &chunkname,
                           const std::string &data,
                           const std::string &public_key,
                           const std::string &public_key_signature,
                           const std::string &request_signature,
                           const maidsafe::ValueType &data_type,
                           const std::string &remote_ip,
                           const boost::uint16_t &remote_port,
                           const std::string &rendezvous_ip,
                           const boost::uint16_t &rendezvous_port,
                           const boost::int16_t &transport_id,
                           maidsafe::StoreChunkResponse *response,
                           rpcprotocol::Controller *controller,
                           google::protobuf::Closure *done) {
  maidsafe::StoreChunkRequest args;
  args.set_chunkname(chunkname);
  args.set_data(data);
  args.set_public_key(public_key);
  args.set_public_key_signature(public_key_signature);
  args.set_request_signature(request_signature);
  args.set_data_type(data_type);
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, remote_ip, remote_port, "", 0,
                               rendezvous_ip, rendezvous_port);
  maidsafe::MaidsafeService::Stub service(&channel);
  service.StoreChunk(controller, &args, response, done);
}

void VaultRpcs::AddToReferenceList(
    const kad::Contact &peer,
    bool local,
    const boost::int16_t &transport_id,
    maidsafe::AddToReferenceListRequest *add_to_reference_list_request,
    maidsafe::AddToReferenceListResponse *add_to_reference_list_response,
    rpcprotocol::Controller *controller,
    google::protobuf::Closure *done) {
  if (peer.node_id().ToStringDecoded() == own_id_) {
    add_to_reference_list_response->set_result(kNotRemote);
    done->Run();
    return;
  }
  std::string local_ip;
  boost::uint16_t local_port(0);
  if (local) {
    local_ip = peer.local_ip();
    local_port = peer.local_port();
  }
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, peer.host_ip(), peer.host_port(),
                               local_ip, local_port, peer.rendezvous_ip(),
                               peer.rendezvous_port());
  maidsafe::MaidsafeService::Stub service(&channel);
  service.AddToReferenceList(controller, add_to_reference_list_request,
                             add_to_reference_list_response, done);
}

void VaultRpcs::GetChunkReferences(
    const kad::Contact &peer,
    bool local,
    const boost::int16_t &transport_id,
    maidsafe::GetChunkReferencesRequest
        *get_chunk_references_request,
    maidsafe::GetChunkReferencesResponse
        *get_chunk_references_response,
    rpcprotocol::Controller *controller,
    google::protobuf::Closure *done) {
  std::string local_ip;
  boost::uint16_t local_port(0);
  if (local) {
    local_ip = peer.local_ip();
    local_port = peer.local_port();
  }
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, peer.host_ip(), peer.host_port(),
                               local_ip, local_port, peer.rendezvous_ip(),
                               peer.rendezvous_port());
  maidsafe::MaidsafeService::Stub service(&channel);
  service.GetChunkReferences(controller, get_chunk_references_request,
                             get_chunk_references_response, done);
}

void VaultRpcs::AmendAccount(
    const kad::Contact &peer,
    bool local,
    const boost::int16_t &transport_id,
    maidsafe::AmendAccountRequest *amend_account_request,
    maidsafe::AmendAccountResponse *amend_account_response,
    rpcprotocol::Controller *controller,
    google::protobuf::Closure *done) {
  std::string local_ip;
  boost::uint16_t local_port(0);
  if (local) {
    local_ip = peer.local_ip();
    local_port = peer.local_port();
  }
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, peer.host_ip(), peer.host_port(),
                               local_ip, local_port, peer.rendezvous_ip(),
                               peer.rendezvous_port());
  maidsafe::MaidsafeService::Stub service(&channel);
  service.AmendAccount(controller, amend_account_request,
                       amend_account_response, done);
}

void VaultRpcs::ExpectAmendment(
    const kad::Contact &peer,
    bool local,
    const boost::int16_t &transport_id,
    maidsafe::ExpectAmendmentRequest *expect_amendment_request,
    maidsafe::ExpectAmendmentResponse *expect_amendment_response,
    rpcprotocol::Controller *controller,
    google::protobuf::Closure *done) {
  std::string local_ip;
  boost::uint16_t local_port(0);
  if (local) {
    local_ip = peer.local_ip();
    local_port = peer.local_port();
  }
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, peer.host_ip(), peer.host_port(),
                               local_ip, local_port, peer.rendezvous_ip(),
                               peer.rendezvous_port());
  maidsafe::MaidsafeService::Stub service(&channel);
  service.ExpectAmendment(controller, expect_amendment_request,
                          expect_amendment_response, done);
}

void VaultRpcs::ConfirmAmendment(
    const kad::Contact &peer,
    bool local,
    const boost::int16_t &transport_id,
    maidsafe::ConfirmAmendmentRequest *confirm_amendment_request,
    maidsafe::ConfirmAmendmentResponse *confirm_amendment_response,
    rpcprotocol::Controller *controller,
    google::protobuf::Closure *done) {
  std::string local_ip;
  boost::uint16_t local_port(0);
  if (local) {
    local_ip = peer.local_ip();
    local_port = peer.local_port();
  }
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, peer.host_ip(), peer.host_port(),
                               local_ip, local_port, peer.rendezvous_ip(),
                               peer.rendezvous_port());
  maidsafe::MaidsafeService::Stub service(&channel);
  service.ConfirmAmendment(controller, confirm_amendment_request,
                           confirm_amendment_response, done);
}

void VaultRpcs::AccountStatus(
    const kad::Contact &peer,
    bool local,
    const boost::int16_t &transport_id,
    maidsafe::AccountStatusRequest *account_status_request,
    maidsafe::AccountStatusResponse *account_status_response,
    rpcprotocol::Controller *controller,
    google::protobuf::Closure *done) {
  std::string local_ip;
  boost::uint16_t local_port(0);
  if (local) {
    local_ip = peer.local_ip();
    local_port = peer.local_port();
  }
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, peer.host_ip(), peer.host_port(),
                               local_ip, local_port, peer.rendezvous_ip(),
                               peer.rendezvous_port());
  maidsafe::MaidsafeService::Stub service(&channel);
  service.AccountStatus(controller, account_status_request,
                        account_status_response, done);
}

void VaultRpcs::CheckChunk(const std::string &chunkname,
                           const std::string &remote_ip,
                           const boost::uint16_t &remote_port,
                           const std::string &rendezvous_ip,
                           const boost::uint16_t &rendezvous_port,
                           const boost::int16_t &transport_id,
                           maidsafe::CheckChunkResponse *response,
                           rpcprotocol::Controller *controller,
                           google::protobuf::Closure *done) {
  maidsafe::CheckChunkRequest args;
  args.set_chunkname(chunkname);
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, remote_ip, remote_port, "", 0,
                               rendezvous_ip, rendezvous_port);
  maidsafe::MaidsafeService::Stub service(&channel);
  service.CheckChunk(controller, &args, response, done);
}

void VaultRpcs::GetChunk(const std::string &chunkname,
                         const std::string &remote_ip,
                         const boost::uint16_t &remote_port,
                         const std::string &rendezvous_ip,
                         const boost::uint16_t &rendezvous_port,
                         const boost::int16_t &transport_id,
                         maidsafe::GetChunkResponse *response,
                         rpcprotocol::Controller *controller,
                         google::protobuf::Closure *done) {
  maidsafe::GetChunkRequest args;
  args.set_chunkname(chunkname);
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, remote_ip, remote_port, "", 0,
                               rendezvous_ip, rendezvous_port);
  maidsafe::MaidsafeService::Stub service(&channel);
  service.GetChunk(controller, &args, response, done);
}

void VaultRpcs::DeleteChunk(const std::string &chunkname,
                            const std::string &,
                            const std::string &,
                            const std::string &request_signature,
                            const maidsafe::ValueType &data_type,
                            const std::string &remote_ip,
                            const boost::uint16_t &remote_port,
                            const std::string &rendezvous_ip,
                            const boost::uint16_t &rendezvous_port,
                            const boost::int16_t &transport_id,
                            maidsafe::DeleteChunkResponse *response,
                            rpcprotocol::Controller *controller,
                            google::protobuf::Closure *done) {
  maidsafe::DeleteChunkRequest args;
  args.set_chunkname(chunkname);
/*  args.set_public_key(public_key);
  args.set_public_key_signature(public_key_signature);*/
  args.set_request_signature(request_signature);
  args.set_data_type(data_type);
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, remote_ip, remote_port, "", 0,
                               rendezvous_ip, rendezvous_port);
  maidsafe::MaidsafeService::Stub service(&channel);
  service.DeleteChunk(controller, &args, response, done);
}

void VaultRpcs::ValidityCheck(const std::string &chunkname,
                              const std::string &random_data,
                              const std::string &remote_ip,
                              const boost::uint16_t &remote_port,
                              const std::string &rendezvous_ip,
                              const boost::uint16_t &rendezvous_port,
                              const boost::int16_t &transport_id,
                              maidsafe::ValidityCheckResponse *response,
                              rpcprotocol::Controller *controller,
                              google::protobuf::Closure *done) {
  maidsafe::ValidityCheckRequest args;
  args.set_chunkname(chunkname);
  args.set_random_data(random_data);
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, remote_ip, remote_port, "", 0,
                               rendezvous_ip, rendezvous_port);
  maidsafe::MaidsafeService::Stub service(&channel);
  service.ValidityCheck(controller, &args, response, done);
}

void VaultRpcs::SwapChunk(const boost::uint32_t request_type,
                          const std::string &chunkname1,
                          const std::string &chunkcontent1,
                          const boost::uint32_t size1,
                          const std::string &remote_ip,
                          const boost::uint16_t &remote_port,
                          const std::string &rendezvous_ip,
                          const boost::uint16_t &rendezvous_port,
                          const boost::int16_t &transport_id,
                          maidsafe::SwapChunkResponse *response,
                          rpcprotocol::Controller *controller,
                          google::protobuf::Closure *done) {
  maidsafe::SwapChunkRequest args;
  args.set_request_type(request_type);
  args.set_chunkname1(chunkname1);
  if (request_type == 0) {
    args.set_size1(size1);
  } else {
    args.set_chunkcontent1(chunkcontent1);
  }
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, remote_ip, remote_port, "", 0,
                               rendezvous_ip, rendezvous_port);
  maidsafe::MaidsafeService::Stub service(&channel);
  service.SwapChunk(controller, &args, response, done);
}

void VaultRpcs::CacheChunk(const std::string &remote_ip,
                           const boost::uint16_t &remote_port,
                           const std::string &rendezvous_ip,
                           const boost::uint16_t &rendezvous_port,
                           const boost::int16_t &transport_id,
                           maidsafe::CacheChunkRequest *request,
                           maidsafe::CacheChunkResponse *response,
                           rpcprotocol::Controller *controller,
                           google::protobuf::Closure *done) {
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, remote_ip, remote_port, "", 0,
                               rendezvous_ip, rendezvous_port);
  maidsafe::MaidsafeService::Stub service(&channel);
  service.CacheChunk(controller, request, response, done);
}

void VaultRpcs::GetSyncData(
    const kad::Contact &peer,
    bool local,
    const boost::int16_t &transport_id,
    maidsafe::GetSyncDataRequest *get_sync_data_request,
    maidsafe::GetSyncDataResponse *get_sync_data_response,
    rpcprotocol::Controller *controller,
    google::protobuf::Closure *done) {
  std::string local_ip;
  boost::uint16_t local_port(0);
  if (local) {
    local_ip = peer.local_ip();
    local_port = peer.local_port();
  }
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, peer.host_ip(), peer.host_port(),
                               local_ip, local_port, peer.rendezvous_ip(),
                               peer.rendezvous_port());
  maidsafe::MaidsafeService::Stub service(&channel);
  service.GetSyncData(controller, get_sync_data_request, get_sync_data_response,
                      done);
}

void VaultRpcs::GetAccount(const kad::Contact &peer,
                           bool local,
                           const boost::int16_t &transport_id,
                           maidsafe::GetAccountRequest *get_account_request,
                           maidsafe::GetAccountResponse *get_account_response,
                           rpcprotocol::Controller *controller,
                           google::protobuf::Closure *done) {
  std::string local_ip;
  boost::uint16_t local_port(0);
  if (local) {
    local_ip = peer.local_ip();
    local_port = peer.local_port();
  }
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, peer.host_ip(), peer.host_port(),
                               local_ip, local_port, peer.rendezvous_ip(),
                               peer.rendezvous_port());
  maidsafe::MaidsafeService::Stub service(&channel);
  service.GetAccount(controller, get_account_request, get_account_response,
                     done);
}

void VaultRpcs::GetChunkInfo(
    const kad::Contact &peer,
    bool local,
    const boost::int16_t &transport_id,
    maidsafe::GetChunkInfoRequest *get_chunk_info_request,
    maidsafe::GetChunkInfoResponse *get_chunk_info_response,
    rpcprotocol::Controller *controller,
    google::protobuf::Closure *done) {
  std::string local_ip;
  boost::uint16_t local_port(0);
  if (local) {
    local_ip = peer.local_ip();
    local_port = peer.local_port();
  }
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, peer.host_ip(), peer.host_port(),
                               local_ip, local_port, peer.rendezvous_ip(),
                               peer.rendezvous_port());
  maidsafe::MaidsafeService::Stub service(&channel);
  service.GetChunkInfo(controller, get_chunk_info_request,
                       get_chunk_info_response, done);
}

void VaultRpcs::GetBufferPacket(
      const kad::Contact &peer,
      bool local,
      const boost::int16_t &transport_id,
      maidsafe::GetBufferPacketRequest *get_buffer_packet_request,
      maidsafe::GetBufferPacketResponse *get_buffer_packet_response,
      rpcprotocol::Controller *controller,
      google::protobuf::Closure *done) {
  std::string local_ip;
  boost::uint16_t local_port(0);
  if (local) {
    local_ip = peer.local_ip();
    local_port = peer.local_port();
  }
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, peer.host_ip(), peer.host_port(),
                               local_ip, local_port, peer.rendezvous_ip(),
                               peer.rendezvous_port());
  maidsafe::MaidsafeService::Stub service(&channel);
  service.GetBufferPacket(controller, get_buffer_packet_request,
                          get_buffer_packet_response, done);
}

void VaultRpcs::GetBPMessages(const std::string &buffer_packet_name,
                              const std::string &public_key,
                              const std::string &public_key_signature,
                              const std::string &remote_ip,
                              const boost::uint16_t &remote_port,
                              const std::string &rendezvous_ip,
                              const boost::uint16_t &rendezvous_port,
                              const boost::int16_t &transport_id,
                              maidsafe::GetBPMessagesResponse *response,
                              rpcprotocol::Controller *controller,
                              google::protobuf::Closure *done) {
  maidsafe::GetBPMessagesRequest args;
  args.set_bufferpacket_name(buffer_packet_name);
  args.set_public_key(public_key);
  args.set_signed_public_key(public_key_signature);
  rpcprotocol::Channel channel(channel_manager_, transport_handler_,
                               transport_id, remote_ip, remote_port, "", 0,
                               rendezvous_ip, rendezvous_port);
  maidsafe::MaidsafeService::Stub service(&channel);
  service.GetBPMessages(controller, &args, response, done);
}

}  // namespace maidsafe_vault
