#pragma once
#include <QObject>
#include <QIODevice>



namespace LSP {

class LanguageServerProtocol : public QObject
{
    Q_OBJECT
public:
    LanguageServerProtocol(QIODevice* device, QObject* parent = Q_NULLPTR);
};

}
