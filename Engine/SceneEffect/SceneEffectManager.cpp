#include "SceneEffectManager.h"

void SceneEffectManager::Update()
{
#pragma region �V�[���J�ڏ���
	//�G���^�[�L�[�������ƃV�[���J��
	//if (input->Trigger(DIK_RETURN)) {
	//	sceneFlag = true;
	//}

	//�V�[���J�ڒ�
	if (sceneFlag == true) {

		//�J�ڒ��̏���������

		//�J�ڎ��s��
		if (fadeFlag == true) {
			addTime2 += 1.0f / 60.0f;
			timeRate2 = min(addTime2 / maxTime, 1.0f);
			Fade->SetPos(XMFLOAT2(easeIn(end, end2, timeRate2),0));

			//�J�ڏI��
			if (Fade->Pos().x == end2) {
				sceneFlag = false;
				fadeFlag = false;
			}
		}
		//�J�ڎ��s�O
		else {
			addTime += 1.0f / 60.0f;
			timeRate = min(addTime / maxTime, 1.0f);
			Fade->SetPos(XMFLOAT2(easeOut(start, end, timeRate), 0));

			//�K��ʒu�ɓ��B������V�[���J�ڎ��s
			//if (Fade->Pos().x == end) {
			//	fadeFlag = true;

			//	//�^�C�g������Q�[����
			//	if (sceneCount == TITLE) {
			//		sceneCount = GAME;
			//	}
			//	//�Q�[�����烊�U���g��
			//	else if (sceneCount == GAME) {
			//		sceneCount = RESLUT;
			//	}
			//	//���U���g����^�C�g����
			//	else if (sceneCount == RESLUT) {
			//		sceneCount = TITLE;
			//	}
			//	//�t�F�[�h���Ɏg���ϐ��̏�����
			//	FadeReset();
			//}
		}
	}

#pragma endregion

}
