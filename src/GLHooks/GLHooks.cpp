#include "GLHooks.h"

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <vector>

#include <detours.h>

// Debug
// #include <fbsdk/fbsdk.h>

/**
 * @typedef GLBINDFRAMEBUFFERTRUE
 * @brief Function pointer type for glBindFramebuffer
 * @note The function glBindFramebuffer is avalilable since OpenGL 3.0, cannot use from gl/GL.h.
 */
typedef void(WINAPI *GLBINDFRAMEBUFFERTRUE)(GLenum target, GLuint framebuffer);

/**
 * @typedef GLBEGINTRUE
 * @brief Function pointer type for glBegin
 */
typedef void(WINAPI *GLBEGINTRUE)(GLenum mode);

/**
 * @typedef GLENDTRUE
 * @brief Function pointer type for glEnd
 */
typedef void(WINAPI *GLENDTRUE)(void);

/**
 * @typedef GLRECTFTRUE
 * @brief Function pointer type for glRectf
 */
typedef void(WINAPI *GLRECTFTRUE)(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

/**
 * @typedef GLVERTEX2DTRUE
 * @brief Function pointer type for glVertex2d
 */
typedef void(WINAPI *GLVERTEX2DTRUE)(GLdouble x, GLdouble y);

static GLBINDFRAMEBUFFERTRUE glBindFramebufferTrue = (GLBINDFRAMEBUFFERTRUE)wglGetProcAddress("glBindFramebuffer");
static GLBEGINTRUE glBeginTrue = glBegin;
static GLENDTRUE glEndTrue = glEnd;
static GLRECTFTRUE glRectfTrue = glRectf;
static GLVERTEX2DTRUE glVertex2dTrue = glVertex2d;

static std::mutex gBoxRangeArrayMutex;         //!< Mutex to protect access to gBoxRectRange
static std::recursive_mutex gFramebufferMutex; //!< Mutex to protect access to gCurrentRelationViewFramebuffer
static std::mutex gGridLineMutex;              //!< Mutex to protect access to gGridLineXCoords and gLastGridSpacing

static std::atomic<bool> gIsCapturingGridLines = false;         //!< Flag to indicate if we are currently capturing grid line coordinates
static std::atomic<bool> gIsDataResetRequired = false;          //!< Flag to indicate if internal data reset is required
static std::atomic<bool> gIsSpacingCalculationRequired = false; //!< Flag to indicate if spacing calculation is required

static std::unordered_map<GLint, std::array<double, 4>> gBoxRectRange;  //!< Array to store the bounding rectangle of rendered boxes in the relation view
static GLint gCurrentRelationViewFramebuffer;                           //!< The framebuffer used by the relation view
static std::unordered_map<GLint, std::vector<double>> gGridLineXCoords; //!< Vector to store captured x-coordinates of grid lines
static std::unordered_map<GLint, double> gLastGridSpacing;              //!< The last calculated grid spacing

double getLastGridSpacing(GLint framebuffer)
{
    std::lock_guard<std::mutex> lock(gGridLineMutex);

    if (gLastGridSpacing.find(framebuffer) != gLastGridSpacing.end())
        return gLastGridSpacing[framebuffer];
    else
        return defaultGLGridSpacing; // Default grid spacing
}

std::array<double, 4> getBoxRectRange(GLint framebuffer)
{
    std::lock_guard<std::mutex> lock(gBoxRangeArrayMutex);

    if (gBoxRectRange.find(framebuffer) != gBoxRectRange.end())
        return gBoxRectRange[framebuffer];
    else
        return defaultBoxRectRange; // Default box rectangle range
}

bool startHook()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // Attach custom functions to the original OpenGL functions
    DetourAttach(&(PVOID &)glBindFramebufferTrue, glBindFramebufferCustom);
    DetourAttach(&(PVOID &)glBeginTrue, glBeginCustom);
    DetourAttach(&(PVOID &)glRectfTrue, glRectfCustom);
    DetourAttach(&(PVOID &)glVertex2dTrue, glVertex2dCustom);
    DetourAttach(&(PVOID &)glEndTrue, glEndCustom);

    // Commit the transaction and start hooking
    if (DetourTransactionCommit() == NO_ERROR)
        return true;
    else
        return false;
}

