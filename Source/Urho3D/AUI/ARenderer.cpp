//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <TurboBadger/tb_core.h>
#include <TurboBadger/tb_system.h>
#include <TurboBadger/tb_debug.h>
#include <TurboBadger/tb_node_tree.h>
#include <TurboBadger/tb_widgets_reader.h>

#include "../Precompiled.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Texture2D.h"
#include "../AUI/ARenderer.h"

namespace Urho3D
{

class TBUIBitmap : public tb::TBBitmap
{
public:
    TBUIBitmap(ARenderer *renderer);
    virtual ~TBUIBitmap();

    bool Init(int width, int height, tb::uint32 *data);

    virtual int Width() { return width_; }
    virtual int Height() { return height_; }
    virtual void SetData(tb::uint32 *data);
public:

    ARenderer *renderer_;
    int width_;
    int height_;
    SharedPtr<Texture2D> texture_;
};

TBUIBitmap::TBUIBitmap(ARenderer *renderer)
    : renderer_(renderer), width_(0), height_(0)
{
}



ARenderer::ARenderer(Context* context)
{
    context_ = context;
}

ARenderer::~ARenderer()
{
}

void ARenderer::BeginPaint(int render_target_w, int render_target_h)
{
    TBRendererBatcher::BeginPaint(render_target_w, render_target_h);
}

void ARenderer::EndPaint()
{
    TBRendererBatcher::EndPaint();
}

tb::TBBitmap *ARenderer::CreateBitmap(int width, int height, tb::uint32 *data)
{
    TBUIBitmap *bitmap = new TBUIBitmap(this);

    if (!bitmap->Init(width, height, data))
    {
        delete bitmap;
        return nullptr;
    }

    return bitmap;
}

void ARenderer::RenderBatch(Batch *batch)
{
    if (!batch->vertex_count)
        return;

    Texture2D* texture = NULL;

    if (batch->bitmap)
    {
        TBUIBitmap* tbuibitmap = (TBUIBitmap*)batch->bitmap;
        texture = tbuibitmap->texture_;
    }

    ABatch b(BLEND_ALPHA , currentScissor_, texture, vertexData_);

    unsigned begin = b.vertexData_->Size();
    b.vertexData_->Resize(begin + batch->vertex_count * AUI_VERTEX_SIZE);
    float* dest = &(b.vertexData_->At(begin));
    b.vertexEnd_ = b.vertexData_->Size();

    for (int i = 0; i < batch->vertex_count; i++)
    {
        Vertex* v = &batch->vertex[i];
        dest[0] = v->x; dest[1] = v->y; dest[2] = 0.0f;
        ((unsigned&)dest[3]) = v->col;
        dest[4] = v->u; dest[5] = v->v;
        dest += AUI_VERTEX_SIZE;
    }

    ABatch::AddOrMerge(b, *batches_);
}

void ARenderer::SetClipRect(const tb::TBRect &rect)
{
    currentScissor_.top_ = rect.y;
    currentScissor_.bottom_ = rect.y + rect.h;
    currentScissor_.left_ = rect.x;
    currentScissor_.right_ = rect.x + rect.w;

}

TBUIBitmap::~TBUIBitmap()
{
    // Must flush and unbind before we delete the texture
    renderer_->FlushBitmap(this);
}

bool TBUIBitmap::Init(int width, int height, tb::uint32 *data)
{
    assert(width == tb::TBGetNearestPowerOfTwo(width));
    assert(height == tb::TBGetNearestPowerOfTwo(height));

    width_ = width;
    height_ = height;

    texture_ = new Texture2D(renderer_->GetContext());
    texture_->SetMipsToSkip(QUALITY_LOW, 0); // No quality reduction
    texture_->SetNumLevels(1);
    texture_->SetAddressMode(COORD_U, ADDRESS_BORDER);
    texture_->SetAddressMode(COORD_V, ADDRESS_BORDER),
            texture_->SetBorderColor(Color(0.0f, 0.0f, 0.0f, 0.0f));
    texture_->SetSize(width_, height_,  Graphics::GetRGBAFormat(), TEXTURE_STATIC);

    SetData(data);

    return true;
}

void TBUIBitmap::SetData(tb::uint32 *data)
{
    renderer_->FlushBitmap(this);
    texture_->SetData(0, 0, 0, width_, height_, data);
}

}
