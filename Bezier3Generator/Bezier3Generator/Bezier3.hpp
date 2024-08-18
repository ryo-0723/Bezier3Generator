#pragma once

#include<Siv3D.hpp>

//ベジェ曲線の作図用クラス
//点を30個描画して疑似的な曲線を再現する
/*
＜流れ＞
点を4つ描画（直線を二つ描画する）
二つ直線を描画したらすぐに曲線が表示されるようにする
それをもとにリアルタイムで描画を更新し続ける

オブジェクトの生成と共に選択された点を基準に曲線描画用のセットを作成する
*/
/*
制御点の格納された配列を受け取り、この中で一つずつのベジェ曲線に分割する
クラスの継承、
表示関数と、何個目の曲線か、各曲線の長さ、各曲線でのある長さでの座標取得関数の実装
制御点の変更関数の実装も
		# P(t)=(1-t)^3*P0 + 3t(1-t)^2*P1 + 3t^2(1-t)*P2 + t^3*P3
		#      =At^3 + Bt^2 + Ct + D
		# とすると
		# A=-P0 + 3P1 - 3P2 + P3
		# B=3P0 - 6P1 + 3P2
		# C=-3P0 + 3P1
		# D=P0
		# 微分すると，
		# P'(t)=3At^2 + 2Bt + C
		# これを二乗する
		# (P'(t))^2 = 9A^2t^4 + 12ABt^3 + (6AC+4B^2)t^2 + 4BCt + C^2
*/
namespace Udon {
	class Bezier3 {
		std::vector<Vec2> points;
		int n;
		size_t index;
		//std::array<double, 5> xx;
	public:
		/// @brief 配列の任意のイテレータを引数にそこから4つの座標を取得し、描画する
		/// @param beginIt 
		Bezier3(std::vector<Vec2>& points, size_t index)
			:points(points)
			, n(1000)
			, index(index)
			//, xx{ 0,0,0,0,0 }
		{
			//Vec2 A = -(*beginIt) + 3 * (*(beginIt + 1)) - 3 * (*(beginIt + 2)) + (*(beginIt + 3));
			//Vec2 B = 3 * (*beginIt) - 6 * (*(beginIt + 1)) + 3 * (*(beginIt + 2));
			//Vec2 C = -3 * (*beginIt) + 3 * (*(beginIt + 1));
			//Vec2 D = (*beginIt);
			//xx = {
			//	9 * sum(A * A),
			//	12 * sum(A * B),
			//	6 * sum(A * C) + 4 * sum(B * B),
			//	4 * sum(B * C),
			//	sum(C * C),
			//};
		}


		Vec2 getPosFromLength(double distance,int div = 10000) {
			double length = 0.0;
			for (int t = 0; t <= div;t++) {
				Vec2 a = getPos((double)t / div);
				Vec2 b = getPos(((double)(t + 1)) / div);
				length += (b - a).length();
				if (length >= distance) {
					return b;
				}
			}
			return getPos(1);
		}
		double getLength_(int div = 1000) {
			double length = 0.0;
			for (int t = 0; t <= div; t++) {
				Vec2 a = getPos((double)t / div);
				Vec2 b = getPos(((double)(t + 1)) / div);
				length += (b - a).length();
			}
			return length;
		}
		Vec2 getPos(const double t) {
			//0 <= tar <= 1.0
			//const auto px = ((1 - t) * (1 - t) * (1 - t)) * (*beginIt).x + t * (3 * ((1 - t) * (1 - t)) * (*(beginIt + 1)).x + t * (3 * (1 - t) * (*(beginIt + 2)).x + t * (*(beginIt + 3)).x));
			//const auto py = ((1 - t) * (1 - t) * (1 - t)) * (*beginIt).y + t * (3 * ((1 - t) * (1 - t)) * (*(beginIt + 1)).y + t * (3 * (1 - t) * (*(beginIt + 2)).y + t * (*(beginIt + 3)).y));
			const auto p = ((1 - t) * (1 - t) * (1 - t)) * points[index] + t * (3 * ((1 - t) * (1 - t)) * (points[index+1]) + t * (3 * (1 - t) * (points[index+2]) + t * (points[index+3])));
			return p;
		}

		Bezier3& draw(bool controlLine, Color color) {
			if (controlLine) {
				Line{ points[index],points[index + 1] }.draw(1, color);
				Line{ points[index + 2],points[index + 3] }.draw(1, color);
			}
			for (int i = 0; i < n; i++) {
				const auto p1 = getPos((double)i / n);
				const auto p2 = getPos((double)(i + 1) / n);
				Line{ p1,p2 }.draw(1, color);
			}
			return *this;
		}

		//double sum(Vec2 v)
		//{
		//	return v.x + v.y;
		//}
		//double getL(double t) {
		//	//t=0.5の地点で二つに分かれるように求める
		//	//0 <= tar <= 1.0
		//	return sqrt(xx[0] * t * t * t * t + xx[1] * t * t * t + xx[2] * t * t + xx[3] * t + xx[4]);
		//}
		//double getLength(int div = 1000) {
		//	double length = 0.0;
		//	double h = 1.0 / div;
		//	for (int t = 1; t <= div; t++) {
		//		double s = (double)t / div;
		//		double k1 = getL(s);
		//		double k2 = getL(s + (h / 2.0));
		//		double k3 = getL(s + h);
		//		double y = h * (k1 + 4.0 * k2 + k3) / 6.0;
		//		length += y;
		//	}
		//	return length;
		//}
	};

	//ベジェ曲線の作図用クラス
	//親クラスのBezier3を継承し,配列にすることで連続的なベジェ曲線を作成する
	//Vec2の配列を受け取って要素数から使用するベジェ曲線の数を決める
	class Beziers3 {
	private:
		std::vector<Vec2> points;
		std::vector<Bezier3> beziers;

	public:
		Beziers3(std::vector<Vec2>& points)
		:points(points)
		{
			if (points.size() % 3 != 1) {//4点の点でベジェ曲線を描き、二点を別の曲線と共有する
				throw "The number of points is not correct";
			}
			else {// size 7
				for (int i = 0; i < points.size()-3; i += 3) {//0,3
					beziers.push_back(Bezier3(points,i));
				}
			}
		}



		Beziers3& getLength() {
			return *this;
		}

		Beziers3& draw(Color color) {
			for (auto&& bezier : beziers) {
				bezier.draw(true,color);
			}
			return *this;
		}
	};
}
