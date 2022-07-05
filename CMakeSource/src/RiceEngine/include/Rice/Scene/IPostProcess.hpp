#include "../stdafx.hpp"
NSP_GL_BEGIN

struct IPostProcess;
typedef RefPtr<IPostProcess> pIPostProcess;

NSP_GL_END

#pragma once

NSP_GL_BEGIN

struct IPostProcess {
    virtual void postProcess() = 0;
	virtual ~IPostProcess() {}
};

NSP_GL_END
