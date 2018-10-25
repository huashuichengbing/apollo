/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef CYBER_TRANSPORT_DISPATCHER_INTRA_DISPATCHER_H_
#define CYBER_TRANSPORT_DISPATCHER_INTRA_DISPATCHER_H_

#include <iostream>
#include <memory>
#include <string>

#include "cyber/common/global_data.h"
#include "cyber/common/log.h"
#include "cyber/common/macros.h"
#include "cyber/message/raw_message.h"
#include "cyber/transport/dispatcher/dispatcher.h"

namespace apollo {
namespace cyber {
namespace transport {

class IntraDispatcher;
using IntraDispatcherPtr = std::shared_ptr<IntraDispatcher>;

class IntraDispatcher : public Dispatcher {
 public:
  virtual ~IntraDispatcher();

  template <typename MessageT>
  void OnMessage(uint64_t channel_id, const std::shared_ptr<MessageT>& message,
                 const MessageInfo& message_info);

  DECLARE_SINGLETON(IntraDispatcher)
};

template <typename MessageT>
void IntraDispatcher::OnMessage(uint64_t channel_id,
                                const std::shared_ptr<MessageT>& message,
                                const MessageInfo& message_info) {
  if (is_shutdown_.load()) {
    return;
  }
  ADEBUG << "intra on message, channel:"
         << common::GlobalData::GetChannelById(channel_id);
  ListenerHandlerBasePtr* handler_base = nullptr;
  if (msg_listeners_.Get(channel_id, &handler_base)) {
    if ((*handler_base)->IsRawMessage()) {
      auto handler =
          std::dynamic_pointer_cast<ListenerHandler<message::RawMessage>>(
              *handler_base);
      auto msg = std::make_shared<message::RawMessage>();
      message->SerializeToString(&msg->message);
      handler->Run(msg, message_info);
    } else {
      auto handler =
          std::dynamic_pointer_cast<ListenerHandler<MessageT>>(*handler_base);
      handler->Run(message, message_info);
    }
  }
}

}  // namespace transport
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TRANSPORT_DISPATCHER_INTRA_DISPATCHER_H_
