#include "PlatformWindowInfo_Conversion.h"
#include "window/PlatformWindowInfo_App.h"
#include "utility/PlatformWindowInfo_Renderer.h"

namespace CSEEngine
{

#if WIN32

CSECore::Any<64> PlatformWindowInfo_AppToRenderer(CSECore::Any<64> windowInfoApp)
{
	CSE_ASSERT(windowInfoApp.IsA<CSEApplication::PlatformWindowInfo_Win32>(), "Given Any doesn't contain a PlatformWindowInfo_Win32.");

	CSEApplication::PlatformWindowInfo_Win32 appWinInfo = windowInfoApp.CastTo<CSEApplication::PlatformWindowInfo_Win32>();

	CSERenderer::PlatformWindowInfo_Win32 renderWinInfo;
	renderWinInfo.hinst = appWinInfo.hinst;
	renderWinInfo.hwnd = appWinInfo.hwnd;
	renderWinInfo.width = appWinInfo.width;
	renderWinInfo.height = appWinInfo.height;

	return CSECore::Any<64>(renderWinInfo);
}

#endif

}