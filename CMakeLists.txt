file(GLOB_RECURSE IMAGE_PROCESSOR_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_executable(
    image_processor
    ${IMAGE_PROCESSOR_SOURCES}
)

target_compile_options(image_processor PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
)
