#pragma once

std::string jstring2string(JNIEnv *env, jstring jStr) {
	if (!jStr)
		return "";

	const jclass stringClass = env->GetObjectClass(jStr);
	const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
	const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

	size_t length = (size_t) env->GetArrayLength(stringJbytes);
	jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

	std::string ret = std::string((char *)pBytes, length);
	env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

	env->DeleteLocalRef(stringJbytes);
	env->DeleteLocalRef(stringClass);
	return ret;
}

jobject getGlobalContext(JNIEnv *env) {
    jclass activityThread = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThread,"currentActivityThread","()Landroid/app/ActivityThread;");
    jobject at = env->CallStaticObjectMethod(activityThread, currentActivityThread);

    jmethodID getApplication = env->GetMethodID(activityThread, "getApplication","()Landroid/app/Application;");
    jobject context = env->CallObjectMethod(at, getApplication);
    
    return context;
}

JavaVM *jvm;
typedef jint(*jni_getvms_t)(JavaVM**, jsize, jsize*);
static jni_getvms_t JNI_GetCreatedJavaVMs_p;

jni_getvms_t getVMFunction_fake() {
	void* library = xdl_open("libart.so", 0);
	if(library == nullptr) {
		//LOGE("Google trolling failed. Giving up.");
		return nullptr;
	}
	void* sym = xdl_sym(library, "JNI_GetCreatedJavaVMs", 0);
	xdl_close(library);
	return (jni_getvms_t)sym;
}

jni_getvms_t getVMFunction() {
	void* library = xdl_open("libnativehelper.so", RTLD_LAZY);
	if(library == nullptr) {
		//LOGE("Time to troll Google!");
		return getVMFunction_fake();
	}
	void* sym = xdl_sym(library, "JNI_GetCreatedJavaVMs", 0);
	xdl_close(library);
	return sym == nullptr ? getVMFunction_fake() : (jni_getvms_t) sym;
}

void Init_JVM()
{
	jsize cnt;
	JNI_GetCreatedJavaVMs_p = getVMFunction();
	if(!JNI_GetCreatedJavaVMs_p || JNI_GetCreatedJavaVMs_p(&jvm, 1, &cnt) != JNI_OK || cnt == 0) {
		LOGE("Failed to find a JVM");
	}
	
    JNIEnv *env;
	jint result = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
	if (result == JNI_EDETACHED)
		result = jvm->AttachCurrentThread(&env, nullptr);
	
	if (result != JNI_OK)
		LOGE("Can't get a JNIEnv");
}

std::string getClipboard() {
    std::string result;
    JNIEnv *env;
    jvm->AttachCurrentThread(&env, NULL);

    jobject mInitialApplication = getGlobalContext(env);

    jclass contextClass = env->FindClass(oxorany("android/content/Context"));
    jmethodID getSystemServiceMethod = env->GetMethodID(contextClass, oxorany("getSystemService"), oxorany("(Ljava/lang/String;)Ljava/lang/Object;"));
    
    jstring serviceName = env->NewStringUTF(oxorany("clipboard"));
    jobject clipboardManager = env->CallObjectMethod(mInitialApplication, getSystemServiceMethod, serviceName);
    env->DeleteLocalRef(serviceName);

    jclass clipboardClass = env->FindClass(oxorany("android/content/ClipboardManager"));
    jmethodID getTextMethod = env->GetMethodID(clipboardClass, oxorany("getText"), oxorany("()Ljava/lang/CharSequence;"));

    jobject textObj = env->CallObjectMethod(clipboardManager, getTextMethod);
    if (textObj) {
        jclass charSequenceClass = env->FindClass(oxorany("java/lang/CharSequence"));
        jmethodID toStringMethod = env->GetMethodID(charSequenceClass, oxorany("toString"), oxorany("()Ljava/lang/String;"));

        jstring textStr = (jstring) env->CallObjectMethod(textObj, toStringMethod);
        const char *textChars = env->GetStringUTFChars(textStr, nullptr);
        result = textChars;
        env->ReleaseStringUTFChars(textStr, textChars);
        env->DeleteLocalRef(textStr);

        env->DeleteLocalRef(charSequenceClass);
        env->DeleteLocalRef(textObj);
    }

    env->DeleteLocalRef(clipboardClass);
    env->DeleteLocalRef(clipboardManager);
    env->DeleteLocalRef(contextClass);
    env->DeleteLocalRef(mInitialApplication);

    jvm->DetachCurrentThread();
    return result;
}


const char* GetCurrentTime2(bool date = false) {
	std::time_t currentTime = std::time(nullptr);
	std::tm* timeInfo = std::localtime(&currentTime);

	std::ostringstream oss;
	if (date) {
		oss << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S");
	} else {
		oss << std::put_time(timeInfo, "%H:%M:%S");
	}
	std::string timeString = oss.str();
	return strdup(timeString.c_str());
}

const char* GetCurrentTime(bool date = false) {
	JNIEnv* env = nullptr;
	if (jvm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK || !env)
		return GetCurrentTime2(date);

	jclass dateClass = env->FindClass("java/util/Date");
	jmethodID dateCtor = env->GetMethodID(dateClass, "<init>", "()V");
	jobject dateObj = env->NewObject(dateClass, dateCtor);

	const char* format = date ? "yyyy-MM-dd HH:mm:ss" : "HH:mm:ss";
	jclass sdfClass = env->FindClass("java/text/SimpleDateFormat");
	jmethodID sdfCtor = env->GetMethodID(sdfClass, "<init>", "(Ljava/lang/String;)V");
	jstring jFormat = env->NewStringUTF(format);
	jobject sdfObj = env->NewObject(sdfClass, sdfCtor, jFormat);

	jclass tzClass = env->FindClass("java/util/TimeZone");
	jmethodID getTZ = env->GetStaticMethodID(tzClass, "getTimeZone", "(Ljava/lang/String;)Ljava/util/TimeZone;");
	jstring jTz = env->NewStringUTF("GMT+7");
	jobject tzObj = env->CallStaticObjectMethod(tzClass, getTZ, jTz);

	jmethodID setTZ = env->GetMethodID(sdfClass, "setTimeZone", "(Ljava/util/TimeZone;)V");
	env->CallVoidMethod(sdfObj, setTZ, tzObj);

	jmethodID formatMethod = env->GetMethodID(sdfClass, "format", "(Ljava/util/Date;)Ljava/lang/String;");
	jstring jResult = (jstring)env->CallObjectMethod(sdfObj, formatMethod, dateObj);

	const char* cResult = env->GetStringUTFChars(jResult, nullptr);
	char* ret = strdup(cResult);
	env->ReleaseStringUTFChars(jResult, cResult);

	return ret;
}