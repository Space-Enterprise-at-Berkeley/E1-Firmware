# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
cc_binary(
    name = "test",
    srcs = ["Test.cpp"],
   deps = [
       ":main",
   ],
)

cc_library(
    name = "main",
    srcs = ["main.cpp"],
    hdrs = ["main.h"],
    strip_include_prefix = "",
    deps = [
        "//src/tests:tests"
    ]
)

cc_library(
    name = "src",
    srcs = ["Automation.cpp", "Comms.cpp", "Ducers.cpp", "Heaters.cpp", "Power.cpp", "Thermocouples.cpp", "Valves.cpp"],
    hdrs = ["Automation.h", "Comms.h", "Ducers.h", "Heaters.h", "Power.h", "Thermocouples.h", "Valves.h"],
    strip_include_prefix = "",
    visibility = ["//visibility:public"],
    deps = [
        "//lib/Scheduler:scheduler",
        "//lib/GpioExpander:gpio",
        "//lib/tempController:tempController",
    ],    
)