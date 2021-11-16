/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#include "fullletterwidget.h"
#include <QDebug>
#include <syslog.h>

FullLetterWidget::FullLetterWidget(QWidget *parent) :
    QWidget(parent)
{
    initUi();
}

FullLetterWidget::~FullLetterWidget()
{
    delete m_ukuiMenuInterface;
    delete m_letterListBottomSpacer;
}

/**
 * 主界面初始化
 */
void FullLetterWidget::initUi()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->setAttribute(Qt::WA_TranslucentBackground);
    m_letterListWid = new QWidget(this);
    m_letterListWid->setFixedSize(Style::LeftWidWidth, Style::AppListWidHeight);
    verticalScrollBar = new QScrollBar(m_scrollArea);
    verticalScrollBar->setOrientation(Qt::Vertical);
    mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0, 0, 40, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_letterListWid);
    this->setLayout(mainLayout);
    m_ukuiMenuInterface = new UkuiMenuInterface;
    initAppListWidget();
    initLetterListWidget();
    flag = true;
    //翻页灵敏度时间调节
    time = new QTimer(this);
    connect(time, &QTimer::timeout, [ = ]() {
        if(flag == false) {
            flag = true;
            time->stop();
        }
    });
    connect(m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &FullLetterWidget::on_setScrollBarValue);
    connect(verticalScrollBar, &QScrollBar::valueChanged, this, &FullLetterWidget::on_setAreaScrollBarValue);
    connect(powerOffButton, &QPushButton::customContextMenuRequested, this, &FullLetterWidget::on_powerOffButton_customContextMenuRequested);
    connect(powerOffButton, &QPushButton::clicked, this, &FullLetterWidget::on_powerOffButton_clicked);
}

/**
 * 初始化应用列表界面
 */
void FullLetterWidget::initAppListWidget()
{
    //    QHBoxLayout* layout=new QHBoxLayout(m_applistWid);
    //    layout->setContentsMargins(0,0,0,0);
    //    m_applistWid->setLayout(layout);
    m_scrollArea = new ScrollArea();
    m_scrollAreaWid = new ScrollAreaWid(this);
    m_scrollArea->setWidget(m_scrollAreaWid);
    m_scrollArea->setFixedSize(Style::AppListWidWidth, Style::AppListWidHeight);
    m_scrollArea->setWidgetResizable(true);
    //    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollAreaWidLayout = new QVBoxLayout;
    m_scrollAreaWidLayout->setContentsMargins(0, 0, 0, 0);
    m_scrollAreaWidLayout->setSpacing(10);
    m_scrollAreaWid->setLayout(m_scrollAreaWidLayout);
    mainLayout->addWidget(m_scrollArea);
    QSpacerItem *m_spaceItem1 = nullptr;
    m_spaceItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    mainLayout->addItem(m_spaceItem1);
    QVBoxLayout *rightButtonLayout = new QVBoxLayout(this);
    rightButtonLayout->setContentsMargins(0, 0, 0, 20);
    rightButtonLayout->setSpacing(0);
    QSpacerItem *m_spaceItem2 = nullptr;
    m_spaceItem2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    rightButtonLayout->addItem(m_spaceItem2);
    powerOffButton = new QPushButton(this);
    powerOffButton->setMinimumSize(QSize(24, 24));
    powerOffButton->setContextMenuPolicy(Qt::CustomContextMenu);
    QIcon icon6;
    icon6.addFile(QString::fromUtf8(":/data/img/mainviewwidget/icon-电源@2x.png"), QSize(), QIcon::Normal, QIcon::Off);
    powerOffButton->setIcon(icon6);
    powerOffButton->setIconSize(QSize(24, 24));
    powerOffButton->setFlat(true);
    powerOffButton->setStyleSheet("padding: 0px;");
    rightButtonLayout->addWidget(verticalScrollBar);
    QSpacerItem *m_spaceItem3 = nullptr;
    m_spaceItem3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    rightButtonLayout->addItem(m_spaceItem3);
    rightButtonLayout->addWidget(powerOffButton);
    rightButtonLayout->setAlignment(verticalScrollBar, Qt::AlignHCenter);
    mainLayout->addLayout(rightButtonLayout);
    connect(m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &FullLetterWidget::valueChangedSlot);
    fillAppList();
    m_scrollAreaWidHeight = m_scrollAreaWid->height();
    initVerticalScrollBar();
}

