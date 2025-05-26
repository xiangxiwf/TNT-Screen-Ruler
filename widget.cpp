#include "widget.h"
#include <QWidget>
#include <QToolButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <qfont.h>
#include <qicon.h>
#include <qnamespace.h>
#include <qwindowdefs.h>
#include <winsock.h>
#include <QLabel>
#include <QParallelAnimationGroup>
#include <QWindowStateChangeEvent>
#include <QPushButton>
#include <QDesktopServices>
#define WINDOW_CLASS_NAME L"RedLinesWindow"

LRESULT CALLBACK Widget::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        case WM_DESTROY:
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    initConnections();
}

Widget::~Widget() {
    if (m_linesWindow) {
        DestroyWindow(m_linesWindow);
    }
}

void Widget::initUI()
{
    initWidgets();
    initWidgetProperties();
    initAppBar();
    initLayout();
}

void Widget::initAppBar()
{
    // 首先设置Qt窗口标志
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    
#ifdef Q_OS_WIN
    // 确保窗口已创建
    create();
    HWND hwnd = (HWND)winId();

    // 启用窗口阴影所需的DWM特性
    DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));
    
    // 设置扩展边框以保持阴影效果
    MARGINS margins = {1, 1, 1, 1};  // 四周都留1像素用于显示阴影
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    
    // 移除标准窗口样式，但保留必要的窗口功能
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    style &= ~WS_CAPTION;
    style |= WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
    SetWindowLongPtr(hwnd, GWL_STYLE, style);
    
    // 强制更新窗口样式
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    
    // 启用DWM作曲效果（动画）
    BOOL enabled = false;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &enabled, sizeof(enabled));
#endif

    // 设置一个默认的最小尺寸
    setMinimumSize(200, 90);
}

void Widget::initWidgets()
{
    appBar = new QWidget(this);
    gitHubButton = new QToolButton(appBar);
    closeButton = new QToolButton(appBar);
    minimizeButton = new QToolButton(appBar);
    pinButton = new QToolButton(appBar);
    titleLabel = new QLabel(" 屏距尺", appBar);

    pbtn_drawRedLines = new QPushButton("显示屏距尺", this);
    tbtn_addRedLine = new QToolButton(this);
    tbtn_reduceRedLine = new QToolButton(this);
    tbtn_reCountRedLine = new QToolButton(this);
}

