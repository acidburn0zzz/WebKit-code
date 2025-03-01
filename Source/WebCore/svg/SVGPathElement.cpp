/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2018-2019 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "SVGPathElement.h"

#include "LegacyRenderSVGPath.h"
#include "RenderSVGPath.h"
#include "RenderSVGResource.h"
#include "SVGDocumentExtensions.h"
#include "SVGElementTypeHelpers.h"
#include "SVGMPathElement.h"
#include "SVGNames.h"
#include "SVGPathUtilities.h"
#include "SVGPoint.h"
#include <wtf/IsoMallocInlines.h>

namespace WebCore {

WTF_MAKE_ISO_ALLOCATED_IMPL(SVGPathElement);

inline SVGPathElement::SVGPathElement(const QualifiedName& tagName, Document& document)
    : SVGGeometryElement(tagName, document, makeUniqueRef<PropertyRegistry>(*this))
{
    ASSERT(hasTagName(SVGNames::pathTag));

    static std::once_flag onceFlag;
    std::call_once(onceFlag, [] {
        PropertyRegistry::registerProperty<SVGNames::dAttr, &SVGPathElement::m_pathSegList>();
    });
}

Ref<SVGPathElement> SVGPathElement::create(const QualifiedName& tagName, Document& document)
{
    return adoptRef(*new SVGPathElement(tagName, document));
}

void SVGPathElement::attributeChanged(const QualifiedName& name, const AtomString& oldValue, const AtomString& newValue, AttributeModificationReason attributeModificationReason)
{
    SVGGeometryElement::attributeChanged(name, oldValue, newValue, attributeModificationReason);
    if (name == SVGNames::dAttr) {
        if (!m_pathSegList->baseVal()->parse(newValue))
            document().accessSVGExtensions().reportError("Problem parsing d=\"" + newValue + "\"");
    }
}

void SVGPathElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (PropertyRegistry::isKnownAttribute(attrName)) {
        ASSERT(attrName == SVGNames::dAttr);
        InstanceInvalidationGuard guard(*this);
        invalidateMPathDependencies();

#if ENABLE(LAYER_BASED_SVG_ENGINE)
        if (auto* path = dynamicDowncast<RenderSVGPath>(renderer()))
            path->setNeedsShapeUpdate();
#endif
        if (auto* path = dynamicDowncast<LegacyRenderSVGPath>(renderer()))
            path->setNeedsShapeUpdate();

        updateSVGRendererForElementChange();
        return;
    }

    SVGGeometryElement::svgAttributeChanged(attrName);
}

void SVGPathElement::invalidateMPathDependencies()
{
    // <mpath> can only reference <path> but this dependency is not handled in
    // markForLayoutAndParentResourceInvalidation so we update any mpath dependencies manually.
    for (auto& element : referencingElements()) {
        if (is<SVGMPathElement>(element))
            downcast<SVGMPathElement>(element.get()).targetPathChanged();
    }
}

Node::InsertedIntoAncestorResult SVGPathElement::insertedIntoAncestor(InsertionType insertionType, ContainerNode& parentOfInsertedTree)
{
    auto result = SVGGeometryElement::insertedIntoAncestor(insertionType, parentOfInsertedTree);
    invalidateMPathDependencies();
    return result;
}

void SVGPathElement::removedFromAncestor(RemovalType removalType, ContainerNode& oldParentOfRemovedTree)
{
    SVGGeometryElement::removedFromAncestor(removalType, oldParentOfRemovedTree);
    invalidateMPathDependencies();
}

float SVGPathElement::getTotalLength() const
{
    return getTotalLengthOfSVGPathByteStream(pathByteStream());
}

ExceptionOr<Ref<SVGPoint>> SVGPathElement::getPointAtLength(float distance) const
{
    // Spec: Clamp distance to [0, length].
    distance = clampTo<float>(distance, 0, getTotalLength());

    // Spec: Return a newly created, detached SVGPoint object.
    return SVGPoint::create(getPointAtLengthOfSVGPathByteStream(pathByteStream(), distance));
}

unsigned SVGPathElement::getPathSegAtLength(float length) const
{
    return getSVGPathSegAtLengthFromSVGPathByteStream(pathByteStream(), length);
}

FloatRect SVGPathElement::getBBox(StyleUpdateStrategy styleUpdateStrategy)
{
    if (styleUpdateStrategy == AllowStyleUpdate)
        document().updateLayoutIgnorePendingStylesheets();

    // FIXME: Eventually we should support getBBox for detached elements.
    // FIXME: If the path is null it means we're calling getBBox() before laying out this element,
    // which is an error.

#if ENABLE(LAYER_BASED_SVG_ENGINE)
        if (auto* path = dynamicDowncast<RenderSVGPath>(renderer()); path && path->hasPath())
            return path->path().boundingRect();
#endif
        if (auto* path = dynamicDowncast<LegacyRenderSVGPath>(renderer()); path && path->hasPath())
            return path->path().boundingRect();

    return { };
}

RenderPtr<RenderElement> SVGPathElement::createElementRenderer(RenderStyle&& style, const RenderTreePosition&)
{
#if ENABLE(LAYER_BASED_SVG_ENGINE)
    if (document().settings().layerBasedSVGEngineEnabled())
        return createRenderer<RenderSVGPath>(*this, WTFMove(style));
#endif
    return createRenderer<LegacyRenderSVGPath>(*this, WTFMove(style));
}

}