void FullLetterWidget::initVerticalScrollBar()
{
    verticalScrollBar->setFixedHeight(200);
    int scrollBarSize = 200 * Style::AppListWidHeight / m_scrollAreaWidHeight + 1;
    QString scrollBarStyle = QString("QScrollBar:vertical"
                                     "{"
                                     "width:4px;"
                                     "background:rgba(0,0,0,60%);"
                                     "margin:0px,0px,0px,0px;"
                                     "border-radius:2px;"
                                     "}"
                                     "QScrollBar::handle:vertical"
                                     "{"
                                     "width:8px;"
                                     "background:rgba(255,255,255,100%);"
                                     " border-radius:2px;"
                                     "min-height:%1;"
                                     "}"
                                     "QScrollBar::add-line:vertical"
                                     "{"
                                     "height:0px;width:0px;"
                                     "subcontrol-position:bottom;"
                                     "}"
                                     "QScrollBar::sub-line:vertical"
                                     "{"
                                     "height:0px;width:0px;"
                                     "subcontrol-position:top;"
                                     "}").arg(scrollBarSize);
    verticalScrollBar->setStyleSheet(scrollBarStyle);
}

void FullLetterWidget::on_powerOffButton_clicked()
{
    QProcess::startDetached(QString("ukui-session-tools"));
}

void FullLetterWidget::on_powerOffButton_customContextMenuRequested(const QPoint &pos)
{
    RightClickMenu m_otherMenu(this);
    // connect(&m_otherMenu, &RightClickMenu::sendMainWinActiveSignal, this, &SideBarWidget::sendShowMainWindowSignal);
    //  Q_EMIT sendShowMainWindowSignal(false);
    int ret = m_otherMenu.showShutdownMenu(powerOffButton->mapToGlobal(pos));
    qDebug() << "SideBarWidget::shutdownBtnRightClickSlot() 开始";

    if(ret >= 10 && ret <= 17) {
        //        Q_EMIT sendHideMainWindowSignal();
        switch (ret) {
            case 10:
                QProcess::startDetached(QString("ukui-screensaver-command -l"));
                break;

            case 11:
                QProcess::startDetached(QString("ukui-session-tools --switchuser"));
                break;

            case 12:
                QProcess::startDetached(QString("ukui-session-tools --logout"));
                break;

            case 13:
                QProcess::startDetached(QString("ukui-session-tools --reboot"));
                break;

            case 14:
                QProcess::startDetached(QString("ukui-session-tools --shutdown"));
                break;

            case 16:
                QProcess::startDetached(QString("ukui-session-tools --suspend"));
                break;

            case 17:
                QProcess::startDetached(QString("ukui-session-tools --sleep"));
                break;

            default:
                break;
        }
    }

    qDebug() << "SideBarWidget::shutdownBtnRightClickSlot() 结束";
}

void FullLetterWidget::setFocusToThis()
{
    QLayoutItem *widItemTop = m_scrollAreaWidLayout->itemAt(1);
    QWidget *widTop = widItemTop->widget();
    FullListView *m_listviewTop = qobject_cast<FullListView *>(widTop);
    letterButtonClick();
    m_listviewTop->setFocus();
    Q_EMIT selectFirstItem();
}

/**
 * 填充应用列表
 */
