// swift-tools-version:5.3

import PackageDescription

let package = Package(
    name: "LayerGps",
    platforms: [
        .iOS(.v11),
    ],
    products: [
        .library(
            name: "LocationManager",
            targets: ["LocationManager"]
        ),
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
        .package(name: "MapCore",
                 url: "https://github.com/openmobilemaps/maps-core.git",
                 .branch("develop")),
    ],
    targets: [
        .target(
            name: "LocationManager",
            dependencies: [
                "LayerGps"
            ],
            path: "ios/LocationManager"
        ),
        .target(
            name: "LayerGps",
            dependencies: [
                .product(name: "MapCore", package: "MapCore"),
                "LayerGpsSharedModule",
            ],
            path: "ios/Layer"
        ),
        .target(
            name: "LayerGpsSharedModule",
            dependencies: [
                "LayerGpsSharedModuleCpp",
                .product(name: "MapCoreSharedModule", package: "MapCore"),
            ],
            path: "bridging/ios",
            publicHeadersPath: ""
        ),
        .target(
            name: "LayerGpsSharedModuleCpp",
            dependencies: [
                .product(name: "MapCoreSharedModuleCpp", package: "MapCore"),
            ],
            path: "shared",
            sources: ["src"],
            publicHeadersPath: "public",
            cxxSettings: [
                .headerSearchPath("**"),
                .headerSearchPath("public"),
                .headerSearchPath("src/gps")
            ]
        ),
    ],
    cxxLanguageStandard: .cxx1z
)
