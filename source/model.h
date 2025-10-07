#pragma once

#include "render_context.h"


class ModelBase {
  public:
    virtual ~ModelBase() = 0;
    virtual void insert(unsigned int offset) = 0;
    virtual unsigned int numVertex();
};