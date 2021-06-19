/**
 * @file NSThreadPool.hpp
 * @author tad (tadwang@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2021-06-16
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef _NS_THREAD_POOL_HPP_
#define _NS_THREAD_POOL_HPP_

#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>

namespace tad {
#if 0
/**
 * @brief 简单的任意类型
 * 
 */
class any final {
public:
    any() : _content(nullptr) {}

    ~any() {
        if (_content != nullptr) {
            delete _content;
            _content = nullptr;
        }
    }

    const std::type_info& type() const {
        return _content ? _content->type() : typeid(void);
    }

    template<typename _Tp>
    any(const _Tp& value) : _content(new holder<_Tp>(value)) {
    }

    any(const any& other): _content(other._content ? other._content->clone() : 0) {
    }

private:
    class placeholder {
    public:
        placeholder() noexcept = default;
        virtual ~placeholder() noexcept = default;

        /**
         * @brief 获取类型
         * 
         * @return const std::type_info&
         */
        virtual const std::type_info & type() const = 0;

        /**
         * @brief 获取值
         * 
         * @return placeholder*
         */
        virtual placeholder* clone() const = 0;
    };

    template<typename _Tp>
    class holder : public placeholder {
    public:
        holder(const _Tp& value) : _value(value) {

        }

        virtual const std::type_info& type() const {
            return typeid(_value);
        }

        virtual placeholder* clone() const {
            return new holder(_value);
        }

    private:
        const _Tp _value;
    
    public:
        template<typename __Tp>
        friend __Tp any_cast(const any& operand);
    };

    placeholder* _content;


public:
    template<typename __Tp>
    friend __Tp any_cast(const any& operand);
};

template<typename _Tp>
_Tp any_cast(const any& operand) {
    if (operand.type() != typeid(_Tp)) {
        throw std::bad_cast();
    }

    // return static_cast<any::holder<_Tp>*>(operand._content)->_value;
    return static_cast<any::holder<_Tp>*>(operand._content)->_value;
}
#endif

#define THREAD_NUMBER_MAX       (20)

/**
 * @brief 通用线程池
 * 
 */
class ThreadPool final {
public:
    using Task = std::function<void()>;

public:
    ThreadPool() noexcept = delete;
    virtual ~ThreadPool() noexcept = default;

    ThreadPool(uint32_t thNum) : _thNum(thNum), _isExit(false) {}

public:
    void Start();
    void Stop();

    template<typename F, typename... Args>
    void AddTask(F task(Args...), Args... args) {
        _taskQ.push([task, args...]() {
            task(args...);
        });
        _cond.notify_one();
    }

protected:
    void Run();

public:
    /**
     * @brief 禁止拷贝构造
     * 
     */
    ThreadPool(const ThreadPool&) noexcept = delete;
    ThreadPool& operator=(const ThreadPool&) noexcept = delete;

private:
    uint32_t _thNum;
    std::mutex _mtx;
    std::thread* _th;
    std::condition_variable _cond;

private:
    // std::priority_queue<boost::any> _taskQ;
    std::queue<Task> _taskQ;

private:
    bool _isExit;
};
}  // namespace tad

#endif  // _NS_THREAD_POOL_HPP_