cmake_minimum_required(VERSION 3.10)
project(TOTP_Examples C)

set(CMAKE_C_STANDARD 99)

#
# ---- OpenSSL Version ----
#

find_package(OpenSSL REQUIRED)

add_executable(totp_openssl
    totp_openssl.c
    base32.c
)

target_link_libraries(totp_openssl
    OpenSSL::Crypto
)

#
# ---- Standalone SHA-1 Version ----
#

add_executable(totp_standalone
    totp_standalone.c
    sha1.c
    base32.c
)

target_include_directories(totp_standalone PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