void endHook()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // Detach custom functions from the original OpenGL functions
    DetourDetach(&(PVOID &)glBindFramebufferTrue, glBindFramebufferCustom);
    DetourDetach(&(PVOID &)glBeginTrue, glBeginCustom);
    DetourDetach(&(PVOID &)glRectfTrue, glRectfCustom);
    DetourDetach(&(PVOID &)glVertex2dTrue, glVertex2dCustom);
    DetourDetach(&(PVOID &)glEndTrue, glEndCustom);

    // Commit the transaction and stop hooking
    DetourTransactionCommit();
}

void glBindFramebufferCustom(GLenum target, GLuint framebuffer)
{
    std::lock_guard<std::recursive_mutex> lock(gFramebufferMutex);

    GLint newFrameBuffer = static_cast<GLint>(framebuffer);

    if (gCurrentRelationViewFramebuffer != newFrameBuffer)
    {
        // Update the current framebuffer
        gCurrentRelationViewFramebuffer = newFrameBuffer;

        // Request data reset for new capture in the next glBegin call
        gIsDataResetRequired = true;
    }

    // Call the original glBindFramebuffer function
    glBindFramebufferTrue(target, framebuffer);
}

void WINAPI glBeginCustom(GLenum mode)
{
    if (mode == GL_LINES && gIsDataResetRequired)
    {
        std::lock_guard<std::recursive_mutex> framebufferLock(gFramebufferMutex);
        std::lock_guard<std::mutex> vectorLock(gGridLineMutex);
        std::lock_guard<std::mutex> boxLock(gBoxRangeArrayMutex);

        // Reset internal data for new capture
        if (gLastGridSpacing.find(gCurrentRelationViewFramebuffer) != gLastGridSpacing.end())
            gLastGridSpacing[gCurrentRelationViewFramebuffer] = defaultGLGridSpacing;
        else
            gLastGridSpacing.insert({gCurrentRelationViewFramebuffer, defaultGLGridSpacing});

        if (gGridLineXCoords.find(gCurrentRelationViewFramebuffer) != gGridLineXCoords.end())
            gGridLineXCoords[gCurrentRelationViewFramebuffer].clear();
        else
            gGridLineXCoords.insert({gCurrentRelationViewFramebuffer, std::vector<double>()});

        if (gBoxRectRange.find(gCurrentRelationViewFramebuffer) != gBoxRectRange.end())
            gBoxRectRange[gCurrentRelationViewFramebuffer] = defaultBoxRectRange;
        else
            gBoxRectRange.insert({gCurrentRelationViewFramebuffer, defaultBoxRectRange});

        // Start capturing grid line coordinates
        gIsCapturingGridLines = true;

        // Reset the flag after data reset
        gIsDataResetRequired = false;
    }

    // Call the original glBegin function
    glBeginTrue(mode);
}

void WINAPI glEndCustom(void)
{
    // Call the original glEnd function
    glEndTrue();

    if (gIsSpacingCalculationRequired)
    {
        std::lock_guard<std::mutex> lock(gGridLineMutex);

        if (gGridLineXCoords.find(gCurrentRelationViewFramebuffer) == gGridLineXCoords.end())
        {
            gIsSpacingCalculationRequired = false;
            return;
        }

        if (gGridLineXCoords[gCurrentRelationViewFramebuffer].size() >= 5)
        {
            // Ignore the first three coordinate, calculate spacing using the 4th and 5th coordinates
            // to ensure precise spacing calculation while panning, scaling, or resizing the relation view.
            double calculatedSpacing = gGridLineXCoords[gCurrentRelationViewFramebuffer][4] - gGridLineXCoords[gCurrentRelationViewFramebuffer][3];

            // If the relation view is being rendered, it seems always being drawn starting from the smallest,
            // so the calculated spacing should always be positive.
            if (calculatedSpacing <= 0.0)
                return;

            // Update the last calculated grid spacing
            if (gLastGridSpacing.find(gCurrentRelationViewFramebuffer) != gLastGridSpacing.end())
                gLastGridSpacing[gCurrentRelationViewFramebuffer] = calculatedSpacing;
            else
                gLastGridSpacing.insert({gCurrentRelationViewFramebuffer, calculatedSpacing});

            // Debug
            // FBTrace("Framebuffer: %d, Calculated grid spacing: %f\n", gCurrentRelationViewFramebuffer, calculatedSpacing);
        }

        // Reset the flag after calculation
        gIsSpacingCalculationRequired = false;
    }
}