void Widget::initWidgetProperties()
{
    resize(1,1);
    this->setWindowIcon(QIcon(":/icons/icons/favicon.ico"));
    appBar->installEventFilter(this);
    appBar->setAutoFillBackground(true); // 启用自动填充背景
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(243, 243, 243));
    appBar->setPalette(palette);
    appBar->setFixedSize(width(), 30); // 设置应用栏的高度
    appBar->setContentsMargins(0, 0, 0, 0); // 设置应用栏的边距为0

    QFont titleFont= titleLabel->font();
    titleFont.setPointSize(13); // 设置标题字体大小
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter); // 设置标题文本右对齐并垂直居中

    gitHubButton->setFixedSize(30, 30);
    gitHubButton->setToolButtonStyle(Qt::ToolButtonIconOnly); // 只显示图标
    gitHubButton->setIcon(QIcon(":/icons/icons/icons8-github-logo-24.png"));
    gitHubButton->setIconSize(QSize(24, 24)); // 图标实际显示尺寸
    gitHubButton->setStyleSheet(
    "QToolButton {"
    "  border-radius: 0px;"
    "  background-color: rgb(243, 243, 243);"
    "}");

    closeButton->setFixedSize(30, 30);
    closeButton->setStyleSheet(
    "QToolButton {"
    "  border-radius: 0px;"
    "  background-color: rgb(243, 243, 243);"
    "}"
    "QToolButton:hover {"
    "  background-color: rgb(196, 43, 28);"
    "}"
    "QToolButton:pressed {"
    "  background-color: rgb(196, 62, 48);"
    "}"
);

    closeButton->setToolButtonStyle(Qt::ToolButtonIconOnly); // 只显示图标
    closeButton->setIcon(QIcon(":/icons/icons/close_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"));
    closeButton->setIconSize(QSize(24, 24)); // 图标实际显示尺寸

    minimizeButton->setFixedSize(30, 30);
    minimizeButton->setStyleSheet(
    "QToolButton {"
    "  border-radius: 0px;"
    "  background-color: rgb(243, 243, 243);"
    "}"
    "QToolButton:hover {"
    "  background-color: rgb(234, 234, 234);"
    "}"
);
    minimizeButton->setToolButtonStyle(Qt::ToolButtonIconOnly); // 只显示图标
    minimizeButton->setIcon(QIcon(":/icons/icons/remove_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"));
    minimizeButton->setIconSize(QSize(24, 24)); // 图标实际显示尺寸

    pinButton->setFixedSize(30, 30);
    pinButton->setCheckable(true); // 设置为可选中状态
    pinButton->setStyleSheet(
    "QToolButton {"
    "  border-radius: 0px;"
    "  background-color: rgb(243, 243, 243);"
    "}"
    "QToolButton:hover {"
    "  background-color: rgb(234, 234, 234);"
    "}"
);
    pinButton->setToolButtonStyle(Qt::ToolButtonIconOnly); // 只显示图标
    pinButton->setIcon(QIcon(":/icons/icons/keep_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"));
    pinButton->setIconSize(QSize(24, 24)); // 图标实际显示尺寸

    pbtn_drawRedLines->setFont(QFont("微软雅黑",11)); // 设置按钮字体
    pbtn_drawRedLines->setCheckable(true); // 设置为可选中状态

    tbtn_addRedLine->setIcon(QIcon(":/icons/icons/zoom_out_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"));
    tbtn_addRedLine->setToolButtonStyle(Qt::ToolButtonIconOnly); // 只显示图标
    tbtn_addRedLine->setIconSize(QSize(24, 24)); // 图标实际显示尺寸

    tbtn_reduceRedLine->setIcon(QIcon(":/icons/icons/zoom_in_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"));
    tbtn_reduceRedLine->setToolButtonStyle(Qt::ToolButtonIconOnly); // 只显示图标
    tbtn_reduceRedLine->setIconSize(QSize(24, 24)); // 图标实际显示尺寸

    tbtn_reCountRedLine->setIcon(QIcon(":/icons/icons/fit_screen_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"));
    tbtn_reCountRedLine->setToolButtonStyle(Qt::ToolButtonIconOnly); // 只显示图标
    tbtn_reCountRedLine->setIconSize(QSize(24, 24)); // 图标实际显示尺寸

    tbtn_addRedLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    tbtn_reduceRedLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    tbtn_reCountRedLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

}

void Widget::initConnections()
{
    connect(closeButton, &QToolButton::clicked, this, &Widget::do_close);
    connect(minimizeButton, &QToolButton::clicked, this, &Widget::do_minimize);
    connect(pinButton, &QToolButton::clicked, this, &Widget::do_keepTop);
    connect(pbtn_drawRedLines, &QPushButton::clicked, this, &Widget::do_drawRedLines);
    connect(tbtn_addRedLine, &QToolButton::clicked, this, &Widget::do_addRedLine);
    connect(tbtn_reduceRedLine, &QToolButton::clicked, this, &Widget::do_reduceRedLine);
    connect(tbtn_reCountRedLine, &QToolButton::clicked, this, &Widget::do_reCountRedLine);
    connect(gitHubButton, &QToolButton::clicked, this, &Widget::do_openGitHub);
}

