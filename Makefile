xcode:
	@gyp Majokko.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/ -Dpomdog_dir="pomdog"

clean:
	@rm -rf ./build.xcodefiles

utf-8:
	@python pomdog/tools/refactor_sources.py Source/
	@python pomdog/tools/refactor_sources.py Platform.Cocoa/
