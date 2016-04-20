2016.01.21更新
1.删除以下API
	dvoplay_GetCacheSize
	dvoplay_GetFilePlayInfo
	dvoplay_GetFps
	dvoplay_GetFrames
	dvoplay_GetRate
	dvoplay_GetTimeEplased
以上API的相关功能被整合到新的API dvoplay_GetPlayerInfo中;
2.修改音频播放组件，由所有播放器共享一个DirectSound和DirectSoundBuffer对象改为只共享DirectSound对象，DirectSoundBuffer为播放器私有;
3.另外在播放声音时，若连续100ms内，没有声音流，则停止播放声音，后续有声音流到来时，则继续播放;
