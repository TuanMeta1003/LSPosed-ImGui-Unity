#pragma once

void DrawESP(ImDrawList *draw)
{
	char tmp_watermark[256];
	std::sprintf(tmp_watermark, oxorany("https://github.com/TuanMeta1003 - %.0f FPS"), ImGui::GetIO().Framerate);
	
	draw->AddText(ImVec2(40.0f, screenHeight - 40.0f), IM_COL32(255, 255, 255, 255), tmp_watermark);
}