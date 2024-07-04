#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QPaintEvent>
#include <QRandomGenerator>
#include <QMouseEvent>
#include <QMap>
#include <QtGlobal>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMetaObject>
#include <QCloseEvent>
#include <QLineEdit>
#include <QMessageBox>
#include <QObject>
#include <QMenuBar>
#include <QMenu>
#include <QUrl>
#include <RulesWindow.h>




const int ROW_COUNT = 15;
const int COLUMN_COUNT = 15;
int BOOM_COUNT = 20;//炸弹的个数

const int BLOCK_WIDTH = 32;
const int BLOCK_HEIGHT = 32;

const int WINDOW_WIDTH = ROW_COUNT * BLOCK_WIDTH;
const int WINDOW_HEIGHT = COLUMN_COUNT * BLOCK_HEIGHT;

bool Restart_sit = false;



enum BLOCK {
    ZERO, ONE, TWO, THREE, FOUR,
    FIVE, SIX, SEVEN, EIGHT,
    BOOM,FLAG, UNKNOWN
};//BOOM,FLAG,UNKNOWN对应值是9,8,10





//定义一个QMap对象，它将枚举类型BLOCK的值映射到对应的QPixmap对象。
QMap<BLOCK, QPixmap> map;


//内联函数，用的逻辑表达式来判断b是否等于ONE到EIGHT之间的任何一个枚举值。如果条件为真，函数返回true，否则返回false。
inline bool isNum(BLOCK b) {
    return b == ONE || b == TWO || b == THREE ||
           b == FOUR || b == FIVE || b == SIX || b == SEVEN ||
           b == EIGHT;
}


class Window : public QWidget {
public:

    //存储每个格子实际的内容,用的block这枚举类建立的，所以每个格子都有blok数据
    BLOCK data[ROW_COUNT][COLUMN_COUNT];
    //代表了玩家当前看到的游戏区域
    BLOCK view[ROW_COUNT][COLUMN_COUNT];

    //重启游戏和关闭游戏
    QPushButton *btnRestart, *btnClose;


    //分别设置标志位:游戏是否结束,玩家是否赢得了游戏,控制是否显示一个矩形区域
    bool gameOver = false;
    bool youWin = false;
    bool showRect = false;
    //存储玩家点击的格子的坐标
    int rx = -1, ry = -1;


    //Window的构造函数
    Window() {
        //菜单的建立
        QMenuBar *menuBar = new QMenuBar(this);
        QMenu *fileMenu = menuBar->addMenu("文件");
        fileMenu->addAction("退出", this, &Window::close);

        //setMenuBar(menuBar);


        // 按钮创建
        btnRestart = new QPushButton("重新开始", this);
        btnClose = new QPushButton("关闭游戏", this);

        // 设置按钮的大小
        btnRestart->setFixedSize(150, 50);
        btnClose->setFixedSize(150, 50);

        // 设置按钮的位置
        int btnX = (WINDOW_WIDTH - BLOCK_WIDTH * 2) /2; // 水平居中
        int btnY = WINDOW_HEIGHT - BLOCK_HEIGHT * 2; // 在游戏窗口底部

        btnRestart->move(btnX - 150, btnY);
        btnClose->move(btnX + 75, btnY);

        // 连接按钮的点击信号到对应的槽函数
        connect(btnRestart, &QPushButton::clicked, this, &Window::restartGame);
        connect(btnClose, &QPushButton::clicked, this, &Window::closeGame);




        //设置窗口参数
        setWindowTitle("扫雷");
        setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        setFont(QFont("微软雅黑", 20));


        // 初始化,把格子都初始化为0和表示玩家未查看过
        for (int i = 0; i < ROW_COUNT; ++i)
            for (int j = 0; j < COLUMN_COUNT; ++j)
                data[i][j] = ZERO, view[i][j] = UNKNOWN;


        // 生成炸弹并放置
        for (int i = 0; i < BOOM_COUNT; ++i) {
            int v = QRandomGenerator::global()->generate() % (ROW_COUNT * COLUMN_COUNT);//除以格子总数确保它在游戏板的有效范围内
            int x = v / ROW_COUNT, y = v % ROW_COUNT;//找到其对应的位置
            data[x][y] = BOOM;
        }

        // 统计格子周围炸弹数量
        for (int i = 0; i < ROW_COUNT; ++i)
            for (int j = 0; j < COLUMN_COUNT; ++j) {
                if (data[i][j] == BOOM) continue;


                int sum = 0;
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        if (i + dx >= 0 && i + dx < ROW_COUNT && j + dy >= 0 && j + dy < COLUMN_COUNT//确保是在界面上的
                            && data[i + dx][j + dy] == BOOM)//并且是有炸弹的格子
                            ++sum;
      }
                }

