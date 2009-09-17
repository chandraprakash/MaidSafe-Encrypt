/*
* ============================================================================
*
* Copyright [2009] maidsafe.net limited
*
* Version:      1.0
* Created:      2009-01-28-10.59.46
* Revision:     none
* Compiler:     gcc
* Author:       Team
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
#include <gtest/gtest.h>
#include <maidsafe/kademlia_service_messages.pb.h>
#include <maidsafe/utils.h>
#include <boost/filesystem/fstream.hpp>

#include "maidsafe/chunkstore.h"
#include "maidsafe/client/localstoremanager.h"
#include "maidsafe/client/sessionsingleton.h"
#include "protobuf/maidsafe_messages.pb.h"
#include "protobuf/maidsafe_service_messages.pb.h"

class FakeCallback {
 public:
  FakeCallback() : result_("") {}
  void CallbackFunc(const std::string &res) {
    result_ = res;
  }
  void Reset() {
    result_ = "";
  }
  std::string result_;
};

void wait_for_result_lsm(const FakeCallback &cb,
    boost::recursive_mutex *mutex) {
  while (true) {
    {
      base::pd_scoped_lock guard(*mutex);
      if (cb.result_ != "") {
        return;
      }
    }
    boost::this_thread::sleep(boost::posix_time::milliseconds(5));
  }
};

class StoreManagerTest : public testing::Test {
 public:
  StoreManagerTest() : cb(), client_chunkstore_(), storemanager(),
      crypto_obj(), rsa_obj(), mutex_() {
    try {
      boost::filesystem::remove_all("./TestStoreManager");
    }
    catch(const std::exception &e) {
      printf("%s\n", e.what());
    }
  }
  ~StoreManagerTest() {
    try {
      boost::filesystem::remove_all("./TestStoreManager");
    }
    catch(const std::exception &e) {
      printf("%s\n", e.what());
    }
  }
 protected:
  void SetUp() {
    try {
      if (boost::filesystem::exists("KademilaDb.db"))
        boost::filesystem::remove(boost::filesystem::path("KademilaDb.db"));
      if (boost::filesystem::exists("StoreChunks"))
        boost::filesystem::remove_all(boost::filesystem::path("StoreChunks"));
    }
    catch(const std::exception &e) {
      printf("%s\n", e.what());
    }
    mutex_ = new boost::recursive_mutex();
    client_chunkstore_ = boost::shared_ptr<maidsafe::ChunkStore>
         (new maidsafe::ChunkStore("./TestStoreManager", 0, 0));
    int count(0);
    while (!client_chunkstore_->is_initialised() && count < 10000) {
      boost::this_thread::sleep(boost::posix_time::milliseconds(10));
      count += 10;
    }
    storemanager = new maidsafe::LocalStoreManager(mutex_, client_chunkstore_);
    // storemanager = new LocalStoreManager();
    storemanager->Init(0, boost::bind(&FakeCallback::CallbackFunc, &cb, _1));
    wait_for_result_lsm(cb, mutex_);
    maidsafe::GenericResponse res;
    ASSERT_TRUE(res.ParseFromString(cb.result_));
    if (res.result() == kNack) {
      FAIL();
      return;
    }
    crypto_obj.set_symm_algorithm(crypto::AES_256);
    crypto_obj.set_hash_algorithm(crypto::SHA_512);
    crypto::RsaKeyPair rsa_obj;
    rsa_obj.GenerateKeys(packethandler::kRsaKeySize);
    maidsafe::SessionSingleton::getInstance()->AddKey(
        maidsafe::MPID, "Me", rsa_obj.private_key(), rsa_obj.public_key(), "");
    cb.Reset();
  }
  void TearDown() {
    cb.Reset();
    storemanager->Close(boost::bind(&FakeCallback::CallbackFunc, &cb, _1),
                        true);
    wait_for_result_lsm(cb, mutex_);
    maidsafe::GenericResponse res;
    ASSERT_TRUE(res.ParseFromString(cb.result_));
    if (res.result() == kAck) {
      try {
        if (boost::filesystem::exists("KademilaDb.db"))
          boost::filesystem::remove(boost::filesystem::path("KademilaDb.db"));
        if (boost::filesystem::exists("StoreChunks"))
          boost::filesystem::remove_all(boost::filesystem::path("StoreChunks"));
      }
      catch(const std::exception &e) {
        printf("%s\n", e.what());
      }
    }
  }
  FakeCallback cb;
  boost::shared_ptr<maidsafe::ChunkStore> client_chunkstore_;
  maidsafe::LocalStoreManager *storemanager;
  crypto::Crypto crypto_obj;
  crypto::RsaKeyPair rsa_obj;
  boost::recursive_mutex *mutex_;
 private:
  StoreManagerTest(const StoreManagerTest&);
  StoreManagerTest &operator=(const StoreManagerTest&);
};

TEST_F(StoreManagerTest, BEH_MAID_StoreSystemPacket) {
  packethandler::GenericPacket gp;
  rsa_obj.GenerateKeys(4096);
  gp.set_data("Generic System Packet Data");
  gp.set_signature(crypto_obj.AsymSign(gp.data(), "",
                  rsa_obj.private_key(), crypto::STRING_STRING));
  std::string gp_name = crypto_obj.Hash(gp.data() + gp.signature(), "",
                        crypto::STRING_STRING, true);
  ASSERT_TRUE(storemanager->KeyUnique(gp_name, false));
  std::string gp_content;
  gp.SerializeToString(&gp_content);
  ASSERT_EQ(0, storemanager->StorePacket(gp_name, gp_content,
      packethandler::BUFFER, maidsafe::PRIVATE, ""));
  ASSERT_FALSE(storemanager->KeyUnique(gp_name, false));
  std::string result;
  storemanager->LoadPacket(gp_name,
                           boost::bind(&FakeCallback::CallbackFunc, &cb, _1));
  wait_for_result_lsm(cb, mutex_);
  maidsafe::GetResponse load_res;
  ASSERT_TRUE(load_res.ParseFromString(cb.result_));
  ASSERT_EQ(kAck, static_cast<int>(load_res.result()));
  ASSERT_EQ(gp_content, load_res.content());
}

TEST_F(StoreManagerTest, BEH_MAID_DeleteSystemPacket) {
  packethandler::GenericPacket gp;
  rsa_obj.GenerateKeys(4096);
  gp.set_data("Generic System Packet Data");
  gp.set_signature(crypto_obj.AsymSign(gp.data(), "", rsa_obj.private_key(),
                   crypto::STRING_STRING));
  std::string gp_name = crypto_obj.Hash(gp.data() + gp.signature(), "",
                        crypto::STRING_STRING, true);
  std::string gp_content;
  gp.SerializeToString(&gp_content);

  std::string signed_public_key = crypto_obj.AsymSign(rsa_obj.public_key(), "",
                                  rsa_obj.private_key(), crypto::STRING_STRING);
  std::string non_hex_gp_name("");
  base::decode_from_hex(gp_name, &non_hex_gp_name);
  std::string signed_request = crypto_obj.AsymSign(crypto_obj.Hash(
      rsa_obj.public_key() + signed_public_key + non_hex_gp_name, "",
      crypto::STRING_STRING, false),
      "", rsa_obj.private_key(), crypto::STRING_STRING);
  ASSERT_EQ(0, storemanager->StorePacket(gp_name, gp_content,
      packethandler::BUFFER, maidsafe::PRIVATE, ""));

  ASSERT_FALSE(storemanager->KeyUnique(gp_name, false));
  storemanager->DeletePacket(gp_name, signed_request, rsa_obj.public_key(),
                             signed_public_key, maidsafe::SYSTEM_PACKET,
                             boost::bind(&FakeCallback::CallbackFunc, &cb, _1));
  wait_for_result_lsm(cb, mutex_);
  maidsafe::DeleteResponse del_res;
  ASSERT_TRUE(del_res.ParseFromString(cb.result_));
  ASSERT_EQ(kAck, static_cast<int>(del_res.result()));
  cb.Reset();
  ASSERT_TRUE(storemanager->KeyUnique(gp_name, false));
}

TEST_F(StoreManagerTest, BEH_MAID_StoreChunk) {
  std::string chunk_content = base::RandomString(256 * 1024);
  std::string non_hex_chunk_name = crypto_obj.Hash(chunk_content, "",
                                   crypto::STRING_STRING, false);
  std::string hex_chunk_name;
  base::encode_to_hex(non_hex_chunk_name, &hex_chunk_name);
  fs::path chunk_path("./TestStoreManager");
  chunk_path /= hex_chunk_name;
  boost::filesystem::ofstream ofs;
  ofs.open(chunk_path.string().c_str());
  ofs << chunk_content;
  ofs.close();
  client_chunkstore_->AddChunkToOutgoing(non_hex_chunk_name, chunk_path);
  ASSERT_TRUE(storemanager->KeyUnique(hex_chunk_name, false));
  storemanager->StoreChunk(hex_chunk_name, maidsafe::PRIVATE, "");

  ASSERT_FALSE(storemanager->KeyUnique(hex_chunk_name, false));
  std::string result_str;
  ASSERT_EQ(0, storemanager->LoadChunk(hex_chunk_name, &result_str));
//  maidsafe::GetResponse load_res;
//  ASSERT_TRUE(load_res.ParseFromString(result_str));
//  ASSERT_EQ(kAck, static_cast<int>(load_res.result()));
  ASSERT_EQ(chunk_content, result_str);
}

TEST_F(StoreManagerTest, BEH_MAID_StoreBufferPacket) {
  std::string owner_id("Juan U. Smer");
  rsa_obj.GenerateKeys(4096);
  std::string public_key = rsa_obj.public_key();
  std::string private_key = rsa_obj.private_key();

  std::string bufferpacketname = crypto_obj.Hash(owner_id + "BUFFER", "",
                                 crypto::STRING_STRING, true);
  ASSERT_TRUE(storemanager->KeyUnique(bufferpacketname, false));
  packethandler::BufferPacket buffer_packet;
  packethandler::GenericPacket *ser_owner_info= buffer_packet.add_owner_info();
  packethandler::BufferPacketInfo buffer_packet_info;
  buffer_packet_info.set_owner(owner_id);
  buffer_packet_info.set_ownerpublickey(public_key);
  buffer_packet_info.set_online(false);

  std::string ser_info;
  buffer_packet_info.SerializeToString(&ser_info);
  ser_owner_info->set_data(ser_info);
  ser_owner_info->set_signature(crypto_obj.AsymSign(ser_info, "", private_key,
                                crypto::STRING_STRING));

  std::string ser_packet;
  buffer_packet.SerializeToString(&ser_packet);
  std::string ser_bp = ser_packet;

  std::string signed_public_key = crypto_obj.AsymSign(public_key, "",
                                  private_key, crypto::STRING_STRING);
  ASSERT_EQ(0, storemanager->StorePacket(bufferpacketname, ser_bp,
      packethandler::BUFFER, maidsafe::PRIVATE, ""));

  ASSERT_FALSE(storemanager->KeyUnique(bufferpacketname, false));
  storemanager->LoadPacket(bufferpacketname,
                           boost::bind(&FakeCallback::CallbackFunc, &cb, _1));
  wait_for_result_lsm(cb, mutex_);
  maidsafe::GetResponse load_res;
  ASSERT_TRUE(load_res.ParseFromString(cb.result_));
  ASSERT_EQ(kAck, static_cast<int>(load_res.result()));
  ASSERT_EQ(ser_bp, load_res.content());
}

TEST_F(StoreManagerTest, BEH_MAID_DeleteSystemPacketNotOwner) {
  packethandler::GenericPacket gp;
  rsa_obj.GenerateKeys(4096);
  std::string public_key = rsa_obj.public_key();
  std::string private_key = rsa_obj.private_key();
  gp.set_data("Generic System Packet Data");
  gp.set_signature(crypto_obj.AsymSign(gp.data(), "", private_key,
                   crypto::STRING_STRING));
  std::string gp_name = crypto_obj.Hash(gp.data() + gp.signature(), "",
                        crypto::STRING_STRING, true);
  std::string gp_content;
  gp.SerializeToString(&gp_content);
  std::string signed_public_key = crypto_obj.AsymSign(public_key, "",
                                  private_key, crypto::STRING_STRING);

  ASSERT_EQ(0, storemanager->StorePacket(gp_name, gp_content,
      packethandler::BUFFER, maidsafe::PRIVATE, ""));
  ASSERT_FALSE(storemanager->KeyUnique(gp_name, false));

  // Creating new public/private keys
  rsa_obj.GenerateKeys(4096);

  signed_public_key = crypto_obj.AsymSign(public_key, "", rsa_obj.private_key(),
                      crypto::STRING_STRING);
  std::string non_hex_gp_name;
  base::decode_from_hex(gp_name, &non_hex_gp_name);
  std::string signed_request = crypto_obj.AsymSign(crypto_obj.Hash(
      rsa_obj.public_key() + signed_public_key + non_hex_gp_name, "",
      crypto::STRING_STRING, false), "", rsa_obj.private_key(),
      crypto::STRING_STRING);

  storemanager->DeletePacket(gp_name, signed_request, rsa_obj.public_key(),
                             signed_public_key, maidsafe::SYSTEM_PACKET,
                             boost::bind(&FakeCallback::CallbackFunc, &cb, _1));
  wait_for_result_lsm(cb, mutex_);
  maidsafe::DeleteResponse del_res;
  ASSERT_TRUE(del_res.ParseFromString(cb.result_));
  ASSERT_EQ(kNack, static_cast<int>(del_res.result()));
  cb.Reset();
  del_res.Clear();

  ASSERT_FALSE(storemanager->KeyUnique(gp_name, false));
  signed_public_key = crypto_obj.AsymSign(rsa_obj.public_key(), "",
                      rsa_obj.private_key(), crypto::STRING_STRING);

  storemanager->DeletePacket(gp_name, signed_request, rsa_obj.public_key(),
                signed_public_key, maidsafe::SYSTEM_PACKET,
                boost::bind(&FakeCallback::CallbackFunc, &cb, _1));
  wait_for_result_lsm(cb, mutex_);
  ASSERT_TRUE(del_res.ParseFromString(cb.result_));
  ASSERT_EQ(kNack, static_cast<int>(del_res.result()));
  cb.Reset();
  del_res.Clear();
  ASSERT_FALSE(storemanager->KeyUnique(gp_name, false));
}

TEST_F(StoreManagerTest, BEH_MAID_DeleteBufferPacketNotOwner) {
  std::string owner_id("Juan U. Smer");
  rsa_obj.GenerateKeys(4096);
  std::string public_key = rsa_obj.public_key();
  std::string private_key = rsa_obj.private_key();

  std::string bufferpacketname = crypto_obj.Hash(owner_id + "BUFFER", "",
                                 crypto::STRING_STRING, true);
  ASSERT_TRUE(storemanager->KeyUnique(bufferpacketname, false));
  packethandler::BufferPacket buffer_packet;
  packethandler::GenericPacket *ser_owner_info= buffer_packet.add_owner_info();
  packethandler::BufferPacketInfo buffer_packet_info;
  buffer_packet_info.set_owner(owner_id);
  buffer_packet_info.set_ownerpublickey(public_key);
  buffer_packet_info.set_online(false);

  std::string ser_info;
  buffer_packet_info.SerializeToString(&ser_info);
  ser_owner_info->set_data(ser_info);
  ser_owner_info->set_signature(crypto_obj.AsymSign(ser_info, "", private_key,
                                crypto::STRING_STRING));

  std::string ser_packet;
  buffer_packet.SerializeToString(&ser_packet);
  std::string ser_bp = ser_packet;

  std::string signed_public_key = crypto_obj.AsymSign(public_key, "",
                                  private_key, crypto::STRING_STRING);

  ASSERT_EQ(0, storemanager->StorePacket(bufferpacketname, ser_bp,
      packethandler::BUFFER, maidsafe::PRIVATE, ""));
  ASSERT_FALSE(storemanager->KeyUnique(bufferpacketname, false));
  rsa_obj.ClearKeys();
  rsa_obj.GenerateKeys(4096);
  std::string signed_public_key1 = crypto_obj.AsymSign(rsa_obj.public_key(), "",
                                   rsa_obj.private_key(),
                                   crypto::STRING_STRING);
  std::string non_hex_bufferpacketname;
  base::decode_from_hex(bufferpacketname, &non_hex_bufferpacketname);
  std::string signed_request = crypto_obj.AsymSign(crypto_obj.Hash(
      rsa_obj.public_key() + signed_public_key1 + non_hex_bufferpacketname, "",
      crypto::STRING_STRING, false), "", rsa_obj.private_key(),
      crypto::STRING_STRING);
  storemanager->DeletePacket(bufferpacketname, signed_request,
      rsa_obj.public_key(), signed_public_key1, maidsafe::BUFFER_PACKET,
      boost::bind(&FakeCallback::CallbackFunc, &cb, _1));
  wait_for_result_lsm(cb, mutex_);
  maidsafe::DeleteResponse del_res;
  ASSERT_TRUE(del_res.ParseFromString(cb.result_));
  ASSERT_EQ(kNack, static_cast<int>(del_res.result()));
  cb.Reset();
  del_res.Clear();
}

TEST_F(StoreManagerTest, BEH_MAID_Add_Get_Clear_BufferPacket_Msgs) {
  std::string owner_id("Juan U. Smer");
  crypto::RsaKeyPair rsa_kp1;
  rsa_kp1.GenerateKeys(4096);
  rsa_obj.GenerateKeys(4096);
  std::string sig_private_key = rsa_kp1.private_key();
  std::string sig_public_key = rsa_kp1.public_key();

  std::string signed_public_key = crypto_obj.AsymSign(sig_public_key, "",
                                  sig_private_key, crypto::STRING_STRING);

  crypto::RsaKeyPair rsa_kp;
  rsa_kp.GenerateKeys(4096);
  std::string sender_mpid_privkey = rsa_kp.private_key();
  std::string sender_mpid_pubkey = rsa_kp.public_key();


  // storing MPID package for the sender
  packethandler::GenericPacket sender_mpid;
  sender_mpid.set_data(sender_mpid_pubkey);
  sender_mpid.set_signature(crypto_obj.AsymSign(sender_mpid_pubkey, "",
                            sig_private_key, crypto::STRING_STRING));
  std::string sender_ser_mpid;
  sender_mpid.SerializeToString(&sender_ser_mpid);
  std::string sender("sender");
  std::string sender_mpid_name = crypto_obj.Hash(sender, "",
                                                 crypto::STRING_STRING, true);

  ASSERT_EQ(0, storemanager->StorePacket(sender_mpid_name, sender_ser_mpid,
      packethandler::MPID, maidsafe::PRIVATE, ""));

  // rsa_obj.GenerateKeys(4096);
  std::string public_key = rsa_obj.public_key();
  std::string private_key = rsa_obj.private_key();
  ASSERT_NE(public_key, sender_mpid.data());
  ASSERT_NE(private_key, sender_mpid_privkey);

  std::string bufferpacketname = crypto_obj.Hash(owner_id + "BUFFER", "",
                                 crypto::STRING_STRING, true);
  ASSERT_TRUE(storemanager->KeyUnique(bufferpacketname, false));
  packethandler::BufferPacket buffer_packet;
  packethandler::GenericPacket *ser_owner_info = buffer_packet.add_owner_info();
  packethandler::BufferPacketInfo buffer_packet_info;
  buffer_packet_info.set_owner(owner_id);
  buffer_packet_info.set_ownerpublickey(public_key);
  buffer_packet_info.set_online(false);

  std::string ser_info;
  buffer_packet_info.SerializeToString(&ser_info);
  ser_owner_info->set_data(ser_info);
  ser_owner_info->set_signature(crypto_obj.AsymSign(ser_info, "", private_key,
                                crypto::STRING_STRING));
  std::string ser_packet;
  buffer_packet.SerializeToString(&ser_packet);
  std::string ser_bp = ser_packet;

  signed_public_key = crypto_obj.AsymSign(public_key, "", private_key,
                                          crypto::STRING_STRING);

  ASSERT_EQ(0, storemanager->StorePacket(bufferpacketname, ser_bp,
      packethandler::BUFFER, maidsafe::PRIVATE, ""));
  ASSERT_FALSE(storemanager->KeyUnique(bufferpacketname, false));
  storemanager->LoadPacket(bufferpacketname,
                           boost::bind(&FakeCallback::CallbackFunc, &cb, _1));
  wait_for_result_lsm(cb, mutex_);
  maidsafe::GetResponse load_res;
  ASSERT_TRUE(load_res.ParseFromString(cb.result_));
  ASSERT_EQ(kAck, static_cast<int>(load_res.result()));
  ASSERT_EQ(ser_bp, load_res.content());

  // Creating msgs to insert
  std::string key("AESkey");
  packethandler::BufferPacketMessage bpmsg;
  bpmsg.set_sender_id(sender);
  bpmsg.set_rsaenc_key(crypto_obj.AsymEncrypt(key, "", public_key,
                       crypto::STRING_STRING));
  bpmsg.set_aesenc_message(crypto_obj.SymmEncrypt("test msg", "",
                           crypto::STRING_STRING, key));
  bpmsg.set_type(packethandler::INSTANT_MSG);
  bpmsg.set_sender_public_key(sender_mpid_pubkey);
  std::string ser_bpmsg;
  bpmsg.SerializeToString(&ser_bpmsg);
  packethandler::GenericPacket bpmsg_gp;
  bpmsg_gp.set_data(ser_bpmsg);
  bpmsg_gp.set_signature(crypto_obj.AsymSign(ser_bpmsg, "", sender_mpid_privkey,
                         crypto::STRING_STRING));
  std::string ser_bpmsg_gp;
  bpmsg_gp.SerializeToString(&ser_bpmsg_gp);

  signed_public_key = crypto_obj.AsymSign(sender_mpid.data(), "",
                      sender_mpid_privkey, crypto::STRING_STRING);

  ASSERT_NE(0, storemanager->StorePacket("incorrect name", ser_bpmsg_gp,
      packethandler::BUFFER_MESSAGE, maidsafe::PRIVATE, ""));

  ASSERT_NE(0, storemanager->StorePacket(bufferpacketname, ser_bpmsg_gp,
      packethandler::BUFFER_MESSAGE, maidsafe::PRIVATE, ""));

  // Checking the bp has not been modified
  cb.Reset();
  storemanager->LoadPacket(bufferpacketname,
                           boost::bind(&FakeCallback::CallbackFunc, &cb, _1));
  wait_for_result_lsm(cb, mutex_);
  ASSERT_TRUE(load_res.ParseFromString(cb.result_));
  ASSERT_EQ(kAck, static_cast<int>(load_res.result()));
  ASSERT_EQ(ser_bp, load_res.content());
  cb.Reset();

  // Changing the msg type
  bpmsg.set_type(packethandler::ADD_CONTACT_RQST);
  bpmsg.set_sender_public_key(sender_mpid_pubkey);
  bpmsg.SerializeToString(&ser_bpmsg);
  bpmsg_gp.clear_data();
  bpmsg_gp.clear_signature();
  bpmsg_gp.set_data(ser_bpmsg);
  bpmsg_gp.set_signature(crypto_obj.AsymSign(ser_bpmsg, "", sender_mpid_privkey,
                         crypto::STRING_STRING));
  bpmsg_gp.SerializeToString(&ser_bpmsg_gp);
  ASSERT_EQ(0, storemanager->StorePacket(bufferpacketname, ser_bpmsg_gp,
      packethandler::BUFFER_MESSAGE, maidsafe::PRIVATE, ""));

  // Checking if new buffer packet has the msg
  storemanager->LoadPacket(bufferpacketname,
                           boost::bind(&FakeCallback::CallbackFunc, &cb, _1));
  wait_for_result_lsm(cb, mutex_);
  ASSERT_TRUE(load_res.ParseFromString(cb.result_));
  ASSERT_EQ(kAck, static_cast<int>(load_res.result()));
  ser_bp = load_res.content();
  packethandler::BufferPacket bp;
  bp.ParseFromString(ser_bp);
  ASSERT_EQ(1, bp.messages_size());
  load_res.Clear();
  cb.Reset();

  // Getting the msgs
  std::string sig_mpid_pubkey = crypto_obj.AsymSign(sender_mpid.data(), "",
                                sender_mpid_privkey, crypto::STRING_STRING);
  std::list<std::string> messages;
  ASSERT_NE(0, storemanager->LoadMessages(bufferpacketname, sender_mpid.data(),
      sig_mpid_pubkey, &messages));

  messages.push_back("Jibber");
  messages.push_back("Jabber");
  signed_public_key = crypto_obj.AsymSign(public_key, "", private_key,
                      crypto::STRING_STRING);
  ASSERT_EQ(0, storemanager->LoadMessages(bufferpacketname, public_key,
      signed_public_key, &messages));
  ASSERT_EQ(size_t(1), messages.size());
}
