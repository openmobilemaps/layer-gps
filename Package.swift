// swift-tools-version:5.5

import PackageDescription

let package = Package(
    name: "LayerGps",
    platforms: [
        .iOS(.v14),
        .macOS(.v10_14),
    ],
    products: [
        .library(
            name: "LayerGps",
            targets: ["LayerGps"]
        ),
        .library(
            name: "LayerGpsSharedModule",
            targets: ["LayerGpsSharedModule"]
        ),
        .library(
            name: "LayerGpsSharedModuleCpp",
            targets: ["LayerGpsSharedModuleCpp"]
        ),
    ],
    dependencies: [
        .package(url: "https://github.com/openmobilemaps/maps-core", from: .init(stringLiteral: "3.0.0-rc.6")),
        .package(url: "https://github.com/UbiqueInnovation/ubkit-ios", .upToNextMinor(from: "2.0.0")),
    ],
    targets: [
        .target(
            name: "LayerGps",
            dependencies: [
                .product(name: "MapCore", package: "maps-core"),
                .product(name: "UBLocation", package: "ubkit-ios"),
                "LayerGpsSharedModule",
            ],
            path: "ios",
            exclude: ["README.md"],
            resources: [
                .process("Resources/Assets.xcassets")
            ]
        ),
        .target(
            name: "LayerGpsSharedModule",
            dependencies: [
                "LayerGpsSharedModuleCpp",
                .product(name: "MapCoreSharedModule", package: "maps-core"),
            ],
            path: "bridging/ios",
            publicHeadersPath: ""
        ),
        .target(
            name: "LayerGpsSharedModuleCpp",
            dependencies: [
                .product(name: "MapCoreSharedModuleCpp", package: "maps-core")
            ],
            path: "shared",
            sources: ["src"],
            publicHeadersPath: "public",
            cxxSettings: [
                .headerSearchPath("**"),
                .headerSearchPath("public"),
                .headerSearchPath("src/gps"),
                .define("DEBUG", to: "1", .when(configuration: .debug)),
                .define("NDEBUG", to: "1", .when(configuration: .release)),
            ]
        ),
    ],
    cxxLanguageStandard: .cxx17
)
