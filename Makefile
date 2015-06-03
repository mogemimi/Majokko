xcode:
	@gyp Build/Majokko.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/

clean:
	@rm -rf ./build.xcodefiles

utf-8:
	@python pomdog/tools/refactor_sources.py Source/