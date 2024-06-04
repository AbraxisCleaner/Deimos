#ifndef _ENGINE_RHI_H_
#define _ENGINE_RHI_H_

#include <pch.h>
#include "FileIO.h"

namespace RHI {
	struct SRhiState;
	extern SRhiState *gRhi;

	bool Initialize(bool bValidate);
	void Release();
	void WaitForRendering();

	void SetRhiState(SRhiState *pState);
	SRhiState *GetRhiState();

	struct SWindowContext;
	bool CreateWindowContext(void *pWnd, SWindowContext *pOut);
	void ReleaseWindowContext(SWindowContext *pCtx);
	void ResizeWindowContext(SWindowContext *pCtx, uint NewWidth, uint NewHeight);
}

#endif // _ENGINE_RHI_H_

