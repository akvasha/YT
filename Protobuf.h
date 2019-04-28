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

template <typename Message>
folly::Future<std::string> Serialize(std::shared_ptr<Message> message) {
    std::string result;
    message->SerializeToString(&result);
    return folly::makeFuture(result);
}

template <typename Message>
folly::Future<std::shared_ptr<Message>> Deserialize(std::string &message) {
    Message result;
    /*std::cout << "Started proc\n";
    fflush(stdout);*/
    result.ParseFromString(message);
    /*std::cout << "Finshed proc\n";
    fflush(stdout);*/
    return folly::makeFuture(std::make_shared<Message>(result));
}

void process(int i) {
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
}

template <typename Message>
void processMessageType(Message msg, int workers, int executions) {
    auto executor = folly::CPUThreadPoolExecutor(workers);
    std::vector<folly::Future<std::string>> futs;
    for (int i = 0; i < executions; ++i) {
        futs.emplace_back(folly::via(&executor, [msg]() {
            return Serialize(std::make_shared<Message>(msg));
        }));
    }
    folly::Future<std::vector<std::string>> resultSerialization = folly::collect(futs).get();
    std::cout << resultSerialization.value().size() << "\n";
    for (int i = 0; i < (int)resultSerialization.value().size(); ++i) {
        std::cout << resultSerialization.value()[i] << "\n";
    }
    std::cout << "Done\n";
    std::vector<folly::Future<std::shared_ptr<Message>>> output;
    fflush(stdout);
    for (int i = 0; i < executions; ++i) {
        output.emplace_back(folly::via(&executor, [&resultSerialization, i]() {
            return Deserialize<Message>(resultSerialization.value()[i]);
        }));
    }
    folly::Future<std::vector<std::shared_ptr<Message>>> resultDeserialization = folly::collect(output).get();
    for (int i = 0; i < (int)resultDeserialization.value().size(); ++i) {
        std::cout << resultDeserialization.value()[i]->s1() << "\n";
    }
}


void work2() {
    Message1 m1;
    m1.set_s1("kek1");
    m1.set_s2("kek2");
    m1.set_s3("kek3");
    m1.set_s4("kek4");
    m1.set_s5("kek5");
    m1.set_int1(1);
    m1.set_int2(2);
    m1.set_int3(3);
    m1.set_int4(4);
    m1.set_int5(5);
    processMessageType<Message1>(m1, 4, 40);
}

#endif //YT_PROTOBUF_H
