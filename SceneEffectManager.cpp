#include "SceneEffectManager.h"

void SceneEffectManager::Update()
{
#pragma region シーン遷移処理
	//エンターキーを押すとシーン遷移
	//if (input->Trigger(DIK_RETURN)) {
	//	sceneFlag = true;
	//}

	//シーン遷移中
	if (sceneFlag == true) {

		//遷移中の処理を書く

		//遷移実行後
		if (fadeFlag == true) {
			addTime2 += 1.0f / 60.0f;
			timeRate2 = min(addTime2 / maxTime, 1.0f);
			Fade->SetPos(XMFLOAT2(easeIn(end, end2, timeRate2),0));

			//遷移終了
			if (Fade->Pos().x == end2) {
				sceneFlag = false;
				fadeFlag = false;
			}
		}
		//遷移実行前
		else {
			addTime += 1.0f / 60.0f;
			timeRate = min(addTime / maxTime, 1.0f);
			Fade->SetPos(XMFLOAT2(easeOut(start, end, timeRate), 0));

			//規定位置に到達したらシーン遷移実行
			//if (Fade->Pos().x == end) {
			//	fadeFlag = true;

			//	//タイトルからゲームに
			//	if (sceneCount == TITLE) {
			//		sceneCount = GAME;
			//	}
			//	//ゲームからリザルトに
			//	else if (sceneCount == GAME) {
			//		sceneCount = RESLUT;
			//	}
			//	//リザルトからタイトルに
			//	else if (sceneCount == RESLUT) {
			//		sceneCount = TITLE;
			//	}
			//	//フェード時に使う変数の初期化
			//	FadeReset();
			//}
		}
	}

#pragma endregion

}
