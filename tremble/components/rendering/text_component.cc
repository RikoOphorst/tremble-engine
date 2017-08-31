#include "text_component.h"
#include "../core/rendering/renderer.h"
#include "../core/rendering/interface_font_renderer.h"

namespace tremble {

	TextComponent::TextComponent()
	{
		width_ = 0;
		height_ = 0;
		measurements_valid_ = false;
		text_ = "";
		position_ = { 0, 0, 0 };
		size_ = 32;
		center_ = { 0, 0, 0 };
		color_ = { 1, 1, 1, 1 };
		layer_ = 0;
		transform_ = DirectX::XMMatrixIdentity();
		font_ = nullptr;
		alignment_ = Left;
		line_seperation_ = 4;
	}

	//------------------------------------------------------------------------------------------------------
	void TextComponent::SetFont(std::string name)
	{
		InterfaceFontRenderer* renderer = Get::Renderer()->GetFontRenderer();
		std::vector<Font>& fonts = renderer->GetFontList();

		for (int i = 0; i < fonts.size(); i++)
		{
			if (fonts[i].name == name)
			{
				font_ = &fonts[i];
				return;
			}
		}

		renderer->AddFont(name);
		SetFont(name);
	}

	//------------------------------------------------------------------------------------------------------
	std::string TextComponent::GetFont()
	{
		if (font_ == nullptr)
		{
			return "";
		}
		else {
			return font_->name;
		}
	}

	//------------------------------------------------------------------------------------------------------
	void TextComponent::Measure()
	{
		width_ = 0;
		height_ = 0;

		if (font_ == nullptr || text_.length() == 0)
		{
			measurements_valid_ = true;
			return;
		}

		// Split lines
		lines_.clear();
		lines_.push_back({});
		int current_line = 0;
		for (int i = 0; i < text_.length(); i++)
		{
			if (text_[i] == '\n')
			{
				lines_.push_back({});
				current_line++;
			}
			else {
				lines_[current_line].text += text_[i];
			}
		}

		// Find reight font
		FontLevel& font = font_->FindLevel(GetSize());
		float scale = GetSize() / font.size;

		float top = 0, bottom = 0, left = 0, right = 0;
		float x = 0;

		char c;
		CharInfo info;


		// Measure each line
		for (int i = 0; i < lines_.size(); i++)
		{
			top = 0;
			bottom = 0;
			left = 0;
			right = 0;
			x = 0;

			// Add all characters
			for (int s = 0; s < lines_[i].text.length(); s++)
			{
				c = lines_[i].text[s];
				info = font.char_info[c];

				left = std::min(left, x + info.offset_x * scale);
				right = std::max(right, x + info.width * scale + info.offset_x * scale);
				top = std::min(top, info.offset_y * scale);
				bottom = std::max(bottom, info.height * scale + info.offset_y * scale);

				x += info.advance * scale;
			}

			// calculate measurements
			lines_[i].width = right - left;
			lines_[i].height = bottom - top;
			width_ = std::max(width_, right - left);
			height_ += (bottom - top) + line_seperation_;
			x = 0;
		}

		measurements_valid_ = true;
	}
}