void FullLetterWidget::fillAppList()
{
    m_letterList.clear();
    QVector<QStringList> vector = UkuiMenuInterface::alphabeticVector;

    for(int i = 0; i < vector.size(); i++) {
        QStringList appList = vector.at(i);

        if(!appList.isEmpty()) {
            QString letterstr;

            if(i < 26) {
                letterstr = QString(QChar(static_cast<char>(i + 65)));
            } else if(i == 26) {
                letterstr = "&";
            } else {
                letterstr = "#";
            }

            m_letterList.append(letterstr);//存储分类字符
            //插入字母分类按钮
            SplitBarFrame *letterbtn = new SplitBarFrame(this, letterstr, m_scrollArea->width() - 12, 30, 1);
            m_scrollAreaWidLayout->addWidget(letterbtn);
            //插入应用列表
            FullListView *listview = new FullListView(this, 1);
            connect(listview, &FullListView::sendSetslidebar, this, &FullLetterWidget::onSetSlider);
            connect(this, &FullLetterWidget::selectFirstItem, listview, &FullListView::selectFirstItem);
            listview->installEventFilter(this);
            //修复异常黑框问题
            connect(m_scrollArea, &ScrollArea::requestUpdate, listview->viewport(), [ = ]() {
                listview->repaint(listview->rect());
            });
            m_scrollAreaWidLayout->addWidget(listview);
            m_data.clear();

            for(int i = 0; i < appList.count(); i++) {
                m_data.append(appList.at(i));
            }

            listview->addData(m_data);
            connect(listview, &FullListView::sendItemClickedSignal, this, &FullLetterWidget::execApplication);
            connect(listview, &FullListView::sendHideMainWindowSignal, this, &FullLetterWidget::sendHideMainWindowSignal);
        }
    }

    resizeScrollAreaControls();
}

/**
 * 执行应用程序
 */
void FullLetterWidget::execApplication(QString desktopfp)
{
    Q_EMIT sendHideMainWindowSignal();
    execApp(desktopfp);
}

void FullLetterWidget::on_setAreaScrollBarValue(int value)
{
    //    m_scrollArea->verticalScrollBar()->setMaximum(maxmumValue);
    m_scrollArea->verticalScrollBar()->setValue(value);
}

/**
 * 更新应用列表
 */
void FullLetterWidget::updateAppListView()
{
    //刷新应用列表界面
    QLayoutItem *child;

    while ((child = m_scrollAreaWidLayout->takeAt(0)) != 0) {
        QWidget *wid = child->widget();
        m_scrollAreaWidLayout->removeWidget(wid);
        wid->setParent(nullptr);
        delete wid;
        delete child;
    }

    fillAppList();

    //刷新字母列表界面
    Q_FOREACH (QAbstractButton *button, m_buttonList) {
        m_btnGroup->removeButton(button);
    }

    m_buttonList.clear();
    m_letterListWidLayout->removeItem(m_topSpacerItem);
    m_letterListWidLayout->removeItem(m_letterListBottomSpacer);

    while ((child = m_letterListWidLayout->takeAt(0)) != 0) {
        QWidget *wid = child->widget();
        m_letterListWidLayout->removeWidget(wid);
        wid->setParent(nullptr);
        delete wid;
        delete child;
    }

    //防止按钮位置偏移
    initLetterListScrollArea();
    m_scrollAreaWidHeight = m_scrollAreaWid->height();
    initVerticalScrollBar();
}

void FullLetterWidget::on_setScrollBarValue(int value)
{
    verticalScrollBar->setMaximum(m_scrollAreaWidHeight - Style::AppListWidHeight);
    verticalScrollBar->setValue(value);
}

/**
 * 设置scrollarea所填充控件大小
 */
