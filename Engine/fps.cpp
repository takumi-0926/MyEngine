#include "fps.h"

void Fps::Init()
{
	QueryPerformanceFrequency(&Freq);
	QueryPerformanceCounter(&StartTime);//現在の時間を取得（1フレーム目)
}

void Fps::PreRun()
{
    //FPSの計算
    if (iCount == 60)//カウントが60の時の処理
    {
        QueryPerformanceCounter(&NowTime);//現在の時間を取得（60フレーム目）
        //FPS = 1秒 / 1フレームの描画にかかる時間
        //    = 1000ms / ((現在の時間ms - 1フレーム目の時間ms) / 60フレーム)
        fps = 1000.0 / (static_cast<double>((NowTime.QuadPart - StartTime.QuadPart) * 1000 / Freq.QuadPart) / 60.0);
        iCount = 0;//カウントを初期値に戻す
        StartTime = NowTime;//1フレーム目の時間を現在の時間にする
    }
    iCount++;//カウント+1
}

void Fps::PostRun()
{
    //Sleepさせる時間の計算
    QueryPerformanceCounter(&NowTime);//現在の時間を取得
    //Sleepさせる時間ms = 1フレーム目から現在のフレームまでの描画にかかるべき時間ms - 1フレーム目から現在のフレームまで実際にかかった時間ms
    //                  = (1000ms / 60)*フレーム数 - (現在の時間ms - 1フレーム目の時間ms)
    SleepTime = static_cast<DWORD>((1000.0 / 60.0) * iCount - (NowTime.QuadPart - StartTime.QuadPart) * 1000 / Freq.QuadPart);
    if (SleepTime > 0 && SleepTime < 18)//大きく変動がなければSleepTimeは1～17の間に納まる
    {
        timeBeginPeriod(1);
        Sleep(SleepTime);
        timeEndPeriod(1);
    }
    else//大きく変動があった場合
    {
        timeBeginPeriod(1);
        Sleep(1);
        timeEndPeriod(1);
    }
}