void Widget::initLayout()
{
    appBarLayout = new QHBoxLayout(appBar);
    appBarLayout->setContentsMargins(0, 0, 0, 0); // 设置应用栏布局的边距为0
    appBarLayout->setSpacing(0); // 设置应用栏布局的间距为0
    appBarLayout->addWidget(titleLabel);
    appBarLayout->addWidget(gitHubButton);
    appBarLayout->addStretch(); // 添加伸缩空间以使按钮靠右对齐
    appBarLayout->addWidget(pinButton);
    appBarLayout->addWidget(minimizeButton);
    appBarLayout->addWidget(closeButton);
    mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 设置主布局的边距为0
    mainLayout->setSpacing(0); // 设置主布局的间距为0
    mainLayout->setAlignment(Qt::AlignTop|Qt::AlignHCenter); // 设置主布局的对齐方式为左上角对齐
    mainLayout->addWidget(appBar, 0, 0, 1, 3); // 将应用栏添加到主布局的第一行
    mainLayout->addWidget(pbtn_drawRedLines, 1, 0, 1, 3); // 将按钮添加到主布局的第二行
    mainLayout->addWidget(tbtn_reduceRedLine, 2, 0,1,1); // 将增加红线按钮添加到主布局的第三行第一列
    mainLayout->addWidget(tbtn_addRedLine, 2, 1,1,1); // 将减少红线按钮添加到主布局的第三行第二列
    mainLayout->addWidget(tbtn_reCountRedLine, 2, 2,1,1); // 将重新计算红线按钮添加到主布局的第三行第三列
    setLayout(mainLayout);

}

void Widget::do_openGitHub()
{
    QDesktopServices::openUrl(QUrl("https://github.com/xiangxiwf/TNT-Screen-Ruler", QUrl::TolerantMode));
}