void FullLetterWidget::resizeScrollAreaControls()
{
    int row = 0;
    int areaHeight = 0;

    while(row < m_scrollAreaWidLayout->count() / 2) {
        //应用界面
        QLayoutItem *widItem = m_scrollAreaWidLayout->itemAt(row * 2 + 1);
        QWidget *wid = widItem->widget();
        FullListView *listview = qobject_cast<FullListView *>(wid);
        listview->adjustSize();
        int dividend = m_scrollArea->width() / Style::AppListGridSizeWidth;
        int rowcount = 0;

        if(listview->model()->rowCount() % dividend > 0) {
            rowcount = listview->model()->rowCount() / dividend + 1;
        } else {
            rowcount = listview->model()->rowCount() / dividend;
        }

        listview->setFixedSize(m_scrollArea->width(), listview->gridSize().height()*rowcount);
        areaHeight += listview->height() + 50;
        row++;
    }

    m_scrollArea->widget()->setFixedSize(m_scrollArea->width(), areaHeight - 10);
}

/**
 * 初始化字母列表界面
 */
void FullLetterWidget::initLetterListWidget()
{
    m_letterListWidLayout = new QVBoxLayout(m_letterListWid);
    m_letterListWidLayout->setContentsMargins(45, 0, 0, 0);
    m_letterListWidLayout->setSpacing(0);
    m_topSpacerItem = new QSpacerItem(20, 40, QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_letterListBottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_btnGroup = new QButtonGroup(m_letterListWid);
    m_animation = new QPropertyAnimation(m_letterListWid, "geometry");
    m_scrollAnimation = new QPropertyAnimation(m_scrollArea->verticalScrollBar(), "value");
    m_scrollAnimation->setEasingCurve(QEasingCurve::OutQuad);
    connect(m_scrollAnimation, &QPropertyAnimation::finished, this, &FullLetterWidget::animationFinishSlot);
    connect(m_scrollAnimation, &QPropertyAnimation::valueChanged, this, &FullLetterWidget::animationValueChangedSlot);
    initLetterListScrollArea();
}

/**
 * 初始化字母列表
 */
void FullLetterWidget::initLetterListScrollArea()
{
    m_letterListWidLayout->addItem(m_topSpacerItem);

    if(m_letterList.contains("&")) {
        m_letterList.replace(m_letterList.indexOf("&"), "&&");
    }

    for(int i = 0; i < m_letterList.size(); i++) {
        LetterClassifyButton *letterbtn = new LetterClassifyButton(m_letterListWid,
                true,
                m_letterList.at(i));
        letterbtn->setFixedSize(Style::LeftLetterBtnHeight, Style::LeftLetterBtnHeight);
        m_buttonList.append(letterbtn);
        m_letterListWidLayout->addWidget(letterbtn);
        m_letterListWidLayout->setAlignment(letterbtn, Qt::AlignLeft);
        connect(letterbtn, &LetterClassifyButton::buttonClicked, m_btnGroup, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked));
    }

    m_letterListWidLayout->addItem(m_letterListBottomSpacer);
    int id = 0;

    Q_FOREACH (QAbstractButton *btn, m_buttonList) {
        m_btnGroup->addButton(btn, id++);
    }

    connect(m_btnGroup, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &FullLetterWidget::btnGroupClickedSlot);

    //    m_letterListWid->widget()->adjustSize();
    if(m_btnGroup->button(0) != nullptr) {
        m_btnGroup->button(0)->click();
    }
}

void FullLetterWidget::btnGroupClickedSlot(QAbstractButton *btn)
{
    disconnect(m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
               this, &FullLetterWidget::valueChangedSlot);

    Q_FOREACH (QAbstractButton *button, m_buttonList) {
        LetterClassifyButton *letterbtn = qobject_cast<LetterClassifyButton *>(button);

        if(m_btnGroup->id(btn) == m_buttonList.indexOf(button)) {
            letterbtn->setChecked(true);
            //此处需实现将被选定的字母包含的应用列表移动到applistWid界面最顶端
            QString letterstr = letterbtn->text();
            int num = m_letterList.indexOf(letterstr);

            if(num != -1) {
                m_beginPos = m_scrollArea->verticalScrollBar()->sliderPosition();
                m_endPos = m_scrollAreaWidLayout->itemAt(m_btnGroup->id(btn) * 2)->widget()->y();
                m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                m_scrollAnimation->stop();
                m_scrollAnimation->setStartValue(m_beginPos);
                m_scrollAnimation->setEndValue(m_endPos);
                m_scrollAnimation->start();
            }
        } else {
            letterbtn->setChecked(false);
        }
    }
}

