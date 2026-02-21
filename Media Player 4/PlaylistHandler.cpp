#include "PlaylistHandler.h"


PlaylistHandler::PlaylistHandler(QString fileName) {
    PlaylistHandler::playlistFileName = new QFile();
    if (fileName != "") {
        PlaylistHandler::playlistFileName->setFileName(fileName); //only sets the file to fileName if it is not default
    }
    PlaylistHandler::songLine = 0; //sets song at the first entry
    PlaylistHandler::connectedAutoPlay = false;
}

//checks to ensure playlistFileName is not emptry and exists
bool PlaylistHandler::checkForFile() {
    if (PlaylistHandler::playlistFileName->exists()) {
        return true;
    }
    debugHandler->addErrorMessage("Selected playlist file does not exist.", PLAYLISTHANDLER_SOURCE);
    return false;
}

//changes filename to new one and sets song to first entry
void PlaylistHandler::changePlaylistFile(QString fileName) {
    if (!PlaylistHandler::connectedAutoPlay) {
        connect(qoaHandler, &QOAHandler::nextSong, this, &PlaylistHandler::nextSong);
        PlaylistHandler::connectedAutoPlay = true;
    }


    PlaylistHandler::playlistFileName->setFileName(fileName);
    PlaylistHandler::songLine = 0;

    if (fileName == "") {
        debugHandler->addErrorMessage("Playlist name is blank.", PLAYLISTHANDLER_SOURCE);
        emit PlaylistHandler::playlistUpdated();

        emit PlaylistHandler::nameChanged();
        return;
    }

    //open file & find the first song
    try {
        if (!PlaylistHandler::playlistFileName->open(QIODevice::ReadOnly | QIODevice::Text)) {
            debugHandler->addErrorMessage("Failed to open playlist file for reading.", PLAYLISTHANDLER_SOURCE);
            return;
        }

        //reads lines until it reaches the song it wants, then exits & keeps it
        int currentLine = 0;
        QTextStream fileRead(PlaylistHandler::playlistFileName); //text stream used to read file
        QString entry = "";
        do {

            entry = fileRead.readLine();
            if (currentLine == PlaylistHandler::songLine && !entry.isEmpty()) {
                break;
            }
            if (!entry.isEmpty()) {
                currentLine++;
            }
        } while (!fileRead.atEnd());
        //fileHandler->changeCurrentSong(entry);
        PlaylistHandler::playlistFileName->close();
    }
    catch (...) {
        PlaylistHandler::playlistFileName->close();
        debugHandler->addErrorMessage("Failed to read playlist file to find first song.", PLAYLISTHANDLER_SOURCE);
        return;
    }
    emit PlaylistHandler::playlistUpdated();

    emit PlaylistHandler::nameChanged();
}

bool PlaylistHandler::nextSong() {
    if (!PlaylistHandler::checkForFile()) {
        return false;
    }

    try {
        //opens file & sets the search for the next song
        PlaylistHandler::songLine++;
        
        if (!PlaylistHandler::playlistFileName->open(QIODevice::ReadOnly | QIODevice::Text)) {
            debugHandler->addErrorMessage("Failed to open playlist file for reading.", PLAYLISTHANDLER_SOURCE);
            return false;
        }

        //reads lines until it reaches the song it wants, then exits & keeps it
        int currentLine = 0;
        QTextStream fileRead(PlaylistHandler::playlistFileName); //text stream used to read file
        QString entry = "";
        bool foundLine = false;
        do {

            entry = fileRead.readLine();
            if (currentLine == PlaylistHandler::songLine && !entry.isEmpty()) {
                foundLine = true;
                break;
            }
            if (!entry.isEmpty()) {
                currentLine++;
            }
        } while (!fileRead.atEnd());

        if (entry == "" || entry.isEmpty() || currentLine != PlaylistHandler::songLine || !foundLine) {
            //set the file in FileHandler & start reading song file if entry is found
            PlaylistHandler::playlistFileName->close();
            PlaylistHandler::songLine--;
            debugHandler->addErrorMessage("Empty line in playlist file encountered. Skipping line.", PLAYLISTHANDLER_SOURCE);
            return false;
        }
        fileHandler->changeCurrentSong(entry);
        PlaylistHandler::playlistFileName->close();
        emit PlaylistHandler::playlistUpdated();
        return true; //returns false if item does not exist
    }
    catch (...) {
        PlaylistHandler::playlistFileName->close();
        debugHandler->addErrorMessage("Failed to read playlist file to find next song.", PLAYLISTHANDLER_SOURCE);
        return false;
    }
    
}

