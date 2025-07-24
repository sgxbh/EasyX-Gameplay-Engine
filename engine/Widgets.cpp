#include "Widgets.h"
#include"Math.h"
#include"InputComponent.h"
#include "UserInterface.h"
#include"GameStatics.h"
#include"Controller.h"
#include"ResourceManager.h"

std::unordered_map<std::string, COLORREF> Characters::text_color_map = {
	{"$0",WHITE},//°×É«
	{"$1",LIGHTGRAY},//Ç³»ÒÉ«
	{"$2",DARKGRAY},
	{"$3",GREEN},
	{"$4",LIGHTGREEN},
	{"$5",BLACK}
};

void Characters::SetCharacters(std::string text, int size, LPCTSTR type)
{
	row = 1; col = 0;
	int temp = 0;
	for (int i = 0; i < text.length(); ++i) {
		if (text[i] == '\n') { row++, col = temp > col ? temp : col, temp = 0; }
		else if (text[i] == '$' && i + 1 < text.length()) {
			std::string buf = "$" + text[i + 1];
			if (text_color_map.find(buf) != text_color_map.end())++i;
			else ++temp;
		}
		else ++temp;
	}
	col = temp > col ? temp : col;
	this->texts = text;
	this->size = size;
	this->type = type;
}

void Characters::PrintCharacters(Vector2D pos, CharactersPattern pattern)
{
	LOGFONT lf;
	gettextstyle(&lf);
	COLORREF oldColor = gettextcolor();

	settextstyle(6 * size, 3 * size, type);
	settextcolor(BLACK);
	int pat = 1;
	switch (pattern) {
	case CharactersPattern::Left:pat = 0; break;
	case CharactersPattern::Middle:pat = 1; break;
	case CharactersPattern::Right:pat = 1; break;
	default:pat = 1; break;
	}

	std::string temp;
	int r = 0;

	for (int i = 0; i < texts.length(); ++i) {
		if (texts[i] == '\n') {
			outtextxy((int)pos.x + (GetWidth() - temp.size() * 3 * size) * pat / 2, pos.y + r * 6 * size, temp.c_str());
			temp.clear();
			++r;
		}
		else if (texts[i] == '$' && i + 1 < texts.length()) {
			std::string buf = "$" + std::string(1, texts[++i]);
			if (text_color_map.find(buf) != text_color_map.end()) {
				settextcolor(text_color_map[buf]);
			}
			else {
				temp.push_back(texts[i]);
			}
		}
		else {
			temp.push_back(texts[i]);
		}
	}
	if (!temp.empty()) {
		outtextxy((int)pos.x + (GetWidth() - temp.size() * 3 * size) * pat / 2, pos.y + r * 6 * size, temp.c_str());
	}

	// »Ö¸´×ÖÌå×´Ì¬
	settextstyle(&lf);
	settextcolor(oldColor);
}



bool Widget::IsUnderCursor() const
{
	float x = InputComponent::GetMousePosition().x;
	float y= InputComponent::GetMousePosition().y;

	int i = Math::Clamp((int)x, 0, 15);
	int j = Math::Clamp((int)y, 0, 8);

	if (!mainWorld.UIDetectZones[i][j].empty()) {
		for (auto it = mainWorld.UIDetectZones[i][j].rbegin(); it != mainWorld.UIDetectZones[i][j].rend(); ++it) {
			if (Widget* widget = Cast<Widget>(*it))
			{
				Vector2D loc = widget->GetScreenPosition();
				if (x < GetSize().x / 2 + loc.x && x > loc.x - GetSize().x / 2
					&& y < GetSize().y / 2 + loc.y && y > loc.y - GetSize().y / 2 && widget == this)
				{
					return true;
				}
			}
		}
		return false;
	}
	
}

Widget::~Widget()
{
	for (int i = point.x; i <= point_1.x; ++i)for (int j = point.y; j <= point_1.y; ++j)mainWorld.UIDetectZones[i][j].erase(this);
}

void Widget::Update()
{
	if (uiPattern != UIPattern::VisibleAndInteractive)return;

	Vector2D pos = GetScreenPosition() - GetSize()/2;
	Pair newPoint(Math::Clamp(int(pos.x) / 100, 0, 15), Math::Clamp(int(pos.y) / 100, 0, 8));
	pos += size;
	Pair newPoint_1(Math::Clamp(int(pos.x) / 100, 0, 15), Math::Clamp(int(pos.y) / 100, 0, 8));
	if (newPoint == point && newPoint_1 == point_1)return;

	if (point != Pair(-1, -1))for (int i = point.x; i <= point_1.x; ++i)for (int j = point.y; j <= point_1.y; ++j)mainWorld.UIDetectZones[i][j].erase(this);
	point = newPoint; point_1 = newPoint_1;
	for (int i = point.x; i <= point_1.x; ++i)for (int j = point.y; j <= point_1.y; ++j)mainWorld.UIDetectZones[i][j].insert(this);
}

