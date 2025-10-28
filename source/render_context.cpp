#include "render_context.h"

#include "model.h"


template<> void Renderer<Mesh>::render()
{
    unsigned int numVertex = 0;
    for (int i = 0; i < models.size(); i++) {
        models[i]->insert(numVertex);
        numVertex += models[i]->getNumVertex();
    }

    glBindVertexArray(vao);
    for (int i = 0; i < numBuffers; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]->id());
        glBufferData(GL_ARRAY_BUFFER, buffers[i]->byteSize() * buffers[i]->size(),
            buffers[i]->data(), GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    for (GLint i = 0; i < numAttribs; i++) {
        glEnableVertexAttribArray(i);
    }
    glDrawArrays(GL_TRIANGLES, 0, numVertex);
    for (GLint i = 0; i < numAttribs; i++) {
        glDisableVertexAttribArray(i);
    }
    glBindVertexArray(0);
}


struct TextureBatch {
    unsigned int offset;
    unsigned int numVertex;
    GLuint texture;
};


template<> void Renderer<TexturedMesh>::render()
{
    std::sort(models.begin(), models.end(), [](const TexturedMesh* a, const TexturedMesh* b) {
        return a->getTexture() <= b->getTexture();
    });

    std::vector<TextureBatch> batches;
    batches.emplace_back(0, models[0]->getNumVertex(), models[0]->getTexture());
    models[0]->insert(0);

    TextureBatch* currBatch = &batches[0];
    for (int i = 1; i < models.size(); i++) {
        if (models[i]->getTexture() != models[i - 1]->getTexture()) {
            batches.emplace_back(currBatch->numVertex, 0, models[i]->getTexture());
            currBatch = &batches.back();
        }

        models[i]->insert(currBatch->numVertex);
        currBatch->numVertex += models[i]->getNumVertex();
    }

    glBindVertexArray(vao);
    for (int i = 0; i < numBuffers; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]->id());
        glBufferData(GL_ARRAY_BUFFER, buffers[i]->byteSize() * buffers[i]->size(),
            buffers[i]->data(), GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    for (GLint i = 0; i < numAttribs; i++) {
        glEnableVertexAttribArray(i);
    }
    for (int i = 0; i < batches.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, batches[i].texture);
        glDrawArrays(GL_TRIANGLES, batches[i].offset, batches[i].numVertex);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    for (GLint i = 0; i < numAttribs; i++) {
        glDisableVertexAttribArray(i);
    }
    glBindVertexArray(0);
}