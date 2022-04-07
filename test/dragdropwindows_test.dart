import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:dragdropwindows/dragdropwindows.dart';

void main() {
  const MethodChannel channel = MethodChannel('dragdropwindows');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
    expect(await Dragdropwindows.platformVersion, '42');
  });
}
