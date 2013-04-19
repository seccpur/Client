#pragma once

#include "Shared.h"

#include <QtCore/QObject>

#include <QtNetwork/QTcpSocket>

class CASPAR_EXPORT AMCPDevice : public QObject
{
    Q_OBJECT

    public:
        explicit AMCPDevice(QObject* parent = 0);
        virtual ~AMCPDevice();

        void connectDevice(const QString& address, int port = 5250);
        void disconnectDevice();

        bool isConnected() const;

    protected:
        enum AMCPCommand
        {
            NONE,
            CONNECTIONSTATE,
            LOAD,
            LOADBG,
            PLAY,
            STOP,
            CG,
            CLS,
            CINF,
            VERSION,
            TLS,
            INFO,
            INFOSYSTEM,
            DATALIST,
            DATARETRIEVE,
            CLEAR,
            SET,
            MIXER,
            CALL,
            REMOVE,
            ADD,
            SWAP,
            STATUS,
            ERROR,
            THUMBNAILLIST,
            THUMBNAILRETRIEVE
        };

        QTcpSocket* socket;
        AMCPCommand command;

        QList<QString> response;

        virtual void sendNotification() = 0;

        int getPort() const;
        const QString& getAddress() const;

        void resetDevice();
        void writeMessage(const QString& message);

    private:
        enum AMCPParserState
        {
            ExpectingHeader,
            ExpectingOneline,
            ExpectingTwoline,
            ExpectingMultiline
        };

        int port;
        int code;
        int state;
        bool connected;
        QString line;
        QString address;
        QString previousLine;

        void parseLine(const QString& line);
        void parseHeader(const QString& line);
        void parseOneline(const QString& line);
        void parseTwoline(const QString& line);
        void parseMultiline(const QString& line);

        AMCPCommand translateCommand(const QString& command);

        Q_SLOT void readMessage();
        Q_SLOT void setConnected();
        Q_SLOT void setDisconnected();
        Q_SLOT void reconnectDevice();
};