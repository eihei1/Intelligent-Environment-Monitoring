import 'package:flutter/material.dart';
import 'package:flutter_svg/flutter_svg.dart';
import 'widgets/LevelBox.dart';
import 'widgets/SquareBox.dart';
import 'widgets/LinkBox.dart';
import 'widgets/VerticalBox.dart';
import 'widgets/tcp.dart';
import 'package:provider/provider.dart';

// 主函数
Future<void> main() async {
  runApp(ChangeNotifierProvider<TcpState>(
      create: (_) => TcpState(),
      child: const MaterialApp(
        home: Scaffold(
          body: MyApp(),
        ),
      )));
}

class MyApp extends StatefulWidget {
  const MyApp({
    super.key,
  });

  @override
  State<MyApp> createState() => _MyApp();
}

class _MyApp extends State<MyApp> with SingleTickerProviderStateMixin {
  late AnimationController _controller;
  late Animation<double> _opacityAnimation;
  @override
  TcpManager? _tcpManager;
  @override
  void initState() {
    super.initState();
    final tcpState = Provider.of<TcpState>(context, listen: false);
    _tcpManager = TcpManager(tcpState);
    setState(() {});
    _controller = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 50),
      value: 0.0, // 初始不透明度设置为0
    )
      ..addListener(() {
        setState(() {}); // 更新UI
      })
      ..forward(); // 立即开始动画
    _opacityAnimation = _controller.view;
  }

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
  }

  @override
  void dispose() {
    _controller.dispose(); // 释放资源
    super.dispose();
    _tcpManager?.disconnect();
  }

  @override
  Widget build(BuildContext context) {
    final tcpState = Provider.of<TcpState>(context);
    double multiple = MediaQuery.of(context).size.width / 360;
    return AnimatedOpacity(
      opacity: _opacityAnimation.value,
      duration: const Duration(seconds: 2),
      child: Center(
        child: SafeArea(
            child: Consumer<TcpState>(builder: (context, tcpState, child) {
          return Column(mainAxisAlignment: MainAxisAlignment.start, children: [
            const SizedBox(height: 36),
            SvgPicture.asset(
              "images/title.svg",
              width: MediaQuery.of(context).size.width / 360 * 321,
            ),
            const SizedBox(height: 13),
            Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
              VerticalBox(
                iconImagePath: 'images/wendu.svg',
                textImagePath: 'images/wenduhuanjin.svg',
                textUnit: "°C",
                number:
                    tcpState.jsonData?["temp"]?["value"]?.toString() ?? '-1',
              ),
              VerticalBox(
                iconImagePath: 'images/shidu.svg',
                textImagePath: 'images/huanjinshudu.svg',
                textUnit: "%",
                number: tcpState.jsonData?["humi"]?.toString() ?? '-1',
              ),
              VerticalBox(
                iconImagePath: 'images/qiti.svg',
                textImagePath: 'images/qitinongdu.svg',
                textUnit: "%",
                number: tcpState.jsonData?["air"]?["value"]?.toString() ?? '-1',
              ),
              VerticalBox(
                iconImagePath: 'images/guangzhao.svg',
                textImagePath: 'images/guangzhaoqiangdu.svg',
                textUnit: "%",
                number:
                    tcpState.jsonData?["light"]?["value"]?.toString() ?? '-1',
              ),
            ]),
            SizedBox(
              width: multiple * 360,
              height: multiple * 251,
              child: Stack(
                children: [
                  Positioned(
                    left: multiple * 17,
                    top: multiple * 13,
                    child: InkWell(
                        onTap: () async {
                          if (tcpState.linked) {
                            await _tcpManager?.disconnect();
                          } else {
                            await _tcpManager?.connect();
                          }
                        },
                        child: LinkBox(link: tcpState.linked)),
                  ),
                  Positioned(
                      left: multiple * 112,
                      top: multiple * 16,
                      child: LevelBox(
                        iconImagePath: "images/qiwen_.svg",
                        textImagePath: "images/shezhiwendu.svg",
                        textUnit: "℃",
                        lable: "temp",
                        send: _tcpManager?.send ?? (String message) async {},
                        shounumb:
                            tcpState.jsonData?["temp"]?["set"]?.toString() ??
                                '-1',
                        submit: _tcpManager?.submit ?? (bool value) {},
                      )),
                  Positioned(
                      left: multiple * 19,
                      top: multiple * 91,
                      child: LevelBox(
                        iconImagePath: "images/qiti_.svg",
                        textImagePath: "images/shezhiqiti.svg",
                        textUnit: "%",
                        lable: "soil_hmd",
                        send: _tcpManager?.send ?? (String message) async {},
                        shounumb:
                            tcpState.jsonData?["air"]?["set"]?.toString() ??
                                '-1',
                        submit: _tcpManager?.submit ?? (bool value) {},
                      )),
                  Positioned(
                      left: multiple * 19,
                      top: multiple * 171,
                      child: LevelBox(
                        iconImagePath: "images/liangdu_.svg",
                        textImagePath: "images/shezhiliangdu.svg",
                        textUnit: "%",
                        lable: "light",
                        send: _tcpManager?.send ?? (String message) async {},
                        shounumb:
                            tcpState.jsonData?["light"]?["set"]?.toString() ??
                                '-1',
                        submit: _tcpManager?.submit ?? (bool value) {},
                      )),
                  Positioned(
                    left: multiple * 265,
                    top: multiple * 113,
                    child: ColorFiltered(
                      colorFilter: ColorFilter.mode(
                          (tcpState.light || tcpState.temp || tcpState.air)
                              ? Colors.red
                              : const Color.fromARGB(255, 115, 115, 115),
                          BlendMode.srcIn),
                      child: SvgPicture.asset(
                        "images/jingao.svg",
                        width: multiple * 32,
                        height: multiple * 32,
                      ),
                    ),
                  ),
                  Positioned(
                    left: multiple * 322,
                    top: multiple * 108,
                    child: ColorFiltered(
                      colorFilter: ColorFilter.mode(
                          tcpState?.temp ?? false
                              ? Colors.red
                              : const Color.fromARGB(255, 0, 191, 255),
                          BlendMode.srcIn),
                      child: SvgPicture.asset(
                        "images/wendu.svg",
                        width: multiple * 32,
                        height: multiple * 32,
                      ),
                    ),
                  ),
                  Positioned(
                    left: multiple * 313,
                    top: multiple * 156,
                    child: ColorFiltered(
                      colorFilter: ColorFilter.mode(
                          tcpState?.light ?? false
                              ? Colors.red
                              : const Color.fromARGB(255, 0, 191, 255),
                          BlendMode.srcIn),
                      child: SvgPicture.asset(
                        "images/guangzhao.svg",
                        width: multiple * 32,
                        height: multiple * 32,
                      ),
                    ),
                  ),
                  Positioned(
                    left: multiple * 271,
                    top: multiple * 174,
                    child: ColorFiltered(
                      colorFilter: ColorFilter.mode(
                          tcpState?.air ?? false
                              ? Colors.red
                              : const Color.fromARGB(255, 0, 191, 255),
                          BlendMode.srcIn),
                      child: SvgPicture.asset(
                        "images/qiti.svg",
                        width: multiple * 32,
                        height: multiple * 32,
                      ),
                    ),
                  ),
                ],
              ),
            ),
            Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
              SquareBox(
                //motor_pwm
                iconImagePath: "images/fans.svg",
                textImagePath: "images/fenshan.svg",
                textUnit: "images/zhuansu.svg",
                lable: "motor_pwm",
                send: _tcpManager?.send ?? (String message) async {},
                shounumb:
                    tcpState.jsonData?["pwm"]?["motor"]?.toString() ?? '-1',
                submit: _tcpManager?.submit ?? (bool value) {},
              ),
              SquareBox(
                //led_pwm
                iconImagePath: "images/light.svg",
                textImagePath: "images/led.svg",
                textUnit: "images/textlianjie.svg",
                lable: "led_pwm",
                send: _tcpManager?.send ?? (String message) async {},
                shounumb: tcpState.jsonData?["pwm"]?["led"]?.toString() ?? '-1',
                submit: _tcpManager?.submit ?? (bool value) {},
              ),
            ]),
            Text(tcpState.error ?? "",
                style: const TextStyle(color: Colors.red))
          ]);
        })),
      ),
    );
  }
}
