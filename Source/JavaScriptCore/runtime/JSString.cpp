/*
 *  Copyright (C) 1999-2002 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2004-2021 Apple Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include "JSString.h"

#include "JSGlobalObjectFunctions.h"
#include "JSGlobalObjectInlines.h"
#include "JSObjectInlines.h"
#include "StringObject.h"
#include "StrongInlines.h"
#include "StructureInlines.h"

namespace JSC {
    
const ClassInfo JSString::s_info = { "string"_s, nullptr, nullptr, nullptr, CREATE_METHOD_TABLE(JSString) };

Structure* JSString::createStructure(VM& vm, JSGlobalObject* globalObject, JSValue proto)
{
    return Structure::create(vm, globalObject, proto, defaultTypeInfo(), info());
}

JSString* JSString::createEmptyString(VM& vm)
{
    JSString* newString = new (NotNull, allocateCell<JSString>(vm)) JSString(vm, *StringImpl::empty());
    newString->finishCreation(vm);
    return newString;
}

template<>
void JSRopeString::RopeBuilder<RecordOverflow>::expand()
{
    RELEASE_ASSERT(!this->hasOverflowed());
    ASSERT(m_strings.size() == JSRopeString::s_maxInternalRopeLength);
    static_assert(3 == JSRopeString::s_maxInternalRopeLength);
    ASSERT(m_length);
    ASSERT(asString(m_strings.at(0))->length());
    ASSERT(asString(m_strings.at(1))->length());
    ASSERT(asString(m_strings.at(2))->length());

    JSString* string = JSRopeString::create(m_vm, asString(m_strings.at(0)), asString(m_strings.at(1)), asString(m_strings.at(2)));
    ASSERT(string->length() == m_length);
    m_strings.clear();
    m_strings.append(string);
}

void JSString::dumpToStream(const JSCell* cell, PrintStream& out)
{
    const JSString* thisObject = jsCast<const JSString*>(cell);
    out.printf("<%p, %s, [%u], ", thisObject, thisObject->className().characters(), thisObject->length());
    uintptr_t pointer = thisObject->fiberConcurrently();
    if (pointer & isRopeInPointer) {
        if (pointer & JSRopeString::isSubstringInPointer)
            out.printf("[substring]");
        else
            out.printf("[rope]");
    } else {
        if (WTF::StringImpl* ourImpl = bitwise_cast<StringImpl*>(pointer)) {
            if (ourImpl->is8Bit())
                out.printf("[8 %p]", ourImpl->characters8());
            else
                out.printf("[16 %p]", ourImpl->characters16());
        }
    }
    out.printf(">");
}

bool JSString::equalSlowCase(JSGlobalObject* globalObject, JSString* other) const
{
    return equalInline(globalObject, other);
}

size_t JSString::estimatedSize(JSCell* cell, VM& vm)
{
    JSString* thisObject = asString(cell);
    uintptr_t pointer = thisObject->fiberConcurrently();
    if (pointer & isRopeInPointer)
        return Base::estimatedSize(cell, vm);
    return Base::estimatedSize(cell, vm) + bitwise_cast<StringImpl*>(pointer)->costDuringGC();
}

template<typename Visitor>
void JSString::visitChildrenImpl(JSCell* cell, Visitor& visitor)
{
    JSString* thisObject = asString(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    Base::visitChildren(thisObject, visitor);
    
    uintptr_t pointer = thisObject->fiberConcurrently();
    if (pointer & isRopeInPointer) {
        if (pointer & JSRopeString::isSubstringInPointer) {
            visitor.appendUnbarriered(static_cast<JSRopeString*>(thisObject)->fiber1());
            return;
        }
        for (unsigned index = 0; index < JSRopeString::s_maxInternalRopeLength; ++index) {
            JSString* fiber = nullptr;
            switch (index) {
            case 0:
                fiber = bitwise_cast<JSString*>(pointer & JSRopeString::stringMask);
                break;
            case 1:
                fiber = static_cast<JSRopeString*>(thisObject)->fiber1();
                break;
            case 2:
                fiber = static_cast<JSRopeString*>(thisObject)->fiber2();
                break;
            default:
                ASSERT_NOT_REACHED();
                return;
            }
            if (!fiber)
                break;
            visitor.appendUnbarriered(fiber);
        }
        return;
    }
    if (StringImpl* impl = bitwise_cast<StringImpl*>(pointer))
        visitor.reportExtraMemoryVisited(impl->costDuringGC());
}

DEFINE_VISIT_CHILDREN(JSString);

template<typename CharacterType>
void JSRopeString::resolveRopeInternalNoSubstring(CharacterType* buffer) const
{
    resolveToBuffer(this, buffer, length());
}

AtomString JSRopeString::resolveRopeToAtomString(JSGlobalObject* globalObject) const
{
    VM& vm = globalObject->vm();
    auto scope = DECLARE_THROW_SCOPE(vm);

    auto convertToAtomString = [](const String& string) -> AtomString {
        ASSERT(!string.impl() || string.impl()->isAtom());
        return static_cast<AtomStringImpl*>(string.impl());
    };

    if (length() > maxLengthForOnStackResolve) {
        scope.release();
        return convertToAtomString(resolveRopeWithFunction(globalObject, [&] (Ref<StringImpl>&& newImpl) {
            return AtomStringImpl::add(newImpl.ptr());
        }));
    }

    AtomString atomString;
    if (!isSubstring()) {
        if (is8Bit()) {
            LChar buffer[maxLengthForOnStackResolve];
            resolveRopeInternalNoSubstring(buffer);
            atomString = AtomString(buffer, length());
        } else {
            UChar buffer[maxLengthForOnStackResolve];
            resolveRopeInternalNoSubstring(buffer);
            atomString = AtomString(buffer, length());
        }
    } else
        atomString = StringView { substringBase()->valueInternal() }.substring(substringOffset(), length()).toAtomString();

    // If we resolved a string that didn't previously exist, notify the heap that we've grown.
    if (atomString.impl()->hasOneRef())
        vm.heap.reportExtraMemoryAllocated(atomString.impl()->cost());

    convertToNonRope(String { atomString });

    return atomString;
}

RefPtr<AtomStringImpl> JSRopeString::resolveRopeToExistingAtomString(JSGlobalObject* globalObject) const
{
    VM& vm = globalObject->vm();
    auto scope = DECLARE_THROW_SCOPE(vm);

    if (length() > maxLengthForOnStackResolve) {
        RefPtr<AtomStringImpl> existingAtomString;
        resolveRopeWithFunction(globalObject, [&] (Ref<StringImpl>&& newImpl) -> Ref<StringImpl> {
            existingAtomString = AtomStringImpl::lookUp(newImpl.ptr());
            if (existingAtomString)
                return Ref { *existingAtomString };
            return WTFMove(newImpl);
        });
        RETURN_IF_EXCEPTION(scope, nullptr);
        return existingAtomString;
    }
    
    RefPtr<AtomStringImpl> existingAtomString;
    if (!isSubstring()) {
        if (is8Bit()) {
            LChar buffer[maxLengthForOnStackResolve];
            resolveRopeInternalNoSubstring(buffer);
            existingAtomString = AtomStringImpl::lookUp(buffer, length());
        } else {
            UChar buffer[maxLengthForOnStackResolve];
            resolveRopeInternalNoSubstring(buffer);
            existingAtomString = AtomStringImpl::lookUp(buffer, length());
        }
    } else
        existingAtomString = StringView { substringBase()->valueInternal() }.substring(substringOffset(), length()).toExistingAtomString().releaseImpl();

    if (existingAtomString)
        convertToNonRope(*existingAtomString);
    return existingAtomString;
}

template<typename Function>
const String& JSRopeString::resolveRopeWithFunction(JSGlobalObject* nullOrGlobalObjectForOOM, Function&& function) const
{
    ASSERT(isRope());
    
    VM& vm = this->vm();
    if (isSubstring()) {
        ASSERT(!substringBase()->isRope());
        auto newImpl = substringBase()->valueInternal().substringSharingImpl(substringOffset(), length());
        convertToNonRope(function(newImpl.releaseImpl().releaseNonNull()));
        return valueInternal();
    }
    
    if (is8Bit()) {
        LChar* buffer;
        auto newImpl = StringImpl::tryCreateUninitialized(length(), buffer);
        if (!newImpl) {
            outOfMemory(nullOrGlobalObjectForOOM);
            return nullString();
        }
        vm.heap.reportExtraMemoryAllocated(newImpl->cost());

        resolveRopeInternalNoSubstring(buffer);
        convertToNonRope(function(newImpl.releaseNonNull()));
        return valueInternal();
    }
    
    UChar* buffer;
    auto newImpl = StringImpl::tryCreateUninitialized(length(), buffer);
    if (!newImpl) {
        outOfMemory(nullOrGlobalObjectForOOM);
        return nullString();
    }
    vm.heap.reportExtraMemoryAllocated(newImpl->cost());
    
    resolveRopeInternalNoSubstring(buffer);
    convertToNonRope(function(newImpl.releaseNonNull()));
    return valueInternal();
}

const String& JSRopeString::resolveRope(JSGlobalObject* nullOrGlobalObjectForOOM) const
{
    return resolveRopeWithFunction(nullOrGlobalObjectForOOM, [] (Ref<StringImpl>&& newImpl) {
        return WTFMove(newImpl);
    });
}

void JSRopeString::outOfMemory(JSGlobalObject* nullOrGlobalObjectForOOM) const
{
    ASSERT(isRope());
    if (nullOrGlobalObjectForOOM) {
        VM& vm = nullOrGlobalObjectForOOM->vm();
        auto scope = DECLARE_THROW_SCOPE(vm);
        throwOutOfMemoryError(nullOrGlobalObjectForOOM, scope);
    }
}

JSValue JSString::toPrimitive(JSGlobalObject*, PreferredPrimitiveType) const
{
    return const_cast<JSString*>(this);
}

double JSString::toNumber(JSGlobalObject* globalObject) const
{
    VM& vm = globalObject->vm();
    auto scope = DECLARE_THROW_SCOPE(vm);
    StringView view = unsafeView(globalObject);
    RETURN_IF_EXCEPTION(scope, 0);
    return jsToNumber(view);
}

inline StringObject* StringObject::create(VM& vm, JSGlobalObject* globalObject, JSString* string)
{
    StringObject* object = new (NotNull, allocateCell<StringObject>(vm)) StringObject(vm, globalObject->stringObjectStructure());
    object->finishCreation(vm, string);
    return object;
}

JSObject* JSString::toObject(JSGlobalObject* globalObject) const
{
    return StringObject::create(globalObject->vm(), globalObject, const_cast<JSString*>(this));
}

bool JSString::getStringPropertyDescriptor(JSGlobalObject* globalObject, PropertyName propertyName, PropertyDescriptor& descriptor)
{
    VM& vm = globalObject->vm();
    if (propertyName == vm.propertyNames->length) {
        descriptor.setDescriptor(jsNumber(length()), PropertyAttribute::DontEnum | PropertyAttribute::DontDelete | PropertyAttribute::ReadOnly);
        return true;
    }
    
    std::optional<uint32_t> index = parseIndex(propertyName);
    if (index && index.value() < length()) {
        descriptor.setDescriptor(getIndex(globalObject, index.value()), PropertyAttribute::DontDelete | PropertyAttribute::ReadOnly);
        return true;
    }
    
    return false;
}

JSString* jsStringWithCacheSlowCase(VM& vm, StringImpl& stringImpl)
{
    ASSERT(stringImpl.length() > 1 || (stringImpl.length() == 1 && stringImpl[0] > maxSingleCharacterString));
    JSString* string = JSString::create(vm, stringImpl);
    vm.lastCachedString.setWithoutWriteBarrier(string);
    return string;
}

} // namespace JSC
