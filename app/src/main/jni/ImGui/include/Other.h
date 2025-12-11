#pragma once
#include <unordered_map>

ImVec4 accentColor = ImColor(89, 164, 232);
ImVec4 watermarkColor = ImColor(89, 164, 232);
ImVec4 watermarkGlowColor = ImColor(89, 164, 232, 127);

namespace Features {
	static inline ImVec2  operator*(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
	static inline ImVec2  operator/(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
	static inline ImVec2  operator+(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x + rhs, lhs.y + rhs); }
	static inline ImVec2  operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
	static inline ImVec2  operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
	static inline ImVec2  operator-(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x - rhs, lhs.y - rhs); }
	static inline ImVec2  operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
	static inline ImVec2  operator/(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
	static inline ImVec2& operator*=(ImVec2& lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
	static inline ImVec2& operator/=(ImVec2& lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
	static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
	static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
	static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
	static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
	static inline ImVec4  operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
	static inline ImVec4  operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
	static inline ImVec4  operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
	
	void AddCircleGlow(ImVec2 position, float radius, ImColor color, int segments, int thickness, int glowSize) {
		ImGui::GetBackgroundDrawList()->AddCircle(position, radius, color, segments, thickness);
		for (int i = 0; i < glowSize; i++) {
			ImGui::GetBackgroundDrawList()->AddCircle(position, radius, ImColor(color.Value.x, color.Value.y, color.Value.z, color.Value.w * (1.0f / (float)glowSize) * (((float)(glowSize - i)) / (float)glowSize)), segments, thickness + i);
		}
	}
	
	inline void AddArcLine(float x, float y, float radius, float min_angle, float max_angle, ImColor color, float thickness) {
		ImGui::GetBackgroundDrawList()->PathArcTo(ImVec2(x, y), radius, Deg2Rad * min_angle, Deg2Rad * max_angle, 32);
		ImGui::GetBackgroundDrawList()->PathStroke(color, false, thickness);
	}
	
	void AddArcTriangle(float x, float y, float radius, float min_angle, float max_angle, ImColor color, float thickness) {
		float half_angle = (max_angle - min_angle) / 2.0f;
		float center_angle = min_angle + half_angle;
		
		ImVec2 center(x + cos(Deg2Rad * center_angle) * radius, y + sin(Deg2Rad * center_angle) * radius);
		float triangle_side = sin(Deg2Rad * half_angle) * radius * 2.0f;
		
		ImVec2 p1(center.x + cos(Deg2Rad * (min_angle + half_angle - 90.0f)) * triangle_side / 2.0f, center.y + sin(Deg2Rad * (min_angle + half_angle - 90.0f)) * triangle_side / 2.0f);
		ImVec2 p2(center.x + cos(Deg2Rad * (min_angle + half_angle)) * triangle_side / 2.0f, center.y + sin(Deg2Rad * (min_angle + half_angle)) * triangle_side / 2.0f);
		ImVec2 p3(center.x + cos(Deg2Rad * (min_angle + half_angle + 90.0f)) * triangle_side / 2.0f, center.y + sin(Deg2Rad * (min_angle + half_angle + 90.0f)) * triangle_side / 2.0f);
		
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(p1, p2, p3, color);
	}
	
	void AddTextCentered(std::string text) {
		auto window_size = ImGui::GetWindowSize().x;
		auto text_size = ImGui::CalcTextSize(text.c_str()).x;
		ImGui::SetCursorPosX((window_size - text_size) * 0.5f);
		ImGui::Text("%s", text.c_str());
		ImGui::PopStyleColor();
	}
	
	void ColorPicker(const char* name, ImVec4 &color) {
		static ImVec4 backupColor;
		
		bool openingPopup = ImGui::ColorButton((string(name) + string(("##3b"))).c_str(), color, ImGuiColorEditFlags_AlphaPreview, ImVec2(25, 25));
		if (openingPopup) {
			ImGui::OpenPopup(name);
			backupColor = color;
		}
	
		if (ImGui::BeginPopup(name)) {
			ImGui::ColorPicker4(("##Color picker"), (float *) &color, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoOptions);
			ImGui::EndPopup();
		}
	}
	
	void Toggle(const char* label, bool* v, bool isEnable = true) {
		ImVec2 position = ImGui::GetCursorScreenPos();
		float height = ImGui::GetFrameHeight();
		float width = height * 1.55f;
		float radius = height * 0.50f;
		
		ImGui::InvisibleButton(label, ImVec2(width, height));
		if (isEnable && ImGui::IsItemClicked())
			*v = !*v;
		
		float t = *v ? 1.0f : 0.0f;
		float animationSpeed = 0.2;
		
		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		if (isEnable && g.LastActiveId == g.CurrentWindow->GetID(label)) {
			float animationTime = ImSaturate(g.LastActiveIdTimer / animationSpeed);
			t = *v ? (animationTime) : (1.0f - animationTime);
		}
		
		if (isEnable)
		{
			if (ImGui::IsItemActive())
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 255, 255, int(255 * t))));
			else
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(200, 200, 200, int(255 * style.Alpha))));
		}
		
		if (!isEnable)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(100, 100, 100, int(255 * style.Alpha))));
		
		ImGui::GetWindowDrawList()->AddRectFilled(position, ImVec2(position.x + width, position.y + height), ImGui::GetColorU32(ImLerp(ImVec4(ImColor(30, 30, 30, 255)), ImColor(accentColor.x, accentColor.y, accentColor.z, accentColor.w * 1), t)), height * 0.5f);
		ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(position.x + radius + t * (width - radius * 2.0f), position.y + radius), radius - 1.5f, ImGui::GetColorU32(ImLerp(ImVec4(ImColor(23, 23, 23, int(255 * style.Alpha))), ImColor(255, 255, 255, int(255 * style.Alpha)), t)));
		
		const ImVec2 labelSize = ImGui::CalcTextSize(label, 0, 1);
		ImVec2 labelPosition = ImVec2(position.x + width + style.ItemInnerSpacing.x + 6, position.y + 7.5f);
		if (labelSize.x > 0.f)
			ImGui::RenderText(labelPosition, label);
		ImGui::PopStyleColor();
	}
	
	bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& gimgui = *GImGui;
		const ImGuiStyle& style = gimgui.Style;
		const ImGuiID id = window->GetID(label);
		const float window_width = ImGui::GetWindowWidth() - 70;
		
		const ImVec2 label_size = ImGui::CalcTextSize(label, 0, 1);
		const ImRect frame_bb(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + label_size.y + 15), ImVec2(window->DC.CursorPos.x + window_width, window->DC.CursorPos.y + label_size.y + 30.5f));
		const ImRect total_bb(window->DC.CursorPos, frame_bb.Max );
		
		ImGui::ItemAdd(total_bb, id, &frame_bb);
		ImGui::ItemSize(total_bb, style.FramePadding.y);
		
		if (format == NULL)
			format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
			
		const bool hovered = ImGui::ItemHoverable(frame_bb, id, gimgui.LastItemData.ItemFlags);
		const bool clicked = (hovered && gimgui.IO.MouseClicked[0]);
		if (clicked) {
			ImGui::SetActiveID(id, window);
			ImGui::SetFocusID(id, window);
			ImGui::FocusWindow(window);
			gimgui.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
		}
		
		ImRect grab_bb;
		const bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
		if (value_changed)
			ImGui::MarkItemEdited(id);
			
		char value_buf[64];
		const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

		static std::unordered_map<ImGuiID, float> values;
		auto value = values.find(id);
		if (value == values.end()) {
			values.insert({id, 0.0f});
			value = values.find(id);
		}
		
		float percent = (grab_bb.Max.x - window->DC.CursorPos.x - frame_bb.Min.x + window->Pos.x) / frame_bb.GetWidth();
		
		value->second = ImLerp(value->second, percent * frame_bb.GetWidth(), 0.125f);
		
		window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, ImColor(30, 30, 30, int(255 * style.Alpha)), 12);
		window->DrawList->AddRectFilled(frame_bb.Min, ImVec2(frame_bb.Min.x + value->second, frame_bb.Max.y), ImColor(accentColor.x, accentColor.y, accentColor.z, accentColor.w * value->second), 12);
		window->DrawList->AddCircleFilled({frame_bb.Min.x + value->second, frame_bb.GetCenter().y}, 13.6, ImColor(255, 255, 255, int(255 * style.Alpha)));
		
		if (hovered || ImGui::IsItemActive())
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 255, 255, int(255 * value->second))));
		else
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(200, 200, 200, int(255 * style.Alpha))));
		
		ImGui::RenderText(total_bb.Min, label);
		ImGui::RenderTextClipped(total_bb.Min, total_bb.Max, value_buf, value_buf_end, NULL, ImVec2( 1.f, 0.f ));
	
		ImGui::PopStyleColor();
		return value_changed;
	}
	
	bool SliderFloat(const char* label, float* v, float v_min = 0.0f, float v_max = 100.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
		return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
	}
	
	bool SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max, const char* format, ImGuiSliderFlags flags) {
		if (format == NULL)
	 	 	format = "%.0f deg";
			
		float v_deg = (*v_rad) * 360.0f / (2 * IM_PI);
		bool value_changed = SliderFloat(label, &v_deg, v_degrees_min, v_degrees_max, format, flags);
		*v_rad = v_deg * (2 * IM_PI) / 360.0f;
		return value_changed;
	}
	
	bool SliderInt(const char* label, int* v, int v_min = 0, int v_max = 100, const char* format = "%d", ImGuiSliderFlags flags = 0) {
		return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
	}

}
