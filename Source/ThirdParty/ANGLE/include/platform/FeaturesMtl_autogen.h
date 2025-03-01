// GENERATED FILE - DO NOT EDIT.
// Generated by gen_features.py using data from mtl_features.json.
//
// Copyright 2022 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FeaturesMtl_autogen.h: Optional features for the Metal renderer.

#ifndef ANGLE_PLATFORM_FEATURESMTL_H_
#define ANGLE_PLATFORM_FEATURESMTL_H_

#include "platform/Feature.h"

namespace angle
{

struct FeaturesMtl : FeatureSetBase
{
    FeaturesMtl();
    ~FeaturesMtl();

    FeatureInfo hasBaseVertexInstancedDraw = {
        "hasBaseVertexInstancedDraw",
        FeatureCategory::MetalFeatures,
        "The renderer supports base vertex instanced draw",
        &members,
    };

    FeatureInfo hasExplicitMemBarrier = {
        "hasExplicitMemBarrier",
        FeatureCategory::MetalFeatures,
        "The renderer supports explicit memory barrier",
        &members,
    };

    FeatureInfo hasCheapRenderPass = {
        "hasCheapRenderPass",
        FeatureCategory::MetalFeatures,
        "The renderer can cheaply break a render pass.",
        &members,
    };

    FeatureInfo hasNonUniformDispatch = {
        "hasNonUniformDispatch",
        FeatureCategory::MetalFeatures,
        "The renderer supports non uniform compute shader dispatch's group size",
        &members,
    };

    FeatureInfo hasShaderStencilOutput = {
        "hasShaderStencilOutput",
        FeatureCategory::MetalFeatures,
        "The renderer supports stencil output from fragment shader",
        &members,
    };

    FeatureInfo hasTextureSwizzle = {
        "hasTextureSwizzle",
        FeatureCategory::MetalFeatures,
        "The renderer supports texture swizzle",
        &members,
    };

    FeatureInfo hasDepthAutoResolve = {
        "hasDepthAutoResolve",
        FeatureCategory::MetalFeatures,
        "The renderer supports MSAA depth auto resolve at the end of render pass",
        &members,
    };

    FeatureInfo hasStencilAutoResolve = {
        "hasStencilAutoResolve",
        FeatureCategory::MetalFeatures,
        "The renderer supports MSAA stencil auto resolve at the end of render pass",
        &members,
    };

    FeatureInfo hasEvents = {
        "hasEvents",
        FeatureCategory::MetalFeatures,
        "The renderer supports MTL(Shared)Event",
        &members,
    };

    FeatureInfo allowInlineConstVertexData = {
        "allowInlineConstVertexData",
        FeatureCategory::MetalFeatures,
        "The renderer supports using inline constant data for small client vertex data",
        &members,
    };

    FeatureInfo allowSeparateDepthStencilBuffers = {
        "allowSeparateDepthStencilBuffers",
        FeatureCategory::MetalFeatures,
        "Some Apple platforms such as iOS allows separate depth and stencil buffers, "
        "whereas others such as macOS don't",
        &members,
    };

    FeatureInfo allowRuntimeSamplerCompareMode = {
        "allowRuntimeSamplerCompareMode",
        FeatureCategory::MetalFeatures,
        "The renderer supports changing sampler's compare mode outside shaders",
        &members,
    };

    FeatureInfo allowSamplerCompareGradient = {
        "allowSamplerCompareGradient",
        FeatureCategory::MetalFeatures,
        "The renderer supports sample_compare with gradients",
        &members,
    };

    FeatureInfo allowSamplerCompareLod = {
        "allowSamplerCompareLod",
        FeatureCategory::MetalFeatures,
        "The renderer supports sample_compare with lod",
        &members,
    };

    FeatureInfo allowBufferReadWrite = {
        "allowBufferReadWrite",
        FeatureCategory::MetalFeatures,
        "The renderer supports buffer read and write in the same shader",
        &members,
    };

