import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';

class ActionPage extends StatefulWidget {
  ActionPage({Key? key, required this.device}) : super(key: key);
  final BluetoothDevice device;

  bool relayStatus = false;

  @override
  ActionPageState createState() => ActionPageState();
}

class ActionPageState extends State<ActionPage> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("Action"),
      ),
      body: _genActionButton(),
    );
  }

  ListView _genActionButton() {
    List<SizedBox> containers = [];

    // Relay switch
    containers.add(SizedBox(
      height: 50,
      child: TextButton(
        child: const Text("Relay"),
        onPressed: () {
          // Flip the relay status
          super.widget.relayStatus = !super.widget.relayStatus;
        },
      ),
    ));

    return ListView(
      padding: const EdgeInsets.all(8),
      children: <Widget>[
        ...containers,
        Text(super.widget.relayStatus.toString()),
      ],
    );
  }
}
