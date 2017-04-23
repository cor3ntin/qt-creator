import qbs 1.0

Project {
    name: "LanguageServerProtocol"

    QtcDevHeaders { }

    QtcLibrary {
        cpp.defines: base.concat(["LANGUAGESERVERPROTOCOL_LIBRARY"])
        cpp.optimization: "fast"
        cpp.cxxLanguageVersion: "c++14"

        Depends { name: "Qt.core" }

        files: [
            "json_serialization.cpp",
            "json_serialization.h",
            "languageserverprotocol.cpp",
            "languageserverprotocol.h",
            "protocol.cpp",
            "protocol.h",
            "protocol_fwd.h",
        ]
    }
}