void FullLetterWidget::animationFinishSlot()
{
    if(m_scrollArea->verticalScrollBar()->value() == m_endPos ||
       m_scrollArea->verticalScrollBar()->value() == m_scrollArea->verticalScrollBar()->maximum()) {
        m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        connect(m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
                this, &FullLetterWidget::valueChangedSlot);
    }
}

void FullLetterWidget::animationValueChangedSlot(const QVariant &value)
{
    Q_UNUSED(value);

    if (sender() != m_scrollAnimation) {
        return;
    }

    QPropertyAnimation *ani = qobject_cast<QPropertyAnimation *>(sender());

    if (m_endPos != ani->endValue()) {
        ani->setEndValue(m_endPos);
    }
}

void FullLetterWidget::valueChangedSlot(int value)
{
    int index = 0;

    while(index <= m_letterList.count() - 1) {
        int min = m_scrollAreaWidLayout->itemAt(2 * index)->widget()->y();
        int max = 0;

        if(index == m_letterList.count() - 1) {
            max = m_scrollAreaWid->height();
        } else {
            max = m_scrollAreaWidLayout->itemAt(2 * (index + 1))->widget()->y();
        }

        if(value >= min && value < max) {
            Q_FOREACH (QAbstractButton *button, m_buttonList) {
                LetterClassifyButton *letterbtn = qobject_cast<LetterClassifyButton *>(button);

                if(index == m_buttonList.indexOf(button)) {
                    letterbtn->setChecked(true);
                } else {
                    letterbtn->setChecked(false);
                }
            }

            break;
        } else {
            index++;
        }
    }

    //    //向下滚动
    //    if((m_buttonList.at(index)->pos().y()+m_buttonList.at(index)->height()+m_letterListScrollArea->widget()->pos().y()) >= m_letterListScrollArea->height())
    //    {
    //        int val=m_letterListScrollArea->verticalScrollBar()->sliderPosition()+m_buttonList.at(index)->height();
    //        m_letterListScrollArea->verticalScrollBar()->setSliderPosition(val);
    //    }
    //    //向上滚动
    //    if((m_buttonList.at(index)->pos().y()+m_letterListScrollArea->widget()->pos().y()) <= 0)
    //    {
    //        int val=m_letterListScrollArea->verticalScrollBar()->value()-m_buttonList.at(index)->height();
    //        m_letterListScrollArea->verticalScrollBar()->setSliderPosition(val);
    //    }
}

void FullLetterWidget::enterAnimation()
{
    m_animation->setDuration(200);//动画总时间
    m_animation->setStartValue(QRect(0, (m_letterListWid->height() - (m_letterList.size() + 1)*Style::LeftLetterBtnHeight) / 2,
                                     0, (m_letterList.size() + 1)*Style::LeftLetterBtnHeight));
    m_animation->setEndValue(QRect(Style::LeftMargin,
                                   (m_letterListWid->height() - (m_letterList.size() + 1)*Style::LeftLetterBtnHeight) / 2,
                                   Style::LeftLetterBtnHeight * 2,
                                   (m_letterList.size() + 1)*Style::LeftLetterBtnHeight));
    m_animation->setEasingCurve(QEasingCurve::InQuart);
    m_animation->start();
    //    m_letterListScrollArea->show();
}

