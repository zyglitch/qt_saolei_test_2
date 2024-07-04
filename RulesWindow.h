#ifndef RULESWINDOW_H
#define RULESWINDOW_H

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




//规程说明窗口
class RulesWindow : public QWidget {

public:
    RulesWindow(QWidget *parent = nullptr);
    void showRules();

private:
    QVBoxLayout *layout;
    QLabel *rulesLabel;
    QPushButton *closeButton;
};




#endif // RULESWINDOW_H
