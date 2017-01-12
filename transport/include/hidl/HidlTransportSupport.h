/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_HIDL_TRANSPORT_SUPPORT_H
#define ANDROID_HIDL_TRANSPORT_SUPPORT_H

#include <hidl/HidlBinderSupport.h>
#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportUtils.h>

namespace android {
namespace hardware {

/* Configures the threadpool used for handling incoming RPC calls in this process.
 *
 * This method MUST be called before interacting with any HIDL interfaces,
 * including the IFoo::getService and IFoo::registerAsService methods.
 *
 * @param maxThreads maximum number of threads in this process
 * @param callerWillJoin whether the caller will join the threadpool later.
 *
 * Note that maxThreads must include the caller thread if callerWillJoin is true;
 *
 * If you want to create a threadpool of 5 threads, without the caller ever joining:
 *   configureRpcThreadPool(5, false);
 * If you want to create a threadpool of 1 thread, with the caller joining:
 *   configureRpcThreadPool(1, true); // transport won't launch any threads by itself
 *
 */
inline void configureRpcThreadpool(size_t maxThreads, bool callerWillJoin) {
    // TODO(b/32756130) this should be transport-dependent
    configureBinderRpcThreadpool(maxThreads, callerWillJoin);
}

/* Joins a threadpool that you configured earlier with
 * configureRpcThreadPool(x, true);
 */
inline void joinRpcThreadpool() {
    // TODO(b/32756130) this should be transport-dependent
    joinBinderRpcThreadpool();
}

// cast the interface IParent to IChild.
// Return nullptr if parent is null or any failure.
template<typename IChild, typename IParent, typename BpChild, typename BpParent>
sp<IChild> castInterface(sp<IParent> parent, const char *childIndicator) {
    if (parent.get() == nullptr) {
        // casts always succeed with nullptrs.
        return nullptr;
    }
    bool canCast = canCastInterface(parent.get(), childIndicator);

    if (!canCast) {
        return sp<IChild>(nullptr); // cast failed.
    }
    // TODO b/32001926 Needs to be fixed for socket mode.
    if (parent->isRemote()) {
        // binderized mode. Got BpChild. grab the remote and wrap it.
        return sp<IChild>(new BpChild(toBinder<IParent, BpParent>(parent)));
    }
    // Passthrough mode. Got BnChild and BsChild.
    return sp<IChild>(static_cast<IChild *>(parent.get()));
}

}  // namespace hardware
}  // namespace android


#endif  // ANDROID_HIDL_TRANSPORT_SUPPORT_H