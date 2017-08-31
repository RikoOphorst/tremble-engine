#pragma once
#include "../../core/scene_graph/component.h"
#include "../../core/math/math.h"

namespace tremble
{

	class Texture;
	struct Font;

	/// Holds line of a string with it's measurements
	struct TextLine {
		/// String of line
		std::string text;
		/// Width of line
		float width;
		/// Height of line
		float height;
	};

	enum Alignment{
		Left,
		Middle,
		Right
	};

	/**
	* @class tremble::TextComponent
	* @author Sander Brouwers
	* @brief Adds text to user interface
	*/
	class TextComponent : public Component
	{
	public:
		TextComponent();

		/// Assigns a font to the text
		void SetFont(std::string);
		/// Returns font name assigned to the text
		std::string GetFont();
		/// Returns font class assigned to the text
		Font* GetFontPtr() { return font_; }

		/// Assigns a string to the text
		void SetText(std::string text) { text_ = text; measurements_valid_ = false;}
		/// Returns the string assigned to the text
		std::string GetText() { return text_; }

		/// Assigns position to image
		void SetPosition(Vector3 position) { position_ = position; }
		/// Returns position assigned to the text
		Vector3 GetPosition() { return position_; }

		/// Assigns size to the text
		void SetSize(float size) { size_ = size; measurements_valid_ = false;}
		/// Returns size assigned to the text
		float GetSize() { return size_; }

		/// Assigns center point to the text
		void SetCenter(Vector3 center) { center_ = center; }
		/// Returns center point assigned to the text
		Vector3 GetCenter() { return center_; }

		/// Assigns custom transformation matrix to the text
		void SetMatrix(DirectX::XMMATRIX matrix) { transform_ = matrix; }
		/// Returns custom transformation matrix of the text
		DirectX::XMMATRIX GetMatrix() { return transform_; }

		/// Assigns render the text
		void SetLayer(int layer) { layer_ = layer; }
		/// Returns render the text
		float GetLayer() { return layer_; }

		/// Assigns color to the text
		void SetColor(Vector4 color) { color_ = color; }
		/// Returns color assigned to the text
		Vector4 GetColor() { return color_; }

		/// Returns measured width of assigned string
		float GetWidth() { if (measurements_valid_ == false) { Measure(); } return width_; }
		/// Returns measured height of assigned string
		float GetHeight() { if (measurements_valid_ == false) { Measure(); } return height_; }

		/// Sets text alignment
		void SetAlignment(Alignment align) { alignment_ = align; }
		/// Returns text alignment
		Alignment GetAlignment() { return alignment_; }

		/// Sets line seperation distance in virtual pixels
		void SetLineSeperation(float separation) { line_seperation_ = separation; }
		/// Returns line seperation distance in virtual pixels
		float GetLineSeperation() { return line_seperation_; }

		/// Returns lines found in assigned string with measurements
		std::vector<TextLine>& GetLines() { return lines_; }

	private:
		void Measure();
		float width_;
		float height_;
		bool measurements_valid_;

		Font* font_;

		std::string text_;
		std::vector<TextLine> lines_;

		Vector3 position_;
		float size_;
		Vector3 center_;
		Vector4 color_;
		int layer_;
		DirectX::XMMATRIX transform_;
		Alignment alignment_;
		float line_seperation_;
	};
}