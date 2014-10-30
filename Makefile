xcode:
	@gyp Build/Majokko.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/

clean:
	@rm -rf ./build.xcodefiles

utf-8:
	@python pomdog/tools/source2utf8-with-signature.py Source/