bool PlaylistHandler::previousSong() {
    if (!PlaylistHandler::checkForFile() || PlaylistHandler::songLine == 0) {
        return false;
    }

    try {
        //opens file & sets the search for the next song
        if (!PlaylistHandler::playlistFileName->open(QIODevice::ReadOnly | QIODevice::Text)) {
            debugHandler->addErrorMessage("Failed to open playlist file for reading.", PLAYLISTHANDLER_SOURCE);
            return false;
        }

        PlaylistHandler::songLine--;

        //reads lines until it reaches the song it wants, then exits & keeps it
        int currentLine = 0;
        QTextStream fileRead(PlaylistHandler::playlistFileName); //text stream used to read file
        QString entry = "";
        bool foundLine = false;
        do {
            entry = fileRead.readLine();
            if (currentLine == PlaylistHandler::songLine && !entry.isEmpty()) {
                foundLine = true;
                break;
            }
            if (!entry.isEmpty()) {
                currentLine++;
            }
        } while (!fileRead.atEnd());

        if (entry == "" || entry.isEmpty() || currentLine != PlaylistHandler::songLine || !foundLine) {
            //set the file in FileHandler & start reading song file if entry is found
            PlaylistHandler::playlistFileName->close();
            debugHandler->addErrorMessage("Failed to find previous song. Likely at the start of the file.", PLAYLISTHANDLER_SOURCE);
            return false;
        }
        fileHandler->changeCurrentSong(entry);
        PlaylistHandler::playlistFileName->close();
        emit PlaylistHandler::playlistUpdated();
        return true; //returns false if item does not exist
    }
    catch (...) {
        PlaylistHandler::playlistFileName->close();
        debugHandler->addErrorMessage("Failed to read playlist file to find previous song.", PLAYLISTHANDLER_SOURCE);
        return false;
    }
    
}

void PlaylistHandler::changePlaylistName(QString newName) {
    if (!PlaylistHandler::checkForFile()) {
        return;
    }

    try {
        PlaylistHandler::playlistFileName->rename(newName);

    }
    catch (...) {
        //send debug line saying it failed
        debugHandler->addErrorMessage("Failed to change playlist file name", PLAYLISTHANDLER_SOURCE);
    }
    emit PlaylistHandler::playlistUpdated();
}

void PlaylistHandler::addSong(QString songPath) {
    if (!PlaylistHandler::checkForFile()) {
        return;
    }

    try {
        if (!PlaylistHandler::playlistFileName->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            debugHandler->addErrorMessage("Failed to open playlist file for writing.", PLAYLISTHANDLER_SOURCE);
            return;
        }
        QTextStream write(PlaylistHandler::playlistFileName);
        write << songPath << "\n";
        write.flush();
        PlaylistHandler::playlistFileName->flush();
        PlaylistHandler::playlistFileName->close();
    }
    catch (...) {
        //send debug line on fail
        debugHandler->addErrorMessage("Failed to write to playlist file to add a song.", PLAYLISTHANDLER_SOURCE);
    }
    emit PlaylistHandler::playlistUpdated();

    emit PlaylistHandler::nameChanged();
}

void PlaylistHandler::removeSongs(QList<int> songsToRemoveByIndex) {
    if (!PlaylistHandler::checkForFile()) {
        return;
    }

    int currentIndex = 0;
    QString savedSongsAsString = "";
    try {
        if (!PlaylistHandler::playlistFileName->open(QIODevice::ReadOnly | QIODevice::Text)) {
            debugHandler->addErrorMessage("Failed to open playlist file for reading.", PLAYLISTHANDLER_SOURCE);
            return;
        }

        QTextStream readWrite(PlaylistHandler::playlistFileName);
        
        QString line = "";
        do {
            line = readWrite.readLine();
            if (!songsToRemoveByIndex.contains(currentIndex) && !line.isEmpty()) {
                savedSongsAsString = savedSongsAsString + "\n" + line;
            }
            
            if (!line.isEmpty()) {
                currentIndex++;
            }
            
            
        } while (!line.isNull() && !readWrite.atEnd());

        //have to close the file and open it in write only as doing it all in read write mode causes it to just... not read the file
        PlaylistHandler::playlistFileName->close();

        if (!PlaylistHandler::playlistFileName->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            debugHandler->addErrorMessage("Failed to open playlist file for writing.", PLAYLISTHANDLER_SOURCE);
            return;
        }

        readWrite << savedSongsAsString << "\n";
        readWrite.flush();
        PlaylistHandler::playlistFileName->flush();
        PlaylistHandler::playlistFileName->close();

    }
    catch (...) {
        //send debug line on fail
        debugHandler->addErrorMessage("Failed to remove songs from playlist.", PLAYLISTHANDLER_SOURCE);
    }
    emit PlaylistHandler::playlistUpdated();
}

QString PlaylistHandler::getFileName() {
    QFileInfo information(PlaylistHandler::playlistFileName->fileName());
    return information.baseName();
}

