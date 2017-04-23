#include "languageserverprotocol.h"
#include "protocol.h"

namespace LSP {

LanguageServerProtocol::LanguageServerProtocol(QIODevice* device, QObject* parent)
    : QObject(parent) {

}

}