                switch (sum) {
                case 0: data[i][j] = ZERO; break;
                case 1: data[i][j] = ONE; break;
                case 2: data[i][j] = TWO; break;
                case 3: data[i][j] = THREE; break;
                case 4: data[i][j] = FOUR; break;
                case 5: data[i][j] = FIVE; break;
                case 6: data[i][j] = SIX; break;
                case 7: data[i][j] = SEVEN; break;
                case 8: data[i][j] = EIGHT; break;
                }
            }
    };


    //炸开空的格子
    void dfs(int i, int j) {
        if (gameOver || youWin) return;
        if (view[i][j] != UNKNOWN) return;

        //把传进来的的格子位置的格子属性传给对应的view，其实就是设置其要显示的值
        view[i][j] = data[i][j];

        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;//不判断自己本身的格子

                int nx = i + dx, ny = j + dy;//把减少/增加的赋给真实的值
                if (nx >= 0 && nx < ROW_COUNT && ny >= 0 && ny < COLUMN_COUNT)/*保障在格子里*/{

                    if (data[nx][ny] == ZERO)
                        dfs(nx, ny);//如果当前格子是空白，则递归调用dfs函数来展开周围的空白区域。
                    else if (isNum(data[nx][ny])) view[nx][ny] = data[nx][ny];//如果当前格子是一个数字，则直接将其显示状态设置为数字，不再递归。
                }
            }
        }
    }

    //绘制游戏窗口
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event)

        //创建了一个 QPainter 对象，并将其关联到游戏窗口。
        QPainter painter(this);

        //绘制格子
        for (int i = 0; i < ROW_COUNT; ++i)
            for (int j = 0; j < COLUMN_COUNT; ++j)
                painter.drawPixmap(BLOCK_WIDTH * i, BLOCK_HEIGHT * j,/*让这个painter实例绘制图片，设置要绘制的图片的坐标xy*/
                                   gameOver ? map[data[i][j]] : map[view[i][j]]);//根据游戏是否结束（gameOver）和玩家是否查看过该格子来决定要绘制的是真实状态(就是原本data存放的图片)还是view存放的

        //鼠标中键辅助显示
        if (showRect) {
            QPen pen;
            pen.setWidth(5);
            pen.setColor(Qt::red);
            painter.setPen(pen);
            painter.drawRect((rx - 1) * BLOCK_WIDTH, (ry - 1) * BLOCK_HEIGHT, BLOCK_WIDTH * 3, BLOCK_HEIGHT * 3);
        }

        // 游戏胜利提示
        if (youWin && Restart_sit==false) {
            painter.fillRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, QColor(255, 0, 0, 100));
            painter.drawText(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Qt::AlignCenter, "You Win!");//填充整个窗口为红色并显示“You Win!”
        }

        // 游戏结束提示
        if (gameOver && Restart_sit==false) {
            painter.fillRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, QColor(255, 0, 0, 100));
            painter.drawText(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Qt::AlignCenter, "Game Over!");
        }

        // 显示重玩和关闭游戏按键
        if ((gameOver || youWin) && Restart_sit==false) {
            btnRestart->show();
            btnClose->show();
        } else {
            btnRestart->hide();
            btnClose->hide();
        }

    }

    //成功判断函数（不用写失败的函数是因为只要点到雷就直接结束，不用看所有的格子）
    bool Win() {
        for (int i = 0; i < ROW_COUNT; ++i)
            for (int j = 0; j < COLUMN_COUNT; ++j)
                if ((view[i][j] == UNKNOWN || view[i][j] == FLAG) && data[i][j] != BOOM) return false;//这个会一直遍历所有的格子
        return true;//如果循环遍历完整个游戏板后，没有找到任何未揭开且不是炸弹的格子,就返回true
    }

    //鼠标点击事件
    void mousePressEvent(QMouseEvent *event) override {
        if (gameOver || youWin) return;//如果游戏结束就不进行了

        //获取点击位置
        int x = event->pos().x() / BLOCK_WIDTH;//event->pos() 返回鼠标点击的屏幕坐标
        int y = event->pos().y() / BLOCK_HEIGHT;

        //左键
        if (event->button() == Qt::LeftButton && view[x][y] == UNKNOWN)
        {

            //检查当前格子是否为炸弹
            if (data[x][y] == BOOM) {
                gameOver = true;

                repaint();//重新绘制游戏界面
            }

            //递归展开当前格子周围的空白区域，并检查玩家是否赢得了游戏
            dfs(x, y);
            youWin = Win();
            repaint();//重新绘制游戏界面
        }
        //右键
        else if (event->button() == Qt::RightButton && view[x][y] == UNKNOWN)
        {
            view[x][y] = view[x][y] == FLAG ? UNKNOWN : FLAG;
            repaint();
        }
        //中键
        else if (event->button() == Qt::MiddleButton)
        {
            showRect = true;
            rx = x, ry = y;
            repaint();
        }

    }

    //负责处理玩家在游戏窗口上释放鼠标中键时的操作
    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::MiddleButton) {
            showRect = false;
            repaint();
        }
    }


    //重启游戏函数
    void restartGame() {

        // 重置标志位
        gameOver = false;
        youWin = false;
        Restart_sit = false;


        // 重置格子状态
        for (int i = 0; i < ROW_COUNT; ++i)
            for (int j = 0; j < COLUMN_COUNT; ++j)
                data[i][j] = ZERO, view[i][j] = UNKNOWN;


        // 重新生成炸弹并放置
        for (int i = 0; i < BOOM_COUNT; ++i) {
            int v = QRandomGenerator::global()->generate() % (ROW_COUNT * COLUMN_COUNT);//除以格子总数确保它在游戏板的有效范围内
            int x = v / ROW_COUNT, y = v % ROW_COUNT;//找到其对应的位置
            data[x][y] = BOOM;
        }

        // 统计格子周围炸弹数量
        for (int i = 0; i < ROW_COUNT; ++i)
            for (int j = 0; j < COLUMN_COUNT; ++j) {
                if (data[i][j] == BOOM) continue;


                int sum = 0;
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        if (i + dx >= 0 && i + dx < ROW_COUNT && j + dy >= 0 && j + dy < COLUMN_COUNT//确保是在界面上的
                            && data[i + dx][j + dy] == BOOM)//并且是有炸弹的格子
                            ++sum;
                    }
                }

                switch (sum) {
                case 0: data[i][j] = ZERO; break;
                case 1: data[i][j] = ONE; break;
                case 2: data[i][j] = TWO; break;
                case 3: data[i][j] = THREE; break;
                case 4: data[i][j] = FOUR; break;
                case 5: data[i][j] = FIVE; break;
                case 6: data[i][j] = SIX; break;
                case 7: data[i][j] = SEVEN; break;
                case 8: data[i][j] = EIGHT; break;
                }
            }


        // 重绘界面
        repaint();
    }


    // 关闭游戏窗口
    void closeGame() {
        // 关闭游戏窗口
        Restart_sit = true;
        close();

    }


};






