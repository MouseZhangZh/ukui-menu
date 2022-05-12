#include "function_Widget.h"
#include "style.h"
#include <QVBoxLayout>
#include <QProcess>
#include <QTimer>
#include <QLocale>
#include <qwindow.h>
#include <QDir>
#include <AbstractInterface.h>
#include <QPluginLoader>
#include "currenttime_interface.h"
#include "style.h"
#include <QtSvg/QSvgRenderer>
#define TIME_FORMAT "org.ukui.control-center.panel.plugins"
#define TIME_FORMAT_KEY "hoursystem"
#include <QDebug>
#include <QPalette>

FunctionWidget::FunctionWidget(QWidget *parent): QWidget(parent)
{
    //    if(QGSettings::isSchemaInstalled("org.ukui.style")){
    //        themeSetting=new QGSettings("org.ukui.style");
    //        themeName=themeSetting->get("style-name").toString();
    //    }
    //    connect(themeSetting,&QGSettings::changed,this,[=](){
    //         changeSearchBoxBackground();
    //    });
    usrInterface = new QDBusInterface("com.kylin.statusmanager.interface",
                                      "/",
                                      "com.kylin.statusmanager.interface",
                                      QDBusConnection::sessionBus());
    QDBusConnection::sessionBus().connect("com.kylin.statusmanager.interface",
                                          "/",
                                          "com.kylin.statusmanager.interface",
                                          "stylename_change_signal",
                                          this,
                                          SLOT(changeSearchBoxBackground(QString))
                                         );
    initUi();
    myTimer = new QTimer();
    myTimer->start(10000);
    connect(myTimer, &QTimer::timeout, [this]() {
        timeLabel->setText(Time->currentTime);
        weekLabel->setText(Time->currentWeek);
        dateLabel->setText(Time->currentDate);
    });

    if (QGSettings::isSchemaInstalled(QString("org.ukui.session").toLocal8Bit())) {
        timeSetting = new QGSettings(TIME_FORMAT);
        connect(timeSetting, &QGSettings::changed, this, [ = ](const QString & key) {
            timeLabel->setText(Time->currentTime);
            weekLabel->setText(Time->currentWeek);
            dateLabel->setText(Time->currentDate);
        });
    }
}

FunctionWidget::~FunctionWidget()
{
    if (themeSetting) {
        delete themeSetting;
    }

    if (myTimer) {
        delete myTimer;
    }

    if (upWidget) {
        delete upWidget;
    }

    if (upLayout) {
        delete upLayout;
    }

    if (leftUpWidget) {
        delete leftUpWidget;
    }

    if (leftUpLayout) {
        delete leftUpLayout;
    }

    if (upLeftWidget) {
        delete upLeftWidget;
    }

    if (upLeftLayout) {
        delete upLeftLayout;
    }

    if (upRightWidget) {
        delete upRightWidget;
    }

    if (upRightLayout) {
        delete upRightLayout;
    }

    if (downWidget) {
        delete downWidget;
    }

    if (downLayout) {
        delete downLayout;
    }

    if (timeLabel) {
        delete timeLabel;
    }

    if (weekLabel) {
        delete weekLabel;
    }

    if (dateLabel) {
        delete dateLabel;
    }

    if (searchEditBtn) {
        delete searchEditBtn;
    }

    if (focusPlug) {
        delete focusPlug;
    }

    if (effect) {
        delete effect;
    }

    themeSetting = nullptr;
    myTimer = nullptr;
    upWidget = nullptr;
    upLayout = nullptr;
    leftUpWidget = nullptr;
    leftUpLayout = nullptr;
    upRightWidget = nullptr;
    downWidget = nullptr;
    downLayout = nullptr;
    timeLabel = nullptr;
    weekLabel = nullptr;
    dateLabel = nullptr;
    searchEditBtn = nullptr;
    focusPlug = nullptr;
    effect = nullptr;
}

