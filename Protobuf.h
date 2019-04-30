//
// Created by akvasha on 16.04.19.
//

#ifndef YT_PROTOBUF_H
#define YT_PROTOBUF_H

#include <vector>
#include "TSharedPtr.h"
#include <folly/futures/Future.h>
#include "folly/executors/ThreadedExecutor.h"
#include "folly/executors/CPUThreadPoolExecutor.h"
#include "messages.pb.h"
#include "boost/variant.hpp"
#include "random"

const int LENGTH = 7;
const int ALPHA = 26;
std::random_device rd;
std::mt19937 rnd(rd());

template <typename Message>
folly::Future<std::string> Serialize(std::shared_ptr<Message> message) {
    std::string result;
    message->SerializeToString(&result);
    return folly::makeFuture(result);
}

template <typename Message>
folly::Future<std::shared_ptr<Message>> Deserialize(std::string &message) {
    Message result;
    result.ParseFromString(message);
    return folly::makeFuture(std::make_shared<Message>(result));
}

/*void process(int i) {
    printf("Kek! %d\n", i);
    sleep(1);
}

void work() {
    auto executor = std::make_unique<folly::CPUThreadPoolExecutor>(4);
    std::vector<folly::Future<folly::Unit>> futs;
    for (int i = 0; i < 40; ++i) {
        futs.emplace_back(folly::via(executor.get(), [i]() { process(i); }));
    }
    folly::collectAll(futs).get();
    printf("Done.\n");
}*/

template <typename Message>
void processMessageType(Message msg, int workers, int executions) {
    auto executor = folly::CPUThreadPoolExecutor(workers);
    auto ptr = std::make_shared<Message>(msg);
    std::vector<folly::Future<std::string>> futs;
    for (int i = 0; i < executions; ++i) {
        futs.emplace_back(folly::via(&executor, [ptr]() {
            return Serialize(ptr);
        }));
    }
    folly::Future<std::vector<std::string>> resultSerialization = folly::collect(futs).get();
    std::vector<folly::Future<std::shared_ptr<Message>>> output;
    for (int i = 0; i < executions; ++i) {
        output.emplace_back(folly::via(&executor, [&resultSerialization, i]() {
            return Deserialize<Message>(resultSerialization.value()[i]);
        }));
    }
    folly::Future<std::vector<std::shared_ptr<Message>>> resultDeserialization = folly::collect(output).get();
}

std::string createString() {
    std::string result(LENGTH, ' ');
    for (int i = 0; i < LENGTH; ++i) {
        result[i] = 'a' + rnd() % ALPHA;
    }
    return result;
}

auto createMessages() {
    std::tuple<Message1, Message2, Message3, Message4> messages;

    Message1 msg1;
    msg1.set_s1(createString());
    msg1.set_s2(createString());
    msg1.set_s3(createString());
    msg1.set_s4(createString());
    msg1.set_s5(createString());
    msg1.set_int1(rnd() % LLONG_MAX);
    msg1.set_int2(rnd() % LLONG_MAX);
    msg1.set_int3(rnd() % LLONG_MAX);
    msg1.set_int4(rnd() % LLONG_MAX);
    msg1.set_int5(rnd() % LLONG_MAX);
    std::get<0>(messages) = msg1;

    Message2 msg2;
    std::get<1>(messages) = msg2;

    Message3 msg3;
    for (int i = 0; i < 5; ++i) {
        msg3.add_str(createString());
        msg3.add_int_val(rnd() % LLONG_MAX);
    }
    std::get<2>(messages) = msg3;

    Message4 msg4;
    for (int i = 0; i < 10; ++i) {
        SubMessage *subMsg = msg4.add_submsg();
        subMsg->set_str(createString());
        subMsg->set_int_val(rnd() % LLONG_MAX);
    }
    std::get<3>(messages) = msg4;
    return messages;
}

void processMain(int messageType, int workers, int executions) {
    auto messages = createMessages();
    switch (messageType) {
        case 0:
            processMessageType(std::get<0>(messages), workers, executions);
            break;
        case 1:
            processMessageType(std::get<1>(messages), workers, executions);
            break;
        case 2:
            processMessageType(std::get<2>(messages), workers, executions);
            break;
        case 3:
            processMessageType(std::get<3>(messages), workers, executions);
            break;
        default:
            break;
    }
}

#endif //YT_PROTOBUF_H
