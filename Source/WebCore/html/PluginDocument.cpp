/*
 * Copyright (C) 2006-2022 Apple Inc. All rights reserved.
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
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "PluginDocument.h"

#include "DocumentLoader.h"
#include "FrameLoader.h"
#include "HTMLBodyElement.h"
#include "HTMLEmbedElement.h"
#include "HTMLHtmlElement.h"
#include "HTMLNames.h"
#include "LocalFrame.h"
#include "LocalFrameLoaderClient.h"
#include "LocalFrameView.h"
#include "PluginViewBase.h"
#include "RawDataDocumentParser.h"
#include "RenderEmbeddedObject.h"
#include <wtf/IsoMallocInlines.h>

namespace WebCore {

WTF_MAKE_ISO_ALLOCATED_IMPL(PluginDocument);

using namespace HTMLNames;

// FIXME: Share more code with MediaDocumentParser.
class PluginDocumentParser final : public RawDataDocumentParser {
public:
    static Ref<PluginDocumentParser> create(PluginDocument& document)
    {
        return adoptRef(*new PluginDocumentParser(document));
    }

private:
    PluginDocumentParser(Document& document)
        : RawDataDocumentParser(document)
    {
    }

    void appendBytes(DocumentWriter&, const uint8_t*, size_t) final;
    void createDocumentStructure();

    WeakPtr<HTMLEmbedElement, WeakPtrImplWithEventTargetData> m_embedElement;
};

void PluginDocumentParser::createDocumentStructure()
{
    auto& document = downcast<PluginDocument>(*this->document());

    auto rootElement = HTMLHtmlElement::create(document);
    document.appendChild(rootElement);
    rootElement->insertedByParser();
    rootElement->setInlineStyleProperty(CSSPropertyHeight, 100, CSSUnitType::CSS_PERCENTAGE);
    rootElement->setInlineStyleProperty(CSSPropertyWidth, 100, CSSUnitType::CSS_PERCENTAGE);

    if (document.frame())
        document.frame()->injectUserScripts(UserScriptInjectionTime::DocumentStart);

#if PLATFORM(IOS_FAMILY)
    // Should not be able to zoom into standalone plug-in documents.
    document.processViewport("user-scalable=no"_s, ViewportArguments::PluginDocument);
#endif

    auto body = HTMLBodyElement::create(document);
    body->setAttributeWithoutSynchronization(marginwidthAttr, "0"_s);
    body->setAttributeWithoutSynchronization(marginheightAttr, "0"_s);
#if PLATFORM(IOS_FAMILY)
    body->setAttribute(styleAttr, "background-color: rgb(217,224,233); height: 100%; width: 100%; overflow:hidden; margin: 0"_s);
#else
    body->setAttribute(styleAttr, "background-color: rgb(38,38,38); height: 100%; width: 100%; overflow:hidden; margin: 0"_s);
#endif

    rootElement->appendChild(body);
        
    auto embedElement = HTMLEmbedElement::create(document);
        
    m_embedElement = embedElement.get();
    embedElement->setAttributeWithoutSynchronization(widthAttr, "100%"_s);
    embedElement->setAttributeWithoutSynchronization(heightAttr, "100%"_s);
    
    embedElement->setAttributeWithoutSynchronization(nameAttr, "plugin"_s);
    embedElement->setAttributeWithoutSynchronization(srcAttr, AtomString { document.url().string() });
    
    ASSERT(document.loader());
    if (RefPtr loader = document.loader())
        m_embedElement->setAttributeWithoutSynchronization(typeAttr, AtomString { loader->writer().mimeType() });

    document.setPluginElement(*m_embedElement);

    body->appendChild(embedElement);
    document.setHasVisuallyNonEmptyCustomContent();
}

void PluginDocumentParser::appendBytes(DocumentWriter&, const uint8_t*, size_t)
{
    if (m_embedElement)
        return;

    createDocumentStructure();

    RefPtr frame = document()->frame();
    if (!frame)
        return;

    document()->updateLayout();

    // Below we assume that renderer->widget() to have been created by
    // document()->updateLayout(). However, in some cases, updateLayout() will 
    // recurse too many times and delay its post-layout tasks (such as creating
    // the widget). Here we kick off the pending post-layout tasks so that we
    // can synchronously redirect data to the plugin.
    frame->view()->flushAnyPendingPostLayoutTasks();

    if (auto renderer = m_embedElement->renderWidget()) {
        if (RefPtr widget = renderer->widget()) {
            frame->loader().client().redirectDataToPlugin(*widget);

            // In a plugin document, the main resource is the plugin. If we have a null widget, that means
            // the loading of the plugin was cancelled, which gives us a null mainResourceLoader(), so we
            // need to have this call in a null check of the widget or of mainResourceLoader().
            if (auto loader = frame->loader().activeDocumentLoader())
                loader->setMainResourceDataBufferingPolicy(DataBufferingPolicy::DoNotBufferData);
        }
    }
}

PluginDocument::PluginDocument(LocalFrame& frame, const URL& url)
    : HTMLDocument(&frame, frame.settings(), url, { }, { DocumentClass::Plugin })
{
    setCompatibilityMode(DocumentCompatibilityMode::NoQuirksMode);
    lockCompatibilityMode();
}

Ref<DocumentParser> PluginDocument::createParser()
{
    return PluginDocumentParser::create(*this);
}

PluginViewBase* PluginDocument::pluginWidget()
{
    if (!m_pluginElement)
        return nullptr;
    auto* renderer = dynamicDowncast<RenderEmbeddedObject>(m_pluginElement->renderer());
    if (!renderer)
        return nullptr;
    return dynamicDowncast<PluginViewBase>(renderer->widget());
}

void PluginDocument::setPluginElement(HTMLPlugInElement& element)
{
    m_pluginElement = &element;
}

void PluginDocument::detachFromPluginElement()
{
    // Release the plugin Element so that we don't have a circular reference.
    m_pluginElement = nullptr;
}

void PluginDocument::cancelManualPluginLoad()
{
    // PluginDocument::cancelManualPluginLoad should only be called once, but there are issues
    // with how many times we call beforeload on object elements. <rdar://problem/8441094>.
    if (!shouldLoadPluginManually())
        return;

    auto& frameLoader = frame()->loader();
    if (auto documentLoader = frameLoader.activeDocumentLoader())
        documentLoader->cancelMainResourceLoad(frameLoader.cancelledError(documentLoader->request()));
    m_shouldLoadPluginManually = false;
}

}