void Widget::do_keepTop(bool checked)
{
#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(this->winId());
    if (checked) {
        // 设置窗口为置顶
        pinButton->setStyleSheet(
    "QToolButton {"
    "  border-radius: 0px;"
    "  background-color: rgb(234, 234, 234);"
    "}"
);
        pinButton->setIcon(QIcon(":/icons/icons/keep_off_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"));
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    } else {
        pinButton->setStyleSheet(
    "QToolButton {"
    "  border-radius: 0px;"
    "  background-color: rgb(243, 243, 243);"
    "}"
);
        pinButton->setIcon(QIcon(":/icons/icons/keep_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"));
        // 取消置顶
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    
#endif
}


void Widget::do_minimize()
{
    // 创建几何动画：窗口缩小到中心
    QRect startGeometry = geometry();
    QRect endGeometry = startGeometry;
    endGeometry.setSize(startGeometry.size() * 1.0);
    endGeometry.moveCenter(startGeometry.center());

    QPropertyAnimation* geometryAnimation = new QPropertyAnimation(this, "geometry", this);
    geometryAnimation->setDuration(100);
    geometryAnimation->setStartValue(startGeometry);
    geometryAnimation->setEndValue(endGeometry);
    geometryAnimation->setEasingCurve(QEasingCurve::InCubic);

    // 创建透明度动画
    QPropertyAnimation* opacityAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    opacityAnimation->setDuration(100);
    opacityAnimation->setStartValue(1.0);
    opacityAnimation->setEndValue(0.0);
    opacityAnimation->setEasingCurve(QEasingCurve::InCubic);

    // 动画组：两个动画同时进行
    QParallelAnimationGroup* group = new QParallelAnimationGroup(this);
    group->addAnimation(geometryAnimation);
    group->addAnimation(opacityAnimation);

    connect(group, &QParallelAnimationGroup::finished, this, [this]() {
        showMinimized();
        setWindowOpacity(1.0);  // 恢复透明度，以便恢复后是正常的
    });

    group->start(QAbstractAnimation::DeleteWhenStopped);
}


void Widget::do_close()
{
    // 创建几何动画：窗口缩小到中心
    QRect startGeometry = geometry();
    QRect endGeometry = startGeometry;
    endGeometry.setSize(startGeometry.size() * 1);
    endGeometry.moveCenter(startGeometry.center());

    QPropertyAnimation* geometryAnimation = new QPropertyAnimation(this, "geometry", this);
    geometryAnimation->setDuration(100);
    geometryAnimation->setStartValue(startGeometry);
    geometryAnimation->setEndValue(endGeometry);
    geometryAnimation->setEasingCurve(QEasingCurve::InCubic);

    // 创建透明度动画
    QPropertyAnimation* opacityAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    opacityAnimation->setDuration(100);
    opacityAnimation->setStartValue(1.0);
    opacityAnimation->setEndValue(0.0);
    opacityAnimation->setEasingCurve(QEasingCurve::InCubic);

    // 动画组：两个动画同时进行
    QParallelAnimationGroup* group = new QParallelAnimationGroup(this);
    group->addAnimation(geometryAnimation);
    group->addAnimation(opacityAnimation);

    connect(group, &QParallelAnimationGroup::finished, this, [this]() {
        this->close();  // 动画结束后再关闭窗口
    });

    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void Widget::do_drawRedLines(bool checked)
{
    if (checked) {
        createLinesWindow();
    } else {
        if (m_linesWindow) {
            DestroyWindow(m_linesWindow);
            m_linesWindow = nullptr;
        }
    }
}

void Widget::do_addRedLine()
{
    if(!pbtn_drawRedLines->isChecked())
    {
        return;
    }
    if(redLines==8){
        redLines=9;
    }else if(redLines==9){
        redLines=10;
    }else if(redLines==10){
        redLines=11;
    }else if(redLines==11){
        redLines=14;
    }else if(redLines==14){
        redLines=17;
    }
    else if(redLines==17){
        redLines=19;
    }else if(redLines==19){
        return;
    }
    if(m_linesWindow) {
        DestroyWindow(m_linesWindow);
        m_linesWindow = nullptr;
    }
    createLinesWindow();
}

void Widget::do_reduceRedLine()
{
    if(!pbtn_drawRedLines->isChecked())
    {
        return;
    }
    if(redLines==19){
        redLines=17;
    }else if(redLines==17){
        redLines=14;
    }else if(redLines==14){
        redLines=11;
    }else if(redLines==11){
        redLines=10;
    }else if(redLines==10){
        redLines=9;
    }else if (redLines==9) {
        redLines=8;
    }else if(redLines==8){
        return;
    }
    if(m_linesWindow) {
        DestroyWindow(m_linesWindow);
        m_linesWindow = nullptr;
    }
    createLinesWindow();
}

void Widget::do_reCountRedLine()
{
    if(!pbtn_drawRedLines->isChecked())
    {
        return;
    }
    if(m_linesWindow&&redLines==11) {
        // 如果窗口已经存在且红线数量为11，则不需要重新创建
        return;
    }
    if (m_linesWindow) {
        DestroyWindow(m_linesWindow);
        m_linesWindow = nullptr;
    }
    redLines = 11; // 重置红线数量为默认值
    createLinesWindow();
}


void Widget::playShowAnimation()
{
    // 获取窗口当前几何信息
    QRect finalGeometry = geometry();
    
    // 计算起始几何信息（从中心点开始，初始大小为目标大小的40%）
    QRect startGeometry = finalGeometry;
    startGeometry.setSize(finalGeometry.size() * 0.6);
    startGeometry.moveCenter(finalGeometry.center());
    
    // 创建几何动画
    QPropertyAnimation* geometryAnimation = new QPropertyAnimation(this, "geometry", this);
    geometryAnimation->setDuration(100);  // 动画持续100毫秒
    geometryAnimation->setStartValue(startGeometry);
    geometryAnimation->setEndValue(finalGeometry);
    geometryAnimation->setEasingCurve(QEasingCurve::OutCubic);  // 使用三次方缓出曲线使动画更丝滑
    
    // 创建透明度动画
    QPropertyAnimation* opacityAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    opacityAnimation->setDuration(350);
    opacityAnimation->setStartValue(0.0);
    opacityAnimation->setEndValue(1.0);
    opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // 显示窗口并开始动画
    setWindowOpacity(0.0);
    show();
    setGeometry(startGeometry);
    
    // 同时启动两个动画
    geometryAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void Widget::createLinesWindow()
{
    // 注册窗口类
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = Widget::WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassExW(&wc);

    // 获取屏幕尺寸
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // 创建窗口
    m_linesWindow = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
        WINDOW_CLASS_NAME,
        L"Red Lines",
        WS_POPUP,
        0, 0, screenWidth, screenHeight,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    // 存储this指针以便在窗口过程中访问
    SetWindowLongPtr(m_linesWindow, GWLP_USERDATA, (LONG_PTR)this);

    // 创建内存DC
    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);
    HBITMAP memBitmap = CreateCompatibleBitmap(screenDC, screenWidth, screenHeight);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

    // 创建并设置白色画刷填充背景为白色
    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    RECT rect = {0, 0, screenWidth, screenHeight};
    FillRect(memDC, &rect, whiteBrush);
    DeleteObject(whiteBrush);

    // 创建红色画笔
    HPEN redPen = CreatePen(PS_SOLID, lineWidth, RGB(255, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(memDC, redPen);

    // 设置文本属性
    SetTextColor(memDC, RGB(255, 0, 0));
    SetBkMode(memDC, TRANSPARENT);
    HFONT font = CreateFont(40, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                          DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                          CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    HFONT oldFont = (HFONT)SelectObject(memDC, font);

    int spacing = screenWidth / (redLines + 1);

    // 使用纯色红色绘制
    for (int i = 0; i < redLines; i++) {
        int x = spacing * (i + 1);
        
        // 绘制竖线
        MoveToEx(memDC, x, 0, NULL);
        LineTo(memDC, x, screenHeight);
        
        // 绘制数字
        wchar_t number[3];
        _itow_s(i + 1, number, 10);
        
        // 计算文本尺寸以便居中显示
        SIZE textSize;
        GetTextExtentPoint32W(memDC, number, wcslen(number), &textSize);
        int textX = x - (textSize.cx / 2);
        int textY = (screenHeight / 2) - (textSize.cy / 2);
        
        // 创建文本背景矩形
        RECT textRect = {textX - 5, textY - 5, 
                        textX + textSize.cx + 5, textY + textSize.cy + 5};
        FillRect(memDC, &textRect, whiteBrush);
        
        TextOutW(memDC, textX, textY, number, wcslen(number));
    }

    // 使用完整不透明的方式更新窗口
    POINT ptSrc = {0, 0};
    SIZE size = {screenWidth, screenHeight};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, 0};  // 完全不透明
    UpdateLayeredWindow(m_linesWindow, NULL, NULL, &size, memDC, &ptSrc, RGB(255, 255, 255), &blend, ULW_COLORKEY);

    // 清理资源
    SelectObject(memDC, oldFont);
    DeleteObject(font);
    SelectObject(memDC, oldPen);
    DeleteObject(redPen);
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
    ReleaseDC(NULL, screenDC);

    // 显示窗口
    ShowWindow(m_linesWindow, SW_SHOW);
    SetForegroundWindow(m_linesWindow);
}

void Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (appBar)
        appBar->setFixedWidth(width());
}

void Widget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateGeometry();
    update(); // 强制刷新窗口
    if (appBar)
        appBar->updateGeometry();
}

void Widget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent *stateEvent = static_cast<QWindowStateChangeEvent*>(event);
        // 判断是否从最小化恢复
        if ((stateEvent->oldState() & Qt::WindowMinimized) && !(windowState() & Qt::WindowMinimized)) {
            // 播放恢复动画
            playShowAnimation();
        }
    }
    QWidget::changeEvent(event);
}
bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == appBar) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                m_dragging = true;
                m_dragPosition = mouseEvent->globalPos() - this->frameGeometry().topLeft();
                return true; // 拦截事件
            }
        } else if (event->type() == QEvent::MouseMove && m_dragging) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            this->move(mouseEvent->globalPos() - m_dragPosition);
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease && m_dragging) {
            m_dragging = false;
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
