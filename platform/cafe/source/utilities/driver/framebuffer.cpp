#include <utilities/driver/framebuffer.hpp>

#include <gx2/display.h>
#include <gx2/shaders.h>
#include <gx2/swap.h>

#include <glm/gtc/type_ptr.hpp>

#include <malloc.h>
#include <stdlib.h>

using namespace love;

Framebuffer::Framebuffer() :
    main {},
    little(nullptr),
    colorBuffer {},
    mode(0),
    scanBuffer(nullptr),
    scanBufferSize(0),
    width(0),
    height(0)
{}

void Framebuffer::Create(Screen screen)
{
    this->id             = screen;
    this->main.modelView = glm::mat4(1.0f);
    this->little         = (Transform*)memalign(0x100, sizeof(Transform));

    if (this->Is(Screen::SCREEN_TV))
        GX2SetTVEnable(true);
    else
        GX2SetDRCEnable(true);

    this->ScanSystemMode();
}

void Framebuffer::ScanSystemMode()
{
    /* early logic return */
    if (this->Is(Screen::SCREEN_GAMEPAD))
    {
        this->mode = GX2_DRC_RENDER_MODE_SINGLE;
        return this->SetSize(854, 480);
    }

    switch (GX2GetSystemTVScanMode())
    {
        case GX2_TV_SCAN_MODE_480I:
        case GX2_TV_SCAN_MODE_480P:
        {
            this->mode = GX2_TV_RENDER_MODE_WIDE_480P;
            this->SetSize(854, 480);
            break;
        }
        case GX2_TV_SCAN_MODE_720P:
        {
            this->mode = GX2_TV_RENDER_MODE_WIDE_720P;
            this->SetSize(1280, 720);
            break;
        }
        case GX2_TV_SCAN_MODE_1080I:
        case GX2_TV_SCAN_MODE_1080P:
        default:
        {
            this->mode = GX2_TV_RENDER_MODE_WIDE_1080P;
            this->SetSize(1920, 1080);
            break;
        }
    }
}

void Framebuffer::InitColorBuffer()
{
    memset(&this->colorBuffer, 0, sizeof(GX2ColorBuffer));

    this->colorBuffer.surface.use       = GX2_SURFACE_USE_TEXTURE_COLOR_BUFFER_TV;
    this->colorBuffer.surface.dim       = GX2_SURFACE_DIM_TEXTURE_2D;
    this->colorBuffer.surface.aa        = GX2_AA_MODE1X;
    this->colorBuffer.surface.width     = this->width;
    this->colorBuffer.surface.height    = this->height;
    this->colorBuffer.surface.depth     = 1;
    this->colorBuffer.surface.mipLevels = 1;
    this->colorBuffer.surface.format    = Framebuffer::FORMAT;
    this->colorBuffer.surface.tileMode  = GX2_TILE_MODE_DEFAULT;
    this->colorBuffer.viewNumSlices     = 1;

    GX2CalcSurfaceSizeAndAlignment(&this->colorBuffer.surface);
    GX2InitColorBufferRegs(&this->colorBuffer);
}

bool Framebuffer::AllocateScanBuffer(MEMHeapHandle handle)
{
    const auto alignment = GX2_SCAN_BUFFER_ALIGNMENT;
    this->scanBuffer     = MEMAllocFromFrmHeapEx(handle, this->scanBufferSize, alignment);

    if (this->scanBuffer == nullptr)
        return false;

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, this->scanBuffer, this->scanBufferSize);

    if (this->Is(Screen::SCREEN_TV))
        this->SetTVScanBuffer();
    else
        this->SetDRCScanBuffer();

    return true;
}

bool Framebuffer::InvalidateColorBuffer(MEMHeapHandle handle)
{
    const auto size      = this->colorBuffer.surface.imageSize;
    const auto alignment = this->colorBuffer.surface.alignment;

    this->colorBuffer.surface.image = MEMAllocFromFrmHeapEx(handle, size, alignment);

    if (this->colorBuffer.surface.image == nullptr)
        return false;

    GX2Invalidate(Framebuffer::INVALIDATE_COLOR_BUFFER, this->colorBuffer.surface.image, size);

    return true;
}

void Framebuffer::SetTVScanBuffer()
{
    const auto mode = (GX2TVRenderMode)this->mode;
    GX2SetTVBuffer(this->scanBuffer, this->scanBufferSize, mode, Framebuffer::FORMAT,
                   Framebuffer::BUFFERING);
}

void Framebuffer::SetDRCScanBuffer()
{
    const auto mode = (GX2DrcRenderMode)this->mode;
    GX2SetDRCBuffer(this->scanBuffer, this->scanBufferSize, mode, Framebuffer::FORMAT,
                    Framebuffer::BUFFERING);
}

void Framebuffer::CopyScanBuffer()
{
    const auto target = Framebuffer::SCAN_TARGETS[(uint8_t)this->id];
    GX2CopyColorBufferToScanBuffer(&this->colorBuffer, target);
}

void Framebuffer::SetSize(int width, int height)
{
    this->width  = width;
    this->height = height;

    if (this->Is(Screen::SCREEN_TV))
        this->SetTVSize();
    else
        this->SetDRCSize();

    this->InitColorBuffer();
}

void Framebuffer::SetTVSize()
{
    uint32_t unk    = 0;
    const auto mode = (GX2TVRenderMode)this->mode;

    GX2CalcTVSize(mode, Framebuffer::FORMAT, Framebuffer::BUFFERING, &this->scanBufferSize, &unk);
    GX2SetTVScale(this->width, this->height);
}

void Framebuffer::SetDRCSize()
{
    uint32_t unk    = 0;
    const auto mode = (GX2DrcRenderMode)this->mode;

    GX2CalcDRCSize(mode, Framebuffer::FORMAT, Framebuffer::BUFFERING, &this->scanBufferSize, &unk);
    GX2SetDRCScale(this->width, this->height);
}

void Framebuffer::SetProjection(const glm::highp_mat4& _projection)
{
    this->main.projection = _projection;

    unsigned int* dstModel = (unsigned int*)glm::value_ptr(this->little->modelView);
    unsigned int* dstProj  = (unsigned int*)glm::value_ptr(this->little->projection);

    unsigned int* model = (unsigned int*)glm::value_ptr(this->main.modelView);
    for (size_t index = 0; index < 16; index++)
        dstModel[index] = __builtin_bswap32(model[index]);

    unsigned int* projection = (unsigned int*)glm::value_ptr(_projection);
    for (size_t index = 0; index < 16; index++)
        dstProj[index] = __builtin_bswap32(projection[index]);
}

void Framebuffer::UseProjection()
{
    GX2Invalidate(Framebuffer::INVALIDATE_UNIFORM, (void*)this->little,
                  Framebuffer::TRANSFORM_SIZE);

    GX2SetVertexUniformBlock(1, Framebuffer::TRANSFORM_SIZE, (void*)this->little);
}