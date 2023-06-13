#include "fps.h"

void Fps::Init()
{
	QueryPerformanceFrequency(&Freq);
	QueryPerformanceCounter(&StartTime);//���݂̎��Ԃ��擾�i1�t���[����)
}

void Fps::PreRun()
{
    //FPS�̌v�Z
    if (iCount == 60)//�J�E���g��60�̎��̏���
    {
        QueryPerformanceCounter(&NowTime);//���݂̎��Ԃ��擾�i60�t���[���ځj
        //FPS = 1�b / 1�t���[���̕`��ɂ����鎞��
        //    = 1000ms / ((���݂̎���ms - 1�t���[���ڂ̎���ms) / 60�t���[��)
        fps = 1000.0 / (static_cast<double>((NowTime.QuadPart - StartTime.QuadPart) * 1000 / Freq.QuadPart) / 60.0);
        iCount = 0;//�J�E���g�������l�ɖ߂�
        StartTime = NowTime;//1�t���[���ڂ̎��Ԃ����݂̎��Ԃɂ���
    }
    iCount++;//�J�E���g+1
}

void Fps::PostRun()
{
    //Sleep�����鎞�Ԃ̌v�Z
    QueryPerformanceCounter(&NowTime);//���݂̎��Ԃ��擾
    //Sleep�����鎞��ms = 1�t���[���ڂ��猻�݂̃t���[���܂ł̕`��ɂ�����ׂ�����ms - 1�t���[���ڂ��猻�݂̃t���[���܂Ŏ��ۂɂ�����������ms
    //                  = (1000ms / 60)*�t���[���� - (���݂̎���ms - 1�t���[���ڂ̎���ms)
    SleepTime = static_cast<DWORD>((1000.0 / 60.0) * iCount - (NowTime.QuadPart - StartTime.QuadPart) * 1000 / Freq.QuadPart);
    if (SleepTime > 0 && SleepTime < 18)//�傫���ϓ����Ȃ����SleepTime��1�`17�̊Ԃɔ[�܂�
    {
        timeBeginPeriod(1);
        Sleep(SleepTime);
        timeEndPeriod(1);
    }
    else//�傫���ϓ����������ꍇ
    {
        timeBeginPeriod(1);
        Sleep(1);
        timeEndPeriod(1);
    }
}
