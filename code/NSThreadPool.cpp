/**
 * @file NSThreadPool.cpp
 * @author tad (tadwang@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2021-06-16
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "NSThreadPool.hpp"

namespace tad {
void ThreadPool::Start() {
    if (_thNum > THREAD_NUMBER_MAX) {
        _thNum = THREAD_NUMBER_MAX;
    }

    _th = new std::thread[_thNum];
    for (uint32_t i = 0; i < _thNum; ++i) {
        _th[i] = std::thread(std::bind(&tad::ThreadPool::Run, this));
    }
}

void ThreadPool::Stop() {
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _isExit = true;
        _cond.notify_all();
    }

    for (uint32_t i = 0; i < _thNum; ++i) {
        if (_th[i].joinable()) {
            _th[i].join();
        }
    }
    delete[] _th;
}

void ThreadPool::Run() {
    while (!_isExit) {
        std::unique_lock<std::mutex> lock(_mtx);
        _cond.wait_for(lock, std::chrono::milliseconds(100));

        if (!_taskQ.empty()) {
            Task task = _taskQ.front();
            _taskQ.pop();

            lock.unlock();
            task();
        }
    }
}

};  // namespace tad
