﻿#import "AppDelegate.h"
#include "../Source/MajokkoGame.hpp"
#include "Pomdog/Platform/Cocoa/BootstrapperCocoa.hpp"
#include "Pomdog/Pomdog.hpp"
#include <iostream>
#include <thread>

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate
{
	Pomdog::ScopedConnection connection;
	std::thread gameRunThread;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	using Pomdog::Log;
	using Pomdog::LogEntry;
	using Pomdog::LogLevel;

#ifdef DEBUG
	connection = Log::Connect([](Pomdog::LogEntry const& entry) {
		//NSString* log = [NSString stringWithUTF8String:entry.Message.c_str()];
		//NSLog(@"\n%@", log);
		std::cout << entry.Message << "\n";
	});
#endif

	Log::SetLevel(LogLevel::Internal);
	Log::Verbose("applicationDidFinishLaunching");

	[[self window] makeKeyAndOrderFront:self];

	gameRunThread = std::thread([self]
	{
		try {
			using Bootstrap = Pomdog::Details::Cocoa::BootstrapperCocoa;
			auto gameHost = Bootstrap().CreateGameHost([self window]);

			Majokko::MajokkoGame game{gameHost};
			gameHost->Run(game);
		}
		catch (std::exception const& e) {
			Log::Critical("Pomdog", e.what());
		}

		// Shutdown your application
		[NSApp terminate:nil];
	});

	Log::Verbose("game mainloop thread run");
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

@end
