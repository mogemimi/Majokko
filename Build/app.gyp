# Examples(Xcode):
# gyp build/app.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
#
# Examples(MSVS 2013):
# gyp build/app.gyp --depth=. -f msvs -G msvs_version=2013 --generator-output=./build.msvc/

{
  'includes': [
    'common.gypi',
    '../pomdog/build/experimental.gypi',
  ],
  'variables': {
    'pomdog_framework_dir': '../pomdog',
  },
  'make_global_settings': [
    ['CXX','/usr/bin/clang++'],
    ['LINK','/usr/bin/clang++'],
  ],
  'target_defaults': {
    'configurations': {
      'Debug': {
        'msvs_settings': {
          'VCLinkerTool': {
            'GenerateDebugInformation': 'true', # /DEBUG
          },
        },
      }, # Debug
    },
    'msvs_settings':{
      'VCCLCompilerTool': {
        'WarningLevel': '4',   # /W4
      },
    },
    'xcode_settings': {
      'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++1y',
      'MACOSX_DEPLOYMENT_TARGET': '10.9', # OS X Deployment Target: 10.9
      'CLANG_CXX_LIBRARY': 'libc++', # libc++ requires OS X 10.7 or later
    },
    'include_dirs': [
      '<@(pomdog_framework_dir)/include',
      '<@(pomdog_framework_dir)/experimental',
      '<@(pomdog_framework_dir)/third_party/rapidjson/include',
    ],
    'conditions': [
      ['component == "shared_library"', {
        'dependencies': [
          '<@(pomdog_framework_dir)/build/pomdog.gyp:pomdog-shared',
        ],
        'defines': [
          'POMDOG_USING_LIBRARY_EXPORTS=1',
        ],
      }, {
        'dependencies': [
          '<@(pomdog_framework_dir)/build/pomdog.gyp:pomdog-static',
        ],
      }],
      ['OS == "win"', {
          'sources': [
          ],
      }], # OS == "win"
      ['OS == "mac"', {
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
              '$(SDKROOT)/System/Library/Frameworks/OpenGL.framework',
            ],
          },
      }], # OS == "mac"
    ],
  },
  'targets': [
    {
      'target_name': 'Majokko',
      'product_name': 'Majokko',
      'type': 'executable',
      'mac_bundle': 1,
      'sources': [
        '<@(pomdog_experimental_2d_sources)',
        '../Source/MajokkoGame.cpp',
        '../Source/MajokkoGame.hpp',
      ],
      'conditions': [
        ['OS == "win"', {
          'sources': [
          ],
        }], # OS == "win"
        ['OS == "mac"', {
          'sources': [
            '../Platform.Cocoa/main.mm',
            '../Platform.Cocoa/Majokko-Prefix.pch',
            '../Platform.Cocoa/AppDelegate.h',
            '../Platform.Cocoa/AppDelegate.mm',
          ],
        }], # OS == "mac"
      ],
      'mac_bundle_resources': [
        '../Platform.Cocoa/Base.lproj/MainMenu.xib',
        '../Platform.Cocoa/English.lproj/InfoPlist.strings',
        #'../Platform.Cocoa/Images.xcassets/',
        '../Content/',
      ],
      'xcode_settings': {
        'INFOPLIST_FILE': '../Platform.Cocoa/Majokko-Info.plist',
        'CLANG_ENABLE_OBJC_ARC': 'YES',
      },
    },
  ],# "targets"
}
