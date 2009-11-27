/*
* ============================================================================
*
* Copyright [2009] maidsafe.net limited
*
* Description:  Functional test for Clientbufferpackethandler
* Version:      1.0
* Created:      2009-11-18-10.09.29
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
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <maidsafe/general_messages.pb.h>

#include "maidsafe/clientbufferpackethandler.h"

#include "fs/filesystem.h"
#include "maidsafe/chunkstore.h"
#include "maidsafe/client/maidstoremanager.h"
#include "maidsafe/client/sessionsingleton.h"
#include "maidsafe/client/systempackets.h"
#include "maidsafe/kademlia_service_messages.pb.h"
#include "tests/maidsafe/localvaults.h"

static std::vector< boost::shared_ptr<maidsafe_vault::PDVault> > pdvaults_;
static const int kNetworkSize_ = 16;
static const int kTestK_ = 16;

class KadCB {
 public:
  KadCB() : result("") {}
  void CallbackFunc(const std::string &res) {
    base::GeneralResponse result_msg;
    if (!result_msg.ParseFromString(res)) {
      result_msg.set_result(kad::kRpcResultFailure);
    }
    result = result_msg.result();
  }
  void Reset() {
    result = "";
  }
  std::string result;
};

class BPCallback {
 public:
  BPCallback() : result(maidsafe::kGeneralError) {}
  void BPOperation_CB(const maidsafe::ReturnCode &res) {
    result = res;
  }
  void BPGetMsgs_CB(const maidsafe::ReturnCode &res,
    const std::list<maidsafe::ValidatedBufferPacketMessage> &rec_msgs) {
    result = res;
    msgs = rec_msgs;
  }
  void Reset() {
    result = maidsafe::kGeneralError;
    msgs.clear();
  }
  maidsafe::ReturnCode result;
  std::list<maidsafe::ValidatedBufferPacketMessage> msgs;
};

class CBPHandlerTest : public testing::Test {
 public:
  CBPHandlerTest() : trans(NULL), ch_man(NULL), knode(), cbph(NULL), bp_rpcs(),
      test_dir_(""),
      kad_config_file_(""), cryp(), keys(), cb_() {
    test_dir_ = std::string("CBPHTest") +
        boost::lexical_cast<std::string>(base::random_32bit_uinteger());
    kad_config_file_ = test_dir_ + std::string("/.kadconfig");
    keys.GenerateKeys(4096);
    boost::filesystem::create_directories(test_dir_);
  }
 protected:
  virtual void SetUp() {
    trans = new transport::Transport;
    ch_man = new rpcprotocol::ChannelManager(trans);
    knode.reset(new kad::KNode(ch_man, trans, kad::VAULT, keys.private_key(),
      keys.public_key(), false, false));
    bp_rpcs.reset(new maidsafe::BufferPacketRpcsImpl(trans, ch_man));
    cbph = new maidsafe::ClientBufferPacketHandler(bp_rpcs, knode);
    ASSERT_TRUE(ch_man->RegisterNotifiersToTransport());
    ASSERT_TRUE(trans->RegisterOnServerDown(
      boost::bind(&kad::KNode::HandleDeadRendezvousServer, knode, _1)));
    EXPECT_EQ(0, trans->Start(0));
    EXPECT_EQ(0, ch_man->Start());

    base::KadConfig kad_config;
    base::KadConfig::Contact *kad_contact = kad_config.add_contact();
    kad_contact->set_node_id(pdvaults_[0]->hex_node_id());
    kad_contact->set_ip(pdvaults_[0]->host_ip());
    kad_contact->set_port(pdvaults_[0]->host_port());
    kad_contact->set_local_ip(pdvaults_[0]->local_host_ip());
    kad_contact->set_local_port(pdvaults_[0]->local_host_port());
    std::fstream output1(kad_config_file_.c_str(),
                         std::ios::out | std::ios::trunc | std::ios::binary);
    ASSERT_TRUE(kad_config.SerializeToOstream(&output1));
    output1.close();

    knode->Join(kad_config_file_, boost::bind(&KadCB::CallbackFunc, &cb_, _1));
    while (cb_.result == "")
      boost::this_thread::sleep(boost::posix_time::milliseconds(500));
    ASSERT_EQ(kad::kRpcResultSuccess, cb_.result);
    ASSERT_TRUE(knode->is_joined());
  }

  virtual void TearDown() {
    knode->Leave();
    trans->Stop();
    ch_man->Stop();
    delete cbph;
    delete trans;
    delete ch_man;
    try {
      if (boost::filesystem::exists(test_dir_))
        boost::filesystem::remove_all(test_dir_);
    }
    catch(const std::exception &e) {
      printf("filesystem error: %s\n", e.what());
    }
  }

  transport::Transport *trans;
  rpcprotocol::ChannelManager *ch_man;
  boost::shared_ptr<kad::KNode> knode;
  maidsafe::ClientBufferPacketHandler *cbph;
  boost::shared_ptr<maidsafe::BufferPacketRpcs> bp_rpcs;
  std::string test_dir_, kad_config_file_;
  crypto::Crypto cryp;
  crypto::RsaKeyPair keys;
  KadCB cb_;
};

TEST_F(CBPHandlerTest, FUNC_MAID_TestBPHOperations) {
  crypto::RsaKeyPair keys;
  keys.GenerateKeys(4096);
  std::string owner_pubkey(keys.public_key()),
    owner_privkey(keys.private_key());
  BPCallback cb;
  std::string signed_pub_key = cryp.AsymSign(owner_pubkey, "",
    owner_privkey, crypto::STRING_STRING);
  maidsafe::BPInputParameters bpip = {cryp.Hash("publicname", "",
    crypto::STRING_STRING, false), owner_pubkey, owner_privkey};

  std::vector<std::string> users;
  cbph->ModifyOwnerInfo(bpip, 0, users, boost::bind(
    &BPCallback::BPOperation_CB, &cb, _1));
  while (cb.result == -1)
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
  ASSERT_EQ(maidsafe::kModifyBPError, cb.result);

  cb.Reset();
  cbph->CreateBufferPacket(bpip, boost::bind(&BPCallback::BPOperation_CB,
    &cb, _1));
  while (cb.result == -1)
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
  ASSERT_EQ(maidsafe::kSuccess, cb.result);
  boost::this_thread::sleep(boost::posix_time::seconds(10));
  keys.ClearKeys();
  keys.GenerateKeys(4096);


  std::string sender_id("user1");
  maidsafe::BPInputParameters bpip1 = {sender_id, keys.public_key(),
    keys.private_key()};
  std::string recv_id = cryp.Hash("publicname", "", crypto::STRING_STRING,
    false);

  cb.Reset();
  cbph->AddMessage(bpip1, owner_pubkey, recv_id, "Hello World",
    maidsafe::INSTANT_MSG,
    boost::bind(&BPCallback::BPOperation_CB, &cb, _1));
  while (cb.result == -1)
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
  ASSERT_EQ(maidsafe::kBPAddMessageError, cb.result);

  cb.Reset();
  cbph->GetMessages(bpip, boost::bind(&BPCallback::BPGetMsgs_CB, &cb, _1, _2));
  while (cb.result == -1)
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
  ASSERT_EQ(maidsafe::kSuccess, cb.result);
  ASSERT_TRUE(cb.msgs.empty());

  users.push_back(cryp.Hash(sender_id, "", crypto::STRING_STRING, false));
  cb.Reset();

  cbph->ModifyOwnerInfo(bpip, 0, users, boost::bind(
    &BPCallback::BPOperation_CB, &cb, _1));
  while (cb.result == -1)
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
  ASSERT_EQ(maidsafe::kSuccess, cb.result);

  cb.Reset();

  cbph->AddMessage(bpip1, owner_pubkey, recv_id, "Hello World",
    maidsafe::INSTANT_MSG,
    boost::bind(&BPCallback::BPOperation_CB, &cb, _1));
  while (cb.result == -1)
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
  ASSERT_EQ(maidsafe::kSuccess, cb.result);

  cb.Reset();
  cbph->GetMessages(bpip, boost::bind(&BPCallback::BPGetMsgs_CB, &cb, _1, _2));
  while (cb.result == -1)
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
  ASSERT_EQ(maidsafe::kSuccess, cb.result);
  ASSERT_EQ(size_t(1), cb.msgs.size());
  ASSERT_EQ("Hello World", cb.msgs.front().message());
  ASSERT_EQ(sender_id, cb.msgs.front().sender());

  // Request BPs not belonging to the sender
  bpip1.sign_id = bpip.sign_id;
  bpip1.public_key = bpip.public_key;
  cb.Reset();
  cbph->GetMessages(bpip1, boost::bind(&BPCallback::BPGetMsgs_CB, &cb, _1, _2));
  while (cb.result == -1)
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
  ASSERT_EQ(maidsafe::kBPMessagesRetrievalError, cb.result);

  cb.Reset();
  cbph->ModifyOwnerInfo(bpip1, 0, users, boost::bind(
    &BPCallback::BPOperation_CB, &cb, _1));
  while (cb.result == -1)
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
  ASSERT_EQ(maidsafe::kModifyBPError, cb.result);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::AddGlobalTestEnvironment(
      new localvaults::Env(kNetworkSize_, kTestK_, &pdvaults_));
  return RUN_ALL_TESTS();
}