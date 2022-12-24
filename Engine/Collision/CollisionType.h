#pragma once

enum CollisionShapeType {
	SHAPE_UNKNOWN = -1,		//未設定
	COLLISIONSHAPE_SQHERE,	//球
	COLLISIONSHAPE_TRYANGLE,//三角形
	COLLISIONSHAPE_MESH,	//メッシュ
};