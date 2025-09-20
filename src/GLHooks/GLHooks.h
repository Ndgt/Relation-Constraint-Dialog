#pragma once

#include <array>

#include <Windows.h>
#include <gl/GL.h>

/**
 * @brief Default grid spacing value in OpenGL coordinate system
 */
constexpr double defaultGLGridSpacing = 34.0;

/**
 * @brief The default bounding rectangle for boxes in the relation view.
 * @note The format is {minLeft, minTop, maxRight, maxBottom}.
 *       The initial values are inverted (min > max) so that the first comparison will always succeed
 *       and set the initial range correctly.
 */
constexpr std::array<double, 4> defaultBoxRectRange = {99900.0, 99900.0, -99900.0, -99900.0};

/**
 * @brief Called by the plugin to get the last calculated grid spacing of rendered relation view's grid lines
 * @param framebuffer The framebuffer used by the relation view
 * @return The last calculated grid spacing. 34.0(default grid spacing) is returned if framebuffer is not found
 */
double getLastGridSpacing(GLint framebuffer);

/**
 * @brief Called by the plugin to get the bounding rectangle of rendered boxes in the relation view
 * @param framebuffer The framebuffer used by the relation view
 * @return An array containing {minLeft, minTop, maxRight, maxBottom}. defaultBoxRectRange is returned if framebuffer is not found
 * @note It seems that around 100000 is the limit for 'framing' function with 'A' hotkey in the relation view to work properly.
 * @note The range is only considered as the range of rectangle coordinates, not other shapes such as connectors and texts.
 */
std::array<double, 4> getBoxRectRange(GLint framebuffer);

/**
 * @brief Called by the plugin to start hooking the OpenGL functions
 * @details Attaches custom functions to OpenGL functions.
 * @return true if initialization is successful, false otherwise
 */
bool startHook();

/**
 * @brief Called by the plugin to end hooking the OpenGL functions
 * @details Detaches custom functions from OpenGL functions.
 */
void endHook();

/**
 * @brief Detour function for OpenGL glBindFramebuffer
 * @details Detects binding of the specific framebuffer to identify relation view rendering context.
 * @param target The target parameter passed to glBindFramebuffer
 * @param framebuffer The framebuffer parameter passed to glBindFramebuffer
 * @note Set gIsRelationViewRendering and gIsDataResetRequired flags when the framebuffer ID
 *       used by the relation view is detected.
 */
void glBindFramebufferCustom(GLenum target, GLuint framebuffer);

/**
 * @brief Detour function for OpenGL glDeleteFramebuffers
 * @param n The n parameter passed to glDeleteFramebuffers
 * @param framebuffers The framebuffers parameter passed to glDeleteFramebuffers
 * @note This monitors the change of framebuffer used by the relation view.
 */
void glDeleteFramebuffersCustom(GLsizei n, const GLuint *framebuffers);

/**
 * @brief Detour function for OpenGL glBegin
 * @details Detects mode GL_LINES to start capturing vertex data for grid line spacing calculation.
 * @param mode The mode parameter passed to glBegin
 * @note If gIsRelationViewRendering and gIsDataResetRequired are true, reset internal data for new capture
 *       and set gIsCapturingGridLines flag to true.
 */
void WINAPI glBeginCustom(GLenum mode);

/**
 * @brief Detour function for OpenGL glEnd
 */
void WINAPI glEndCustom(void);

/**
 * @brief Detour function for OpenGL glRectf
 * @param x1 The x1 coordinate passed to glRectf
 * @param y1 The y1 coordinate passed to glRectf
 * @param x2 The x2 coordinate passed to glRectf
 * @param y2 The y2 coordinate passed to glRectf
 */
void WINAPI glRectfCustom(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

/**
 * @brief Detour function for OpenGL glVertex2d
 * @details Captures x-coordinates of grid lines when rendering the relation view.
 * @param x The x coordinate passed to glVertex2d
 * @param y The y coordinate passed to glVertex2d
 * @note Set gIsSpacingCalculationRequired flag and reset gIsCapturingGridLines flag
 *       when enough grid line coordinates have been captured so that we can calculate
 *       the spacing in the glEndCustom function(at the end of drawing all grid lines).
 */
void WINAPI glVertex2dCustom(GLdouble x, GLdouble y);