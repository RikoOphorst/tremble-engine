#include "image_component.h"

tremble::ImageComponent::ImageComponent()
{
	custom_matrix_ = false;
	texture_ = nullptr;
	position_ = {0, 0, 0};
	size_ = {64, 64, 1};
	rotation_ = 0;
	center_ = {0, 0, 0};
	color_ = { 1, 1, 1, 1 };
	layer_ = 0;
	transform_ = DirectX::XMMatrixIdentity();
	uv_min_ = { 0, 0 };
	uv_max_ = { 1, 1 };
}
