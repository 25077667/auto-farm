import 'package:flutter/material.dart';
import 'bt.dart';

class HomePage extends StatelessWidget {
  const HomePage({Key? key, required this.title}) : super(key: key);

  final String title;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(title),
      ),
      body: Column(
        children: [
          const Spacer(),
          Expanded(
              child: ElevatedButton(
            child: const Text('Bluetooth'),
            onPressed: () {
              Navigator.push(
                  context,
                  MaterialPageRoute(
                    builder: (context) => BluetoothPage(),
                  ));
            },
          )),
          const Spacer(),
          Row(), // Padding the width
        ],
      ),
    );
  }
}
