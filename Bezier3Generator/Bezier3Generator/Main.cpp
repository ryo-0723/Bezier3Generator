# include <Siv3D.hpp> // Siv3D v0.6.15
#include "Bezier3.hpp"

void Main()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
	Scene::Resize(1800, 1000);
	Scene::SetResizeMode(ResizeMode::Actual);
	double lens = 0;
	Vec2 oldPos = { 0,0 };
	uint64 oldTime = 0;
	while (System::Update())
	{
		ClearPrint();

		std::vector<Vec2> point{ { 0, 0 }, { 400, 0 }, { 400, 500 }, Cursor::PosF()};
		std::vector<Vec2> points{ { 100, 10 }, { 300, 50 }, { 300, 500 }, {400,200},{700,700},{900,800},{1500,900} };
		Udon::Bezier3 bezier3{ point,0 };
		Udon::Beziers3 beziers3{ points };
		auto p = bezier3.getPos(fmod(Time::GetMillisec() / 2000., 1));
		//Circle{ p, 10 }.draw();

		for (auto&& points : point) {
			Circle{ points,10 }.draw();
		}
		double len = bezier3.getLength_();
		Print << len;
		SimpleGUI::Slider(lens, { 500,100 });
		Vec2 pos = bezier3.getPosFromLength(len * fmod(Time::GetMillisec() / 5000.0, 1));
		//Print << pos;
		Vec2 def= pos - oldPos;
		Print << def.length() / (Time::GetMillisec() - oldTime)*1000;
		Circle{ pos,10 }.draw(Palette::Aqua);
		bezier3.draw(true,Palette::Black);
		beziers3.draw(Palette::Aqua);

		oldPos = pos;
		oldTime = Time::GetMillisec();
	}
}
