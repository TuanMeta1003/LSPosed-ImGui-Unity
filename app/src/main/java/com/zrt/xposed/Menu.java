package com.zrt.xposed;

import android.app.Activity;
import android.content.Context;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.os.Build;
import android.view.Gravity;
import android.view.ViewTreeObserver;
import android.view.WindowManager;
import android.view.Display;

public class Menu {

	public static WindowManager manager;
	public static WindowManager.LayoutParams vParams;
	public static WindowManager windowManager, xfqManager;

	public static void Start(Context context) {
		System.loadLibrary("TuanMeta");

		manager = ((Activity) context).getWindowManager();
		vParams = getAttributes(false);
		WindowManager.LayoutParams wParams = getAttributes(true);
		GLES3JNIView display = new GLES3JNIView(context);

		// Lấy kích thước thực tế của màn hình
		Display realDisplay = manager.getDefaultDisplay();
		Point realSize = new Point();
		realDisplay.getRealSize(realSize);
		int realWidth = realSize.x;
		int realHeight = realSize.y;

		// Cập nhật LayoutParams với kích thước thực tế
		wParams.width = realWidth;
		wParams.height = realHeight;

		// Cập nhật kích thước trong native code
		display.resize(realWidth, realHeight);
		
		manager.addView(display, wParams);

		// Lắng nghe thay đổi bố cục
		display.getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
			@Override
			public void onGlobalLayout() {
				Point updatedSize = new Point();
				realDisplay.getRealSize(updatedSize);
				wParams.width = updatedSize.x;
				wParams.height = updatedSize.y;
				// Chỉ cập nhật nếu kích thước thay đổi
				if (wParams.width != updatedSize.x || wParams.height != updatedSize.y) {
					display.resize(updatedSize.x, updatedSize.y);
					manager.updateViewLayout(display, wParams);
				}
			}
		});
	}

	public static WindowManager.LayoutParams getAttributes(boolean isWindow) {
		WindowManager.LayoutParams params = new WindowManager.LayoutParams();
		int aditionalFlags = 0;
		if (Build.VERSION.SDK_INT >= 11)
			aditionalFlags = WindowManager.LayoutParams.FLAG_SPLIT_TOUCH;
		if (Build.VERSION.SDK_INT >= 3)
			aditionalFlags |= WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM;

		params = new WindowManager.LayoutParams(
				WindowManager.LayoutParams.MATCH_PARENT,
				WindowManager.LayoutParams.MATCH_PARENT,
				0,
				0,
				WindowManager.LayoutParams.TYPE_APPLICATION,
				WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE |
				WindowManager.LayoutParams.FLAG_LAYOUT_IN_OVERSCAN |
				WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN |
				WindowManager.LayoutParams.FLAG_FULLSCREEN |
				WindowManager.LayoutParams.FLAG_SPLIT_TOUCH | aditionalFlags,
				PixelFormat.TRANSPARENT
		);

		if (isWindow) {
			params.flags |= WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE;
		}
		params.format = PixelFormat.RGBA_8888;
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
			params.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
		}
		params.gravity = Gravity.LEFT | Gravity.TOP;
		params.x = params.y = 0;
		params.width = params.height = isWindow ? WindowManager.LayoutParams.MATCH_PARENT : 0;
		return params;
	}
	
	public static native boolean MotionEventClick(int action, float x, float y, int pointerCount);
}