import 'package:flutter/material.dart';
import 'package:flutter_svg/flutter_svg.dart';

double multiple = 1.0;

class VerticalBox extends StatefulWidget {
  final String iconImagePath;
  final String textImagePath;
  final String textUnit;
  final String number;
  const VerticalBox({
    super.key,
    required this.iconImagePath,
    required this.textImagePath,
    required this.textUnit,
    required this.number,
  });

  @override
  State<VerticalBox> createState() => _VerticalBoxState();
}

class _VerticalBoxState extends State<VerticalBox> {
  @override
  Widget build(BuildContext context) {
    multiple = MediaQuery.of(context).size.width / 360;
    return Container(
      width: multiple * 56,
      height: multiple * 164,
      decoration: BoxDecoration(
        gradient: const LinearGradient(
          colors: [
            Color.fromARGB(255, 172, 224, 249),
            Color.fromARGB(255, 255, 241, 235),
          ],
          begin: Alignment.topCenter,
          end: Alignment.bottomCenter,
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
          SizedBox(height: multiple * 8),
          SvgPicture.asset(
            widget.iconImagePath,
            width: multiple * 32,
            height: multiple * 32,
          ),
          SizedBox(height: multiple * 10),
          SvgPicture.asset(
            widget.textImagePath,
            width: multiple * 40,
            height: multiple * 53,
          ),
          // 在容器内显示可变文本
          SizedBox(height: multiple * 17),
          Text(
            widget.number + widget.textUnit, // 显示的文本
            style: TextStyle(
              color: Colors.black, // 文本颜色
              fontSize: multiple * 20, // 文本大小
            ),
          ),
        ],
      ),
    );
  }
}
