/*
 * Copyright (c) 2017 - 2018, Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once
#include <cstdint>
#include <cstdlib>
#include <memory>
#include "runtime/gmm_helper/gmm_lib.h"
#include "runtime/api/cl_types.h"

namespace OCLRT {
struct HardwareInfo;
struct FeatureTable;
struct WorkaroundTable;
struct ImageInfo;
class GraphicsAllocation;
class GmmResourceInfo;
class GmmClientContext;

enum OCLPlane {
    NO_PLANE = 0,
    PLANE_Y,
    PLANE_U,
    PLANE_V,
    PLANE_UV
};

constexpr uint32_t cacheDisabledIndex = 0;
constexpr uint32_t cacheEnabledIndex = 4;

class Gmm {
  public:
    static const uint32_t maxPossiblePitch = 2147483648;

    virtual ~Gmm();

    void create();
    static Gmm *create(const void *alignedPtr, size_t alignedSize, bool uncacheable);
    static Gmm *create(GMM_RESOURCE_INFO *inputGmm);

    static bool initContext(const PLATFORM *pPlatform, const FeatureTable *pSkuTable, const WorkaroundTable *pWaTable, const GT_SYSTEM_INFO *pGtSysInfo);
    static void loadLib();
    static void destroyContext();

    static uint32_t getMOCS(uint32_t type);

    void queryImageParams(ImageInfo &imgInfo, const HardwareInfo &hwInfo);

    static Gmm *createGmmAndQueryImgParams(ImageInfo &imgInfo, const HardwareInfo &hwInfo);

    static void queryImgFromBufferParams(ImageInfo &imgInfo, GraphicsAllocation *gfxAlloc);

    static bool allowTiling(const cl_image_desc &imageDesc);

    static uint64_t canonize(uint64_t address);
    static uint64_t decanonize(uint64_t address);

    static GMM_CUBE_FACE_ENUM getCubeFaceIndex(uint32_t target);
    static uint32_t getRenderTileMode(uint32_t tileWalk);
    static uint32_t getRenderMultisamplesCount(uint32_t numSamples);
    static GMM_YUV_PLANE convertPlane(OCLPlane oclPlane);

    uint32_t getRenderHAlignment();
    uint32_t getRenderVAlignment();
    static uint32_t getRenderAlignment(uint32_t alignment);
    void applyAuxFlags(ImageInfo &imgInfo, const HardwareInfo &hwInfo);
    bool unifiedAuxTranslationCapable() const;

    uint32_t queryQPitch(GFXCORE_FAMILY gfxFamily, GMM_RESOURCE_TYPE resType);

    void updateImgInfo(ImageInfo &imgInfo, cl_image_desc &imgDesc, cl_uint arrayIndex);
    uint8_t resourceCopyBlt(void *sys, void *gpu, uint32_t pitch, uint32_t height, unsigned char upload, OCLPlane plane);

    GMM_RESCREATE_PARAMS resourceParams = {};
    std::unique_ptr<GmmResourceInfo> gmmResourceInfo;

    static decltype(&GmmInitGlobalContext) initGlobalContextFunc;
    static decltype(&GmmDestroyGlobalContext) destroyGlobalContextFunc;
    static decltype(&GmmCreateClientContext) createClientContextFunc;
    static decltype(&GmmDeleteClientContext) deleteClientContextFunc;
    static std::unique_ptr<GmmClientContext> (*createGmmContextWrapperFunc)(GMM_CLIENT);

    bool isRenderCompressed = false;
    static bool useSimplifiedMocsTable;
    static std::unique_ptr<GmmClientContext> gmmClientContext;
    static bool isLoaded;
};
} // namespace OCLRT
