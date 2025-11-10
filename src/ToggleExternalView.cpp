#include <XPLMPlugin.h>
#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>
#include <XPLMCamera.h>

// Command Declarations

XPLMCommandRef toggleExternalView;
int CommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon);

XPLMCommandRef setExternal;
XPLMCommandRef setInternal;

// DataRef Declarations

XPLMDataRef DR_isExternal;

XPLMDataRef DR_internalRoll,
			DR_internalHeading,
			DR_internalPitch;

XPLMDataRef DR_internalX,
			DR_internalY,
			DR_internalZ;

// Variable Declarations

XPLMCameraPosition_t previousExternalPosition;
XPLMCameraPosition_t previousInternalPosition;

// Variable Definitions

struct Position
{
	float roll, heading, pitch;
	float x, y, z;
};

bool externalPositionSaved{false};
bool internalPositionSaved{false};

Position external;
Position internal;

// Function Definitions

int setPosition (XPLMCameraPosition_t *outCameraPosition, int inIsLosingControl, void *inRefcon)
{
	if (!inIsLosingControl)
	{
		XPLMGetDatai(DR_isExternal) ?
		*outCameraPosition = previousExternalPosition :
		*outCameraPosition = previousInternalPosition;

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
	toggleExternalView = XPLMFindCommand("sim/view/3d_cockpit_toggle");

	setExternal = XPLMFindCommand("sim/view/chase");
	setInternal = XPLMFindCommand("sim/view/3d_cockpit_cmnd_look");

	// Assign the command to a usuable function (handler)
	XPLMRegisterCommandHandler(toggleExternalView, CommandHandler, 1, nullptr);

	// DataRef Definitions

	DR_isExternal = XPLMFindDataRef("sim/graphics/view/view_is_external");

	DR_internalRoll = XPLMFindDataRef("sim/graphics/view/pilots_head_phi");
	DR_internalHeading = XPLMFindDataRef("sim/graphics/view/pilots_head_psi");
	DR_internalPitch = XPLMFindDataRef("sim/graphics/view/pilots_head_the");

	DR_internalX = XPLMFindDataRef("sim/graphics/view/pilots_head_x");
	DR_internalY = XPLMFindDataRef("sim/graphics/view/pilots_head_y");
	DR_internalZ = XPLMFindDataRef("sim/graphics/view/pilots_head_z");
	return 1;
}

// Enables the plugin
PLUGIN_API int XPluginEnable(void)
{ return 1; }

// Unused required empty functions
PLUGIN_API void XPluginDisable(void);
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage, void *inParam);

// CommandHandler is called on execution of the 3D cockpit toggle
int CommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon)
{
	// Code is executed on button up.
	if (inPhase == xplm_CommandEnd)
	{
        if (XPLMGetDatai(DR_isExternal))
        {
			// Save the external OpenGL camera transformations
			XPLMReadCameraPosition(&previousExternalPosition);
			externalPositionSaved = true;

			//----
            XPLMCommandOnce(setInternal);
			//----

			// OpenGL camera must temporaly be controlled for zoom to be set
			if (internalPositionSaved)
				XPLMControlCamera(xplm_ControlCameraUntilViewChanges, setPosition, nullptr);
			
			// Internal DataRef camera transformations are set
			XPLMSetDataf(DR_internalRoll, internal.roll);
			XPLMSetDataf(DR_internalHeading, internal.heading);
			XPLMSetDataf(DR_internalPitch, internal.pitch);
			
			XPLMSetDataf(DR_internalX, internal.x);
			XPLMSetDataf(DR_internalY, internal.y);
			XPLMSetDataf(DR_internalZ, internal.z);
        }
        else
        {
			// Save the internal OpenGL camera transformations
			XPLMReadCameraPosition(&previousInternalPosition);
			internalPositionSaved = true;
			
			// Save the internal DataRef camera transformations
			internal.roll = XPLMGetDataf(DR_internalRoll);
			internal.heading = XPLMGetDataf(DR_internalHeading);
			internal.pitch = XPLMGetDataf(DR_internalPitch);

			internal.x = XPLMGetDataf(DR_internalX);
			internal.y = XPLMGetDataf(DR_internalY);
			internal.z = XPLMGetDataf(DR_internalZ);

			//----
			XPLMCommandOnce(setExternal);
			//----

			// OpenGL camera must temporaly be controlled for zoom to be set
			if (externalPositionSaved)
				XPLMControlCamera(xplm_ControlCameraUntilViewChanges, setPosition, nullptr);
			
			/* NOTE:
			   External DataRef camera transformations cannot currently
			   be set, as the DataRefs are read only. The only solutions
			   are modifying the values in memory or taking full control
			   and managment of the OpenGL camera when set to external.
			*/
        }
	}
	// Returning 0 disables further processing by X-Plane, therefore
    // overwriting the default command behaviour with ours
	return 0;
}

// Unregisters the CommandHanlder when it is terminated by X-Plane
PLUGIN_API void XPluginStop(void)
{ XPLMUnregisterCommandHandler(toggleExternalView, CommandHandler, 0, 0); }