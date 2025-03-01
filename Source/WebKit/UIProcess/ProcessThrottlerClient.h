/*
 * Copyright (C) 2015 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ProcessThrottlerClient_h
#define ProcessThrottlerClient_h

#include <wtf/CompletionHandler.h>

namespace WebKit {

enum class IsSuspensionImminent : bool;
enum class ProcessThrottleState : uint8_t;

class ProcessThrottlerClient {
public:
    virtual ~ProcessThrottlerClient() { }

    virtual void sendPrepareToSuspend(IsSuspensionImminent, double remainingRunTime, CompletionHandler<void()>&&) = 0;
    enum ResumeReason : bool { ForegroundActivity, BackgroundActivity };
    virtual void sendProcessDidResume(ResumeReason) = 0;
    virtual void didChangeThrottleState(ProcessThrottleState) { };
    virtual ASCIILiteral clientName() const = 0;
    virtual String environmentIdentifier() const { return emptyString(); }
    virtual void prepareToDropLastAssertion(CompletionHandler<void()>&& completionHandler) { completionHandler(); }
};

} // namespace WebKit

#endif // ProcessThrottlerClient_h