    FeatureInfo allowMultisampleStoreAndResolve = {
        "allowMultisampleStoreAndResolve",
        FeatureCategory::MetalFeatures,
        "The renderer supports MSAA store and resolve in the same pass",
        &members,
    };

    FeatureInfo allowGenMultipleMipsPerPass = {
        "allowGenMultipleMipsPerPass",
        FeatureCategory::MetalFeatures,
        "The renderer supports generating multiple mipmaps per pass",
        &members,
    };

    FeatureInfo forceD24S8AsUnsupported = {
        "forceD24S8AsUnsupported",
        FeatureCategory::MetalFeatures,
        "Force Depth24Stencil8 format as unsupported.",
        &members,
    };

    FeatureInfo forceBufferGPUStorage = {
        "forceBufferGPUStorage",
        FeatureCategory::MetalFeatures,
        "On systems that support both buffer' memory allocation on GPU and shared memory (such as "
        "macOS), force using GPU memory allocation for buffers everytime or not.",
        &members,
    };

    FeatureInfo forceNonCSBaseMipmapGeneration = {
        "forceNonCSBaseMipmapGeneration",
        FeatureCategory::MetalFeatures,
        "Turn this feature on to disallow Compute Shader based mipmap generation. Compute Shader "
        "based mipmap generation might cause GPU hang on some older iOS devices.",
        &members,
    };

    FeatureInfo emulateTransformFeedback = {
        "emulateTransformFeedback",
        FeatureCategory::MetalFeatures,
        "Turn this on to allow transform feedback in Metal using a 2-pass VS for GLES3.",
        &members,
    };

    FeatureInfo intelExplicitBoolCastWorkaround = {
        "intelExplicitBoolCastWorkaround",
        FeatureCategory::MetalWorkarounds,
        "Insert explicit casts for float/double/unsigned/signed int on macOS 10.15 with Intel "
        "driver",
        &members,
    };

    FeatureInfo intelDisableFastMath = {
        "intelDisableFastMath",
        FeatureCategory::MetalWorkarounds,
        "Disable fast math in atan and invariance cases when running below macOS 12.0",
        &members,
    };

    FeatureInfo allowRenderpassWithoutAttachment = {
        "allowRenderpassWithoutAttachment",
        FeatureCategory::MetalFeatures,
        "Allow creation of render passes without any attachments",
        &members,
    };

    FeatureInfo avoidStencilTextureSwizzle = {
        "avoidStencilTextureSwizzle",
        FeatureCategory::MetalFeatures,
        "Do not create swizzled views of stencil textures",
        &members,
    };

    FeatureInfo emulateAlphaToCoverage = {
        "emulateAlphaToCoverage",
        FeatureCategory::MetalWorkarounds,
        "Some GPUs ignore alpha-to-coverage when [[sample_mask]] is written",
        &members,
    };

    FeatureInfo multisampleColorFormatShaderReadWorkaround = {
        "multisampleColorFormatShaderReadWorkaround", FeatureCategory::MetalWorkarounds,
        "Add shaderRead usage to some multisampled texture formats", &members,
        "http://anglebug.com/7049"};

    FeatureInfo copyIOSurfaceToNonIOSurfaceForReadOptimization = {
        "copyIOSurfaceToNonIOSurfaceForReadOptimization", FeatureCategory::MetalWorkarounds,
        "some GPUs are faster to read an IOSurface texture by first copying the texture to a "
        "non-IOSurface texture",
        &members, "http://anglebug.com/7117 http://anglebug.com/7573"};

    FeatureInfo copyTextureToBufferForReadOptimization = {
        "copyTextureToBufferForReadOptimization", FeatureCategory::MetalWorkarounds,
        "some GPUs are faster to read a texture by first copying the texture to a buffer", &members,
        "http://anglebug.com/7117"};

    FeatureInfo limitMaxDrawBuffersForTesting = {
        "limitMaxDrawBuffersForTesting", FeatureCategory::MetalFeatures,
        "Used to check the backend works when the device's advertized limit is less than the "
        "code's limit",
        &members, "http://anglebug.com/7280"};

