#include "Rice/Util/ConcurrentList.hpp"
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <stop_token>
#include <thread>
#include <vector>

template <typename T> using Queue = rigtorp::SPSCQueue<T>;
using namespace Rice::Util;
using namespace std::chrono_literals;

void test3pushFunc(std::stop_token token, Queue<int> &bag);
void test3popFunc(std::stop_token token, Queue<int> &bag);

bool test1();
bool test2();
bool test3();

int main() {
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
    for (int i = 0; i < 100; i++) {
        if (!test3()) {
            std::cout << "test3 failed" << std::endl;
            return 3;
        }
    }
    std::cout << "test3 passed" << std::endl;

    return 0;
}

bool test1() {
    Queue<int> queue(10);
    queue.push(1);
    queue.push(2);
    queue.push(3);
    queue.push(4);
    queue.push(5);

    return queue.size() == 5;
}

bool test2() {
    Queue<int> queue(10);
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
        std::cout << "value: " << *queue.front() << std::endl;
        queue.pop();
    }
    return queue.empty();
}

bool test3() {
    Queue<int> queue(10000);
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
    while (!queue.empty())
        queue.pop();
    return queue.empty();
}

void test3pushFunc(std::stop_token token, Queue<int> &bag) {
    while (!token.stop_requested()) {
        // int value;
        bag.pop();
        // putchar(value + '0');
    }
}

void test3popFunc(std::stop_token token, Queue<int> &bag) {
    while (!token.stop_requested()) {
        bag.push(rand() % 10);
    }
}