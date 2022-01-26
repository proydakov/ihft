#pragma once

const char * const VALID_TOML_DOC =
R"(title = "my toml configuration"

[library]
name = "toml++"
authors = ["Mark Gillard <mark.gillard@outlook.com.au>"]
cities = ["上海", "北京", "深圳"]
bogatyrs = ["Добрыня Никитич", "Илья Муромец", "Алёша Попович"]

[dependencies]
lang = "C++"
cpp_version = 20
cpp_compiler = "clang"
operation_system = "linux"
)";
