#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "imstb_rectpack.h"
#include "imstb_textedit.h"
#include "imstb_truetype.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
#include "include/Call_Include.h"
#include "images/Call_Images.h"

int screenWidth = 0;
int screenHeight = 0;

bool g_Initialized = false;

void ShutdownImGui()
{
    ImGui_ImplAndroid_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}