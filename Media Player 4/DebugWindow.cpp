#include "DebugWindow.h"


DebugWindow::DebugWindow(QWidget* parent) : QMainWindow(parent) {
    this->setAttribute(Qt::WA_DeleteOnClose);
    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);

    DebugWindow::displayChoice = 0;

    DebugWindow::title = new QLabel();
    DebugWindow::title->setFrameStyle(QFrame::Box | QFrame::Raised);
    DebugWindow::title->setText("Error Messages");
    DebugWindow::title->setAlignment(Qt::AlignCenter);
    DebugWindow::title->setTextInteractionFlags(Qt::NoTextInteraction);

    DebugWindow::debugTextArea = new QTextEdit();
    DebugWindow::debugTextArea->setFrameStyle(QFrame::Box | QFrame::Sunken);
    DebugWindow::debugTextArea->setText("");
    DebugWindow::debugTextArea->setTextInteractionFlags(Qt::NoTextInteraction);

    DebugWindow::textScroll = new QScrollArea();
    DebugWindow::textScroll->setWidget(DebugWindow::debugTextArea);
    DebugWindow::textScroll->setWidgetResizable(true);

    //debug message selector
    QWidget* selectionsWidget = new QWidget();
    QHBoxLayout* selectionsLayout = new QHBoxLayout(selectionsWidget);

    QLabel* selectionsTitle = new QLabel();
    selectionsTitle->setFrameStyle(QFrame::Box | QFrame::Raised);
    selectionsTitle->setText("Display Messages From: ");
    selectionsTitle->setAlignment(Qt::AlignCenter);
    selectionsTitle->setTextInteractionFlags(Qt::NoTextInteraction);

    QComboBox* selections = new QComboBox();
    selections->insertItem(0, "Errors");
    selections->insertItem(1, "Decoded Samples");
    connect(selections, &QComboBox::activated, this, &DebugWindow::changeDisplay);

    selectionsLayout->addWidget(selectionsTitle);
    selectionsLayout->addWidget(selections);

    QPushButton* closeButton = new QPushButton("Close Window");
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);

    mainLayout->addWidget(DebugWindow::title);
    mainLayout->addWidget(DebugWindow::textScroll);
    mainLayout->addWidget(selectionsWidget);
    mainLayout->addWidget(closeButton);

    DebugWindow::setCentralWidget(mainWidget);

    DebugWindow::setMinimumSize(400, 500);
    DebugWindow::resize(400, 500);

    connect(this, &DebugWindow::requestLog, debugHandler, &DebugHandler::getMessages);
    connect(debugHandler, &DebugHandler::sendLog, this, &DebugWindow::actuallyChangeDisplay);

    DebugWindow::changeDisplay(0);
}

void DebugWindow::changeDisplay(int index) {
    switch (index) {
    case 0:
        DebugWindow::title->setText("Error Messages");
        DebugWindow::requestLog(ERROR_MESSAGES);
        //DebugWindow::debugTextArea->setText(debugHandler->getMessages(ERROR_MESSAGES));
        break;
    case 1:
        DebugWindow::title->setText("Decoded Samples (First 1000 played)");
        DebugWindow::requestLog(SAMPLE_MESSAGES);
        //DebugWindow::debugTextArea->setText(debugHandler->getMessages(SAMPLE_MESSAGES));
        break;
    default:
        DebugWindow::title->setText("No message display option selected. If you see this, an error has occured.");
        break;
    }
}

void DebugWindow::actuallyChangeDisplay(QString message) {
    DebugWindow::debugTextArea->setText(message);
}

