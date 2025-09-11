#include "XPLMPlugin.h"
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "XPLMCamera.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Command Declarations

XPLMCommandRef ToggleExternalView;
static int CommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon);

XPLMCommandRef External;
XPLMCommandRef Internal;

// DataRef Declarations

XPLMDataRef IsExternal;

// Variable Declarations

XPLMCameraPosition_t PreviousCockpitPosition;

// Function Definitions

static int SetCockpitPosition (XPLMCameraPosition_t *outCameraPosition, int inIsLosingControl, void *inRefcon)
{
	if (!inIsLosingControl)
	{
		*outCameraPosition = PreviousCockpitPosition;
		XPLMDontControlCamera();
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------------

PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
	// Plugin Info
	strcpy(outName, "ToggleExternalView");
	strcpy(outSig, "Toggle.External.View");
	strcpy(outDesc, "Toggle the cockpit view.");

	// Command Definitions
	ToggleExternalView = XPLMFindCommand("sim/view/3d_cockpit_toggle");

	External = XPLMFindCommand("sim/view/chase");
	Internal = XPLMFindCommand("sim/view/3d_cockpit_cmnd_look");

	// Assign the command to a usuable function (handler)
	XPLMRegisterCommandHandler(ToggleExternalView, CommandHandler, 1, nullptr);

	// DataRef Definitions

	IsExternal = XPLMFindDataRef("sim/graphics/view/view_is_external");

	return 1;
}

// Enables the plugin
PLUGIN_API int XPluginEnable(void)
{ return 1; }

// Unused required empty functions
PLUGIN_API void XPluginDisable(void){}
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage, void *inParam){}

// CommandHandler is called on execution of the 3D cockpit toggle
int CommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon)
{
	// Code is executed on button up.
	if (inPhase == xplm_CommandEnd)
	{
        if (XPLMGetDatai(IsExternal))
        {
            XPLMCommandOnce(Internal);
			XPLMControlCamera(xplm_ControlCameraUntilViewChanges, SetCockpitPosition, nullptr);
        }
        else
        {
			XPLMReadCameraPosition(&PreviousCockpitPosition);
			XPLMCommandOnce(External);
        }
	}
	// Returning 0 disables further processing by X-Plane, therefore
    // overwriting the default command behaviour with ours
	return 0;
}

// Unregisters the plugin when it is terminated by X-Plane
PLUGIN_API void XPluginStop(void)
{ XPLMUnregisterCommandHandler(ToggleExternalView, CommandHandler, 0, 0); }