void FunctionWidget::initUi()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("border:0px solid #ff0000;background:transparent;");
    this->setFocusPolicy(Qt::NoFocus);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    this->setLayout(mainLayout);
    //时间+搜索框
    upWidget = new QWidget();
    upWidget->setFixedSize(400, 232);
    upLayout = new QVBoxLayout();
    upWidget->setLayout(upLayout);
    upLayout->setContentsMargins(0, 0, 0, 32);
    upLayout->setSpacing(0);
    //上
    leftUpWidget = new QWidget();
    leftUpWidget->setFixedSize(400, 96);
    leftUpLayout = new QHBoxLayout();
    leftUpWidget->setLayout(leftUpLayout);
    leftUpLayout->setContentsMargins(0, 0, 0, 0);
    leftUpLayout->setSpacing(0);
    //左上左
    upLeftWidget = new QWidget;
    upLeftWidget->setFixedSize(263, 96);
    upLeftLayout = new QHBoxLayout();
    upLeftWidget->setLayout(upLeftLayout);
    upLeftLayout->setContentsMargins(0, 0, 0, 0);
    //    upLeftWidget->setStyleSheet("border-width:1px;border-style:solid;border-color:red");
    //左上右
    upRightWidget = new QWidget;
    upRightWidget->setFixedSize(170, 96);
    upRightLayout = new QVBoxLayout();
    upRightWidget->setLayout(upRightLayout);
    upRightLayout->setContentsMargins(0, 0, 0, 0);
    upRightLayout->setSpacing(0);
    //    upRightWidget->setStyleSheet("border-width:1px;border-style:solid;border-color:red");
    //左下
    downWidget = new QWidget;
    downLayout = new QVBoxLayout();
    downWidget->setLayout(downLayout);
    downWidget->setFixedSize(400, 104);
    downLayout->setSpacing(0);
    downLayout->setContentsMargins(0, 24, 0, 0);
    //左侧控件
    timeLabel = new QLabel();
    weekLabel = new QLabel();
    dateLabel = new QLabel();
    //搜索
    searchEditBtn = new QPushButton();
    searchEditBtn->setFocusPolicy(Qt::NoFocus);
    searchEditBtn->setFixedSize(400, 80);
    searchEditBtn->setIcon(QIcon(":/data/img/mainviewwidget/ukui-search-blue.svg"));
    searchEditBtn->setIconSize(QPixmap(":/data/img/mainviewwidget/ukui-search-blue.svg").size());
    searchEditBtn->setText(tr("Search"));
    QDBusReply<QString> styleName = usrInterface->call(QString("get_current_stylename"));
    changeSearchBoxBackground(styleName);
    connect(searchEditBtn, &QPushButton::clicked, this, &FunctionWidget::obtainSearchResult);
    downLayout->addWidget(searchEditBtn);
    upLayout->addWidget(leftUpWidget);
    upLayout->addWidget(downWidget);
    mainLayout->addWidget(upWidget);
    focusPlug = new pluginwidget(this);
    //加入专注模式
    //    if(plugin)
    //    {
    mainLayout->addWidget(focusPlug);
    focusPlug->setFixedSize(400, 638);
    //    }
    mainLayout->addStretch();
    leftUpLayout->addWidget(upLeftWidget);
    leftUpLayout->addWidget(upRightWidget);
    Time = new CurrentTimeInterface;
    timeLabel->setText(Time->currentTime);
    weekLabel->setText(Time->currentWeek);
    dateLabel->setText(Time->currentDate);
    upLeftLayout->addWidget(timeLabel);
    upRightLayout->addWidget(weekLabel);
    upRightLayout->addWidget(dateLabel);
    upRightLayout->setContentsMargins(0, 0, 0, 0);
    upRightLayout->setSpacing(0);
    weekLabel->setContentsMargins(10, 8, 0, 0);
    dateLabel->setContentsMargins(10, 6, 0, 0);
    timeLabel->setStyleSheet("border:0px;background:transparent;font-size:96px;color:white;");
    dateLabel->setStyleSheet("border:0px;background:transparent;font-size:32px;color:white;");
    weekLabel->setStyleSheet("border:0px;background:transparent;font-size:38px;color:white;");
    effect = new QGraphicsDropShadowEffect(this);
    effect->setXOffset(0);
    effect->setYOffset(0);
    effect->setBlurRadius(8);
    effect->setColor(QColor(38, 38, 38, 100));
    upWidget->setGraphicsEffect(effect);
}

void FunctionWidget::setDownOpacityEffect(const qreal &num)
{
    opacity = num;
    QDBusReply<QString> styleName = usrInterface->call(QString("get_current_stylename"));
    changeSearchBoxBackground(styleName);
}

void FunctionWidget::obtainSearchResult()
{
    QDBusInterface iface("com.ukui.search.service",
                         "/",
                         "org.ukui.search.service",
                         QDBusConnection::sessionBus());

    if (iface.isValid()) {
        iface.call("showWindow");
    }
}

//搜索框适配主题
void FunctionWidget::changeSearchBoxBackground(QString styleName)
{
    QString styleSheetDark = QString("border-radius:40px;background:rgba(44,50,57,%1);color:white;font-size:24px;text-align:left;padding-left:24px;").arg(opacity);
    QString styleSheetLight = QString("border-radius:40px;background:rgba(255,255,255,%1);"
                                      "color:rgba(58,67,78,0.25);font-size:24px;text-align:left;padding-left:24px;").arg(opacity);

    //    themeName=themeSetting->get("style-name").toString();
    if (styleName == "ukui-dark") {
        searchEditBtn->setStyleSheet(styleSheetDark);
    } else {
        searchEditBtn->setStyleSheet(styleSheetLight);
    }
}

