import 'package:flutter/material.dart';
import 'package:flutter_svg/flutter_svg.dart';

class LinkBox extends StatefulWidget {
  final bool link;
  const LinkBox({
    super.key,
    required this.link,
  });

  @override
  State<LinkBox> createState() => _LinkBoxState();
}

class _LinkBoxState extends State<LinkBox> with SingleTickerProviderStateMixin {
  @override
  Widget build(BuildContext context) {
    double multiple = MediaQuery.of(context).size.width / 360;
    return SizedBox(
      width: multiple * 72,
      height: multiple * 65,
      child: Column(
        mainAxisAlignment: MainAxisAlignment.start,
        children: [
          SvgPicture.asset(
            "images/textlianjie.svg",
            width: multiple * 40,
          ),
          ColorFiltered(
            colorFilter: ColorFilter.mode(
                widget.link
                    ? const Color.fromARGB(255, 0, 191, 255)
                    : const Color.fromARGB(255, 115, 115, 115),
                BlendMode.srcIn),
            child: SvgPicture.asset(
              "images/lianjie.svg",
              width: multiple * 72,
            ),
          )
        ],
      ),
    );
  }
}