void set_map_block(QPixmap *tiles)
{
    //用tiles->copy()把对应的图片复制出来，插入到map中，与相应的BLOCK枚举值关联起来。
    map.insert(ZERO, tiles->copy(0, 0, 32, 32));
    map.insert(ONE, tiles->copy(BLOCK_WIDTH * 1, 0, BLOCK_WIDTH, BLOCK_HEIGHT));
    map.insert(TWO, tiles->copy(BLOCK_WIDTH * 2, 0, BLOCK_WIDTH, BLOCK_HEIGHT));
    map.insert(THREE, tiles->copy(BLOCK_WIDTH * 3, 0, BLOCK_WIDTH, BLOCK_HEIGHT));
    map.insert(FOUR, tiles->copy(BLOCK_WIDTH * 4, 0, BLOCK_WIDTH, BLOCK_HEIGHT));
    map.insert(FIVE, tiles->copy(BLOCK_WIDTH * 5, 0, BLOCK_WIDTH, BLOCK_HEIGHT));
    map.insert(SIX, tiles->copy(BLOCK_WIDTH * 6, 0, BLOCK_WIDTH, BLOCK_HEIGHT));
    map.insert(SEVEN, tiles->copy(BLOCK_WIDTH * 7, 0, BLOCK_WIDTH, BLOCK_HEIGHT));
    map.insert(EIGHT, tiles->copy(BLOCK_WIDTH * 8, 0, BLOCK_WIDTH, BLOCK_HEIGHT));
    map.insert(BOOM, tiles->copy(BLOCK_WIDTH * 9, 0, BLOCK_WIDTH, BLOCK_HEIGHT));
    map.insert(UNKNOWN, tiles->copy(BLOCK_WIDTH * 10, 0, BLOCK_WIDTH, BLOCK_HEIGHT));
    map.insert(FLAG, tiles->copy(BLOCK_WIDTH * 11, 0, BLOCK_WIDTH, BLOCK_HEIGHT));
}


//主函数
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    //说明窗口的建立
    RulesWindow rulesWindow;
    rulesWindow.showRules();



    //获得一个包含所有要用的图片的值QPixmap指针
    auto *tiles = new QPixmap(":/src/tiles.jpg");

    //用tiles->copy()把对应的图片复制出来，插入到map中，与相应的BLOCK枚举值关联起来。
    set_map_block(tiles);

    //创建了一个Window类的实例
    Window window;
    //调用了window对象的show方法，使主窗口及其子控件在屏幕上可见
    window.show();



    //开始Qt的事件循环
    return a.exec();
}
