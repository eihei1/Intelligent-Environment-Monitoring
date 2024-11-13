import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_svg/flutter_svg.dart';

class SquareBox extends StatefulWidget {
  final String iconImagePath;
  final String textImagePath;
  final String textUnit;
  final String lable;
  final Future<void> Function(String message) send;
  final String shounumb;
  final void Function(bool value) submit;
  const SquareBox({
    super.key,
    required this.iconImagePath,
    required this.textImagePath,
    required this.textUnit,
    required this.lable,
    required this.send,
    required this.shounumb,
    required this.submit,
  });

  @override
  State<SquareBox> createState() => _SquareBoxState();
}

class _SquareBoxState extends State<SquareBox> {
  @override
  Widget build(BuildContext context) {
    double multiple = MediaQuery.of(context).size.width / 360;
    return Container(
      width: multiple * 90,
      height: multiple * 90,
      decoration: BoxDecoration(
        gradient: const LinearGradient(
          colors: [
            Color.fromARGB(255, 172, 224, 249),
            Color.fromARGB(255, 255, 241, 235),
          ],
          begin: Alignment.topLeft,
          end: Alignment.bottomRight,
        ),
        borderRadius: BorderRadius.circular(10.0),
        boxShadow: [
          BoxShadow(
            color: Colors.black.withOpacity(0.1),
            spreadRadius: 0,
            blurRadius: multiple * 8,
            offset: Offset(multiple * 2, multiple * 3),
          ),
          BoxShadow(
            color: Colors.black.withOpacity(0.09),
            spreadRadius: 0,
            blurRadius: multiple * 14,
            offset: Offset(multiple * 8, multiple * 12),
          ),
          BoxShadow(
            color: Colors.black.withOpacity(0.05),
            spreadRadius: 0,
            blurRadius: multiple * 19,
            offset: Offset(multiple * 19, multiple * 26),
          ),
          BoxShadow(
            color: Colors.black.withOpacity(0.01),
            spreadRadius: 0,
            blurRadius: multiple * 23,
            offset: Offset(multiple * 33, multiple * 46),
          )
        ],
      ),
      child: Column(
        mainAxisAlignment: MainAxisAlignment.start,
        children: [
          Row(mainAxisAlignment: MainAxisAlignment.start, children: [
            SvgPicture.asset(
              widget.iconImagePath,
              width: multiple * 32,
              height: multiple * 32,
            ),
            SizedBox(width: multiple * 8),
            SvgPicture.asset(
              widget.textImagePath,
              width: multiple * 40,
            ),
          ]),
          SizedBox(height: multiple * 8),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              // 在容器内显示可变文本
              SvgPicture.asset(
                widget.textUnit,
                width: multiple * 40,
              ),
              SizedBox(
                width: multiple * 30, // 设置宽度为200
                height: multiple * 24, // 设置高度为24
                child: TextField(
                  controller: TextEditingController(text: widget.shounumb),
                  keyboardType: TextInputType.number, // 设置键盘类型为数字
                  style: TextStyle(fontSize: multiple * 20), // 设置输入文字大小
                  maxLength: 2, // 设置最大长度为2
                  decoration: const InputDecoration(
                    counterText: "", // 隐藏计数器文字
                  ), // 隐藏默认的装饰
                  maxLengthEnforcement: MaxLengthEnforcement.enforced,
                  inputFormatters: [
                    // 设置输入格式化器
                    FilteringTextInputFormatter.digitsOnly, // 只允许输入数字
                    LengthLimitingTextInputFormatter(2), // 限制长度为2
                  ],
                  onSubmitted: (value) async {
                    widget.submit(true);
                    await widget
                        .send('${widget.lable}:$value'); // 调用传入的send函数，并等待其完成
                  },
                  onTap: () {
                    widget.submit(false); // 调用传入的submit函数，并传入true
                  }, // 点击事件为空
                ),
              )
            ], // 强制限制输入长度
          ),
        ],
      ),
    );
  }
}