void Widget::ShowInfoBox()
{
	if (bInfoBox && uiPattern == UIPattern::VisibleAndInteractive && IsUnderCursor())
	{
		setlinecolor(BLACK);
		setlinestyle(PS_SOLID | PS_JOIN_BEVEL);
		setfillcolor(RGB(255, 247, 213));

		Vector2D pos = InputComponent::GetMousePosition() + Vector2D(-15, 15);
		fillrectangle((int)pos.x, (int)pos.y, (int)pos.x + InfoText.GetWidth(), (int)pos.y + InfoText.GetHeight());
		InfoText.PrintCharacters(pos);
	}
}

void Widget::SetUIPattern(UIPattern pattern)
{
	uiPattern = pattern;
	for (auto& child : children)
	{
		child->SetUIPattern(pattern);
	}
	if (uiPattern != UIPattern::VisibleAndInteractive)
	{
		if (point != Pair(-1, -1))for (int i = point.x; i <= point_1.x; ++i)for (int j = point.y; j <= point_1.y; ++j)mainWorld.UIDetectZones[i][j].erase(this);
		point = { -1, -1 }, point_1 = { -1, -1 };
	}
}

Vector2D Widget::GetSize() const
{
	return  GetScreenScale()*size;
}

void Widget::AttachTo(Widget* par)
{
	if (par) {
		par->children.insert(this);
		parent = par;
	}
}

void Widget::DetachFrom(Widget* par)
{
	if (par) {
		par->children.erase(this);
		parent = nullptr;
	}
}

Vector2D Widget::GetScreenPosition() const
{
	if (parent)return parent->GetScreenPosition() + GetRelativePosition() + GetLayoutOffset();
	else return GetRelativePosition();
}

float Widget::GetScreenRotation() const
{
	if (parent)return parent->GetScreenRotation() + GetRelativeRotation() ;
	else return GetRelativeRotation();
}

Vector2D Widget::GetScreenScale() const
{
	if (parent)return parent->GetScreenScale() * GetRelativeScale();
	else return GetRelativeScale();
}

Vector2D Widget::GetLayoutOffset() const
{
	switch (layoutPattern) {
	case LayoutPattern::LeftTop:return Vector2D(-parent->GetSize().x / 2, -parent->GetSize().y/2);
	case LayoutPattern::MiddleTop:return Vector2D(0, -parent->GetSize().y / 2);
	case LayoutPattern::RightTop:return Vector2D(parent->GetSize().x / 2, -parent->GetSize().y / 2);
	case LayoutPattern::LeftMiddle:return Vector2D(-parent->GetSize().x / 2, 0);
	case LayoutPattern::Center:return Vector2D(0, 0);
	case LayoutPattern::RightMiddle:return Vector2D(parent->GetSize().x/2, 0);
	case LayoutPattern::LeftBottom:return Vector2D(-parent->GetSize().x / 2, parent->GetSize().y / 2);
	case LayoutPattern::MiddleBottom:return Vector2D(0, parent->GetSize().y / 2);
	case LayoutPattern::RightBottom:return Vector2D(parent->GetSize().x / 2, parent->GetSize().y / 2);
	defualt:return Vector2D(0, 0);
	}
	return Vector2D(0, 0);
}

Panel::~Panel()
{
	if (members_ui.empty())return;
	for (auto& ui : members_ui)
	{
		ui->RemoveFromViewport();
	}
}

void Panel::Update()
{
	Widget::Update();
	if (members.empty())return;
	int32_t index = 0;
	for (auto& member : members)
	{
		AdjustMemberPosition(member, index++);
	}
}

void Panel::SetUnitSize(Vector2D size)
{
	unitSize = size;
	Panel::Update();
}
void Panel::AddMember(Widget* member, int32_t index)
{
	member->AttachTo(this);
	if (index >= 0)members.emplace(members.begin() + index, member);
	else members.push_back(member);
	member->attachedPanel = this;
	AdjustMemberSizeToUnit(member);
	AdjustMemberPosition(member, index >= 0 ? index : members.size() - 1);
}
void Panel::RemoveMember(Widget* member)
{
	member->DetachFrom(this);
	members.erase(std::remove(members.begin(), members.end(), member), members.end());
	member->attachedPanel = nullptr;
}
void Panel::AddMember(UserInterface* member, int32_t index)
{
	member->rootCanvas->AttachTo(this);
	index = Math::Clamp(index, -1, int32_t(members.size()));
	if (index >= 0)members.emplace(members.begin() + index, member->rootCanvas);
	else members.push_back(member->rootCanvas);
	members_ui.push_back(member);
	member->rootCanvas->attachedPanel = this;
	AdjustMemberSizeToUnit(member->rootCanvas);
	AdjustMemberPosition(member->rootCanvas, index >= 0 ? index : members.size() - 1);
}
void Panel::RemoveMember(UserInterface* member)
{
	member->rootCanvas->DetachFrom(this);
	members.erase(std::remove(members.begin(), members.end(), member->rootCanvas), members.end());
	members_ui.erase(std::remove(members_ui.begin(), members_ui.end(), member), members_ui.end());
	member->rootCanvas->attachedPanel = nullptr;
}

void Panel::AdjustMemberSizeToUnit(Widget* member)
{
	member->SetRelativeScale(unitSize / member->GetSize());
}

