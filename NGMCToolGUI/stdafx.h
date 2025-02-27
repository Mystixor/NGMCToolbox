// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//#include "targetver.h"

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <winternl.h>
#include <imagehlp.h>

#ifdef DLL_EXPORT
#define DINPUT8_API __declspec(dllexport)
#else
#define DINPUT8_API __declspec(dllimport)
#endif

// reference additional headers your program requires here
#include <vector>
#include <string>
#include <format>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdio.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// basic table macros
#define ROW_BEGIN(name)            \
	ImGui::TableNextRow();         \
	ImGui::TableSetColumnIndex(0); \
	ImGui::TextUnformatted(name);  \
	ImGui::TableSetColumnIndex(1)

#define ROW_CSTRING(name, cstring) \
	ROW_BEGIN(name);               \
	ImGui::TextUnformatted(cstring)

#define ROW_FORMAT(name, ...) \
	ROW_BEGIN(name);          \
	ImGui::TextUnformatted(std::format(__VA_ARGS__).c_str())

#define ROW_STRING(name, string) \
	ROW_BEGIN(name);             \
	ImGui::TextUnformatted((string).c_str())

// extended table macros
#define ROW_PRINTF(name, ...) \
	ROW_BEGIN(name);          \
	ImGui::Text(__VA_ARGS__)

#define ROW_QUOTE(name, value) \
	ROW_FORMAT(name, "\"{}\"", value)

#define ROW_SIZE(name, size) \
	ROW_STRING(name, GetPrettySize(size))

#define ROW_VALUE(name, value) \
	ROW_FORMAT(name, "{}", value)