    FeatureInfo limitMaxColorTargetBitsForTesting = {
        "limitMaxColorTargetBitsForTesting", FeatureCategory::MetalFeatures,
        "Metal iOS has a limit on the number of color target bits per pixel.", &members,
        "http://anglebug.com/7280"};

    FeatureInfo preemptivelyStartProvokingVertexCommandBuffer = {
        "preemptivelyStartProvokingVertexCommandBuffer", FeatureCategory::MetalFeatures,
        "AMD Metal Drivers appear to have a bug this works around", &members,
        "http://anglebug.com/7635"};

    FeatureInfo uploadDataToIosurfacesWithStagingBuffers = {
        "uploadDataToIosurfacesWithStagingBuffers", FeatureCategory::MetalWorkarounds,
        "When uploading data to IOSurface-backed textures, use a staging buffer.", &members,
        "http://anglebug.com/7573"};

    FeatureInfo disableProgrammableBlending = {
        "disableProgrammableBlending", FeatureCategory::MetalFeatures,
        "Disable programmable blending in order to test read_write pixel local storage textures",
        &members, "http://anglebug.com/7279"};

    FeatureInfo disableRWTextureTier2Support = {
        "disableRWTextureTier2Support", FeatureCategory::MetalFeatures,
        "Disable tier2 read_write textures in order to test tier1 support", &members,
        "http://anglebug.com/7279"};

    FeatureInfo disableRasterOrderGroups = {
        "disableRasterOrderGroups", FeatureCategory::MetalFeatures,
        "Disable raster order groups in order to test pixel local storage memory barriers",
        &members, "http://anglebug.com/7279"};

    FeatureInfo enableInMemoryMtlLibraryCache = {
        "enableInMemoryMtlLibraryCache", FeatureCategory::MetalFeatures,
        "Cache MTLLibrary objects in memory.", &members, "http://crbug.com/1385510"};

    FeatureInfo enableParallelMtlLibraryCompilation = {
        "enableParallelMtlLibraryCompilation", FeatureCategory::MetalFeatures,
        "Compile MTLLibrary in multiple threads.", &members, "http://crbug.com/1385510"};

    FeatureInfo alwaysPreferStagedTextureUploads = {
        "alwaysPreferStagedTextureUploads", FeatureCategory::MetalFeatures,
        "Always prefer to upload texture data via a staging buffer and avoid "
        "MTLTexture::replaceRegion.",
        &members, "http://crbug.com/1380790"};

    FeatureInfo disableStagedInitializationOfPackedTextureFormats = {
        "disableStagedInitializationOfPackedTextureFormats", FeatureCategory::MetalFeatures,
        "Staged GPU upload of some packed texture formats such as RGB9_E5 fail on Intel GPUs.",
        &members, "http://anglebug.com/8092"};

    FeatureInfo compileMetalShaders = {
        "compileMetalShaders", FeatureCategory::MetalFeatures,
        "Compiles metal shaders using command line tools and saves to BlobCache. "
        "Requires using --no-sandbox and disabling enableParallelMtlLibraryCompilation.",
        &members, "http://crbug.com/1423136"};

    FeatureInfo loadMetalShadersFromBlobCache = {
        "loadMetalShadersFromBlobCache", FeatureCategory::MetalFeatures,
        "Loads metal shaders from blob cache. Useful if compile_metal_shaders was used to "
        "generate shaders.",
        &members, "http://crbug.com/1423136"};

    FeatureInfo printMetalShaders = {"printMetalShaders", FeatureCategory::MetalFeatures,
                                     "Prints the source to a shader before it's compiled.",
                                     &members, "http://crbug.com/1423136"};
};

inline FeaturesMtl::FeaturesMtl()  = default;
inline FeaturesMtl::~FeaturesMtl() = default;

}  // namespace angle

#endif  // ANGLE_PLATFORM_FEATURESMTL_H_
