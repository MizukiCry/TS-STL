cc_library(
    name = "test_utils",
    hdrs = ["test_utils.h"],
    visibility = [
        "//test/benchmark:__pkg__",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    copts = ["-std=c++17", "-O2"],
    deps = [
        "//src:ts-stl",
        "test_utils"
    ]
)

cc_test(
    name = "vector_test",
    size = "small",
    srcs = ["vector_test.cpp"],
    copts = ["-std=c++17"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//src:ts-stl",
        "test_utils",
    ],
)

cc_test(
    name = "stack_test",
    size = "small",
    srcs = ["stack_test.cpp"],
    copts = ["-std=c++17"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//src:ts-stl",
        "test_utils",
    ],
)

cc_test(
    name = "queue_test",
    size = "small",
    srcs = ["queue_test.cpp"],
    copts = ["-std=c++17"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//src:ts-stl",
        "test_utils",
    ]
)

cc_test(
    name = "deque_test",
    size = "small",
    srcs = ["deque_test.cpp"],
    copts = ["-std=c++17"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//src:ts-stl",
        "test_utils",
    ]
)

cc_test(
    name = "bitset_test",
    size = "small",
    srcs = ["bitset_test.cpp"],
    copts = ["-std=c++17"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//src:ts-stl",
        "test_utils",
    ]
)

cc_test(
    name = "hashmap_test",
    size = "small",
    srcs = ["hashmap_test.cpp"],
    copts = ["-std=c++17"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//src:ts-stl",
        "test_utils",
    ]
)

cc_test(
    name = "array_test",
    size = "small",
    srcs = ["array_test.cpp"],
    copts = ["-std=c++17"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//src:ts-stl",
        "test_utils",
    ]
)

cc_test(
    name = "map_test",
    size = "small",
    srcs = ["map_test.cpp"],
    copts = ["-std=c++17"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//src:ts-stl",
        "test_utils",
    ]
)