/*
* Copyright (C) 2021-2023 Apple Inc. All rights reserved.
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

#if USE(JPEGXL)

#include <algorithm>

#if PLATFORM(COCOA)
#import <pal/spi/cocoa/AppleJPEGXL/AppleJPEGXLSPI.h>
#else
#include <jxl/decode.h>
#endif

class JxlDecoderPtr {
public:
    JxlDecoderPtr() = default;

    JxlDecoderPtr(JxlDecoder* decoder)
        : m_decoder(decoder)
    {
    }

    JxlDecoderPtr(const JxlDecoderPtr&) = delete;

    JxlDecoderPtr(JxlDecoderPtr&& other)
        : m_decoder(other.m_decoder)
    {
        other.m_decoder = nullptr;
    }

    JxlDecoderPtr& operator=(const JxlDecoderPtr&) = delete;

    JxlDecoderPtr& operator=(JxlDecoderPtr&& other)
    {
        reset();
        std::swap(m_decoder, other.m_decoder);
        return *this;
    }

    ~JxlDecoderPtr()
    {
        reset();
    }

    void reset()
    {
        if (m_decoder)
            JxlDecoderDestroy(m_decoder);
        m_decoder = nullptr;
    }

    operator bool() const
    {
        return m_decoder;
    }

    JxlDecoder* get() const
    {
        return m_decoder;
    }

private:
    JxlDecoder* m_decoder { nullptr };
};

static inline JxlDecoderPtr JxlDecoderMake(const JxlMemoryManager* memoryManager)
{
    return JxlDecoderPtr(JxlDecoderCreate(memoryManager));
}

#endif // USE(JPEGXL)
