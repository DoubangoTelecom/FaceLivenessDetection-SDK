setlocal
set PYTHONPATH=%PYTHONPATH%;.;../../../python
set PATH=%PATH%;%~dp0
python ../../../samples/python/deepfake/deepfake.py ^
	--video ../../../assets/videos/deepfake.mp4 ^
	--assets ../../../assets
endlocal
