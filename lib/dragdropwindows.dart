import 'dart:async';

import 'package:flutter/services.dart';

enum DropType {
  text,
  file,
}

typedef void OnDropCallback(DropType type, List<String> results);

class Dragdropwindows {
  static const MethodChannel _channel = MethodChannel('dragdropwindows');

  static Future<String?> get platformVersion async {
    final String? version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }

  /// to remove the callback call the function again with null as parameter
  static void start(OnDropCallback? callback) async {
    if (callback != null) {
      _channel.setMethodCallHandler((MethodCall call) async {
        print("method=${call.method} argumemts=${call.arguments}");
        assert(call.method == 'onDragDrop');
        String info = call.arguments;
        DropType type = info[0] == '1' ? DropType.text : DropType.file;
        List<String> results = info.substring(1).split('|');
        callback(type, results);
      });
      await _channel.invokeMethod('modifyWindowAcceptFiles');
    }
  }

  static void stop() async {
    _channel.setMethodCallHandler(null);
    await _channel.invokeMethod('resetWindowAcceptFiles');
  }
}
