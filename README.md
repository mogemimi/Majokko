# Majokko

![Majokko-chan](Content/Majokko.png)

## Requirements

* Mac OS X 10.9+
* Xcode 5.1+ (Apple LLVM 5.1/Clang 3.4)
* OpenGL 4 and Direct3D 11.2/12
* Python 2.7+

## How to build

### Prerequisite: Installing GYP

First, install GYP from https://chromium.googlesource.com/external/.  
Make sure git is installed.
From the root of your engine directory, run:  
```bash
git clone https://chromium.googlesource.com/external/gyp.git Tools/gyp
```

Second, run setup.py.

**Linux and Mac OS X**

To install globally with gyp:

```bash
cd Tools/gyp
[sudo] python setup.py install
```

**Windows**

On Windows systems you can do:

```bash
cd Tools/gyp
python setup.py install
```

### Installing dependencies

```bash
cd Majokko
git clone https://github.com/mogemimi/pomdog.git pomdog
cd pomdog
git checkout -b gamedev origin/gamedev
```

### Building under Mac OS X and Xcode

**1. Generating the Xcode project file**

```bash
gyp Build/app.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
```

**2. Building (Release/Debug)**

```
xcodebuild -project build.xcodefiles/Build/app.xcodeproj
```

To build in release mode, use `-configuration` option:

```bash
xcodebuild -project build.xcodefiles/Build/app.xcodeproj -configuration Release
```

**3. Running app**

```bash
open Build/build/Release/Majokko.app
```
