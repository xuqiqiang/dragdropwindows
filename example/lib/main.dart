import 'dart:async';
import 'dart:io';

import 'package:dragdropwindows/dragdropwindows.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  String _platformVersion = 'Unknown';
  String _dropInfo = 'Drag drop anything to here';
  bool _start = true;

  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  // Platform messages are asynchronous, so we initialize in an async method.
  Future<void> initPlatformState() async {
    String platformVersion;
    // Platform messages may fail, so we use a try/catch PlatformException.
    // We also handle the message potentially returning null.
    try {
      platformVersion =
          await Dragdropwindows.platformVersion ?? 'Unknown platform version';
    } on PlatformException {
      platformVersion = 'Failed to get platform version.';
    }

    startDragDrop();

    // If the widget was removed from the tree while the asynchronous platform
    // message was in flight, we want to discard the reply rather than calling
    // setState to update our non-existent appearance.
    if (!mounted) return;

    setState(() {
      _platformVersion = platformVersion;
    });
  }

  startDragDrop() {
    if (Platform.isWindows) {
      try {
        Dragdropwindows.start((DropType type, List<String> results) {
          print("type=$type results=$results");
          setState(() {
            _dropInfo =
                '${type == DropType.text ? 'Drop text' : 'Drop files'} : ${results.toString()}';
          });
        });
      } on PlatformException {
        setState(() {
          _dropInfo = 'Error to drop';
        });
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Column(
            children: [
              const Spacer(),
              Text('Running on: $_platformVersion\n$_dropInfo',
                  textAlign: TextAlign.center),
              const Spacer(),
              ElevatedButton(
                onPressed: () {
                  if (_start) {
                    Dragdropwindows.stop();
                  } else {
                    startDragDrop();
                  }
                  setState(() {
                    _start = !_start;
                  });
                },
                child: Text(_start ? 'Cancel drop' : 'Start drop'),
              ),
              const SizedBox(height: 50)
            ],
          ),
        ),
      ),
    );
  }
}
