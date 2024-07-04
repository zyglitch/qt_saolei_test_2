#include <RulesWindow.h>

//规程说明窗口构造函数
RulesWindow::RulesWindow(QWidget *parent) : QWidget(parent) {
    layout = new QVBoxLayout(this);
    setWindowTitle("游戏规则说明"); // 设置窗口标题
    rulesLabel = new QLabel("左键是点击格子\n"
                            "右键是给这个格子插上旗帜\n"
                            "中键是显示这个格子的周围覆盖的区域\n"
                            "\n"
                            "胜利条件是把所有的雷标记并把不是雷的格子点亮", this);
    closeButton = new QPushButton("关闭", this);

    layout->addWidget(rulesLabel);
    layout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, &RulesWindow::close);

}



void RulesWindow::showRules() {
    show();
}

































