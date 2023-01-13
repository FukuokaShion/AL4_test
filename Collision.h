#pragma once
#include "CollisionPrimitive.h"

class Collision
{
public:
	//レイと平面の当たり判定
	//<summary>
	//<param name="lay">レイ</param>
	//<param name="plane">平面</param>
	//<param name="inter">距離(出力用)</param>
	//<param name="inter">交差(出力用)</param>
	//<returns>交差しているか否か</returns>
	static bool CheckRay2Plane(const Ray& ray, const Plane& plane, float* distance);

	static bool CheckRay2Mirror(const Ray& ray, const Plane& plane, float* distance);


};