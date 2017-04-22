#include "languageserverprotocol.h"
#include "base_protocol.h"

namespace LSP {

LanguageServerProtocol::LanguageServerProtocol(QIODevice* device, QObject* parent)
    : QObject(parent) {

}

}
