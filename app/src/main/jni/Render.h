#pragma once

extern "C" {
	JNIEXPORT void JNICALL Java_com_zrt_xposed_GLES3JNIView_init(JNIEnv* env, jclass cls);
	JNIEXPORT void JNICALL Java_com_zrt_xposed_GLES3JNIView_resize(JNIEnv* env, jobject obj, jint width, jint height);
	JNIEXPORT void JNICALL Java_com_zrt_xposed_GLES3JNIView_step(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_zrt_xposed_GLES3JNIView_imgui_Shutdown(JNIEnv* env, jobject obj);
	JNIEXPORT jboolean JNICALL Java_com_zrt_xposed_Menu_MotionEventClick(JNIEnv* env, jclass clazz, jint action, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_zrt_xposed_GLES3JNIView_real(JNIEnv* env, jobject obj, jint width, jint height);
	JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved);
};

JNIEXPORT void JNICALL
Java_com_zrt_xposed_GLES3JNIView_init(JNIEnv* env, jclass cls) {
	if (!g_Initialized) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		
		PurpleStyle();
		
		inter = io.Fonts->AddFontFromMemoryTTF(Inter, sizeof(Inter), 28.0f, NULL, io.Fonts->GetGlyphRangesVietnamese());
		
		float baseFontSize = 28.0f;
		float iconFontSize = baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly
		static constexpr ImWchar iconsRanges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
		ImFontConfig iconsConfig;
		iconsConfig.MergeMode = true;
		iconsConfig.PixelSnapH = true;
		iconsConfig.GlyphMinAdvanceX = iconFontSize;
		io.Fonts->AddFontFromMemoryCompressedTTF(fa_solid_900_compressed_data, fa_solid_900_compressed_size, iconFontSize, &iconsConfig, iconsRanges);
		
		interbold = io.Fonts->AddFontFromMemoryTTF(InterBold, sizeof(InterBold), 28.f, NULL, io.Fonts->GetGlyphRangesVietnamese());
		HorstFont = io.Fonts->AddFontFromMemoryTTF(MonsterFont, sizeof(MonsterFont), 34.0f);
		smallFont = io.Fonts->AddFontFromMemoryTTF(Inter, sizeof(Inter), 19.0f, NULL, io.Fonts->GetGlyphRangesVietnamese());
		IconsFont = io.Fonts->AddFontFromMemoryTTF(Icons, sizeof(Icons), 35);
		
		ImGui_ImplAndroid_Init();
		ImGui_ImplOpenGL3_Init("#version 300 es");
		
		ImGui::GetStyle().ScrollbarSize = 10.0f;
		ImGui::GetStyle().ScaleAllSizes(3.0f);
		
		g_Initialized = true;
	}
	return;
}

JNIEXPORT void JNICALL
Java_com_zrt_xposed_GLES3JNIView_resize(JNIEnv* env, jobject obj, jint width, jint height) {
	if (g_Initialized) {
		screenWidth = (int)width;
		screenHeight = (int)height;
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = NULL;
		ImGui::GetIO().DisplaySize = ImVec2((float)width, (float)height);
	}
	return;
}

JNIEXPORT void JNICALL
Java_com_zrt_xposed_GLES3JNIView_imgui_Shutdown(JNIEnv* env, jobject obj) {
	if (!g_Initialized) return;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplAndroid_Shutdown();
	ImGui::DestroyContext();

	g_Initialized = false;
}

JNIEXPORT jboolean JNICALL
Java_com_zrt_xposed_Menu_MotionEventClick(JNIEnv* env, jclass clazz, jint action, jfloat x, jfloat y) {
	if (!g_Initialized)
		return JNI_FALSE;

	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(x, y);

	switch (action) {
		case 0: // ACTION_DOWN
			io.MouseDown[0] = true;
			break;
		case 1: // ACTION_UP
		case 3: // ACTION_CANCEL
			io.MouseDown[0] = false;
			break;
		case 2: // ACTION_MOVE
			break;
		default:
			return JNI_FALSE;
	}
	return io.WantCaptureMouse ? JNI_TRUE : JNI_FALSE;
}