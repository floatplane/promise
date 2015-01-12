//
//  main.cpp
//  promise
//
//  Created by Brian Sharon on 1/9/15.
//  Copyright (c) 2015 Brian Sharon. All rights reserved.
//

#include <iostream>
#include "Promise.h"
#include <dispatch/dispatch.h>

Promise<int> sleep(float secs)
{
    __block Promise<int> p;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(secs * NSEC_PER_SEC)),
                   dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
                   ^{
                       p.Resolve(3);
                   });
    return p;
}

template <typename T>
void ResolveAfter(const Promise<T> & promise, const T & value, float secs)
{
    __block Promise<T> p(promise);
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(secs * NSEC_PER_SEC)),
                   dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
                   ^{
                       p.Resolve(value);
                   });
}

Promise<float> temp(const string & json)
{
    printf("received some 'json': '%s'", json.c_str());
    return Promise<float>(10.f);
}

int main(int argc, const char * argv[])
{
    auto sema = dispatch_semaphore_create(0);

    Promise<string> json;
    ResolveAfter(json, string("here's some JSON"), 5.f);
    auto end = json.Then([](const string & json) {
        printf("received some 'json': '%s'\n", json.c_str());
        return 10.f;
    }).Then([](const float &) {
        return true;
    }).Then([](const bool &) {
        return 10;
    }).Then([sema](const int & x) {
        printf("end of the line: int value %d\n", x);
        dispatch_semaphore_signal(sema);
        return string("you made it");
    });

    dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER);

//    end.Then([](const string & msg) {
//        printf("promise chain ended: %s\n", msg.c_str());
//    });

    printf("waiting for async method\n");
    sleep(2.f).Then([sema](const int & result) {
        printf("done sleeping!\n");
        return string("a result");
    }).Then([sema](const string & result) {
        printf("first lambda returned '%s'\n", result.c_str());
        dispatch_semaphore_signal(sema);
        return false;
    });

    dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER);

    printf("all done!\n");
    return 0;
}
