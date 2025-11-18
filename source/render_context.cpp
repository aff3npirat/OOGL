#include "render_context.h"

#include <algorithm>

#include "model.h"
#include "render_context.h"


template<> void Detail::Renderer<Mesh, RenderBatch>::render()
{
    glBindVertexArray(vao);
    for (GLint i = 0; i < numAttribs; i++) {
        glEnableVertexAttribArray(i);
    }
    for (int i = 0; i < renderBatches.size(); i++) {
        glDrawArrays(GL_TRIANGLES, renderBatches[i].offset, renderBatches[i].numVertex);
    }
    for (GLint i = 0; i < numAttribs; i++) {
        glDisableVertexAttribArray(i);
    }
    glBindVertexArray(0);
}


template<> void Detail::Renderer<Mesh, RenderBatch>::generateBatches()
{
    unsigned int numVertex = 0;
    for (int i = 0; i < toRender.size(); i++) {
        toRender[i].insert(numVertex);
        numVertex += toRender[i].getNumVertex();
    }

    renderBatches.emplace_back(0, numVertex);

    for (int i = 0; i < numBuffers; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]->id());
        glBufferData(GL_ARRAY_BUFFER, buffers[i]->byteSize() * buffers[i]->size(),
            buffers[i]->data(), GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


template<> void Detail::Renderer<TexturedMesh, TextureBatch>::render()
{
    glBindVertexArray(vao);
    for (GLint i = 0; i < numAttribs; i++) {
        glEnableVertexAttribArray(i);
    }
    for (int i = 0; i < renderBatches.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, renderBatches[i].texture);
        glDrawArrays(GL_TRIANGLES, renderBatches[i].offset, renderBatches[i].numVertex);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    for (GLint i = 0; i < numAttribs; i++) {
        glDisableVertexAttribArray(i);
    }
    glBindVertexArray(0);
}


template<> void Detail::Renderer<TexturedMesh, TextureBatch>::generateBatches()
{
    std::sort(toRender.begin(), toRender.end(), [](const TexturedMesh& a, const TexturedMesh& b) {
        return a.getTexture() <= b.getTexture();
    });

    renderBatches.emplace_back(0, toRender[0].getNumVertex(), toRender[0].getTexture());
    toRender[0].insert(0);

    TextureBatch* currBatch = &renderBatches[0];
    for (int i = 1; i < toRender.size(); i++) {
        if (toRender[i].getTexture() != toRender[i - 1].getTexture()) {
            renderBatches.emplace_back(currBatch->numVertex, 0, toRender[i].getTexture());
            currBatch = &renderBatches.back();
        }

        toRender[i].insert(currBatch->numVertex);
        currBatch->numVertex += toRender[i].getNumVertex();
    }

    for (int i = 0; i < numBuffers; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]->id());
        glBufferData(GL_ARRAY_BUFFER, buffers[i]->byteSize() * buffers[i]->size(),
            buffers[i]->data(), GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}