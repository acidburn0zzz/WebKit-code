/*
 * Copyright (C) 2010-2019 Apple Inc. All rights reserved.
 * Copyright (C) 2017 Sony Interactive Entertainment Inc.
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

#include <wtf/Forward.h>
#include <wtf/ThreadSafeRefCounted.h>

#if USE(UNIX_DOMAIN_SOCKETS)
#include <wtf/unix/UnixFileDescriptor.h>
#endif

#if OS(WINDOWS)
#include <wtf/win/Win32Handle.h>
#endif

#if OS(DARWIN)
#include <wtf/MachSendRight.h>
#endif

namespace IPC {
class Decoder;
class Encoder;
class Connection;

template<typename, typename> struct ArgumentCoder;
}

namespace WebCore {
class FragmentedSharedBuffer;
class ProcessIdentity;
class SharedBuffer;
}

namespace WebKit {

enum class MemoryLedger { None, Default, Network, Media, Graphics, Neural };

class SharedMemoryHandle {
    WTF_MAKE_NONCOPYABLE(SharedMemoryHandle);
public:
    using Type =
#if USE(UNIX_DOMAIN_SOCKETS)
        UnixFileDescriptor;
#elif OS(DARWIN)
        MachSendRight;
#elif OS(WINDOWS)
        Win32Handle;
#endif

    SharedMemoryHandle() = default;
    SharedMemoryHandle(SharedMemoryHandle&&) = default;
    SharedMemoryHandle(SharedMemoryHandle::Type&&, size_t);

    SharedMemoryHandle& operator=(SharedMemoryHandle&&) = default;

    bool isNull() const;

    size_t size() const { return m_size; }

    // Take/Set ownership of the memory for jetsam purposes.
    void takeOwnershipOfMemory(MemoryLedger) const;
    void setOwnershipOfMemory(const WebCore::ProcessIdentity&, MemoryLedger) const;

#if USE(UNIX_DOMAIN_SOCKETS)
    UnixFileDescriptor releaseHandle();
#endif

private:
    friend struct IPC::ArgumentCoder<SharedMemoryHandle, void>;
    friend class SharedMemory;
#if USE(UNIX_DOMAIN_SOCKETS)
    friend class IPC::Connection;
#endif

    Type m_handle;
    size_t m_size { 0 };
};

class SharedMemory : public ThreadSafeRefCounted<SharedMemory> {
public:
    using Handle = SharedMemoryHandle;

    enum class Protection : bool { ReadOnly, ReadWrite };

    // FIXME: Change these factory functions to return Ref<SharedMemory> and crash on failure.
    static RefPtr<SharedMemory> allocate(size_t);
    static RefPtr<SharedMemory> copyBuffer(const WebCore::FragmentedSharedBuffer&);
    static RefPtr<SharedMemory> map(Handle&&, Protection);
#if USE(UNIX_DOMAIN_SOCKETS)
    static RefPtr<SharedMemory> wrapMap(void*, size_t, int fileDescriptor);
#elif OS(DARWIN)
    static RefPtr<SharedMemory> wrapMap(void*, size_t, Protection);
#endif
#if OS(WINDOWS)
    static RefPtr<SharedMemory> adopt(HANDLE, size_t, Protection);
#endif

    ~SharedMemory();

    std::optional<Handle> createHandle(Protection);

    size_t size() const { return m_size; }
    void* data() const
    {
        ASSERT(m_data);
        return m_data;
    }

#if OS(WINDOWS)
    HANDLE handle() const { return m_handle.get(); }
#endif

#if PLATFORM(COCOA)
    Protection protection() const { return m_protection; }
#endif

    Ref<WebCore::SharedBuffer> createSharedBuffer(size_t) const;

private:
#if OS(DARWIN)
    WTF::MachSendRight createSendRight(Protection) const;
#endif

    size_t m_size;
    void* m_data;
#if PLATFORM(COCOA)
    Protection m_protection;
#endif

#if USE(UNIX_DOMAIN_SOCKETS)
    UnixFileDescriptor m_fileDescriptor;
    bool m_isWrappingMap { false };
#elif OS(DARWIN)
    MachSendRight m_sendRight;
#elif OS(WINDOWS)
    Win32Handle m_handle;
#endif
};

} // namespace WebKit

namespace IPC {

template<> struct ArgumentCoder<WebKit::SharedMemoryHandle, void> {
    static void encode(Encoder&, WebKit::SharedMemoryHandle&&);
    static std::optional<WebKit::SharedMemoryHandle> decode(Decoder&);
};

} // namespace IPC