QString PlaylistHandler::getSongs() {
    QString allSongs = "";
    if (!PlaylistHandler::checkForFile()) {
        return allSongs;
    }


    
    try {
        if (!PlaylistHandler::playlistFileName->open(QIODevice::ReadOnly | QIODevice::Text)) {
            debugHandler->addErrorMessage("Failed to open playlist file for reading.", PLAYLISTHANDLER_SOURCE);
            return allSongs;
        }

        QTextStream reader(PlaylistHandler::playlistFileName);
        while (!reader.atEnd()) {
            QString line = reader.readLine();
            if (!line.isEmpty()) {
                allSongs = allSongs + PlaylistHandler::getSongName(line) + "\n";
            }
        }
        PlaylistHandler::playlistFileName->flush();
        PlaylistHandler::playlistFileName->close();

    }
    catch (...) {
        //send debug line on fail
        debugHandler->addErrorMessage("Failed to get all songs from playlist file.", PLAYLISTHANDLER_SOURCE);
    }
    return allSongs;
}

QList<QString> PlaylistHandler::getSongList() {
    QList<QString> allSongs;
    if (!PlaylistHandler::checkForFile()) {
        return allSongs;
    }



    try {
        if (!PlaylistHandler::playlistFileName->open(QIODevice::ReadOnly | QIODevice::Text)) {
            debugHandler->addErrorMessage("Failed to open playlist file for reading.", PLAYLISTHANDLER_SOURCE);
            return allSongs;
        }

        QTextStream reader(PlaylistHandler::playlistFileName);
        while (!reader.atEnd()) {
            QString line = reader.readLine();
            if (!line.isEmpty()) {
                allSongs.push_back(PlaylistHandler::getSongName(line));
            }
        }
        PlaylistHandler::playlistFileName->flush();
        PlaylistHandler::playlistFileName->close();

    }
    catch (...) {
        //send debug line on fail
        debugHandler->addErrorMessage("Failed to get playlist songs as a list.", PLAYLISTHANDLER_SOURCE);
    }
    return allSongs;
}

QString PlaylistHandler::getSongsFormatted() {
    QString allSongs = "";
    if (!PlaylistHandler::checkForFile()) {
        return allSongs;
    }



    try {
        if (!PlaylistHandler::playlistFileName->open(QIODevice::ReadOnly | QIODevice::Text)) {
            debugHandler->addErrorMessage("Failed to open playlist file for reading.", PLAYLISTHANDLER_SOURCE);
            return allSongs;
        }

        QTextStream reader(PlaylistHandler::playlistFileName);
        int currentLine = 0;
        while (!reader.atEnd()) {
            QString line = reader.readLine();
            if (!line.isEmpty() && currentLine == PlaylistHandler::songLine) {
                allSongs = allSongs + ">>" + PlaylistHandler::getSongName(line) + "\n";
                currentLine++;
            }
            else if (!line.isEmpty()) {
                allSongs = allSongs + PlaylistHandler::getSongName(line) + "\n";
                currentLine++;
            }
        }
        PlaylistHandler::playlistFileName->flush();
        PlaylistHandler::playlistFileName->close();

    }
    catch (...) {
        //send debug line on fail
        debugHandler->addErrorMessage("Failed to get playlist songs formatted.", PLAYLISTHANDLER_SOURCE);
    }
    return allSongs;
}

QString PlaylistHandler::getSongName(QString songPath) {
    QFileInfo information(songPath);
    return information.baseName();
}

void PlaylistHandler::playFirstSong() {
    if (!PlaylistHandler::checkForFile()) {
        return;
    }

    try { //try... catch for error handling
        //opens file & sets the search for the next song
        if (!PlaylistHandler::playlistFileName->open(QIODevice::ReadOnly | QIODevice::Text)) { //attempts to open the playlist file
            debugHandler->addErrorMessage("Failed to open playlist file for reading.", PLAYLISTHANDLER_SOURCE);
            return;
        }

        //reads lines until it reaches the song it wants, then exits & keeps it
        int currentLine = 0; //which line is currently being read
        QTextStream fileRead(PlaylistHandler::playlistFileName); //text stream used to read file
        QString entry = ""; //current line's data
        bool foundLine = false; //boolean for whether or not the first audio file is found
        do {

            entry = fileRead.readLine(); //uses the readLine() method to read the next line, and stores it into entry
            if (currentLine == PlaylistHandler::songLine && !entry.isEmpty()) { //checks to see if there is data that is not a "\n" in that line, if so exits
                foundLine = true;
                break;
            }
            if (!entry.isEmpty()) { //increments the current line if the actual line is empty
                currentLine++;
            }
        } while (!fileRead.atEnd());

        fileHandler->changeCurrentSong(entry);
        PlaylistHandler::playlistFileName->close(); //closes the file
        emit PlaylistHandler::playlistUpdated();
        return; 
    }
    catch (...) {
        PlaylistHandler::playlistFileName->close(); //close the playlist file (if possible) on error
        debugHandler->addErrorMessage("Failed to read playlist file to find first song.", PLAYLISTHANDLER_SOURCE);
        return;
    }
}

PlaylistHandler* playlistHandler = new PlaylistHandler();
