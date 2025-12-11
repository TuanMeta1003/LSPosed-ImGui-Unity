package com.zrt.xposed;

import android.content.Context;
import android.content.res.AssetManager;
import android.app.Application;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;

import java.lang.annotation.Target;
import java.lang.reflect.Method;

import androidx.annotation.NonNull;

import io.github.libxposed.api.XposedInterface;
import io.github.libxposed.api.XposedModule;
import io.github.libxposed.api.annotations.BeforeInvocation;
import io.github.libxposed.api.annotations.XposedHooker;

public class StartMenu extends XposedModule {

	private static final String TAG = "TuanMeta";
	
	public StartMenu(@NonNull XposedInterface base, @NonNull ModuleLoadedParam param) {
		super(base, param);
		
		Log.d(TAG, "Initialized for process: " + param.getProcessName());
	}

	@Override
	public void onPackageLoaded(@NonNull PackageLoadedParam param) {
		super.onPackageLoaded(param);

		if (param.isFirstPackage()) {
			// Hook onCreate của UnityPlayerActivity
			try {
				Class<?> unityPlayerActivityClass = param.getClassLoader()
					.loadClass("com.unity3d.player.UnityPlayerActivity");
				
				// Tìm phương thức onCreate
				java.lang.reflect.Method onCreateMethod = unityPlayerActivityClass.getDeclaredMethod("onCreate", Bundle.class);
				
				// Hook phương thức onCreate
				hook(onCreateMethod, OnCreateHooker.class);
				
				Method dispatchTouch = unityPlayerActivityClass.getMethod("dispatchTouchEvent", MotionEvent.class);
				hook(dispatchTouch, DispatchTouchEventHooker.class);
				
			} catch (ClassNotFoundException e) {
				Log.e(TAG, "Failed to find UnityPlayerActivity class: " + e.getMessage());
			} catch (NoSuchMethodException e) {
				Log.e(TAG, "Failed to find onCreate method: " + e.getMessage());
			}
		} else {
			Log.d(TAG, "Not the first package: " + param.getPackageName());
		}
	}

	@XposedHooker
	public static class OnCreateHooker implements XposedInterface.Hooker {

		@BeforeInvocation
		public static void beforeInvocation(XposedInterface.BeforeHookCallback callback) {
			// Lấy this object (UnityPlayerActivity instance)
			Object thisObject = callback.getThisObject();
			if (thisObject != null) {
				// Xác minh rằng thisObject là UnityPlayerActivity
				String className = thisObject.getClass().getName();
				if ("com.unity3d.player.UnityPlayerActivity".equals(className)) {
					
				} else {
					Log.w(TAG, "Hooked onCreate of unexpected class: " + className);
				}

				// Gọi Menu.Start với context (p0)
				Context context = (Context) thisObject;
				try {
					Menu.Start(context);
				} catch (Exception e) {
					Log.e(TAG, "Error invoking Menu.Start: " + e.getMessage());
				}
			} else {
				Log.e(TAG, "This object is null in onCreate hook");
			}
		}
	}

	@XposedHooker
	public static class DispatchTouchEventHooker implements XposedInterface.Hooker {
		@BeforeInvocation
		public static void before(XposedInterface.BeforeHookCallback callback) {
			MotionEvent event = (MotionEvent) callback.getArgs()[0];
			int action = event.getActionMasked();
			int pointerCount = event.getPointerCount();
			float x = event.getRawX();
			float y = event.getRawY();

			try {
				boolean consumed = Menu.MotionEventClick(action, x, y, pointerCount);
				if (consumed) {
					callback.returnAndSkip(true); // Chặn sự kiện nếu ImGui xử lý
				}
			} catch (Throwable e) {
				Log.e(TAG, "MotionEventClick error: " + e.getMessage());
			}
		}
	}
}