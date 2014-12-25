//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import "AppDelegate.h"

#include "MajokkoGame.hpp"
#include <Pomdog/Platform/Cocoa/BootstrapperCocoa.hpp>
#include <Pomdog/Pomdog.hpp>
#include <iostream>
#include <thread>

using Pomdog::GameHost;
using Pomdog::Log;
using Pomdog::LogEntry;
using Pomdog::LogLevel;
using Pomdog::ScopedConnection;

@implementation AppDelegate
{
	ScopedConnection connection;
	std::thread gameRunThread;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
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

	gameRunThread = std::thread([self] {
		using Pomdog::Details::Cocoa::BootstrapperCocoa;

		BootstrapperCocoa bootstrapper;
		bootstrapper.Run([self window], [](std::shared_ptr<GameHost> const& gameHost) {
			try {
				Majokko::MajokkoGame game{gameHost};
				gameHost->Run(game);
			}
			catch (std::exception const& e) {
				Log::Critical("Pomdog", e.what());
			}
		});
		
		// Shutdown your application
		[NSApp terminate:nil];
	});

	Log::Verbose("game mainloop thread run");
}

@end
