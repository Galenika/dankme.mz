#include "Menu.hpp"

#include "Options.hpp"

#include "Structs.hpp"

#include "features/Miscellaneous.hpp"
#include "features/KitParser.hpp"
#include "features/Skinchanger.hpp"

#include "imgui/imgui_internal.h"

#include <functional>
#include <experimental/filesystem> // hack

#define NIGHTMODE

namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, int height_in_items = -1)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size(), height_in_items);
	}

	static bool ListBox(const char* label, int* current_item, std::function<const char*(int)> lambda, int items_count, int height_in_items)
	{
		return ImGui::ListBox(label, current_item, [](void* data, int idx, const char** out_text)
		{
			*out_text = (*reinterpret_cast< std::function<const char*(int)>* >(data))(idx);
			return true;
		}, &lambda, items_count, height_in_items);
	}

	bool LabelClick(const char* concatoff, const char* concaton, const char* label, bool* v, const char* unique_id)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		// The concatoff/on thingies were for my weapon config system so if we're going to make that, we still need this aids.
		char Buf[64];
		_snprintf(Buf, 62, "%s%s", ((*v) ? concatoff : concaton), label);

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(unique_id);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(label_size.y + style.FramePadding.y * 2, label_size.y + style.FramePadding.y * 2));
		ItemSize(check_bb, style.FramePadding.y);

		ImRect total_bb = check_bb;
		if (label_size.x > 0)
			SameLine(0, style.ItemInnerSpacing.x);

		const ImRect text_bb(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + label_size);
		if (label_size.x > 0)
		{
			ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
			total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
		}

		if (!ItemAdd(total_bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
			*v = !(*v);

		if (label_size.x > 0.0f)
			RenderText(check_bb.GetTL(), Buf);

		return pressed;
	}

	void KeyBindButton(ButtonCode_t* key)
	{
		bool clicked = false;

		std::string text = g_InputSystem->ButtonCodeToString(*key);
		std::string unique_id = std::to_string((int)key);

		if (*key <= BUTTON_CODE_NONE)
			text = "Key not set";

		if (input_shouldListen && input_receivedKeyval == key) {
			clicked = true;
			text = "...";
		}
		text += "]";

		ImGui::SameLine();
		ImGui::LabelClick("[", "[", text.c_str(), &clicked, unique_id.c_str());

		if (clicked)
		{
			input_shouldListen = true;
			input_receivedKeyval = key;
		}

		if (*key == KEY_DELETE)
		{
			*key = BUTTON_CODE_NONE;
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Bind the \"del\" key to remove current bind.");
	}

	ImGuiID Colorpicker_Close = 0;
	__inline void CloseLeftoverPicker() { if (Colorpicker_Close) ImGui::ClosePopup(Colorpicker_Close); }
	void ColorPickerBox(const char* picker_idname, float col_ct[], float col_t[], float col_loc[], float col_ct_invis[], float col_t_invis[], float col_loc_invis[], bool alpha = true)
	{
		ImGui::SameLine();
		//static bool switch_entity_teams = false;
		static int team_selections = 0;
		static bool switch_color_vis = false;
		bool open_popup;
		//bool open_popup = ImGui::ColorButton(picker_idname, switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
		switch (team_selections)
		{
		case 0:
			open_popup = ImGui::ColorButton(picker_idname, switch_color_vis ? col_t : col_t_invis, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
			break;
		case 1:
			open_popup = ImGui::ColorButton(picker_idname, switch_color_vis ? col_ct : col_ct_invis, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
			break;
		case 2:
			open_popup = ImGui::ColorButton(picker_idname, switch_color_vis ? col_loc : col_loc_invis, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
			break;
		default:
			open_popup = ImGui::ColorButton(picker_idname, switch_color_vis ? col_t : col_t_invis, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
			team_selections = 0;
			break;
		}
		if (open_popup)
		{
			ImGui::OpenPopup(picker_idname);
			Colorpicker_Close = ImGui::GetID(picker_idname);
		}

		if (ImGui::BeginPopup(picker_idname))
		{
			//const char* button_name0 = switch_entity_teams ? "Terrorists" : "Counter-Terrorists";
			const char* button_name0;
			switch (team_selections)
			{
			case 0:
				button_name0 = "Terrorists";
				break;
			case 1:
				button_name0 = "Counter-Terrorists";
				break;
			case 2:
				button_name0 = "Local Player";
				break;
			}
			if (ImGui::Button(button_name0, ImVec2(-1, 0)))
				team_selections++;
				//switch_entity_teams = !switch_entity_teams;

			const char* button_name1 = switch_color_vis ? "Invisible" : "Visible";
			if (ImGui::Button(button_name1, ImVec2(-1, 0)))
				switch_color_vis = !switch_color_vis;

			std::string id_new = picker_idname;
			id_new += "##pickeritself_";

			//ImGui::ColorPicker4(id_new.c_str(), switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
			switch (team_selections)
			{
			case 0:
				ImGui::ColorPicker4(id_new.c_str(), switch_color_vis ? col_t : col_t_invis, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
				break;
			case 1:
				ImGui::ColorPicker4(id_new.c_str(), switch_color_vis ? col_ct : col_ct_invis, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
				break;
			case 2:
				ImGui::ColorPicker4(id_new.c_str(), switch_color_vis ? col_loc : col_loc_invis, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
				break;
			}
			ImGui::EndPopup();
		}
	}
	void ColorPickerBox(const char* picker_idname, float col_ct[], float col_t[], float col_loc[], bool alpha = true)
	{
		ImGui::SameLine();
		//static bool switch_entity_teams = false;
		static int team_selections = 0;
		bool open_popup;
		//bool open_popup = ImGui::ColorButton(picker_idname, switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
		switch (team_selections)
		{
		case 0:
			open_popup = ImGui::ColorButton(picker_idname, col_t, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
			break;
		case 1:
			open_popup = ImGui::ColorButton(picker_idname, col_ct, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
			break;
		case 2:
			open_popup = ImGui::ColorButton(picker_idname, col_loc, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
			break;
		default:
			open_popup = ImGui::ColorButton(picker_idname, col_t, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
			team_selections = 0;
			break;
		}
		if (open_popup)
		{
			ImGui::OpenPopup(picker_idname);
			Colorpicker_Close = ImGui::GetID(picker_idname);
		}

		if (ImGui::BeginPopup(picker_idname))
		{
			//const char* button_name0 = switch_entity_teams ? "Terrorists" : "Counter-Terrorists";
			const char* button_name0;
			switch (team_selections)
			{
			case 0:
				button_name0 = "Terrorists";
				break;
			case 1:
				button_name0 = "Counter-Terrorists";
				break;
			case 2:
				button_name0 = "Local Player";
				break;
			}
			if (ImGui::Button(button_name0, ImVec2(-1, 0)))
				team_selections++;
			//switch_entity_teams = !switch_entity_teams;

			std::string id_new = picker_idname;
			id_new += "##pickeritself_";

			//ImGui::ColorPicker4(id_new.c_str(), switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
			switch (team_selections)
			{
			case 0:
				ImGui::ColorPicker4(id_new.c_str(), col_t, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
				break;
			case 1:
				ImGui::ColorPicker4(id_new.c_str(), col_ct, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
				break;
			case 2:
				ImGui::ColorPicker4(id_new.c_str(), col_loc, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
				break;
			}
			ImGui::EndPopup();
		}
	}

	void ColorPickerBox(const char* picker_idname, float col_vis[], float col_invis[], float col_wireframe[], int overload)
	{
		ImGui::SameLine();
		//static bool switch_entity_teams = false;
		static int team_selections = 0;
		bool open_popup;
		//bool open_popup = ImGui::ColorButton(picker_idname, switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
		switch (team_selections)
		{
		case 0:
			open_popup = ImGui::ColorButton(picker_idname, col_vis, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
			break;
		case 1:
			open_popup = ImGui::ColorButton(picker_idname, col_invis, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
			break;
		case 2:
			open_popup = ImGui::ColorButton(picker_idname, col_wireframe, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
			break;
		default:
			open_popup = ImGui::ColorButton(picker_idname, col_vis, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
			team_selections = 0;
			break;
		}
		if (open_popup)
		{
			ImGui::OpenPopup(picker_idname);
			Colorpicker_Close = ImGui::GetID(picker_idname);
		}

		if (ImGui::BeginPopup(picker_idname))
		{
			//const char* button_name0 = switch_entity_teams ? "Terrorists" : "Counter-Terrorists";
			const char* button_name0;
			switch (team_selections)
			{
			case 0:
				button_name0 = "Visible";
				break;
			case 1:
				button_name0 = "Invisible";
				break;
			case 2:
				button_name0 = "Wireframe";
				break;
			}
			if (ImGui::Button(button_name0, ImVec2(-1, 0)))
				team_selections++;
			//switch_entity_teams = !switch_entity_teams;

			std::string id_new = picker_idname;
			id_new += "##pickeritself_";

			//ImGui::ColorPicker4(id_new.c_str(), switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
			switch (team_selections)
			{
			case 0:
				ImGui::ColorPicker4(id_new.c_str(), col_vis, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar);
				break;
			case 1:
				ImGui::ColorPicker4(id_new.c_str(), col_invis, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar);
				break;
			case 2:
				ImGui::ColorPicker4(id_new.c_str(), col_wireframe, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar);
				break;
			}
			ImGui::EndPopup();
		}
	}

	void ColorPickerBox(const char* picker_idname, float col_n[], bool alpha = true)
	{
		ImGui::SameLine();
		bool open_popup = ImGui::ColorButton(picker_idname, col_n, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
		if (open_popup)
		{
			ImGui::OpenPopup(picker_idname);
			Colorpicker_Close = ImGui::GetID(picker_idname);
		}

		if (ImGui::BeginPopup(picker_idname))
		{
			std::string id_new = picker_idname;
			id_new += "##pickeritself_";

			ImGui::ColorPicker4(id_new.c_str(), col_n, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
			ImGui::EndPopup();
		}
	}
	// This can be used anywhere, in group boxes etc.
	void SelectTabs(int *selected, const char* items[], int item_count, ImVec2 size = ImVec2(0, 0))
	{
		auto color_grayblue = GetColorU32(ImVec4(0.29, 0.29, 0.29, 0.30));
		auto color_deepblue = GetColorU32(ImVec4(0.14, 0.14, 0.14, 0.25));
		auto color_shade_hover = GetColorU32(ImVec4(1, 1, 1, 0.05));
		auto color_shade_clicked = GetColorU32(ImVec4(1, 1, 1, 0.1));
		auto color_black_outlines = GetColorU32(ImVec4(0, 0, 0, 1));

		ImGuiStyle &style = GetStyle();
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		std::string names;
		for (int32_t i = 0; i < item_count; i++)
			names += items[i];

		ImGuiContext* g = GImGui;
		const ImGuiID id = window->GetID(names.c_str());
		const ImVec2 label_size = CalcTextSize(names.c_str(), NULL, true);

		ImVec2 Min = window->DC.CursorPos;
		ImVec2 Max = ((size.x <= 0 || size.y <= 0) ? ImVec2(Min.x + GetContentRegionMax().x - style.WindowPadding.x, Min.y + label_size.y * 2) : Min + size);

		ImRect bb(Min, Max);
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return;

		//PushClipRect(ImVec2(Min.x, Min.y - 1), ImVec2(Max.x, Max.y + 1), false);

		window->DrawList->AddRectFilledMultiColor(Min, Max, color_grayblue, color_grayblue, color_deepblue, color_deepblue); // Main gradient.

		ImVec2 mouse_pos = GetMousePos();
		bool mouse_click = g->IO.MouseClicked[0];

		float TabSize = ceil((Max.x - Min.x) / item_count);

		for (int32_t i = 0; i < item_count; i++)
		{
			ImVec2 Min_cur_label = ImVec2(Min.x + (int)TabSize * i, Min.y);
			ImVec2 Max_cur_label = ImVec2(Min.x + (int)TabSize * i + (int)TabSize, Max.y);

			// Imprecision clamping. gay but works :^)
			Max_cur_label.x = (Max_cur_label.x >= Max.x ? Max.x : Max_cur_label.x);

			if (mouse_pos.x > Min_cur_label.x && mouse_pos.x < Max_cur_label.x &&
				mouse_pos.y > Min_cur_label.y && mouse_pos.y < Max_cur_label.y)
			{
				if (mouse_click)
					*selected = i;
				else if (i != *selected)
					window->DrawList->AddRectFilled(Min_cur_label, Max_cur_label, color_shade_hover);
			}

			if (i == *selected) {
				window->DrawList->AddRectFilled(Min_cur_label, Max_cur_label, color_shade_clicked);
				window->DrawList->AddRectFilledMultiColor(Min_cur_label, Max_cur_label, color_deepblue, color_deepblue, color_grayblue, color_grayblue);
				window->DrawList->AddLine(ImVec2(Min_cur_label.x - 1.5f, Min_cur_label.y - 1), ImVec2(Max_cur_label.x - 0.5f, Min_cur_label.y - 1), color_black_outlines);
			}
			else
				window->DrawList->AddLine(ImVec2(Min_cur_label.x - 1, Min_cur_label.y), ImVec2(Max_cur_label.x, Min_cur_label.y), color_black_outlines);
			window->DrawList->AddLine(ImVec2(Max_cur_label.x - 1, Max_cur_label.y), ImVec2(Max_cur_label.x - 1, Min_cur_label.y - 0.5f), color_black_outlines);

			const ImVec2 text_size = CalcTextSize(items[i], NULL, true);
			float pad_ = style.FramePadding.x + g->FontSize + style.ItemInnerSpacing.x;
			ImRect tab_rect(Min_cur_label, Max_cur_label);
			RenderTextClipped(Min_cur_label, Max_cur_label, items[i], NULL, &text_size, style.WindowTitleAlign, &tab_rect);
		}

		window->DrawList->AddLine(ImVec2(Min.x, Min.y - 0.5f), ImVec2(Min.x, Max.y), color_black_outlines);
		window->DrawList->AddLine(ImVec2(Min.x, Max.y), Max, color_black_outlines);
		PopClipRect();
	}

	void MultiFloatSlider(const char* id, const char* lable[], float vs[], float vmin, float vmax, int lenght, const char* display_format = "%.3")
	{
	/*	static int selection = 0;
		if (ImGui::Button("�� ", CalcTextSize("��  ", NULL, true)))
			selection--;
		if (selection < 0)
			selection = lenght - 1;
		ImGui::SameLine();
		if (ImGui::Button("�� ", CalcTextSize("��  ", NULL, true)))
			selection++;
		if (selection >= lenght)
			selection = 0;
		ImGui::SameLine();
		ImGui::SliderFloat(lable[selection], &vs[selection], vmin, vmax, display_format);
	*/
		ImGui::SliderFloat(lable[0], &vs[0], vmin, vmax, display_format);
		ImGui::SameLine();
		std::string button_name = "Open";
		button_name += id;
		bool open_popup = ImGui::Button(button_name.c_str(), /*CalcTextSize("Open", NULL, true)*/ ImVec2(36, 0));
		if (open_popup)
		{
			ImGui::OpenPopup(id);
			Colorpicker_Close = ImGui::GetID(id);
		}

		if (ImGui::BeginPopup(id))
		{
			std::string id_new = id;
			id_new += "##slideritself_";
			
			for (int i = 1; i < lenght; i++)
			{
				ImGui::SliderFloat(lable[i], &vs[i], vmin, vmax, display_format);
			}
			ImGui::EndPopup();
		}
	}
}

namespace GladiatorMenu
{
	ImFont* cheat_font;
	ImFont* title_font;
	
	void GUI_Init(HWND window, IDirect3DDevice9 *pDevice)
	{
		static int hue = 140;

		ImGui_ImplDX9_Init(window, pDevice);

		ImGuiStyle &style = ImGui::GetStyle();

		ImVec4 col_text = ImColor::HSV(hue / 255.f, 20.f / 255.f, 235.f / 255.f);
		ImVec4 col_main = ImColor(75, 75, 75); //ImColor(9, 82, 128);
		ImVec4 col_back = ImColor(10, 10, 10);
		ImVec4 col_area = ImColor(25, 25, 25);
		ImVec4 col_slider = ImColor(255, 0, 100);

		ImVec4 col_title = ImColor(50, 50, 50);

		style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, col_area.w + .1f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(255, 255, 255, 0.25f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(col_title.x, col_title.y, col_title.z, 0.85f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_title.x, col_title.y, col_title.z, 0.90f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_title.x, col_title.y, col_title.z, 0.95f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.85f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.90f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.95f, 0.05f, 0.10f, 0.65f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.90f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.85, 0.10, 0.10, 0.65f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.85, 0.10, 0.10, 0.75f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.85, 0.10, 0.10, 0.85f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

		style.Alpha = 1.0f;
		style.WindowPadding = ImVec2(4, 4);
		style.WindowMinSize = ImVec2(32, 32);
		style.WindowRounding = 16.f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.ChildWindowRounding = 16.f;
		style.FramePadding = ImVec2(2, 2);
		style.FrameRounding = 3.f;
		style.ItemSpacing = ImVec2(6, 4);
		style.ItemInnerSpacing = ImVec2(4, 4);
		style.TouchExtraPadding = ImVec2(0, 0);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 3.0f;
		style.ScrollbarSize = 12.0f;
		style.ScrollbarRounding = 16.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 3.0f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.DisplayWindowPadding = ImVec2(22, 22);
		style.DisplaySafeAreaPadding = ImVec2(4, 4);
		style.AntiAliasedLines = true;
		style.AntiAliasedShapes = true;
		style.CurveTessellationTol = 1.25f;
		style.WindowPadThickness = 4.f;

		d3dinit = true;
		cheat_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 13);
		title_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Corbel.ttf", 14);
	}

	void openMenu()
	{
		static bool bDown = false;
		static bool bClicked = false;
		static bool bPrevMenuState = menuOpen;

		if (pressedKey[VK_INSERT])
		{
			bClicked = false;
			bDown = true;
		}
		else if (!pressedKey[VK_INSERT] && bDown)
		{
			bClicked = true;
			bDown = false;
		}
		else
		{
			bClicked = false;
			bDown = false;
		}

		if (bClicked)
		{
			menuOpen = !menuOpen;
			ImGui::CloseLeftoverPicker();
		}

		if (bPrevMenuState != menuOpen)
		{
			Global::bMenuOpen = menuOpen;
		}

		bPrevMenuState = menuOpen;
	}

	void mainWindow()
	{
		junkcode::call();
		ImGui::SetNextWindowSize(ImVec2(860, 540), ImGuiSetCond_FirstUseEver);

		// I don't know if you want this but this disables all that tab to change value bullshit.
		//ImGui::PushAllowKeyboardFocus(false);

		ImGui::PushFont(title_font);
		if (ImGui::Begin("dankme.mz repasted", &menuOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_OnlyDragByTitleBar))
		{
			ImGui::BeginGroup();

			static int selected_Tab = 0;
			static const char* items[6] = { "Legitbot", "Ragebot", "Visual", "Misc", "HVH", "Skinchanger" };
			ImGui::SelectTabs(&selected_Tab, items, 6);

			ImGui::EndGroup();

			ImGui::BeginGroup();
			ImGui::PushID(selected_Tab);		

			ImGui::PushFont(cheat_font);
			switch (selected_Tab)
			{
			case 0:

				legitTab();
				break;

			case 1:

				ragebotTab();
				break;

			case 2:

				visualTab();
				break;

			case 3:

				miscTab();
				break;

			case 4:

				hvhTab();
				break;

			case 5:

				skinchangerTab();
				break;
			}
			ImGui::PopFont();

			ImGui::PopID();
			ImGui::EndGroup();

			ImGui::End();
		}
		ImGui::PopFont();

		if (g_Options.debug_window)
		{
			if (ImGui::Begin("Gladiatorcheatz_[DEBUG PARAMETERS]", &g_Options.debug_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders))
			{
				ImGui::Checkbox("ESP_SHOW_POSEPARAMETERS", &g_Options.debug_showposes); // POSE 12 = PITCH (1 = fully down), POSE 2 = LBY/YAW
				ImGui::Checkbox("ESP_SHOW_ACTIVITES", &g_Options.debug_showactivities);
				ImGui::Checkbox("ESP_HEAD_3D_BOX", &g_Options.debug_headbox);
				ImGui::Checkbox("RESOLVER_FLIP_ENTITIES_180", &g_Options.debug_fliponkey);
				ImGui::SameLine();
				ImGui::KeyBindButton(&g_Options.debug_flipkey);
				if (ImGui::Button("Print debug info to console", ImVec2(-1, 0)))
				{
					// Useful for reversing, remove in final version
					g_CVar->ConsolePrintf("\nPrinting debug info:\n");
					g_CVar->ConsoleColorPrintf(Color(255, 0, 0, 255), "Clientstate: %d\n", (int)g_ClientState);
					g_CVar->ConsoleColorPrintf(Color(255, 255, 0, 255), "Local Player: %d\n", (int)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
				}
				if (ImGui::Button("Unload Cheat", ImVec2(-1, 0)))
				{
					Global::bShouldUnload = true;
				}
				ImGui::End();
			}
			ImGui::ShowTestWindow();
		}
	}

	void legitTab()
	{
		ImGui::BeginChild("LEGITBOTCHILD", ImVec2(0, 0), true);
		{
			ImGui::Text("Aim Assistance");
			ImGui::Separator();
			ImGui::Columns(1, NULL, true);
			{
				ImGui::Checkbox("Aimlock##Legit", &g_Options.legit_enabled);
				ImGui::Checkbox("RCS##Legit", &g_Options.legit_rcs);
				ImGui::Checkbox("Backtrack##Legit", &g_Options.legit_backtrack);
				ImGui::Checkbox("Trigger##Legit", &g_Options.legit_trigger);
				ImGui::Checkbox("Trigger With Aimkey##Legit", &g_Options.legit_trigger_with_aimkey);
				ImGui::SliderInt("Trigger Delay##Legit", &g_Options.legit_trigger_delay, 0, 1000);
			}

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Columns(1, NULL, true);
			{
				ImGui::Text("Aimkey");
				ImGui::KeyBindButton(&g_Options.legit_aimkey1);
				ImGui::Text("Aimkey 2");
				ImGui::KeyBindButton(&g_Options.legit_aimkey2);
			}

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text("Aim Spot");
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Pre Aim Spot");
				ImGui::NewLine();
				ImGui::Text("Bullet After Aim");
				ImGui::NewLine();
				ImGui::Text("After Aim Spot");
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::Combo("##PREAIMSPOT", &g_Options.legit_preaim, opt_AimSpot, 4);
				ImGui::NewLine();
				ImGui::SliderInt("##BULLETAFTERAIM", &g_Options.legit_aftershots, 3, 15);
				ImGui::NewLine();
				ImGui::Combo("##AFTERAIMSPOT", &g_Options.legit_afteraim, opt_AimSpot, 4);
				ImGui::PopItemWidth();
			}

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text("Fov & Smooth");
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Fov");
				ImGui::NewLine();
				ImGui::Text("Smooth");
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat("##FOV", &g_Options.legit_fov, 1.f, 20.f, "%.2f");
				ImGui::NewLine();
				ImGui::SliderFloat("##SMOOTH", &g_Options.legit_smooth_factor, 1.f, 10.f, "%.2f");
				ImGui::PopItemWidth();
			}

			ImGui::Columns(1);
			ImGui::Separator();

			ImGui::EndChild();
		}
	}

	void ragebotTab()
	{
		ImGui::Columns(1, NULL, false);
		{
			ImGui::BeginChild("RAGEBOTCHILD", ImVec2(0, 0), true);
			{
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Enable Rage##Rage", &g_Options.rage_enabled);
					/*if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Enabling rage disables legit");*/
				}

				ImGui::Checkbox("Use Rage Aimkey##Rage", &g_Options.rage_usekey);
				ImGui::KeyBindButton(&g_Options.rage_aimkey);
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Columns(2, NULL, true);
				{
					ImGui::Combo("Selection##Rage", &g_Options.rage_targetselection, opt_TargetType, 5);
					ImGui::Checkbox("Friendly Fire##Rage", &g_Options.rage_friendlyfire);
					ImGui::Checkbox("Silent##Rage", &g_Options.rage_silent);
					//ImGui::Checkbox("PSilent##Rage", &g_Options.rage_psilent);
					ImGui::Checkbox("No Recoil##Rage", &g_Options.rage_norecoil);
					ImGui::Checkbox("Nospread Fix##Rage", &g_Options.rage_nospread);
					ImGui::Checkbox("Auto Shoot##Rage", &g_Options.rage_autoshoot);
					ImGui::Checkbox("Auto Revolver##Rage", &g_Options.rage_autocockrevolver);
				}
				ImGui::NextColumn();
				{
					ImGui::Checkbox("Auto Scope##Rage", &g_Options.rage_autoscope);
					ImGui::Checkbox("Auto Crouch##Rage", &g_Options.rage_autocrouch);
					ImGui::Checkbox("Auto Stop##Rage", &g_Options.rage_autostop);
					ImGui::Checkbox("Auto Body-Aim##Rage", &g_Options.rage_autobaim);
					if (g_Options.rage_autobaim)
						ImGui::SliderInt("Body-Aim After Shots##Rage", &g_Options.rage_baim_after_x_shots, 0, 10);
				}

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Columns(2, NULL, false);
				{
					ImGui::PushItemWidth(235);
					ImGui::Checkbox("Lag Compensation##Rage", &g_Options.rage_lagcompensation);
					if (g_Options.rage_lagcompensation)
					{
						ImGui::Combo("Type##Rage", &g_Options.rage_lagcompensation_type, opt_LagCompType, 3);
						ImGui::SliderFloat("Fake Latency##Rage", &g_Options.rage_fakelatency, 0, 0.8);
					}

					//ImGui::Checkbox("Synchronize##Rage", &g_Options.rage_fixup_entities);
					ImGui::PopItemWidth();
				}
				ImGui::NextColumn();
				{
					// lets be hacky :>
				}

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Columns(2, NULL, true);
				{
					ImGui::Text("Autowall");
					//ImGui::SliderFloat("Min. damage##Rage", &g_Options.rage_mindmg, 0.f, 120.f, "%.2f");
					const char* lable_dmg[] = {"Generic Min. dmage##Rage", "Pistol Min. dmage##Rage", "Revolver Min. dmage##Rage", "SMG Min. dmage##Rage", "Rifle Min. dmage##Rage", "Autosniper Min. dmage##Rage", "Scout Min. dmage##Rage", "AWP Min. dmage##Rage"};
					ImGui::MultiFloatSlider("##mindmg", lable_dmg, g_Options.rage_mindmg_wep, 0, 120, 8, "%.2f");
				}
				ImGui::NextColumn();
				{
					ImGui::Text("Hitchance");
					//ImGui::SliderFloat("Amount %##Rage", &g_Options.rage_hitchance_amount, 0.f, 100.0f, "%.2f");
					const char* lable_hc[] = { "Generic Amount##Rage", "Pistol Amount##Rage", "Revolver Amount##Rage", "SMG Amount##Rage", "Rifle Amount##Rage", "Autosniper Amount##Rage", "Scout Amount##Rage", "AWP Amount##Rage" };
					ImGui::MultiFloatSlider("##hitchance", lable_hc, g_Options.rage_hitchance_wep, 0, 100, 8, "%.2f");
				}
				

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Target");
				ImGui::Separator();
				ImGui::Columns(2, NULL, true);
				{
					ImGui::Checkbox("Prioritize Selected Hitbox##Rage", &g_Options.rage_prioritize);
					ImGui::Combo("Select Hitbox##Rage", &g_Options.rage_hitbox, opt_AimHitboxSpot, 4);
					ImGui::Checkbox("Multipoint##Rage", &g_Options.rage_multipoint);
					if (g_Options.rage_multipoint)
						ImGui::SliderFloat("Pointscale##Rage", &g_Options.rage_pointscale, 0.0f, 1.0f);
				}
				ImGui::NextColumn();
				{
					ImGui::Text("Hitboxes To Scan:");
					ImGui::BeginChild("#MULTIPOINT", ImVec2(0, 120), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
					for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
					{
						ImGui::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxes[i]);
					}
					ImGui::EndChild();
				}
				ImGui::Columns(1);
				ImGui::Separator();

				ImGui::EndChild();
			}
		}
	}

	void visualTab()
	{
		ImGui::Columns(2, NULL, false);
		{
			ImGui::BeginChild("COL1", ImVec2(0, 0), true);
			{
				ImGui::Text("ESP");
				ImGui::Separator();
				ImGui::Columns(1);
				{
					ImGui::Combo("Box Type##BOXTYPE", &g_Options.esp_player_boxtype, opt_EspType, 3);
					ImGui::ColorPickerBox("##Picker_box", g_Options.esp_player_bbox_color_ct, g_Options.esp_player_bbox_color_t, g_Options.esp_player_bbox_color_local, g_Options.esp_player_bbox_color_ct_visible, g_Options.esp_player_bbox_color_t_visible, g_Options.esp_player_bbox_color_local_visiable, false);
					ImGui::Combo("Bounds Style##BOUNDSTYPE", &g_Options.esp_player_boundstype, opt_BoundsType, 2);
					ImGui::SliderFloat("Fill Alpha##ESP", &g_Options.esp_fill_amount, 0.f, 255.f);
					ImGui::ColorPickerBox("##Picker_fill", g_Options.esp_player_fill_color_ct, g_Options.esp_player_fill_color_t, g_Options.esp_player_fill_color_local, g_Options.esp_player_fill_color_ct_visible, g_Options.esp_player_fill_color_t_visible, g_Options.esp_player_fill_color_local_visiable, false);
					ImGui::Checkbox("Ignore Team##ESP", &g_Options.esp_enemies_only);
					ImGui::Checkbox("Farther ESP##ESP", &g_Options.esp_farther);
					ImGui::Checkbox("Name##ESP", &g_Options.esp_player_name);
					ImGui::Checkbox("Health##ESP", &g_Options.esp_player_health);
					ImGui::Checkbox("Weapon Name##ESP", &g_Options.esp_player_weapons);
					ImGui::Checkbox("Snaplines##ESP", &g_Options.esp_player_snaplines);
					ImGui::Checkbox("Offscreen##ESP", &g_Options.esp_player_offscreen);
					ImGui::Checkbox("Skelet##ESP", &g_Options.esp_player_skelet);
					ImGui::Checkbox("Backtracked Trail##ESP", &g_Options.esp_backtracked_player_skelet);
					ImGui::Checkbox("Backtrack Chams##ESP", &g_Options.esp_backtracked_player_chams);
					ImGui::ColorPickerBox("##Picker_btchamcolor", g_Options.esp_backtrack_chams_color, true);
					ImGui::Checkbox("Lag Compensated Hitboxes##ESP", &g_Options.esp_lagcompensated_hitboxes);
					if (g_Options.esp_lagcompensated_hitboxes)
						ImGui::Combo("Type##ESP", &g_Options.esp_lagcompensated_hitboxes_type, std::vector<std::string>{ "Only Lag Compensated", "Lag(Red) & Non(Blue) Lag Compensated" });
					ImGui::Checkbox("Angle Lines##ESP", &g_Options.esp_player_anglelines);
					ImGui::Checkbox("Chams##ESP", &g_Options.esp_player_chams);
					ImGui::Checkbox("Fake Chams##ESP", &g_Options.esp_player_fakechams);
					ImGui::Combo("Chams Type##ESP", &g_Options.esp_player_chams_type, opt_Chams, 4);
					ImGui::ColorPickerBox("##Picker_chams_players", g_Options.esp_player_chams_color_ct, g_Options.esp_player_chams_color_t, g_Options.esp_player_chams_color_local, g_Options.esp_player_chams_color_ct_visible, g_Options.esp_player_chams_color_t_visible, g_Options.esp_player_chams_color_local_visiable, false);
				}

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Glow");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Enable Glow##Glow", &g_Options.glow_enabled);
					ImGui::Checkbox("Players##Glow", &g_Options.glow_players);
					ImGui::Combo("Glow type players##ESP_player", &g_Options.glow_players_style, opt_GlowStyles, 3);
					ImGui::ColorPickerBox("##Picker_glow_players", g_Options.glow_player_color_ct, g_Options.glow_player_color_t, g_Options.glow_player_color_local, g_Options.glow_player_color_ct_visible, g_Options.glow_player_color_t_visible, g_Options.glow_player_color_local_visiable);
					ImGui::Checkbox("Others##Glow", &g_Options.glow_others);
					ImGui::Combo("Glow type others##ESP_other", &g_Options.glow_others_style, opt_GlowStyles, 3);
				}
				
				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("COL2", ImVec2(0, 0), true);
			{
				ImGui::Text("Others");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Hit Marker##Others", &g_Options.visuals_others_hitmarker);
					ImGui::Checkbox("Bullet Impacts##Others", &g_Options.visuals_others_bulletimpacts);
					//ImGui::ColorPickerBox("##Picker_impacts", g_Options.visuals_others_bulletimpacts_color, false);
					ImGui::ColorPickerBox("##Picker_impacts", g_Options.visuals_others_bulletimpacts_color_ct, g_Options.visuals_others_bulletimpacts_color_t, g_Options.visuals_others_bulletimpacts_color, true);
					ImGui::Checkbox("Grenade Prediction##Others", &g_Options.visuals_others_grenade_pred);
					ImGui::Checkbox("Watermark##Others", &g_Options.visuals_others_watermark);
					ImGui::Checkbox("AntiAim Indicater##Others", &g_Options.visuals_lbylc_indicater);
					ImGui::Checkbox("Developer Info##Others", &g_Options.visuals_devinfo);
					ImGui::Checkbox("Taser Range##Others", &g_Options.visuals_weaprange);
					ImGui::Checkbox("Spread Crosshair##Others", &g_Options.esp_player_spreadch);
					ImGui::Combo("Hand Type##Others", &g_Options.esp_hand_chams, opt_hands, 4);
					ImGui::ColorPickerBox("##Handcol", g_Options.esp_hand_chams_color_vis, g_Options.esp_hand_chams_color_invis, g_Options.esp_hand_chams_color_wire, 0);
					ImGui::SliderFloat("Field of View##Others", &g_Options.visuals_others_player_fov, 0, 80);
					ImGui::SliderFloat("Viewmodel Field of View##Others", &g_Options.visuals_others_player_fov_viewmodel, 0, 80);
					ImGui::Checkbox("Remove Zoom##Others", &g_Options.visuals_others_player_fov_ignore_zoom);
				}

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Removals");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Flash##Removals", &g_Options.removals_flash);
					ImGui::Checkbox("Smoke##Removals", &g_Options.removals_smoke);
					if (g_Options.removals_smoke)
					{
						ImGui::Combo("Smoke Type##Removals", &g_Options.removals_smoke_type, opt_nosmoketype, 2);
					}
					ImGui::Checkbox("Scope##Removals", &g_Options.removals_scope);
					ImGui::Checkbox("Recoil##Removals", &g_Options.removals_novisualrecoil);
					ImGui::Checkbox("Post-processing##Removals", &g_Options.removals_postprocessing);
				}

				ImGui::Separator();
				ImGui::Text("Other esp");
				ImGui::Separator();
				ImGui::Combo("Dropped Weapons##ESP", &g_Options.esp_dropped_weapons, opt_WeaponBoxType, 4);
				ImGui::Checkbox("Planted C4##ESP", &g_Options.esp_planted_c4);
				ImGui::Checkbox("Grenade ESP##ESP", &g_Options.esp_grenades);
				ImGui::Combo("Grenade ESP type##ESP", &g_Options.esp_grenades_type, opt_GrenadeESPType, 4);

//#ifdef NIGHTMODE
				ImGui::Checkbox("Nightmode##ESP", &g_Options.visuals_others_nightmode);
				ImGui::Checkbox("Asuswall##ESP", &g_Options.visuals_others_asuswall);
				ImGui::ColorPickerBox("##Picker_nightmode", g_Options.visuals_others_nightmode_color, true);
				ImGui::Checkbox("Player Light##ESP", &g_Options.visuals_others_dlight);

//#endif

				ImGui::Combo("Skybox##ESP", &g_Options.visuals_others_sky, opt_Skynames, 15);

				ImGui::Checkbox("Radar##ESP", &g_Options.misc_radar);

				ImGui::EndChild();
			}
		}
	}

	void miscTab()
	{
		ImGui::Columns(2, NULL, false);
		{
			ImGui::BeginChild("COL1", ImVec2(0, 0), true);
			{
				ImGui::Text("Movement");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Bhop##Movement", &g_Options.misc_bhop);
					ImGui::Checkbox("Auto-Strafe##Movement", &g_Options.misc_autostrafe);
					ImGui::Checkbox("Circle-Strafe##Movement", &g_Options.misc_circlestrafe);
					ImGui::KeyBindButton(&g_Options.misc_circlestrafe_bind);
				}

				static char nName[127] = "";
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Nickname");
				ImGui::Separator();
				ImGui::Columns(2, NULL, true);
				{
					ImGui::PushItemWidth(-1);
					ImGui::InputText("##NNAMEINPUT", nName, 127);
					ImGui::PopItemWidth();
				}
				ImGui::NextColumn();
				{
					if (ImGui::Button("Set Nickname##Nichnamechanger"))
						Miscellaneous::Get().ChangeName(nName);

					ImGui::SameLine();
					if (ImGui::Button("No Name##Nichnamechanger", ImVec2(-1, 0)))
						Miscellaneous::Get().changes = 0;
				}

				static char ClanChanger[127] = "";
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Clan Tag");
				ImGui::Separator();
				ImGui::Columns(2, NULL, true);
				{
					ImGui::PushItemWidth(-1);
					ImGui::InputText("##CLANINPUT", ClanChanger, 127);
					ImGui::PopItemWidth();
				}
				ImGui::NextColumn();
				{
					if (ImGui::Button("Set Clan-Tag"))
						Utils::SetClantag(ClanChanger);

				}
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Other");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					if (ImGui::Button("Anti-Cheat Detector##ACD"))
					{
						acinfo output;
						Miscellaneous::Get().DetectAC(output);
						auto print_bool = [](bool in)
						{
							if (in)
								g_CVar->ConsoleColorPrintf(Color(255, 0, 0, 255), "True\n");
							else
								g_CVar->ConsoleColorPrintf(Color(0, 255, 0, 255), "False\n");
						};
						g_EngineClient->ExecuteClientCmd("toggleconsole");

						g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Core       : "); print_bool(output.smac_core);
						g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Aimbot     : "); print_bool(output.smac_aimbot);
						g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Triggerbot : "); print_bool(output.smac_autotrigger);
						g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Client     : "); print_bool(output.smac_client);
						g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Commands   : "); print_bool(output.smac_commands);
						g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC ConVars    : "); print_bool(output.smac_cvars);
						g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Eyeangle   : "); print_bool(output.smac_eyetest);
						g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Speedhack  : "); print_bool(output.smac_speedhack);
						g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Spinbot    : "); print_bool(output.smac_spinhack);
					}
					ImGui::Checkbox("Auto-Pistol##Other", &g_Options.misc_auto_pistol);
					ImGui::Checkbox("Auto-Accept##Other", &g_Options.misc_autoaccept);
					ImGui::Checkbox("Reveal Ranks##Other", &g_Options.misc_revealAllRanks);
					ImGui::Checkbox("Chat Spamer##Other", &g_Options.misc_chatspamer);
					ImGui::Checkbox("SMAC Bypass##Other", &g_Options.misc_smacbypass);
					ImGui::Combo("Auto TrashTalk##Other", &g_Options.misc_trashchat, opt_shittalk, 3);
					ImGui::Checkbox("Anti VoteKick##Other", &g_Options.misc_antikick);
					ImGui::Checkbox("Name Stealer##Other", &g_Options.misc_stealname);
					ImGui::Checkbox("Ayyware Crasher##Other", &g_Options.misc_antiayy);
					ImGui::Checkbox("Name Hider##Other", &g_Options.misc_hidename);
					ImGui::Checkbox("Name Spam##Other", &g_Options.misc_namespam);
					ImGui::Checkbox("Event Logs##Other", &g_Options.misc_logevents);
					ImGui::Checkbox("Spectator List##Other", &g_Options.misc_spectatorlist);
					ImGui::Checkbox("DankMe.Mz Clan-Tag##Other", &g_Options.misc_animated_clantag);
					ImGui::Checkbox("Thirdperson##Other", &g_Options.misc_thirdperson);
					ImGui::KeyBindButton(&g_Options.misc_thirdperson_bind);
					ImGui::Checkbox("Fakewalk##Other", &g_Options.misc_fakewalk);
					ImGui::KeyBindButton(&g_Options.misc_fakewalk_bind);
					//ImGui::Checkbox("Tickbase manipulation##Other", &g_Options.misc_instant_defuse_plant);

					ImGui::Checkbox("Debug Options##Other", &g_Options.debug_window);
				}

				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("COL2", ImVec2(0, 0), true);
			{
				ImGui::Text("Fakelag");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Enabled##Fakelag", &g_Options.misc_fakelag_enabled);
					ImGui::Checkbox("Adaptive##Fakelag", &g_Options.misc_fakelag_adaptive);
					ImGui::Combo("Activation Type##Fakelag", &g_Options.misc_fakelag_activation_type, std::vector<std::string>{ "Always", "While Moving", "In Air" });
					if (g_Options.misc_fakelag_adaptive)
						ImGui::Combo("Fakelag Type##Fakelag", &g_Options.misc_fakelag_adeptive_type, std::vector<std::string>{ "Normal", "EXTREME", "S U P R E M E" });
					else
						ImGui::SliderInt("Choke##Fakelag", &g_Options.misc_fakelag_value, 0, 14);
				}

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Config");
				ImGui::Separator();
				static std::vector<std::string> configItems = Config::Get().GetAllConfigs();
				static int configItemCurrent = -1;

				static char fName[128] = "default";

				ImGui::Columns(1, NULL, true);
				{
					if (ImGui::Button("Refresh##Config"))
						configItems = Config::Get().GetAllConfigs();

					ImGui::SameLine();
					if (ImGui::Button("Save##Config"))
					{
						if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int)configItems.size()))
						{
							std::string fPath = std::string(Global::my_documents_folder) + "\\DANKMEMZ_Repasted\\" + configItems[configItemCurrent] + ".meme";
							Config::Get().SaveConfig(fPath);
						}
					}

					ImGui::SameLine();
					if (ImGui::Button("Remove##Config"))
					{
						if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int)configItems.size()))
						{
							std::string fPath = std::string(Global::my_documents_folder) + "\\DANKMEMZ_Repasted\\" + configItems[configItemCurrent] + ".meme";
							std::remove(fPath.c_str());

							configItems = Config::Get().GetAllConfigs();
							configItemCurrent = -1;
						}
					}

					ImGui::PushItemWidth(138);
					{
						ImGui::InputText("", fName, 128);
					}
					ImGui::PopItemWidth();

					ImGui::SameLine();
					if (ImGui::Button("Add##Config"))
					{
						std::string fPath = std::string(Global::my_documents_folder) + "\\DANKMEMZ_Repasted\\" + fName + ".meme";
						Config::Get().SaveConfig(fPath);

						configItems = Config::Get().GetAllConfigs();
						configItemCurrent = -1;
					}

					ImGui::PushItemWidth(178);
					{
						if (ImGui::ListBox("##Config", &configItemCurrent, configItems, 3))
						{
							std::string fPath = std::string(Global::my_documents_folder) + "\\DANKMEMZ_Repasted\\" + configItems[configItemCurrent] + ".meme";
							Config::Get().LoadConfig(fPath);
						}
					}
					ImGui::PopItemWidth();
				}

				ImGui::Columns(1);
				ImGui::Text("LUA");
				ImGui::Separator();
				static std::vector<std::string> luaItems = Config::Get().GetAllLUAs();
				static int luaItemCurrent = -1;

				ImGui::Columns(1, NULL, true);
				{
					if (ImGui::Button("Refresh##LUA"))
						luaItems = Config::Get().GetAllLUAs();

					ImGui::PushItemWidth(178);
					{
						if (ImGui::ListBox("##LUA", &luaItemCurrent, luaItems, 3))
						{
							std::string fPath = std::string(Global::my_documents_folder) + "\\DANKMEMZ_Repasted\\" + luaItems[luaItemCurrent] + ".lua";
							Global::currentLUA = fPath;
						}
					}
					ImGui::PopItemWidth();
				}

				ImGui::EndChild();
			}
		}
	}

	void hvhTab()
	{
		ImGui::Columns(1, NULL, true);
		{
			ImGui::BeginChild("COL1", ImVec2(0, 0), true);
			{

				ImGui::Text("Anti Aim");
				ImGui::Columns(1);
				ImGui::Checkbox("Use New System", &g_Options.hvh_antiaim_new);
				ImGui::Separator();
				if (g_Options.hvh_antiaim_new)
				{
					static bool real = false;
					static bool moving = false;
					static bool freestanding = false;
					ImGui::Checkbox("Real", &real); ImGui::SameLine();
					ImGui::Checkbox("Moving", &moving); ImGui::SameLine();
					ImGui::Checkbox("Freestanding", &freestanding);

					ImGui::Combo("Pitch", &g_Options.hvh_antiaim_new_options[real][0][moving][freestanding], opt_AANewPitch, 4);
					ImGui::Combo("Yaw", &g_Options.hvh_antiaim_new_options[real][1][moving][freestanding], opt_AANewYaw, 7);
					ImGui::Combo("On-Air", &g_Options.hvh_antiaim_y_onair, opt_AAonair, 5);

					ImGui::NewLine();

					ImGui::SliderFloat("Pitch Offset", &g_Options.hvh_antiaim_new_offset[real][0][moving][freestanding], -89.f, 89.f);
					ImGui::SliderFloat("Pitch Extra", &g_Options.hvh_antiaim_new_extra[real][0][moving][freestanding], -180.f, 180.f);

					ImGui::NewLine();

					ImGui::SliderFloat("Yaw Offset", &g_Options.hvh_antiaim_new_offset[real][1][moving][freestanding], -180.f, 180.f);
					ImGui::SliderFloat("Yaw Extra", &g_Options.hvh_antiaim_new_extra[real][1][moving][freestanding], -360.f, 360.f);

					ImGui::NewLine();

					ImGui::Text("Menual Anti-Aim Key"); ImGui::SameLine(); ImGui::KeyBindButton(&g_Options.hvh_antiaim_menkey);

					ImGui::NewLine();

					if (real)
					{
						ImGui::Checkbox("LBY Breaker", &g_Options.hvh_antiaim_new_breakLBY[freestanding]);
						if (g_Options.hvh_antiaim_new_breakLBY[freestanding])
						ImGui::SliderFloat("LBY Delta", &g_Options.hvh_antiaim_new_LBYDelta[freestanding], 35, 180);
					}
					ImGui::Checkbox("Show Real Angles##AntiAim", &g_Options.hvh_show_real_angles);
					ImGui::Checkbox("Disable Anti-Untrusted##AntiAim", &g_Options.hvh_disable_antiut);
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("WARNING: DO NOT ENABLE ON VALVE SERVER");
				}
				else
				{
					ImGui::Columns(2, NULL, true);
					{
						ImGui::Text("AntiAim Pitch");
						ImGui::NewLine();
						ImGui::Text("AntiAim Base Yaw");
						ImGui::NewLine();
						ImGui::Text("AntiAim Yaw");
						ImGui::NewLine();
						ImGui::Text("Fake AntiAim Yaw");
						ImGui::NewLine();
						ImGui::Text("Moving AntiAim Yaw");
						ImGui::NewLine();
						ImGui::Text("Moving Fake AntiAim Yaw");
						ImGui::NewLine();
						ImGui::Text("On-Air AntiAim Yaw");
						ImGui::NewLine();
						ImGui::SliderFloat("Real Angle Delta##AntiAim", &g_Options.hvh_antiaim_real_delta, 0, 360);
						ImGui::SliderFloat("Fake Angle Delta##AntiAim", &g_Options.hvh_antiaim_fake_delta, 0, 360);
						ImGui::NewLine();
						ImGui::Checkbox("Freestanding", &g_Options.hvh_antiaim_freestand);
						ImGui::Checkbox("Aggresive Freestanding", &g_Options.hvh_antiaim_freestand_aggresive);
						ImGui::Checkbox("LBY Breaker##AntiAim", &g_Options.hvh_antiaim_lby_breaker);
						ImGui::SliderFloat("LBY Delta##AntiAim", &g_Options.hvh_antiaim_lby_delta, 0, 360);
						ImGui::Checkbox("Dynamic Delta##AntiAim", &g_Options.hvh_antiaim_lby_dynamic);
						ImGui::Checkbox("Show Real Angles##AntiAim", &g_Options.hvh_show_real_angles);
					}
					ImGui::NextColumn();
					{
						ImGui::PushItemWidth(-1);
						ImGui::Combo("##AAX", &g_Options.hvh_antiaim_x, opt_AApitch, 7);
						ImGui::NewLine();
						ImGui::Combo("##AABY", &g_Options.hvh_antiaim_base, opt_AAYawBase, 4);
						ImGui::NewLine();
						ImGui::Combo("##AAY", &g_Options.hvh_antiaim_y, opt_AAyaw, 7);
						ImGui::NewLine();
						ImGui::Combo("##FAAY", &g_Options.hvh_antiaim_y_fake, opt_AAfakeyaw, 8);
						ImGui::NewLine();
						ImGui::Combo("##MAAY", &g_Options.hvh_antiaim_y_moving, opt_AAmovyaw, 7);
						ImGui::NewLine();
						ImGui::Combo("##MFAAY", &g_Options.hvh_antiaim_y_movingfake, opt_AAfakeyaw, 8);
						ImGui::NewLine();
						ImGui::Combo("##OAAAY", &g_Options.hvh_antiaim_y_onair, opt_AAonair, 5);
						ImGui::NewLine();
						ImGui::KeyBindButton(&g_Options.hvh_antiaim_menkey);
						ImGui::NewLine();
						ImGui::Checkbox("Legit AA##AntiAim", &g_Options.hvh_antiaim_legit);

						ImGui::PopItemWidth();
					}
				}

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Resolver");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Resolve All##Resolver", &g_Options.hvh_resolver);
					ImGui::Checkbox("Display Resolving Mode##Resolver", &g_Options.hvh_resolver_display);
					//ImGui::Checkbox("Experimental Resolver (unfinished)##Resolver", &g_Options.hvh_resolver_experimental);
					//ImGui::Combo("##EXPRESTP", &g_Options.hvh_resolver_experimental_type, opt_resolvtype, 4);
					ImGui::Checkbox("Resolver Override##Resolver", &g_Options.hvh_resolver_override);
					ImGui::KeyBindButton(&g_Options.hvh_resolver_override_key);
				}

				ImGui::EndChild();
			}
		}
	}

	void skinchangerTab()
	{
		ImGui::BeginChild("SKINCHANGER", ImVec2(0, 0), true);
		{
			if (ImGui::Checkbox("Enabled##Skinchanger", &g_Options.skinchanger_enabled))
				Skinchanger::Get().bForceFullUpdate = true;

			std::vector<EconomyItem_t> &entries = Skinchanger::Get().GetItems();

			// If the user deleted the only config let's add one
			if (entries.size() == 0)
				entries.push_back(EconomyItem_t());

			static int selected_id = 0;

			ImGui::Columns(2, nullptr, false);

			// Config selection
			{
				ImGui::PushItemWidth(-1);

				char element_name[64];

				ImGui::ListBox("##skinchangerconfigs", &selected_id, [&element_name, &entries](int idx)
				{
					sprintf_s(element_name, "%s (%s)", entries.at(idx).name, k_weapon_names.at(entries.at(idx).definition_vector_index).name);
					return element_name;
				}, entries.size(), 15);

				ImVec2 button_size = ImVec2(ImGui::GetColumnWidth() / 2 - 12.8f, 25);

				if (ImGui::Button("Add Item", button_size))
				{
					entries.push_back(EconomyItem_t());
					selected_id = entries.size() - 1;
				}
				ImGui::SameLine();

				if (ImGui::Button("Remove Item", button_size))
					entries.erase(entries.begin() + selected_id);

				ImGui::PopItemWidth();
			}

			ImGui::NextColumn();

			selected_id = selected_id < int(entries.size()) ? selected_id : entries.size() - 1;

			EconomyItem_t &selected_entry = entries[selected_id];
			{
				// Name
				ImGui::InputText("Name", selected_entry.name, 32);
				ImGui::Dummy(ImVec2(1, 4));

				// Item to change skins for
				ImGui::Combo("Item", &selected_entry.definition_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_weapon_names[idx].name;
					return true;
				}, nullptr, k_weapon_names.size(), 5);
				ImGui::Dummy(ImVec2(1, 3));

				// Enabled
				ImGui::Checkbox("Enabled", &selected_entry.enabled);
				ImGui::Dummy(ImVec2(1, 3));

				// Pattern Seed
				ImGui::InputInt("Seed", &selected_entry.seed);
				ImGui::Dummy(ImVec2(1, 4));

				// Custom StatTrak number
				ImGui::InputInt("StatTrak", &selected_entry.stat_trak);
				ImGui::Dummy(ImVec2(1, 4));

				// Wear Float
				ImGui::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);
				ImGui::Dummy(ImVec2(1, 4));

				// Paint kit
				if (selected_entry.definition_index != GLOVE_T_SIDE)
				{
					ImGui::Combo("PaintKit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_skins[idx].name.c_str();
						return true;
					}, nullptr, k_skins.size(), 10);
				}
				else
				{
					ImGui::Combo("PaintKit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_gloves[idx].name.c_str();
						return true;
					}, nullptr, k_gloves.size(), 10);
				}
				ImGui::Dummy(ImVec2(1, 4));

				// Quality
				ImGui::Combo("Quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_quality_names[idx].name;
					return true;
				}, nullptr, k_quality_names.size(), 5);
				ImGui::Dummy(ImVec2(1, 4));

				// Yes we do it twice to decide knifes
				selected_entry.UpdateValues();

				// Item defindex override
				if (selected_entry.definition_index == WEAPON_KNIFE_CT)
				{
					ImGui::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_knife_names.at(idx).name;
						return true;
					}, nullptr, k_knife_names.size(), 5);
				}
				else if (selected_entry.definition_index == GLOVE_T_SIDE)
				{
					ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_glove_names.at(idx).name;
						return true;
					}, nullptr, k_glove_names.size(), 5);
				}
				else
				{
					// We don't want to override weapons other than knives or gloves
					static auto unused_value = 0;
					selected_entry.definition_override_vector_index = 0;
					ImGui::Combo("Unavailable", &unused_value, "Only available for knives or gloves!\0");
				}
				ImGui::Dummy(ImVec2(1, 4));

				selected_entry.UpdateValues();

				// Custom Name tag
				ImGui::InputText("Nametag", selected_entry.custom_name, 32);
				ImGui::Dummy(ImVec2(1, 4));
			}

			ImGui::NextColumn();

			ImGui::Columns(1, nullptr, false);

			ImGui::Separator();

			ImGui::Dummy(ImVec2(1, 10));

			ImGui::Columns(3, nullptr, false);

			ImGui::PushItemWidth(-1);

			// Lower buttons for modifying items and saving
			{
				ImVec2 button_size = ImVec2(ImGui::GetColumnWidth() - 17.f, 25);

				if (ImGui::Button("Force Update##Skinchanger", button_size))
					Skinchanger::Get().bForceFullUpdate = true;
				ImGui::NextColumn();

				if (ImGui::Button("Save##Skinchanger", button_size))
					Skinchanger::Get().SaveSkins();
				ImGui::NextColumn();

				if (ImGui::Button("Load##Skinchanger", button_size))
					Skinchanger::Get().LoadSkins();
				ImGui::NextColumn();
			}

			ImGui::PopItemWidth();
			ImGui::Columns(1);

			ImGui::EndChild();
		}
	}

	bool d3dinit = false;
}