void FullLetterWidget::setLetterBtnGeometry()
{
    //    m_letterListScrollArea->setGeometry(QRect(Style::LeftMargin,
    //                                              (m_letterListWid->height()-(m_letterList.size()+1)*Style::LeftLetterBtnHeight)/2,
    //                                              Style::LeftLetterBtnHeight*2,
    //                                        (m_letterList.size()+1)*Style::LeftLetterBtnHeight));
    //    m_letterListScrollArea->show();
}

void FullLetterWidget::repaintWidget()
{
    updateAppListView();
}

void FullLetterWidget::widgetMakeZero()
{
    Q_FOREACH (QAbstractButton *button, m_buttonList) {
        QString letterstr = button->text().at(0);
        int num = m_letterList.indexOf(letterstr);

        if(num != -1) {
            m_btnGroup->button(num)->click();
            //            m_letterListScrollArea->verticalScrollBar()->setSliderPosition(0);
            break;
        }
    }

    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void FullLetterWidget::moveScrollBar(int type)
{
    int height = Style::primaryScreenHeight;

    if(type == 0) {
        m_scrollArea->verticalScrollBar()->setSliderPosition(m_scrollArea->verticalScrollBar()->sliderPosition() - height * 100 / 1080);
    } else {
        m_scrollArea->verticalScrollBar()->setSliderPosition(m_scrollArea->verticalScrollBar()->sliderPosition() + height * 100 / 1080);
    }
}

void FullLetterWidget::onSetSlider(int value)
{
    //    if(flag)
    //    {
    //        flag = false;
    //        time->start(100);
    int curvalue = m_scrollArea->verticalScrollBar()->value();
    m_scrollArea->verticalScrollBar()->setValue(curvalue + value);
    //    }
}

QAbstractButton *FullLetterWidget::getCurLetterButton(int value)
{
    return m_buttonList.at(value);
}

bool FullLetterWidget::eventFilter(QObject *watched, QEvent *event)
{
    if( event->type() == QEvent::KeyPress ) {
        QLayoutItem *widItem = m_scrollAreaWidLayout->itemAt(2 * m_buttonList.size() - 1);
        QWidget *wid = widItem->widget();
        FullListView *m_listview = qobject_cast<FullListView *>(wid);
        QLayoutItem *widItemTop = m_scrollAreaWidLayout->itemAt(1);
        QWidget *widTop = widItemTop->widget();
        FullListView *m_listviewTop = qobject_cast<FullListView *>(widTop);
        QKeyEvent *ke = (QKeyEvent *)event;

        if( ke->key() == Qt::Key_Tab ) {
            // m_letterListScrollAreaWid->setFocus();
            // m_letterListScrollArea->setFocus();
            // return true;
            Q_EMIT setFocusToSideWin();
            return true;
        }

        if(ke->key() == Qt::Key_Up) {
            if(!m_listviewTop->hasFocus()) {
                QAbstractButton *buttonTop = getCurLetterButton(( --m_index) % m_buttonList.size());
                btnGroupClickedSlot(buttonTop);
                this->m_scrollArea->setFocusToPreChild();
            } else {
                m_listview->setFocus();
                QAbstractButton *button = getCurLetterButton(m_buttonList.size() - 1);
                btnGroupClickedSlot(button);
                m_index = m_buttonList.size() - 1;
            }

            Q_EMIT selectFirstItem();
            return true;
        }

        if(ke->key() == Qt::Key_Down) {
            if(!m_listview->hasFocus()) {
                QAbstractButton *button = getCurLetterButton(( ++m_index) % m_buttonList.size());
                btnGroupClickedSlot(button);
                this->m_scrollArea->setFocusToNextChild();
            } else {
                m_listviewTop->setFocus();
                QAbstractButton *buttonTop = getCurLetterButton(0);
                btnGroupClickedSlot(buttonTop);
                m_index = 0;
            }

            Q_EMIT selectFirstItem();
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void FullLetterWidget::letterButtonClick()
{
    if(m_btnGroup->button(0) != nullptr) {
        m_btnGroup->button(0)->click();
    }

    m_index = 0;
}