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
    string q("abc");
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(secs * NSEC_PER_SEC)),
                   dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
                   ^{
                       p.Resolve(3);
                   });
    return p;
}

int main(int argc, const char * argv[])
{
    auto sema = dispatch_semaphore_create(0);

    sleep(5.f).Then<bool>([sema](const int & result) {
        printf("done sleeping!\n");
        dispatch_semaphore_signal(sema);
        return true;
    });

    dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER);

    printf("all done!\n");
    return 0;
}
