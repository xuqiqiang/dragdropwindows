# dragdropwindows
This plugin modify windows style to accep Drag&Drop text or files and when Drop text or files from other window, this plugin does call a dart callback function.
flutter gets the text or the file name from the callback function.

## Supported Platforms
- [ ] macOS
- [x] Windows
- [ ] Linux

## Usage
```dart
import 'package:dragdropwindows/dragdropwindows.dart';
.
.
.
if (Platform.isWindows == true) {
      // Platform messages may fail, so we use a try/catch PlatformException.
      try {
        DropfilesWindow.start((DropType type, List<String> results) {
          print("type=$type results=$results");
          setState(() {
            _dropInfo = '${type == DropType.text ? 'Drop text' : 'Drop files'} : ${results.toString()}';
          });
        });
      } on PlatformException {
        _dropInfo = 'Error to drop';
      }
    }
```
refer /example/lib/main.dart for all source code