void HorizontalPanel::AdjustMemberPosition(Widget* member, int32_t index)
{
	if (index < 0)return;
	Vector2D pos = Vector2D(index * (unitSize.x + spacing), 0) + Vector2D(unitSize.x, unitSize.y) * 0.5f;
	member->SetRelativePosition(pos);
}

Vector2D HorizontalPanel::GetSize() const
{
	return members.empty() ? Vector2D() : Vector2D(members.size() * (unitSize.x + spacing) - spacing, unitSize.y);
}

void VerticalPanel::AdjustMemberPosition(Widget* member, int32_t index)
{
	if (index < 0)return;
	Vector2D pos = Vector2D(0, index * (unitSize.y + spacing));
	member->SetRelativePosition(pos);
}

Vector2D VerticalPanel::GetSize() const
{
	return members.empty() ? Vector2D() : Vector2D(unitSize.x, members.size() * (unitSize.y + spacing) - spacing);
}

void GridPanel::AdjustMemberPosition(Widget* member, int32_t index)
{
	if (index < 0)return;
	Vector2D pos = Vector2D((index % column) * (unitSize.x + spacingX), (index / column) * (unitSize.y + spacingY));
	member->SetRelativePosition(pos);
}

Vector2D GridPanel::GetSize() const
{
	return members.empty() ? Vector2D() : Vector2D(column * (unitSize.x + spacingX) - spacingX, row * (unitSize.y + spacingY) - spacingY);
}






void Text::Update()
{
	Widget::Update();
	if (bindedText)texts.SetCharacters(*bindedText);
	size = Vector2D(float(texts.GetWidth()), float(texts.GetHeight()));
}

void Text::Render()
{
	texts.PrintCharacters(GetScreenPosition() - size * 0.5f, textPattern);
}








void Image::Update()
{
	Widget::Update();
	if (!sprite)return;

	if (copy)spriteInfo.size = Pair(copy->getwidth(), copy->getheight());
	else spriteInfo.size = Pair(sprite->getwidth(), sprite->getheight());

	if (GetScreenRotation() != angle)
	{
		angle = GetScreenRotation();
		RotateImage(PI * angle / 180);
		if (filterLayers.size() > 0)FilterImage();
	}
}

void Image::Render()
{
	if (!sprite)return;

	Vector2D pos = GetScreenPosition();
	HDC dstDC = GetImageHDC();

	IMAGE* img = copy ? copy : sprite;
	HDC srcDC = GetImageHDC(img);
	int w = spriteInfo.endLoc.x - spriteInfo.startLoc.x;
	int h = spriteInfo.endLoc.y - spriteInfo.startLoc.y;

	int dw = int(GetSize().x * w / img->getwidth());
	int dh = int(GetSize().y * h / img->getheight());

	BLENDFUNCTION bf = { AC_SRC_OVER,0,alpha,AC_SRC_ALPHA };
	if (filterLayers.size() > 0 && filter)srcDC = GetImageHDC(filter);
	AlphaBlend(dstDC, (int)pos.x - GetSize().x / 2, (int)pos.y - GetSize().y / 2, dw, dh,
		srcDC, (int)spriteInfo.startLoc.x, (int)spriteInfo.startLoc.y, w, h, bf);
}

bool Image::IsMouseOn()
{
	return IsUnderCursor();
}

void Button::Update()
{
	Image::Update();

	if (!isHover && IsMouseOn()) { OnMouseHoverBegin.BroadCast(); isHover = true; }
	else if (isHover && !IsMouseOn()) { OnMouseHoverEnd.BroadCast(); isHover = false; }

	if (!isPressed && IsMousePressed()) { OnMousePressedBegin.BroadCast(); isPressed = true; }
	else if (isPressed && !IsMousePressed()) { OnMousePressedEnd.BroadCast(); isPressed = false; }


	if (!hover && !pressed) { sprite = normal; return; }

	if (!isHover) { sprite = normal; spriteInfo.endLoc = { normal->getwidth(), normal->getheight() }; }
	else if (!isPressed) { sprite = hover; spriteInfo.endLoc = { hover->getwidth(), hover->getheight() }; }
	else { sprite = pressed; spriteInfo.endLoc = { pressed->getwidth(), pressed->getheight() }; }
}

void Button::LoadNormalPicture(std::string path)
{
	normal = mainWorld.resourcePool->Fetch(path);
	if (!normal)return;
	spriteInfo.endLoc = { normal->getwidth(), normal->getheight() };
}

void Button::LoadHoverPicture(std::string path)
{
	hover = mainWorld.resourcePool->Fetch(path);
	if (!hover)return;
	spriteInfo.endLoc = { hover->getwidth(), hover->getheight() };
}

void Button::LoadClickPicture(std::string path)
{
	pressed = mainWorld.resourcePool->Fetch(path);
	if (!pressed)return;
	spriteInfo.endLoc = { pressed->getwidth(), pressed->getheight() };
}

bool Button::IsMousePressed()
{
	return IsMouseOn() && GameStatics::GetController()->IsMouseClicked();
}
