import 'dart:convert';
import 'dart:io';
import 'package:flutter/material.dart';

class TcpState extends ChangeNotifier {
  bool linked = false;
  Map<String, dynamic>? jsonData = jsonDecode(
      '{"temp":{"value":-1,"set":-1},"humi":-1,"light":{"value":-1,"set":-1},"air":{"value":-1,"set":-1},"pwm":{"led":-1,"motor":-1}}');
  bool temp = false;
  bool light = false;
  bool air = false;
  String error = "";
  bool submit = true;
  TcpState({
    this.linked = false,
    this.jsonData,
  });

  void updateLinked(bool value) {
    linked = value;
    // if (!value) {
    //   jsonData = jsonDecode(
    //       '{"temp":{"value":null,"set":null},"humi":-1,"light":{"value":-1,"set":-1},"air":{"value":-1,"set":-1},"pwm":{"led":-1,"motor":-1}}');
    //   temp = light = air = false;
    // }
    notifyListeners(); // 通知监听者状态已更改
  }

  void updateJsonData(Map<String, dynamic>? data) {
    jsonData = data;
    temp =
        jsonData?["temp"]["value"] >= jsonData?["temp"]["set"] ? true : false;
    light =
        jsonData?["light"]["value"] <= jsonData?["light"]["set"] ? true : false;
    air = jsonData?["air"]["value"] >= jsonData?["air"]["set"] ? true : false;
    if (submit) {
      notifyListeners(); // 通知监听者状态已更改
    }
  }

  void uppdateError(String value) {
    error = value;
    notifyListeners(); // 通知监听者状态已更改
  }
}

class TcpManager {
  final TcpState _tcpState;

  TcpManager(this._tcpState);

  bool linked = false;
  Socket? socket;
  Map<String, dynamic>? jsonData = jsonDecode(
      '{"temp":{"value":-1,"set":-1},"humi":-1,"light":{"value":-1,"set":-1},"air":{"value":-1,"set":-1},"pwm":{"led":-1,"motor":-1}}');
  Future<void> connect() async {
    try {
      socket = await Socket.connect('192.168.4.1', 8080);
      linked = true;
      _tcpState.updateLinked(true);
      _tcpState.updateJsonData(jsonData);
      receive(); // 连接成功后开始监听数据
    } catch (e) {
      _tcpState.uppdateError('error: $e');
      await socket?.close();
    }
  }

  Future<void> disconnect() async {
    try {
      await socket?.close();
      linked = false;
      _tcpState.updateLinked(false);
    } catch (e) {
      _tcpState.uppdateError('error: $e');
    }
  }

  Future<void> send(String message) async {
    if (_tcpState.linked) {
      socket?.add(utf8.encode('$message\n')); // 确保消息以换行符结束
      await socket?.flush();
    }
  }

  Future<void> receive() async {
    socket?.listen((data) {
      // 处理接收到的数据
      String receivedData = utf8.decode(data);
      //_tcpState.uppdateError(receivedData);
      handleData(receivedData);
    }, onDone: () async {
      linked = false;
      _tcpState.updateLinked(false);
      await socket?.close();
    }, onError: (err) async {
      _tcpState.uppdateError('error: $err');
      linked = false; // 如果发生错误，断开连接
      _tcpState.updateLinked(false);
      await socket?.close();
    });
  }

  void submit(bool value) {
    _tcpState.submit = value;
  }

  void handleData(String data) {
    try {
      // 解析JSON数据
      final parsedData = json.decode(data);
      _tcpState.updateJsonData(parsedData);
    } catch (e) {
      _tcpState.uppdateError('jsondecodeerror: $e');
    }
  }
}
