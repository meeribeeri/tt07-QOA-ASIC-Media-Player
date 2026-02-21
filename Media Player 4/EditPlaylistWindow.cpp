#include "EditPlaylistWindow.h"


EditPlaylistWindow::EditPlaylistWindow(QWidget* parent) : QMainWindow(parent) {
    QWidget* main = new QWidget(this);
    //layout
    QVBoxLayout* layout = new QVBoxLayout(main);

    //name/title
    EditPlaylistWindow::name = new QLineEdit(playlistHandler->getFileName());
    layout->addWidget(EditPlaylistWindow::name);


    //buttons
    EditPlaylistWindow::addSongButton = new QPushButton("&Add Song",this);
    connect(EditPlaylistWindow::addSongButton, &QPushButton::clicked, fileHandler, &FileHandler::addSongToPlaylist);
    layout->addWidget(EditPlaylistWindow::addSongButton);

    EditPlaylistWindow::closeButton = new QPushButton("&Save and Close");
    connect(EditPlaylistWindow::closeButton, &QPushButton::clicked, this, &EditPlaylistWindow::closeWindow);
    EditPlaylistWindow::removeSongButton = new QPushButton("&Remove Selected Songs");
    connect(EditPlaylistWindow::removeSongButton, &QPushButton::clicked, this, &EditPlaylistWindow::removeSongs);

    //remove song area
    EditPlaylistWindow::mainScrollWidget = new QWidget(this);
    EditPlaylistWindow::removeLayout = new QVBoxLayout(EditPlaylistWindow::mainScrollWidget);

    for (QString song : playlistHandler->getSongList()) {
        EditPlaylistWindow::removeLayout->addWidget(EditPlaylistWindow::addSongToRemoveWidgetDisplay(song));
    }

    QScrollArea* scroll = new QScrollArea();
    scroll->setWidget(EditPlaylistWindow::mainScrollWidget);
    scroll->setWidgetResizable(true);
    layout->addWidget(scroll);

    layout->addWidget(EditPlaylistWindow::removeSongButton);
    layout->addWidget(EditPlaylistWindow::closeButton);

    EditPlaylistWindow::setCentralWidget(main);

    connect(playlistHandler, &PlaylistHandler::playlistUpdated,this,&EditPlaylistWindow::update);

    //resize and set min size
    EditPlaylistWindow::setMinimumSize(250,250);
    EditPlaylistWindow::resize(250, 250);
}

void EditPlaylistWindow::closeWindow() {
    //set playlist name to edited name

    if (playlistHandler->getFileName() != EditPlaylistWindow::name->text()) {
        playlistHandler->changePlaylistName(EditPlaylistWindow::name->text());
    }

    //save playlist songs

    //close
    this->close();
}

QWidget* EditPlaylistWindow::addSongToRemoveWidgetDisplay(QString song) {
    QWidget* widget = new QWidget(this);

    QLabel* songLabel = new QLabel(widget);
    songLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    songLabel->setText(song);
    songLabel->setAlignment(Qt::AlignCenter);
    songLabel->setTextInteractionFlags(Qt::NoTextInteraction);

    QCheckBox* checkBox = new QCheckBox(this);

    EditPlaylistWindow::songCheckBoxes.push_back(checkBox);
    EditPlaylistWindow::songLabels.push_back(songLabel);

    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addWidget(checkBox);
    layout->addWidget(songLabel);
    layout->addStretch(1);

    EditPlaylistWindow::songWidgets.push_back(widget);

    return widget;
}

void EditPlaylistWindow::removeSongs() {
    QList<int> songsToRemove;

    int currentSongNum = 0;
    for (QCheckBox* box : EditPlaylistWindow::songCheckBoxes) {
        if (box->isChecked()) {
            songsToRemove.push_back(currentSongNum);
        }
        currentSongNum++;
    }
    playlistHandler->removeSongs(songsToRemove);

    int removalOffset = 0;
    for (int song : songsToRemove) {
        song = song - removalOffset;
        QCheckBox* box2 = EditPlaylistWindow::songCheckBoxes.at(song);
        EditPlaylistWindow::songCheckBoxes.removeAt(song);
        delete box2;

        QLabel* label = EditPlaylistWindow::songLabels.at(song);
        EditPlaylistWindow::songLabels.removeAt(song);
        delete label;

        QWidget* widget = EditPlaylistWindow::songWidgets.at(song);
        EditPlaylistWindow::songWidgets.removeAt(song);
        delete widget;

        removalOffset++;
    }
}

void EditPlaylistWindow::update() {
    int currentSongCount = EditPlaylistWindow::songLabels.count() - 1;
    int currentIndex = 0;
    for (QString song : playlistHandler->getSongList()) {
        if (currentIndex > currentSongCount) {
            EditPlaylistWindow::removeLayout->addWidget(EditPlaylistWindow::addSongToRemoveWidgetDisplay(song));
        }
        currentIndex++;
    }

    EditPlaylistWindow::mainScrollWidget->show();


}