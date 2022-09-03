#include <Cocoa/Cocoa.h>
#include "file-dialog.hpp"


@interface FileDialog : NSObject
+ (void)exportDocumentWithName:(NSString*)name withCallback:(FileDialogCallback)cb;
+ (void)importDocumentWithCallback:(FileDialogCallback)cb;
@end


void showSaveDialogCxx(const std::string& name, FileDialogCallback cb) {
	NSLog(@"the name is: %s", name.c_str());
	[FileDialog exportDocumentWithName:@(name.c_str()) withCallback:cb];
}

void showOpenDialogCxx(FileDialogCallback cb) {
	[FileDialog importDocumentWithCallback:cb];
}

@implementation FileDialog

+ (void)exportDocumentWithName:(NSString*)name withCallback:(FileDialogCallback)cb {
	// Set the default name for the file and show the panel.
	auto panel = [NSSavePanel savePanel];
	[panel setNameFieldStringValue:name];
	[panel setAllowedFileTypes:@[@"gmd"]];
	[panel beginWithCompletionHandler:^(NSInteger result) {
		if (result == NSFileHandlingPanelOKButton) {
			auto fileUrl = [panel URL];
			auto path = std::string([[fileUrl path] UTF8String], [[fileUrl path] lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
			cb(path);
		}
	}];
}

+ (void)importDocumentWithCallback:(FileDialogCallback)cb {
	NSOpenPanel* panel = [NSOpenPanel openPanel];
	[panel setAllowedFileTypes:@[@"gmd"]];
	[panel beginWithCompletionHandler:^(NSInteger result){
		if (result == NSFileHandlingPanelOKButton) {
			auto fileUrl = [[panel URLs] objectAtIndex:0];
			auto path = std::string([[fileUrl path] UTF8String], [[fileUrl path] lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
			cb(path);
		}
	}];
}

@end