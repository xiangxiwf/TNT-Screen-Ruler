#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <qwidget.h>
#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif
class QWidget;
class QToolButton;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    int redLines=11; // 红线数量
    int lineWidth =3;
    HWND m_linesWindow;
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    bool m_dragging = false;
    QPoint m_dragPosition;

    QWidget *appBar;
    QToolButton *gitHubButton;
    QToolButton *closeButton;
    QToolButton *minimizeButton;
    QToolButton *pinButton;
    QLabel *titleLabel;

    QPushButton *pbtn_drawRedLines;
    QToolButton *tbtn_addRedLine;
    QToolButton *tbtn_reduceRedLine;
    QToolButton *tbtn_reCountRedLine;

    QGridLayout *mainLayout;
    QHBoxLayout *appBarLayout;

    void initUI();
    void initWidgets();
    void initWidgetProperties();
    void initAppBar();
    void initLayout();
    void initConnections();

    void createLinesWindow();
public:
    void playShowAnimation();
private slots:
    void do_openGitHub();
    void do_keepTop(bool checked);
    void do_minimize();
    void do_close();
    void do_drawRedLines(bool checked);
    void do_addRedLine();
    void do_reduceRedLine();
    void do_reCountRedLine();
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
};
#endif // WIDGET_H
