#include <bits/chrono.h>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <stop_token>
#include <thread>
#include <vector>

#include <cds/init.h>

#include <cds/gc/hp.h>

#include <cds/container/moir_queue.h>

struct traits : public cds::container::msqueue::traits {
    using item_counter = cds::atomicity::item_counter;
};

template <typename T> using Queue = cds::container::MoirQueue<cds::gc::HP, T, traits>;
using namespace Rice::Util;
using namespace std::chrono_literals;

void test3pushFunc(std::stop_token token, Queue<int> &bag);
void test3popFunc(std::stop_token token, Queue<int> &bag);

bool test1();
bool test2();
bool test3();

int main() {

    using namespace std::chrono;

    cds::Initialize();
    {
        // Initialize Hazard Pointer singleton
        cds::gc::HP hpGC;
        // If main thread uses lock-free containers
        // the main thread should be attached to libcds infrastructure
        cds::threading::Manager::attachThread();

        if (!test1()) {
            std::cout << "test1 failed" << std::endl;
            return 1;
        }
        std::cout << "test1 passed" << std::endl;
        if (!test2()) {
            std::cout << "test2 failed" << std::endl;
            return 2;
        }
        std::cout << "test2 passed" << std::endl;
        auto start = std::chrono::system_clock::now();
        for (int i = 0; i < 1000; i++) {
            if (!test3()) {
                std::cout << "test3 failed" << std::endl;
                return 3;
            }
        }
        std::cout << "test3 passed in "
                  << duration_cast<milliseconds>(std::chrono::system_clock::now() - start)
                  << std::endl;

        cds::threading::Manager::detachThread();
    }
    // Terminate libcds
    cds::Terminate();
    return 0;
}

bool test1() {
    Queue<int> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);
    queue.push(4);
    queue.push(5);

    return queue.size() == 5;
}

bool test2() {
    Queue<int> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);
    queue.push(4);
    queue.push(5);
    queue.push(6);
    queue.push(7);
    queue.push(8);
    queue.push(9);
    queue.push(10);
    if (queue.size() != 10) {
        return false;
    }
    int v;
    while (!queue.empty()) {
        int value;
        queue.pop(value);
        std::cout << "value: " << value << std::endl;
    }
    return queue.empty();
}

bool test3() {
    Queue<int> queue;
    constexpr auto numThreads = 10;
    std::vector<ptr<std::jthread>> push_threads;
    std::vector<ptr<std::jthread>> pop_threads;

    for (int i = 0; i < numThreads; i++) {
        push_threads.push_back(std::make_shared<std::jthread>(test3pushFunc, std::ref(queue)));
        pop_threads.push_back(std::make_shared<std::jthread>(test3popFunc, std::ref(queue)));
    }

    std::this_thread::sleep_for(2ns);

    for (auto &t : push_threads) {
        t->request_stop();
        t->join();
    }

    for (auto &t : pop_threads) {
        t->request_stop();
        t->join();
    }
    // std::cout << std::endl;
    int val;
    while (!queue.empty())
        queue.pop(val);
    return queue.empty();
}

void test3pushFunc(std::stop_token token, Queue<int> &bag) {
    if (!cds::threading::Manager::isThreadAttached())
        cds::threading::Manager::attachThread();
    while (!token.stop_requested()) {
        int value;
        bag.pop(value);
        // putchar(value + '0');
    }
    cds::threading::Manager::detachThread();
}

void test3popFunc(std::stop_token token, Queue<int> &bag) {
    if (!cds::threading::Manager::isThreadAttached())
        cds::threading::Manager::attachThread();
    while (!token.stop_requested()) {
        bag.push(rand() % 10);
    }
    cds::threading::Manager::detachThread();
}