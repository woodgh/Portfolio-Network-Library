echo Generating to Makefiles...

if [ ! -d Build ]; then
	mkdir Build
fi

cd Build

cmake ../
cmake --build . --config Release
make

cd ..