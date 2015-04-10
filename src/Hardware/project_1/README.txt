So place this folder + board repository in C/Temp
Export to SDK
Copy the cpp and hpp files and linker script to src folder in SDK folder
The linker script should be good but if you want to regenerate it, make sure everything is being written to bram_ilmb and set the stack and heap to at least 16kB
To compile in C++11, add the flag -std=c++11 to the C++ build/compiler option. Its found in the properties. If it works, there should be no compile errors about the maps in angles.hpp
Also set the optimiser to optimise for speed
Also might be important, the files are updated to peter's changes on monday afternoon