/*
* ============================================================================
*
* Copyright [2010] maidsafe.net limited
*
* Description:  Class for caching client or vault's account status.
* Created:      2010-05-18
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

#include "maidsafe/accountstatusmanager.h"

namespace maidsafe {

AccountStatusManager::~AccountStatusManager() {
  StopUpdating();
}

void AccountStatusManager::StartUpdating(
    boost::function<void()> update_functor) {
  boost::mutex::scoped_lock lock(mutex_);
  update_functor_ = update_functor;
  work_.reset(new boost::asio::io_service::work(io_service_));
  worker_thread_ = boost::thread(&AccountStatusManager::Run, this);
  timer_.reset(new boost::asio::deadline_timer(io_service_,
                                               kMaxUpdateInterval_));
  timer_->async_wait(boost::bind(&AccountStatusManager::DoUpdate, this, _1));
}

void AccountStatusManager::StopUpdating() {
  boost::mutex::scoped_lock lock(mutex_);
  if (timer_.get()) {
    timer_->cancel();
    timer_.reset();
  }
  work_.reset();
  worker_thread_.join();
  update_functor_.clear();
}

void AccountStatusManager::Run() {
  while (true) {
    try {
      io_service_.run();
      break;
    } catch(const std::exception &e) {
#ifdef DEBUG
      printf("AccountStatusManager::Run, %s\n", e.what());
#endif
    }
  }
}

void AccountStatusManager::DoUpdate(const boost::system::error_code &error) {
  if (error) {
    if (error != boost::asio::error::operation_aborted) {
#ifdef DEBUG
      printf("AccountStatusManager::DoUpdate, %s\n", error.message().c_str());
#endif
    }
  } else {
    boost::mutex::scoped_lock lock(mutex_);
    if (!update_functor_.empty()) {
      if (!awaiting_update_result_) {
        awaiting_update_result_ = true;
        strand_.dispatch(update_functor_);
      }
      // Start new timer running
      timer_.reset(new boost::asio::deadline_timer(io_service_,
                                                   kMaxUpdateInterval_));
      timer_->async_wait(boost::bind(&AccountStatusManager::DoUpdate, this,
                                     _1));
    }
  }
}

void AccountStatusManager::SetAccountStatus(
    const boost::uint64_t &space_offered,
    const boost::uint64_t &space_given,
    const boost::uint64_t &space_taken) {
  boost::mutex::scoped_lock lock(mutex_);
  space_offered_ = space_offered;
  space_given_ = space_given;
  space_taken_ = space_taken;
  amendments_since_update_ = 0;
  awaiting_update_result_ = false;
}

void AccountStatusManager::AccountStatus(boost::uint64_t *space_offered,
                                         boost::uint64_t *space_given,
                                         boost::uint64_t *space_taken) {
  boost::mutex::scoped_lock lock(mutex_);
  *space_offered = space_offered_;
  *space_given = space_given_;
  *space_taken = space_taken_ + space_reserved_;
}

void AccountStatusManager::ReserveSpace(const boost::uint64_t &reserved_value) {
  boost::mutex::scoped_lock lock(mutex_);
  reserved_values_.insert(reserved_value);
  space_reserved_ += reserved_value;
}

void AccountStatusManager::UnReserveSpace(
    const boost::uint64_t &reserved_value) {
  boost::mutex::scoped_lock lock(mutex_);
  std::multiset<boost::uint64_t>::iterator it =
      reserved_values_.find(reserved_value);
  if (it != reserved_values_.end()) {
    reserved_values_.erase(it);
    space_reserved_ -= reserved_value;
  }
}

void AccountStatusManager::AmendmentDone(
    const AmendAccountRequest::Amendment &amendment_type,
    const boost::uint64_t &amendment_value) {
  boost::mutex::scoped_lock lock(mutex_);
  switch (amendment_type) {
    case AmendAccountRequest::kSpaceOffered:
      space_offered_ = amendment_value;
      break;
    case AmendAccountRequest::kSpaceGivenInc:
      space_given_ += amendment_value;
      break;
    case AmendAccountRequest::kSpaceGivenDec:
      if (amendment_value >= space_given_)
        space_given_ = 0;
      else
        space_given_ -= amendment_value;
      break;
    case AmendAccountRequest::kSpaceTakenInc:
      space_taken_ += amendment_value;
      break;
    case AmendAccountRequest::kSpaceTakenDec:
      if (amendment_value >= space_taken_)
        space_taken_ = 0;
      else
        space_taken_ -= amendment_value;
      break;
  }
  ++amendments_since_update_;
  if (amendments_since_update_ > kMaxAmendments_ && !awaiting_update_result_ &&
      !update_functor_.empty()) {
    // Try to reset current timer
    if (timer_->expires_from_now(kMaxUpdateInterval_) > 0) {
      // Reset successful - run update functor & start new asynchronous wait.
      awaiting_update_result_ = true;
      strand_.dispatch(update_functor_);
      timer_->async_wait(boost::bind(&AccountStatusManager::DoUpdate, this,
                                     _1));
    }
  }
}

void AccountStatusManager::UpdateFailed() {
  boost::mutex::scoped_lock lock(mutex_);
  awaiting_update_result_ = false;
  if (!update_functor_.empty()) {
    // Try to reset current timer
    if (timer_->expires_from_now(kFailureRetryInterval_) > 0) {
      // Reset successful - start new asynchronous wait.
      timer_->async_wait(boost::bind(&AccountStatusManager::DoUpdate, this,
                                     _1));
    }
  }
}

bool AccountStatusManager::AbleToStore(const boost::uint64_t &size) {
  boost::mutex::scoped_lock lock(mutex_);
  return space_taken_ + space_reserved_ + size <= space_offered_;
}

}  // namespace maidsafe