void WINAPI glRectfCustom(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    std::lock_guard<std::mutex> gridLock(gGridLineMutex);

    // Epsilon for comparison of floating point numbers
    const static double e = 0.0001;

    double currentGridSpacing;

    if (gLastGridSpacing.find(gCurrentRelationViewFramebuffer) != gLastGridSpacing.end())
        currentGridSpacing = gLastGridSpacing[gCurrentRelationViewFramebuffer];
    else
        currentGridSpacing = defaultGLGridSpacing;

    // Ignore when not in relation view rendering and small rectangles(connectors, not boxes itself)
    // The value 17.7 seems to be the width(height) of the connector part of a box when grid spacing is default defaultGLGridSpacing
    if ((x2 - x1) <= 17.7 / defaultGLGridSpacing * currentGridSpacing + e)
    {
        glRectfTrue(x1, y1, x2, y2);
        return;
    }

    std::lock_guard<std::mutex> boxLock(gBoxRangeArrayMutex);

    if (gBoxRectRange.find(gCurrentRelationViewFramebuffer) == gBoxRectRange.end())
        gBoxRectRange.insert({gCurrentRelationViewFramebuffer, defaultBoxRectRange});
    else
    {
        if (x1 < gBoxRectRange[gCurrentRelationViewFramebuffer][0])
            gBoxRectRange[gCurrentRelationViewFramebuffer][0] = x1; // minLeft

        if (y1 < gBoxRectRange[gCurrentRelationViewFramebuffer][1])
            gBoxRectRange[gCurrentRelationViewFramebuffer][1] = y1; // minTop

        if (x2 > gBoxRectRange[gCurrentRelationViewFramebuffer][2])
            gBoxRectRange[gCurrentRelationViewFramebuffer][2] = x2; // maxRight

        if (y2 > gBoxRectRange[gCurrentRelationViewFramebuffer][3])
            gBoxRectRange[gCurrentRelationViewFramebuffer][3] = y2; // maxBottom

        /*
        // Debug
        FBTrace("Framebuffer: %d, Box Rect Updated: left=%f, top=%f, right=%f, bottom=%f\n", gCurrentRelationViewFramebuffer,
                gBoxRectRange[gCurrentRelationViewFramebuffer][0], gBoxRectRange[gCurrentRelationViewFramebuffer][1],
                gBoxRectRange[gCurrentRelationViewFramebuffer][2], gBoxRectRange[gCurrentRelationViewFramebuffer][3]);
        */
    }

    // Call the original glRectf function
    glRectfTrue(x1, y1, x2, y2);
}

void WINAPI glVertex2dCustom(GLdouble x, GLdouble y)
{
    if (gIsCapturingGridLines)
    {
        std::lock_guard<std::mutex> lock(gGridLineMutex);

        if (gGridLineXCoords.find(gCurrentRelationViewFramebuffer) != gGridLineXCoords.end())
        {
            // Capture the x-coordinate of the grid line
            gGridLineXCoords[gCurrentRelationViewFramebuffer].push_back(x);

            if (gGridLineXCoords[gCurrentRelationViewFramebuffer].size() >= 5)
            {
                // We only need the first five x-coordinates to calculate the spacing
                gIsCapturingGridLines = false;

                // Request spacing calculation in glEnd(at the end of drawing all grid lines)
                gIsSpacingCalculationRequired = true;
            }
        }
        else
            gGridLineXCoords.insert({gCurrentRelationViewFramebuffer, std::vector<double>{x}});
    }

    // Call the original glVertex2d function
    glVertex2dTrue(x, y);
}
