//
//  Promise.h
//  promise
//
//  Created by Brian Sharon on 1/9/15.
//  Copyright (c) 2015 Brian Sharon. All rights reserved.
//

#pragma once

#include <functional>
#include <mutex>
#include <deque>
#include <string>

#include <boost/optional/optional.hpp>

using boost::optional;
using namespace std;

template<typename ResultType>
class Promise
{
public:
    enum class State
    {
        Pending,
        Fulfilled,
        Rejected
    };

    using ResultHandler = std::function<void(ResultType)>;

private:
    class InternalState
    {
    private:
        std::mutex _Mutex;
        State _State = State::Pending;
        std::deque<ResultHandler> _ResultHandlers;
        optional<ResultType> _Result;
    public:
        void Resolve(const ResultType & result)
        {
            std::lock_guard<std::mutex> lock(_Mutex);
            for (auto handler : _ResultHandlers)
            {
                handler(result);
            }
            _ResultHandlers.clear();
        }

        template <typename ResultType2>
        Promise<ResultType2> Then(std::function<ResultType2(const ResultType &)> func)
        {
            Promise<ResultType2> chainedResult;
            std::lock_guard<std::mutex> lock(_Mutex);
            _ResultHandlers.emplace_back([=](const ResultType & result) {
                func(result);
                // chainedResult.Resolve(func(result));
            });
            return chainedResult;
        }

    };
    std::shared_ptr<InternalState> _State;

public:
    // A default constructed promise
    Promise()
    :
    _State(make_shared<InternalState>())
    {
    }

    // A promise that immediately succeeds
    Promise(const ResultType & result)
    :
    Promise()
    {
        _State->Resolve(result);
    }

    void Resolve(const ResultType & result)
    {
        _State->Resolve(result);
    }
    
    template <typename ResultType2>
    Promise<ResultType2> Then(std::function<ResultType2(const ResultType &)> func)
    {
        return _State->Then(func);
    }

private:
    void DispatchNotifications()
    {
        if (_State == State::Fulfilled)
        {
            //            _ErrorHandlers.clear();
            //            ResultType result = _Result.get();
            //            for (auto & f : _ResultHandlers)
            //            {
            //                Dispatcher::DispatchAsync([=result]() {
            //                    f(result);
            //                });
            //            }
            //            _ResultHandlers.clear();
        }
    }
    
};
/*

class DefaultDispatcher
{
public:
    static void DispatchAsync(std::function<void()> func);
    static void DispatchAsyncMainThread(std::function<void()> func);
};

template<typename ResultType, typename ErrorType>
class DeferredObserver
{
public:

};

template<typename ResultType, typename ErrorType>
class DeferredState
{
public:
    enum class State
    {
        Pending,
        Fulfilled,
        Rejected
    };
    using ResultHandler = std::function<void(ResultType)>;
    using ErrorHandler = std::function<void(ErrorType)>;

private:
    std::mutex _Mutex;
    State _State = State::Pending;
    optional<ResultType> _Result;
    optional<ErrorType> _Error;
};

template<typename ResultType>
class ResultReceiver
{
public:
    // 2 specializations here: one returns a promise, the other one returns a result.
    template<typename ReceiverReturnType>
    ReceiverReturnType NotifyReceiver(const ResultType &);
};

template<typename ResultType, typename ErrorType = std::string, typename Dispatcher = DefaultDispatcher>
class Promise
{
public:
    enum class State
    {
        Pending,
        Fulfilled,
        Rejected
    };
    using ResultHandler = std::function<void(ResultType)>;
    using ErrorHandler = std::function<void(ErrorType)>;

private:
    std::mutex _Mutex;
    State _State = State::Pending;
    std::queue<ResultHandler> _ResultHandlers;
    std::queue<ErrorHandler> _ErrorHandlers;
    optional<ResultType> _Result;
    optional<ErrorType> _Error;

public:
    // A default constructed promise
    Promise()
    {
    }

    // A promise that immediately succeeds
    Promise(ResultType && result)
    {
        Fulfill(result);
    }

    // A promise that immediately fails
    Promise(ErrorType && error)
    {
        Reject(error);
    }

    void Fulfill(ResultType && result)
    {
        std::lock_guard<std::mutex> lock(_Mutex);
        {
            DebugAssert(_State = State::Pending);
            _State = State::Fulfilled;
            _Result = result;
        }

        DispatchNotifications();
    }

    void Then(ResultHandler resultHandler, ErrorHandler errorHandler = nullptr)
    {

    }

private:
    void DispatchNotifications()
    {
        if (_State == State::Fulfilled)
        {
//            _ErrorHandlers.clear();
//            ResultType result = _Result.get();
//            for (auto & f : _ResultHandlers)
//            {
//                Dispatcher::DispatchAsync([=result]() {
//                    f(result);
//                });
//            }
//            _ResultHandlers.clear();
        }
    }
    
};
*/