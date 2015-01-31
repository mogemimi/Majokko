{
  'includes': ['common.gypi'],
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
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++14',
      'MACOSX_DEPLOYMENT_TARGET': '10.9',
      'CLANG_CXX_LIBRARY': 'libc++',
	  #'SKIP_INSTALL': 'NO',
      # Warnings:
      'CLANG_WARN_EMPTY_BODY': 'YES',
      'GCC_WARN_64_TO_32_BIT_CONVERSION': 'YES',
      'GCC_WARN_ABOUT_DEPRECATED_FUNCTIONS': 'YES',
      'GCC_WARN_ABOUT_MISSING_FIELD_INITIALIZERS': 'YES',
      'GCC_WARN_ABOUT_MISSING_NEWLINE': 'YES',
      'GCC_WARN_ABOUT_RETURN_TYPE': 'YES',
      'GCC_WARN_CHECK_SWITCH_STATEMENTS': 'YES',
      'GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS': 'YES',
      #'GCC_WARN_INITIALIZER_NOT_FULLY_BRACKETED': 'YES',
      'GCC_WARN_MISSING_PARENTHESES': 'YES',
      'GCC_WARN_NON_VIRTUAL_DESTRUCTOR': 'YES',
      'GCC_WARN_SHADOW': 'YES',
      'GCC_WARN_SIGN_COMPARE': 'YES',
      'GCC_WARN_TYPECHECK_CALLS_TO_PRINTF': 'YES',
      'GCC_WARN_UNINITIALIZED_AUTOS': 'YES',
      'GCC_WARN_UNKNOWN_PRAGMAS': 'YES',
      'GCC_WARN_UNUSED_FUNCTION': 'YES',
      'GCC_WARN_UNUSED_LABEL': 'YES',
      'GCC_WARN_UNUSED_VALUE': 'YES',
      'GCC_WARN_UNUSED_VARIABLE': 'YES',
      'GCC_TREAT_WARNINGS_AS_ERRORS': 'YES',
    },
    'include_dirs': [
      '../pomdog/experimental',
      '../pomdog/include',
    ],
    'dependencies': [
      '../pomdog/build/experimental.gyp:pomdog_experimental',
    ],
    'conditions': [
      ['component == "shared_library"', {
        'dependencies': [
          '../pomdog/build/pomdog.gyp:pomdog-shared',
        ],
        'defines': [
          'POMDOG_USING_LIBRARY_EXPORTS=1',
        ],
      }, {
        'dependencies': [
          '../pomdog/build/pomdog.gyp:pomdog-static',
        ],
      }],
      ['OS == "mac"', {
        'link_settings': {
          'libraries': [
            '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
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
        '../Source/Actor.hpp',
        '../Source/BoundingCircle.cpp',
        '../Source/BoundingCircle.hpp',
        '../Source/MajokkoGame.cpp',
        '../Source/MajokkoGame.hpp',
        '../Source/MajokkoGameLevel.cpp',
        '../Source/MajokkoGameLevel.hpp',
        '../Source/ObjectFactory.cpp',
        '../Source/ObjectFactory.hpp',
        '../Source/RenderLayers/GameWorldLayer.cpp',
        '../Source/RenderLayers/GameWorldLayer.hpp',
        '../Source/RenderLayers/HUDLayer.cpp',
        '../Source/RenderLayers/HUDLayer.hpp',
        '../Source/Component/Breakable.cpp',
        '../Source/Component/Breakable.hpp',
      ],
      'conditions': [
        ['OS == "win"', {
          'sources': [
          ],
        }], # OS == "win"
        ['OS == "mac"', {
          'sources': [
            '../Platform.Cocoa/main.mm',
            '../Platform.Cocoa/Prefix.pch',
            '../Platform.Cocoa/AppDelegate.h',
            '../Platform.Cocoa/AppDelegate.mm',
          ],
          'actions': [
            {
              'action_name': 'XcodeResourcesUpdater',
              'description': 'Always update resources on Xcode',
              'inputs': [],
              'outputs': [],
              'action': ['./update-bundle-resources.sh', '${SOURCE_ROOT}/../Content/.'],
            },
          ],
        }], # OS == "mac"
      ],
      'mac_bundle_resources': [
        '../Platform.Cocoa/Base.lproj/MainMenu.xib',
        '../Platform.Cocoa/English.lproj/InfoPlist.strings',
        '../Platform.Cocoa/Images.xcassets/',
        '../Content/',
      ],
      'xcode_settings': {
        'INFOPLIST_FILE': '../Platform.Cocoa/Info.plist',
        'CLANG_ENABLE_OBJC_ARC': 'YES',
      },
    },
  ],
}
