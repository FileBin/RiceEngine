#include "../stdafx.hpp"
NSP_GL_BEGIN

interface IPostProcess;
typedef SmartPtr<IPostProcess> pIPostProcess;

NSP_GL_END

#pragma once

NSP_GL_BEGIN

interface IPostProcess {
	virtual void PostProcess() = 0;
	virtual ~IPostProcess() {}
};

NSP_GL_END
