cc_library(
    name = "test_utils",
    hdrs = ["test_utils.h"],
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