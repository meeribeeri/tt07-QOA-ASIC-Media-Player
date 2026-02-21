#include "COMSelector.h"


COMSelector::COMSelector(QWidget* parent) : QMainWindow(parent) {
    this->setAttribute(Qt::WA_DeleteOnClose); //delete self on close to prevent mem leak
    QWidget* main = new QWidget(this);
    //layout
    QVBoxLayout* layout = new QVBoxLayout(main);

    //title
    COMSelector::title = new QLabel();
    COMSelector::title->setFrameStyle(QFrame::Panel | QFrame::Raised);
    COMSelector::title->setText("Select The COM Ports Used:");
    COMSelector::title->setAlignment(Qt::AlignCenter);
    COMSelector::title->setTextInteractionFlags(Qt::NoTextInteraction);

    //text area
    COMSelector::textArea = new QLabel();
    COMSelector::textArea->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    COMSelector::textArea->setText("If you do not know which COM ports to enter, plug the emulator or ASIC into your PC and open device manager.\nUnder Ports check what ports there are. Unplug and check which disappear, then enter those ports.\n\n**IMPORTANT**\nOnly enter the number, do not enter any letters.");
    COMSelector::textArea->setTextInteractionFlags(Qt::NoTextInteraction);

    //scroll area for text
    COMSelector::scroll = new QScrollArea();
    COMSelector::scroll->setWidget(COMSelector::textArea);

    //COM port line edits
    COMSelector::readWriteCOM = new QLineEdit();
    
    if (qoaHandler->getReadWrite() == -1) {
        COMSelector::readWriteCOM->setPlaceholderText("Enter just the Read & Write COM Port's Number");
    }
    else {
        COMSelector::readWriteCOM->setText(QString::number(qoaHandler->getReadWrite()));
    }
    COMSelector::readWriteCOM->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*"), this));
    connect(COMSelector::readWriteCOM, &QLineEdit::editingFinished, this, &COMSelector::COMPortChanged);
    
    COMSelector::dedicatedReadCOM = new QLineEdit();

    if (qoaHandler->getDedicatedRead() == -1) {
        COMSelector::dedicatedReadCOM->setPlaceholderText("Enter just the Dedicated Read COM Port's Number");
    }
    else {
        COMSelector::dedicatedReadCOM->setText(QString::number(qoaHandler->getDedicatedRead()));
    }
    COMSelector::dedicatedReadCOM->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*"), this));
    connect(COMSelector::dedicatedReadCOM, &QLineEdit::editingFinished, this, &COMSelector::COMPortChanged);


    COMSelector::closeButton = new QPushButton("&Close COM Port Selector");
    connect(COMSelector::closeButton, &QPushButton::clicked, this, &QWidget::close);

    //save checkbox
    COMSelector::saveCheckBox = new QCheckBox();
    COMSelector::saveCheckBox->setChecked(settings->getCOMPortSave());
    connect(COMSelector::saveCheckBox, &QCheckBox::checkStateChanged, settings, &Settings::setCOMPortSave);

    QLabel* saveLabel = new QLabel();
    saveLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    saveLabel->setText("Check to save COM ports on closing the program.");
    saveLabel->setTextInteractionFlags(Qt::NoTextInteraction);

    QWidget* saveWidget = new QWidget();
    QHBoxLayout* saveLayout = new QHBoxLayout(saveWidget);
    saveLayout->addWidget(saveLabel);
    saveLayout->addWidget(COMSelector::saveCheckBox);

    //add widgets to main layout
    layout->addWidget(COMSelector::title);
    layout->addWidget(COMSelector::scroll);

    layout->addWidget(COMSelector::readWriteCOM);
    layout->addWidget(COMSelector::dedicatedReadCOM);

    layout->addWidget(saveWidget);

    layout->addWidget(COMSelector::closeButton);

    COMSelector::setCentralWidget(main);

    //set min size & resize
    COMSelector::setMinimumSize(300,250);
    COMSelector::resize(300, 250);

}

void COMSelector::COMPortChanged() {
    qoaHandler->changeCOMPorts(COMSelector::readWriteCOM->text().toInt(), COMSelector::dedicatedReadCOM->text().toInt());
}

COMSelector::~COMSelector() {
    fileHandler->playCurrentSongFile();
}