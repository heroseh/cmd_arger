
# Example

A two in one example of cmd arger. <br/>
The build script that builds the example program uses cmd arger to parse it's arguments. <br/>
**build_script.c** is the "real world" example where as **example.c** an simple example to look at first.

Build the build script that builds the program with. <br/>
if you mess with **build_script.c** you must repeat this step

Unix:
```
./build_build_script.sh
```
Windows:
```
build_build_script.bat
```

Now build the simple example program by running the script. <br/>
if you mess with **example.c** you only need to repeat this step.

Unix:
```
./build_script
```
Windows:
```
build_script.exe
```

Now test out the example with and see the results.

Unix:
```
./example string_value 88 5.67
```
Windows:
```
example.exe string_value 88 5.67
```

Now test out the example with some optional arguments and see the results

Unix:
```
./example --flag --string "overwritten value" string_value 88 --integer 2048 5.67
```
Windows:
```
example.exe --flag --string "overwritten value" string_value 88 --integer 2048 5.67
```

