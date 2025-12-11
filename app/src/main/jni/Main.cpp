#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <thread>
#include "Call_Me.h"
#include "Render.h"
#include "Draw.h"

JNIEXPORT void JNICALL
Java_com_zrt_xposed_GLES3JNIView_step(JNIEnv* env, jobject obj) {
	if (g_Initialized && jvm != nullptr) {
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplAndroid_NewFrame(screenWidth, screenHeight);
		ImGui::NewFrame();
		
		DrawESP(ImGui::GetBackgroundDrawList());
		
		ImGui::Begin("ImGui Window", nullptr);
		ImGui::End();
		
		ImGui::RenderNotifications();
		ImGui::EndFrame();
		ImGui::Render();

		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

ProcMap il2cppMap;
using KittyScanner::RegisterNativeFn;
void *Init_Thread(){
	
	sleep(3);
	
	while (!il2cppMap.isValid()) {
		il2cppMap = KittyMemory::getLibraryBaseMap(oxorany("libil2cpp.so"));
		sleep(1);
	}

	Attach();

	return nullptr;
}

void on_library_loaded(const char *name, void *handle)
{
	if (ends_with(std::string(name), "libil2cpp.so"))
	{
		
	}
}

extern "C" [[gnu::visibility("default")]] [[gnu::used]]
NativeOnModuleLoaded native_init(const NativeAPIEntries *entries) {
	hook_func = entries->hook_func;

	return on_library_loaded;
}

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* globalEnv;
	vm->GetEnv((void**)&globalEnv, JNI_VERSION_1_6);
	
	jvm = vm;
	
	if (jvm == nullptr) {
		Init_JVM();
	}
	
	std::thread(Init_Thread).detach();

	return JNI_VERSION_1_6;
}