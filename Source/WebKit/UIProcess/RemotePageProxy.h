/*
 * Copyright (C) 2022 Apple Inc. All rights reserved.
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

#pragma once

#include "MessageReceiver.h"
#include "MessageSender.h"
#include <WebCore/FrameIdentifier.h>
#include <WebCore/PageIdentifier.h>
#include <WebCore/RegistrableDomain.h>

namespace IPC {
class Connection;
class Decoder;
class Encoder;
}

namespace WebCore {
enum class FrameLoadType : uint8_t;
enum class HasInsecureContent : bool;
enum class MouseEventPolicy : uint8_t;

class CertificateInfo;
class ResourceResponse;
class ResourceRequest;

struct PolicyCheckIdentifierType;

using PolicyCheckIdentifier = ProcessQualified<ObjectIdentifier<PolicyCheckIdentifierType>>;
}

namespace WebKit {

class UserData;
class WebFrameProxy;
class WebPageProxy;
class WebProcessProxy;

struct FrameInfoData;

class RemotePageProxy : public RefCounted<RemotePageProxy>, public IPC::MessageReceiver, public IPC::MessageSender {
    WTF_MAKE_FAST_ALLOCATED;
public:
    static Ref<RemotePageProxy> create(WebPageProxy& page, WebProcessProxy& process, const WebCore::RegistrableDomain& domain) { return adoptRef(*new RemotePageProxy(page, process, domain)); }
    ~RemotePageProxy();

    WebProcessProxy& process() { return m_process.get(); }

private:
    RemotePageProxy(WebPageProxy&, WebProcessProxy&, const WebCore::RegistrableDomain&);
    IPC::Connection* messageSenderConnection() const final;
    uint64_t messageSenderDestinationID() const final;
    void didReceiveMessage(IPC::Connection&, IPC::Decoder&);
    bool didReceiveSyncMessage(IPC::Connection&, IPC::Decoder&, UniqueRef<IPC::Encoder>&);
    void decidePolicyForResponse(WebCore::FrameIdentifier, FrameInfoData&&, WebCore::PolicyCheckIdentifier, uint64_t navigationID, const WebCore::ResourceResponse&, const WebCore::ResourceRequest&, bool canShowMIMEType, const String& downloadAttribute, uint64_t listenerID);
    void didCommitLoadForFrame(WebCore::FrameIdentifier, FrameInfoData&&, WebCore::ResourceRequest&&, uint64_t navigationID, const String& mimeType, bool frameHasCustomContentProvider, WebCore::FrameLoadType, const WebCore::CertificateInfo&, bool usedLegacyTLS, bool privateRelayed, bool containsPluginDocument, WebCore::HasInsecureContent, WebCore::MouseEventPolicy, const UserData&);

    const WebCore::PageIdentifier m_webPageID;
    const Ref<WebProcessProxy> m_process;
    WeakPtr<WebPageProxy> m_page;
    const WebCore::RegistrableDomain m_domain;
};

}
