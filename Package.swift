// swift-tools-version:5.5

import PackageDescription

let package = Package(
    name: "LayerGps",
    platforms: [
        .iOS(.v11),
        .macOS(.v10_13)
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
        .package(url: "https://github.com/openmobilemaps/maps-core.git", from: "2.5.1"),
        .package(url: "https://github.com/UbiqueInnovation/ubkit-ios", from: "1.8.1"),
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
                .product(name: "MapCoreSharedModuleCpp", package: "maps-core"),
            ],
            path: "shared",
            sources: ["src", "public"],
            publicHeadersPath: "public",
            cxxSettings: [
                .headerSearchPath("public"),
                .headerSearchPath("src"),
                .headerSearchPath("src/gps"),
            ]
        ),
    ],
    cxxLanguageStandard: .cxx17
)
