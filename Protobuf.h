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
folly::Future<std::string> Serialize(TSharedPtr<Message> message) {
    std::string result;
    message->SerializeToString(&result);
    return folly::makeFuture(result);
}

template <typename Message>
folly::Future<TSharedPtr<Message>> Deserialize(std::string &message) {
    TSharedPtr<Message> result = MakeShared<Message>();
    result->ParseFromString(message);
    return folly::makeFuture(result);
}

template <typename Message>
void processMessageType(Message msg, int workers, int executions) {
    std::chrono::time_point<std::chrono::system_clock> stime, etime;
    stime = std::chrono::system_clock::now();
    auto executor = folly::CPUThreadPoolExecutor(workers);
    auto ptr = MakeShared<Message>(msg);
    std::vector<folly::Future<std::string>> futs;
    for (int i = 0; i < executions; ++i) {
        futs.emplace_back(folly::via(&executor, [ptr]() {
            return Serialize(ptr);
        }));
    }
    folly::Future<std::vector<std::string>> resultSerialization = folly::collect(futs).get();
    std::vector<folly::Future<TSharedPtr<Message>>> output;
    for (int i = 0; i < executions; ++i) {
        output.emplace_back(folly::via(&executor, [&resultSerialization, i]() {
            return Deserialize<Message>(resultSerialization.value()[i]);
        }));
    }
    folly::Future<std::vector<TSharedPtr<Message>>> resultDeserialization = folly::collect(output).get();
    etime = std::chrono::system_clock::now();
    double execution_time = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(etime - stime).count()) / 1000; //counted in seconds
    std::cout << execution_time << std::endl;
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
    auto submsg1 = msg2.mutable_submsg1();
    submsg1->set_s1(createString());
    submsg1->set_s2(createString());
    submsg1->set_s3(createString());
    submsg1->set_s4(createString());
    submsg1->set_s5(createString());
    auto submsg2 = msg2.mutable_submsg2();
    submsg2->set_int1(rnd() % LLONG_MAX);
    submsg2->set_int2(rnd() % LLONG_MAX);
    submsg2->set_int3(rnd() % LLONG_MAX);
    submsg2->set_int4(rnd() % LLONG_MAX);
    submsg2->set_int5(rnd() % LLONG_MAX);
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
