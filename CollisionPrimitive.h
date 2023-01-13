#pragma once
#include<cmath>
#include"Vector3.h"

///平面

struct Plane
{
	//法線ベクトル
	Vector3 normal = { 0,1,0 };

	//原点からの距離
	float distance = 0.0f;

	//面の中心座標
	Vector3 pos = { 0,0,0 };

	//面の大きさ
	Vector3 size = { 1,1,1 };
};


//レイ
struct Ray
{
	//始点座標
	Vector3 start = { 0,0,0 };
	//方向
	//DirectX::XMVECTOR dir = { 1,0,0,0 };
	Vector3 dir = { 1,0,0 };

	bool isReflection = false;
};