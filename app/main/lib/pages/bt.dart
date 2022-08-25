// Modified from https://blog.kuzzle.io/communicate-through-ble-using-flutter

import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';

import 'action.dart';

class BluetoothPage extends StatefulWidget {
  BluetoothPage({super.key});

  final FlutterBlue flutterBlue = FlutterBlue.instance;
  // Scanned devices (might not connected)
  final List<BluetoothDevice> devicesList = [];

  @override
  BluetoothPageState createState() => BluetoothPageState();
}

class BluetoothPageState extends State<BluetoothPage> {
  List<BluetoothService> _services = [];
  late BluetoothDevice _connectedDevice;
  bool connected = false;

  @override
  Widget build(BuildContext context) {
    if (connected == false) {
      return Scaffold(
        appBar: AppBar(
          title: const Text("Bluetooth menu"),
        ),
        body: _buildListViewOfDevices(),
      );
    } else {
      // Go to the action page
      return MaterialApp(
        home: ActionPage(device: _connectedDevice),
      );
    }
  }

  _addDeviceToList(final BluetoothDevice device) {
    if (!widget.devicesList.contains(device)) {
      setState(() {
        widget.devicesList.add(device);
      });
    }
  }

  @override
  void initState() {
    super.initState();
    widget.flutterBlue.connectedDevices
        .asStream()
        .listen((List<BluetoothDevice> devices) {
      for (BluetoothDevice device in devices) {
        _addDeviceToList(device);
      }
    });
    widget.flutterBlue.scanResults.listen((List<ScanResult> results) {
      for (ScanResult result in results) {
        _addDeviceToList(result.device);
      }
    });
    widget.flutterBlue.startScan();
  }

  ListView _buildListViewOfDevices() {
    List<SizedBox> containers = [];
    for (BluetoothDevice device in widget.devicesList) {
      containers.add(
        SizedBox(
          height: 50,
          child: Row(
            children: <Widget>[
              Expanded(
                child: Column(
                  children: <Widget>[
                    Text(device.name == '' ? '(unknown device)' : device.name),
                    Text(device.id.toString()),
                  ],
                ),
              ),
              TextButton(
                style: TextButton.styleFrom(
                  primary: Colors.blue,
                ),
                child: const Text(
                  'Connect',
                  style: TextStyle(color: Colors.white),
                ),
                onPressed: () {
                  setState(() async {
                    widget.flutterBlue.stopScan();
                    try {
                      await device.connect();
                    } catch (e) {
                      if (e.toString() != 'already_connected') {
                        rethrow;
                      }
                    } finally {
                      _services = await device.discoverServices();
                    }
                    _connectedDevice = device;
                    connected = true;
                  });
                },
              ),
            ],
          ),
        ),
      );
    }

    return ListView(
      padding: const EdgeInsets.all(8),
      children: <Widget>[
        ...containers,
      ],
    );
  }
}
