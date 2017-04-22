import qbs 1.0

Project {
    name: "LanguageServerProtocol"

    QtcDevHeaders { }

    QtcLibrary {
        cpp.defines: base.concat(["LANGUAGESERVERPROTOCOL_LIBRARY"])
        cpp.optimization: "fast"

        Depends { name: "Qt.core" }

        files: [
            "base_protocol.h",
            "languageserverprotocol.cpp",
            "languageserverprotocol.h",
        ]
    }
}
