setlocal
set PYTHONPATH=%PYTHONPATH%;.;../../../python
set PATH=%PATH%;%~dp0
python ../../../samples/python/liveness/liveness.py --image ../../../assets/images/disguise.jpg --assets ../../../assets
endlocal