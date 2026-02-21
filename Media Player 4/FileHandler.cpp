#include "FileHandler.h"

FileHandler::FileHandler() {
    FileHandler::closing = false;

    //QDir::setCurrent(QCoreApplication::applicationDirPath());
    QFile settingsFile("settings.cfg");
    if (settingsFile.exists()) {
        if (!settingsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            debugHandler->addErrorMessage("Failed to open settings file.", FILEHANDLER_SOURCE);
            return;
        }
        FileHandler::settingsFull = settingsFile.readLine();
        FileHandler::settingsFull = FileHandler::settingsFull + settingsFile.readLine();
    }
    else {
        debugHandler->addErrorMessage("Settings file not found", FILEHANDLER_SOURCE);
        FileHandler::settingsFull = "Volume: 100\nCOM1: -1 | COM2: -1";
    }

    settingsFile.flush();
    settingsFile.close();
}

void FileHandler::newPlaylist() {
    QDir::setCurrent(QDir::homePath() + "/Music"); //sets the path of where files are opened/saved to the user's Music directory

    //create Playlist directory if necessary
    if (!QDir("../Playlists").exists()) { //checks if a Playlist folder exits and creates it if not
        debugHandler->addErrorMessage("Playlist directory does not exist. Creating new playlist directory.", FILEHANDLER_SOURCE);
        QDir("").mkdir("Playlists");
    }
    QDir::setCurrent("Playlists"); //enters the Playlist folder

    //get new file
    FileHandler::playlistFileName = QFileDialog::getSaveFileName(nullptr,tr("New Playlist"),QString(),tr("Text files (*.txt)"));

    QFile playlistFile(FileHandler::playlistFileName);
    //make new file (done via the open() method, which creates a file if not already present
    if (!playlistFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        debugHandler->addErrorMessage("Failed to open playlist file for writing.", FILEHANDLER_SOURCE);
        return;
    }

    QFile songFile(FileHandler::currentSong);
    //store current song into file if it exists
    if (songFile.exists()) { //if the audio file currently in use exists, add it to the playlist
        QTextStream writer(&playlistFile);
        writer << songFile.fileName() << "\n"; //a writer stream is used to write data to files
    }
    playlistFile.flush(); //clear the file's buffer
    playlistFile.close(); //close the file

    qoaHandler->resetCOMFallback();

    //change playlist handler's file to new playlist file
    playlistHandler->changePlaylistFile(FileHandler::playlistFileName);
    emit FileHandler::newPlaylistMade(); //send signal for main window to recieve
}


QString FileHandler::getPlaylistFileName() {
    return FileHandler::playlistFileName;
}

void FileHandler::openPlaylist() {
    //create Playlist directory if necessary
    QDir::setCurrent(QDir::homePath() + "/Music");

    if (!QDir("../Playlists").exists()) {
        debugHandler->addErrorMessage("Playlist directory does not exist. Creating new playlist directory.", FILEHANDLER_SOURCE);
        QDir("").mkdir("Playlists");
    }
    QDir::setCurrent("Playlists");

    FileHandler::playlistFileName = QFileDialog::getOpenFileName(nullptr, tr("Open Playlist"), QString(), tr("Text files (*.txt)"));

    qoaHandler->resetCOMFallback();

    playlistHandler->changePlaylistFile(FileHandler::playlistFileName);
    emit FileHandler::newPlaylistMade(); //send signal for main window to recieve, same signal since it does the same thing anyway, why change it?
}

void FileHandler::addSongToPlaylist() {
    //setting current directory to root directory
    QDir::setCurrent(QDir::rootPath());

    //getting the song to add from user
    QString songFile = QFileDialog::getOpenFileName(nullptr, tr("Add Song"), QString(), tr("QOA Audio Files (*.qoa)"));

    //add song file
    if (!songFile.isEmpty() && songFile != "") {
        playlistHandler->addSong(songFile);
    }
}

QString FileHandler::getCurrentSongFileName() {
    QFileInfo information(FileHandler::currentSong);
    return information.baseName();
}

void FileHandler::changeCurrentSong(QString song) {
    FileHandler::currentSong = song;
    qoaHandler->splitAndSend(FileHandler::currentSong, false);
}

void FileHandler::openSongFile() {
    QDir::setCurrent(QDir::homePath() + "/Music");

    QString lastSong = FileHandler::currentSong;
    FileHandler::currentSong = QFileDialog::getOpenFileName(nullptr,"Open A QOA File", "", tr("QOA Audio Files (*.qoa))"));

    if (FileHandler::currentSong.isEmpty() || FileHandler::currentSong == "") {
        FileHandler::currentSong = lastSong;
        return;
    }

    FileHandler::playlistFileName = "";

    qoaHandler->resetCOMFallback();

    emit FileHandler::openedSongManually();

    qoaHandler->splitAndSend(FileHandler::currentSong);
}

void FileHandler::saveSettings(QString volume, QString COM) {
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    QFile settingsFile("settings.cfg");

    if (!settingsFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        debugHandler->addErrorMessage("Failed to open settings file for writing.", FILEHANDLER_SOURCE);
        return;
    }

    QTextStream writer(&settingsFile);
    writer << volume << "\n" << COM << "\n";
    
    settingsFile.flush();
    settingsFile.close();

}

QString FileHandler::getSettings() {
    return FileHandler::settingsFull;
}

void FileHandler::playCurrentSongFile() {
    if (FileHandler::closing) {
        return;
    }
    QFile songFile(FileHandler::currentSong);
    if (!songFile.exists()) {
        debugHandler->addErrorMessage("Attempted to auto play selected song file. Song file does not exist", FILEHANDLER_SOURCE);
        return;
    }
    qoaHandler->splitAndSend(FileHandler::currentSong);
}

void FileHandler::saveLogFile(QString fileName, QString append) {
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    //create log file directory if necessary
    if (!QDir("../Log Files").exists()) {
        debugHandler->addErrorMessage("Log file directory does not exist. Creating new log file directory.", FILEHANDLER_SOURCE);
        QDir("").mkdir("Log Files");
    }
    QDir::setCurrent("Log Files");

    QFile logFile(fileName);

    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        debugHandler->addErrorMessage("Failed to open log file for writing.", FILEHANDLER_SOURCE);
        return;
    }

    QTextStream writer(&logFile);
    writer << append << "\n";

    if (FileHandler::closing) {
        writer << "Application closing. Log file closed." << "\n";
    }

    logFile.flush();
    logFile.close();
}

void FileHandler::applicationClosing() {
    FileHandler::closing = true;
    FileHandler::saveLogFile(debugHandler->getSaveFileName(), debugHandler->getSaveData());
}


FileHandler* fileHandler = new FileHandler();