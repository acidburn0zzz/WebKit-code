/*
 * Copyright (C) 2004-2022 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "CaretAnimator.h"

#include "GraphicsContext.h"
#include "Page.h"

namespace WebCore {

Page* CaretAnimator::page() const
{
    if (auto* document = m_client.document())
        return document->page();
    
    return nullptr;
}

void CaretAnimator::stop(CaretAnimatorStopReason)
{
    if (!m_isActive)
        return;

    didEnd();
}

void CaretAnimator::serviceCaretAnimation(ReducedResolutionSeconds timestamp)
{
    if (!isActive())
        return;

    updateAnimationProperties(timestamp);
}

void CaretAnimator::scheduleAnimation()
{
    if (auto* page = this->page())
        page->scheduleRenderingUpdate(RenderingUpdateStep::CaretAnimation);
}

void CaretAnimator::paint(const Node&, GraphicsContext& context, const FloatRect& caret, const Color& color, const LayoutPoint&, const std::optional<VisibleSelection>&) const
{
    context.fillRect(caret, color);
}

LayoutRect CaretAnimator::caretRepaintRectForLocalRect(LayoutRect rect) const
{
    return rect;
}

} // namespace WebCore
